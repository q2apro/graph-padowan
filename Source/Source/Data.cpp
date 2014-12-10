/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2007 Ivan Johansen
 *
 * Graph is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
#include "Graph.h"
#pragma hdrstop
#include <memory>
#include <algorithm>
#include <complex>
#include <Registry.hpp>
#include "Clipboard.h"
#include "VersionInfo.h"
#include "IGraphic.h"
#include <fstream>
#include "ConfigFile.h"
#include <float.h>
#include <functional>
#include <boost/lexical_cast.hpp>
#include "OleServer.h"
#include <iterator>

namespace Graph
{
TUndoList UndoList(50);
//---------------------------------------------------------------------------
TData::TData() : TopElem(new TTopGraphElem(this))
{
}
//---------------------------------------------------------------------------
 TData::TData(const TData &OldData) : Axes(OldData.Axes), CustomFunctions(OldData.CustomFunctions),
  Modified(false), OnAbortUpdate(NULL), TopElem(OldData.TopElem->Clone(this))
{
  for(unsigned int I = 0; I < TopElem->ChildCount(); I++)
    for(unsigned J = 0; J < TopElem->GetChild(I)->ChildCount(); J++)
      if(TShading *Shade = dynamic_cast<TShading*>(TopElem->GetChild(I)->GetChild(J).get()))
        if(Shade->Func2)
        {
          //Update pointer cross references
          int Index = OldData.TopElem->GetChildIndex(Shade->Func2);
          Shade->Func2 = boost::dynamic_pointer_cast<TBaseFuncType>(TopElem->GetChild(Index));
        }
  Update();
}
//---------------------------------------------------------------------------
void TData::WriteInfoToIni(TConfigFile &IniFile)
{
  TVersionInfo VerInfo;
  TConfigFileSection &Section = IniFile.Section(L"Graph");
  Section.Write(L"Version", VerInfo.FileVersion().Text());
  Section.Write(L"MinVersion", MINVERSION);
  Section.Write(L"OS", GetWindowsVersion());
}
//---------------------------------------------------------------------------
void TData::CheckIniInfo(const TConfigFile &IniFile)
{
  std::wstring MinVersion = IniFile.Section(L"Graph").Read(L"MinVersion", L"1.0");
  if(TVersion(MinVersion) > TVersionInfo().ProductVersion())
    throw EGraphError(LoadRes(548, MinVersion));

  TVersion SavedByVersion = IniFile.Section(L"Graph").Read(L"Version", TVersion(100));
  if(SavedByVersion < MIN_SAVED_VERSION)
    throw EGraphError(LoadString(549));
}
//---------------------------------------------------------------------------
void TData::Clear()
{
  while(TopElem->ChildCount())
    TopElem->RemoveChild(0);
  CustomFunctions.Clear();
  AnimationInfo.Clear();
  PluginData.clear();
}
//---------------------------------------------------------------------------
void TData::ClearCache()
{
  TopElem->ClearCache();
}
//---------------------------------------------------------------------------
struct TElemCount
{
  unsigned Func;
  unsigned PointSeries;
  unsigned Shade;
  unsigned Label;
  unsigned Relation;
  unsigned OleObject;
  TElemCount::TElemCount() : Func(0), PointSeries(0), Shade(0), Label(0),
    Relation(0), OleObject(0) {}
};
//---------------------------------------------------------------------------
void TData::WriteElem(TConfigFile &IniFile, const TGraphElemPtr &Elem, struct TElemCount &Count)
{
  if(TBaseFuncType *Func = dynamic_cast<TBaseFuncType*>(Elem.get()))
  {
    std::wstring SectionName = L"Func" + ToWString(++Count.Func);
    Func->WriteToIni(IniFile.Section(SectionName));

    //Write tangents
    unsigned TanCount = 0;
    for(unsigned N = 0; N < Func->ChildCount(); N++)
      if(dynamic_cast<TTan*>(Func->GetChild(N).get()))
        Func->GetChild(N)->WriteToIni(IniFile.Section(SectionName + L"Tan" + ToWString(++TanCount)));
    IniFile.Section(SectionName).Write(L"TanCount", TanCount, 0U);
  }
  else if(TPointSeries *Series = dynamic_cast<TPointSeries*>(Elem.get()))
    Series->WriteToIni(IniFile.Section(L"PointSeries" + ToWString(++Count.PointSeries)));
  else if(TTextLabel *Label = dynamic_cast<TTextLabel*>(Elem.get()))
    Label->WriteToIni(IniFile.Section(L"Label" + ToWString(++Count.Label)));
  else if(TRelation *Relation = dynamic_cast<TRelation*>(Elem.get()))
    Relation->WriteToIni(IniFile.Section(L"Relation" + ToWString(++Count.Relation)));
  else if(TAxesView *AxesView = dynamic_cast<TAxesView*>(Elem.get()))
    AxesView->WriteToIni(IniFile.Section(L"Axes"));

  for(unsigned J = 0; J < Elem->ChildCount(); J++)
    if(dynamic_cast<TShading*>(Elem->GetChild(J).get()))
      Elem->GetChild(J)->WriteToIni(IniFile.Section(L"Shade" + ToWString(++Count.Shade)));
}
//---------------------------------------------------------------------------
void TData::WriteCount(TConfigFile &ConfigFile, const TElemCount &Count)
{
  TConfigFileSection &DataSection = ConfigFile.Section(L"Data");
  //Don't use "LabelCount" because it gives problems in version 2.7.1
  DataSection.Write(L"TextLabelCount", Count.Label);
  DataSection.Write(L"FuncCount", Count.Func);
  DataSection.Write(L"PointSeriesCount", Count.PointSeries);
  DataSection.Write(L"ShadeCount", Count.Shade);
  DataSection.Write(L"RelationCount", Count.Relation);
  DataSection.Write(L"OleObjectCount", Count.OleObject);
}
//---------------------------------------------------------------------------
std::wstring TData::SaveToString(const TGraphElemPtr &Elem) const
{
  TConfigFile ConfigFile;
	TElemCount ElemCount;
	WriteInfoToIni(ConfigFile);
	//Use Temp data with only the element we want to copy.
	//This is done to make sure TShade references to functions are correct
	TData Temp;
	TGraphElemPtr TempElem = Elem->Clone();
	Temp.Insert(TempElem);
	unsigned Count = TempElem->ChildCount();
	for(int I = Count-1; I >= 0; I--)
	{
		TShadingPtr Shading = boost::dynamic_pointer_cast<TShading>(TempElem->GetChild(I));
		if(Shading && Shading->Func2)
			TempElem->RemoveChild(I);
	}
	Temp.WriteElem(ConfigFile, TempElem, ElemCount);
	WriteCount(ConfigFile, ElemCount);
	return ConfigFile.GetAsString();
}
//---------------------------------------------------------------------------
void TData::SaveData(TConfigFile &ConfigFile) const
{
  TElemCount Count;
  for(unsigned I = 0; I < ElemCount(); I++)
    WriteElem(ConfigFile, GetElem(I), Count);
  WriteCount(ConfigFile, Count);
}
//---------------------------------------------------------------------------
void TData::LoadData(const TConfigFile &IniFile)
{
  for(unsigned I = 0; I < IniFile.SectionCount(); I++)
  {
    boost::shared_ptr<TGraphElem> Elem;
    const TConfigFileSection& Section = IniFile.Section(I);
    const std::wstring SectionName = Section.GetName();
    if(SectionName.substr(0, 4) == L"Func" && SectionName.find(L"Tan") == std::wstring::npos)
      switch(Section.Read(L"FuncType", ftStdFunc))
      {
        case ftStdFunc: Elem.reset(new TStdFunc); break;
        case ftParFunc: Elem.reset(new TParFunc); break;
        case ftPolFunc: Elem.reset(new TPolFunc); break;
        default: continue; //Ignore unknown function types
      }
    else if(SectionName.substr(0, 11) == L"PointSeries")
      Elem.reset(new TPointSeries);
    else if(SectionName.substr(0, 5) == L"Label")
      Elem.reset(new TTextLabel);
    else if(SectionName.substr(0, 8) == L"Relation")
      Elem.reset(new TRelation);
    else if(SectionName.substr(0, 10) == L"Axes")
      Elem.reset(new TAxesView);
    else
      continue; //No known elem type

    //Stream data from inifile
    TopElem->InsertChild(Elem);
    Elem->ReadFromIni(Section);

    //Create list of tangents
    int TanCount = Section.Read(L"TanCount", 0);
    for(int I = 0; I < TanCount; I++)
    {
      boost::shared_ptr<TTan> Tan(new TTan);
      Elem->InsertChild(Tan);
      Tan->ReadFromIni(IniFile.Section(SectionName + L"Tan" + ToWString(I+1)));
    }

    Elem->Update(); //Needed to update a and q for tangents. Must be called after ParentFunc is set
  }

  //We need to load shades after all functions are loaded
  unsigned ShadeCount = IniFile.Section(L"Data").Read(L"ShadeCount", 0U);
  for(unsigned I = 0; I < ShadeCount; I++)
  {
    boost::shared_ptr<TGraphElem> Shade(new TShading);
    const TConfigFileSection &Section = IniFile.Section(L"Shade" + ToWString(I+1));
    int FuncNo = Section.Read(L"FuncNo", 0) - 1;
    if(FuncNo != -1)
      GetFuncFromIndex(FuncNo)->InsertChild(Shade);
    Shade->ReadFromIni(Section);
  }
}
//---------------------------------------------------------------------------
//Find pixel coordinate on Func nearest to (X,Y)
//Returns -1 if there is none
int FindNearestPointN(const TBaseFuncType *Func, int X, int Y)
{
  unsigned BestN = 0;
  double BestDist = INF;

  //If no part of function visible
  if(Func->Points.size() == 0)
    return -1;

  for(unsigned N = 0; N < Func->Points.size(); N++)
  {
    double dX = X - Func->Points[N].x;
    double dY = Y - Func->Points[N].y;
    double Dist = dX*dX + dY*dY;
    if(Dist < BestDist)
    {
      BestDist = Dist;
      BestN = N;
    }
  }

  return BestN;
}
//---------------------------------------------------------------------------
//Returns NAN if there is none
double FindNearestPoint(const TBaseFuncType *Func, int X, int Y)
{
  int N = FindNearestPointN(Func, X, Y);
  return N == -1 ? NAN : Func->sList[N].t;
}
//---------------------------------------------------------------------------
//Find interception between Func and other functions
//Returns NAN if no interception is found
double TData::FindInterception(const TBaseFuncType *Func, int X, int Y) const
{
  if(Func->Points.size() < 3)
    return NAN;
  unsigned N = FindNearestPointN(Func, X, Y);
  if(N == 0)
    N = 1;
  else if(N == Func->Points.size() - 1)
    N--;

  typedef std::vector<Func32::TCoordSet<> >::const_iterator TCoordSetIter;
  TCoordSetIter Begin = Func->sList.begin();
  TCoordSetIter End = Func->sList.end();
  TCoordSetIter p1 = Begin + N; //p1 used to search backwards
  TCoordSetIter p2 = p1;        //p2 is always p1+1
  TCoordSetIter p3 = p2;        //p3 used to search forward
  TCoordSetIter p4 = p3;        //p4 is always p3+1
  --p1;
  ++p4;

  while(p1 != Begin || p4 != End)
  {
    for(unsigned I = 0; I < TopElem->ChildCount(); I++)
      if(const TBaseFuncType *Func2 = dynamic_cast<TBaseFuncType*>(TopElem->GetChild(I).get()))
        if(Func2->GetVisible())
        {
          const Func32::TFunc *StdFunc2 = dynamic_cast<const Func32::TFunc*>(&Func2->GetFunc());

          if(StdFunc2 && Func == Func2)
            continue; //If it is the same function, and the function is a TFunc there is no crossing

          if(p1 != Begin)
          {
            //Standard function is a special case optimized for speed
            if(StdFunc2)
              try
              {
                if(Sign(p1->y - Func32::GetReal(StdFunc2->CalcY(Func32::TComplex(p1->x)))) != Sign(p2->y - Func32::GetReal(StdFunc2->CalcY(Func32::TComplex(p2->x)))))
                  return Func32::FindCrossing(Func->GetFunc(), p1->t, p2->t, *StdFunc2, p1->x, p2->x);
              }
              catch(Func32::EFuncError&) //Errors are ignored; Just continue
              {
              }
            else
            {
              TCoordSetIter Iter = Func2->sList.end();
              if(Func == Func2)
              { //Special handling for crossing with self: Split the search in two
                if(p1 != Func->sList.begin()+1)
                {
                  Iter = FindCrossing(p1, Func->sList.begin(), p1-2);
                  if(Iter == p1-2)
                    Iter = FindCrossing(p1, p1+2, Func->sList.end());
                }
              }
              else
                Iter = FindCrossing(p1, Func2->sList.begin(), Func2->sList.end());
              if(Iter != Func2->sList.end())
              {
                unsigned N = Range<unsigned>(1, p1 - Begin, Func->sList.size() - 3);
                unsigned M = Range<unsigned>(1, Iter - Func2->sList.begin(), Func2->sList.size() - 3);
                //Use binary search to increase acuracy of found position
                return Func32::FindCrossing(Func->GetFunc(), Func->sList[N].t, Func->sList[N+1].t, Func2->GetFunc(), Func2->sList[M].t, Func2->sList[M+1].t);
              }
            }
          }

          if(p4 != End)
          {
            //Standard function is a special case optimized for speed
            if(StdFunc2)
              try
              {
                if(Sign(p3->y - Func32::GetReal(StdFunc2->CalcY(Func32::TComplex(p3->x)))) != Sign(p4->y - Func32::GetReal(StdFunc2->CalcY(Func32::TComplex(p4->x)))))
                  return Func32::FindCrossing(Func->GetFunc(), p3->t, p4->t, *StdFunc2, p3->x, p4->x);
              }
              catch(Func32::EFuncError&) //Errors are ignored; Just continue
              {
              }
            else
            {
              TCoordSetIter Iter = Func2->sList.end();
              if(Func == Func2)
              { //Special handling for crossing with self: Split the search in two
                if(p3 != Func->sList.begin() + 1)
                {
                  Iter = FindCrossing(p3, Func->sList.begin(), p3-2);
                  if(Iter == p3-2)
                    Iter = FindCrossing(p3, p3+2, Func->sList.end());
                }
              }
              else
                Iter = FindCrossing(p3, Func2->sList.begin(), Func2->sList.end());
              if(Iter != Func2->sList.end())
              {
                unsigned N = Range<unsigned>(1, p3 - Begin, Func->Points.size() - 3);
                unsigned M = Range<unsigned>(1, Iter - Func2->sList.begin(), Func2->sList.size() - 3);
                return Func32::FindCrossing(Func->GetFunc(), Func->sList[N].t, Func->sList[N+1].t, Func2->GetFunc(), Func2->sList[M].t, Func2->sList[M+1].t);
              }
            }
          }
        }

    if(p1 != Begin)
      --p1, --p2;
    if(p4 != End)
      ++p3, ++p4;
  }

  return NAN; //Signal no crossing found
}
//---------------------------------------------------------------------------
std::wstring TData::CreatePointSeriesDescription() const
{
  std::wstring Str = LoadString(RES_SERIES) + L" ";
  int I = 1;
  while(true)
  {
    std::wstring CmpStr = Str + ToWString(I++);
    unsigned N;
    for(N = 0; N < TopElem->ChildCount(); N++)
      if(TopElem->GetChild(N)->GetLegendText() == CmpStr)
        break;
    if(N == TopElem->ChildCount())
      return CmpStr;
  }
}
//---------------------------------------------------------------------------
boost::shared_ptr<TTextLabel> TData::FindLabel(int X, int Y) const
{
  for(unsigned I = 0; I < TopElem->ChildCount(); I++)
  {
    if(boost::shared_ptr<TTextLabel> Label = boost::dynamic_pointer_cast<TTextLabel>(TopElem->GetChild(I)))
      if(Label->GetVisible() && Label->IsInsideRect(X, Y))
        return Label;
  }
  return boost::shared_ptr<TTextLabel>();
}
//---------------------------------------------------------------------------
void TData::ImportUserModels(const std::wstring &Str)
{
  TConfigFile IniFile;
  IniFile.LoadFromString(Str);

  for(unsigned I = 0; I < IniFile.SectionCount(); I++)
  {
    const TConfigFileSection &Section = IniFile.Section(I);
    TUserModel UserModel;
    UserModel.Model = Section.Read(L"Model", L"$a*x+$b");

    for(TConfigFileSection::TIterator Iter = Section.Begin(); Iter != Section.End(); ++Iter)
    {
      if(Iter->first[0] == '$')
        UserModel.Defaults.push_back(std::make_pair(Iter->first, Iter->second));
    }

    UserModels[Section.GetName()] = UserModel;
  }
}
//---------------------------------------------------------------------------
std::wstring TData::ExportUserModels() const
{
  TConfigFile IniFile;

  for(TUserModels::const_iterator Iter = UserModels.begin(); Iter != UserModels.end(); ++Iter)
  {
    TConfigFileSection &Section = IniFile.Section(Iter->first);
    Section.Write(L"Model", Iter->second.Model);

    for(unsigned J = 0; J < Iter->second.Defaults.size(); J++)
      Section.Write(Iter->second.Defaults[J].first, Iter->second.Defaults[J].second);
  }

  return IniFile.GetAsString();
}
//---------------------------------------------------------------------------
void TData::SetModified()
{
  Modified = true;
  SendOleAdvise(acDataChanged);
}
//---------------------------------------------------------------------------
double FindNearestValue(const std::vector<Func32::TCoordSet<> > &Values, int X, int Y, const TDraw &Draw)
{
  if(Values.empty())
    throw EGraphError(L"No data in vector");

  double MinDist = INF;
  double Result;

  for(std::vector<Func32::TCoordSet<> >::const_iterator Iter = Values.begin(); Iter != Values.end(); ++Iter)
  {
    double dx = X - Draw.xPoint(Iter->x);
    double dy = Y - Draw.yPoint(Iter->y);
    double Dist = dx*dx + dy*dy;
    if(Dist < MinDist)
      MinDist = Dist, Result = Iter->t;
  }

  return Result;
}
//---------------------------------------------------------------------------
double TraceFunction(const TBaseFuncType *Func, TTraceType TraceType, int X, int Y, const TData &Data, const TDraw &Draw)
{
  switch(TraceType)
  {
    case ttTrace:
      return FindNearestPoint(Func, X, Y); //Returns NAN if no point found

    case ttIntersection:
      return Data.FindInterception(Func, X, Y);

    case ttXAxis:
    case ttYAxis:
    {
      double Min, Max, ds;
      Func->GetCurrentRange(Min, Max, ds);
      std::vector<Func32::TCoordSet<> > List = AnalyseFunction(Func->GetFunc(), Min, Max,
        Draw.GetAxesRect().Width(), 1E-16, TraceType == ttXAxis ? Func32::atXAxisCross : Func32::atYAxisCross);
      if(!List.empty())
        return FindNearestValue(List, X, Y, Draw);
      return NAN;
    }

    case ttExtremeY:
    case ttExtremeX:
    {
      double Min, Max, ds;
      Func->GetCurrentRange(Min, Max, ds);
      //WARNING: bcc codegen bug when operator ?: is used and AnalyseFunction() throws an exception
      Func32::TFunc F;
      if(TraceType == ttExtremeY)
        F = Func->GetFunc().ConvYToFunc().MakeDif();
      else
        F = Func->GetFunc().ConvXToFunc().MakeDif();
      std::vector<Func32::TCoordSet<> > List = AnalyseFunction(F, Min, Max, Draw.GetAxesRect().Width(), 1E-16, Func32::atXAxisCross);

      //Convert the list of f'(x) coordinates to f(x) coordinates. Notice that List2 may have less
      //elements than List, because we may have found some extremums that don't have valid coordinates.
      std::vector<Func32::TCoordSet<> > List2;
      Transform(List.begin(), List.end(), std::back_inserter(List2), Func32::TEvalCoordSet<>(Func->GetFunc())); //Fill list with coordinates for f(x) instead of f'(x)
      if(!List2.empty())
        return FindNearestValue(List2, X, Y, Draw);
      return NAN;
    }

    default:
      return NAN;
  }
}
//---------------------------------------------------------------------------
bool ExportPointSeries(const TPointSeries *Series, const wchar_t *FileName, char Delimiter, char DecimalSeparator)
{
  std::wofstream File(FileName);
  if(!File)
    return false;

  const std::vector<TPointSeriesPoint> &Points = Series->GetPointData();
  for(unsigned I = 0; I < Points.size(); I++)
  {
    std::wstring xText = Points[I].First;
    std::wstring yText = Points[I].Second;
    if(DecimalSeparator != '.')
    {
      std::replace(xText.begin(), xText.end(), '.', DecimalSeparator);
      std::replace(yText.begin(), yText.end(), '.', DecimalSeparator);
    }
    File << xText << Delimiter << yText << std::endl;
  }
  return true;
}
//---------------------------------------------------------------------------
void TData::Delete(const TGraphElemPtr &Elem)
{
  const TGraphElemPtr &Parent = Elem->GetParent();
  if(Parent)
    Parent->RemoveChild(Parent->GetChildIndex(Elem));
}
//---------------------------------------------------------------------------
int TData::GetIndex(const TGraphElemPtr &Elem)
{
  const TGraphElemPtr &Parent = Elem->GetParent();
  return Parent->GetChildIndex(Elem);
}
//---------------------------------------------------------------------------
void TData::Replace(unsigned Index, const TGraphElemPtr &Elem)
{
  TopElem->ReplaceChild(Index, Elem);
}
//---------------------------------------------------------------------------
void TData::Replace(const TGraphElemPtr &OldElem, const TGraphElemPtr &Elem)
{
  int Index = OldElem->GetParent()->GetChildIndex(OldElem);
  const boost::shared_ptr<TTopGraphElem> &TopElem = OldElem->GetData().TopElem;
  OldElem->GetParent()->ReplaceChild(Index, Elem);

  while(OldElem->ChildCount() > 0)
  {
    const TGraphElemPtr &Child = OldElem->GetChild(0);
    OldElem->RemoveChild(0);
    Elem->InsertChild(Child);
  }

  for(unsigned I = 0; I < TopElem->ChildCount(); I++)
    for(unsigned J = 0; J < TopElem->GetChild(I)->ChildCount(); J++)
      if(const boost::shared_ptr<TShading> &Shade = boost::dynamic_pointer_cast<TShading>(TopElem->GetChild(I)->GetChild(J)))
        if(Shade->Func2 == OldElem)
          Shade->Func2 = boost::dynamic_pointer_cast<TBaseFuncType>(Elem);

  Elem->Update(); //Update tangents
}
//---------------------------------------------------------------------------
void TData::Insert(const boost::shared_ptr<TGraphElem> &Elem, int Index)
{
  TopElem->InsertChild(Elem, Index);
}
//---------------------------------------------------------------------------
boost::shared_ptr<TBaseFuncType> TData::GetFuncFromIndex(unsigned Index) const
{
  unsigned Count = 0;
  for(unsigned I = 0; I < TopElem->ChildCount(); I++)
    if(boost::shared_ptr<TBaseFuncType> Func = boost::dynamic_pointer_cast<TBaseFuncType>(TopElem->GetChild(I)))
      if(Count++ == Index)
        return Func;
  return boost::shared_ptr<TBaseFuncType>();
}
//---------------------------------------------------------------------------
boost::shared_ptr<TGraphElem> TData::GetElem(unsigned Index) const
{
  BOOST_ASSERT(Index < TopElem->ChildCount());
  return TopElem->GetChild(Index);
}
//---------------------------------------------------------------------------
void TData::Update()
{
  CustomFunctions.Update(*this);
  TopElem->Update();
}
//---------------------------------------------------------------------------
void TData::LoadPluginData(const TConfigFileSection &Section)
{
  for(TConfigFileSection::TIterator Iter = Section.Begin(); Iter != Section.End(); ++Iter)
    PluginData[Iter->first] = Iter->second;
}
//---------------------------------------------------------------------------
bool TData::IsDependent(const std::wstring &Expression, const std::wstring &SymbolName) const
{
  try
  {
    return Func32::TFunc(Expression, L"", CustomFunctions.SymbolList).IsDependent(SymbolName);
  }
  catch(Func32::EFuncError &E)
  {
    return false;
  }
}
//---------------------------------------------------------------------------
} //namespace Graph

