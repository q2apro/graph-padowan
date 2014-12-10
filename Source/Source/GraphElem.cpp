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
#include "GraphElem.h"
#include <float.h>
#include "ConfigFile.h"
#include "IGraphic.h"
#include "Tokenizer.h"
#include "VersionInfo.h"
#include "GuiUtil.h"
#include <boost/lexical_cast.hpp>
#include <boost/mem_fn.hpp>
//---------------------------------------------------------------------------
namespace Graph
{
////////////////
// TTextValue //
////////////////
TTextValue::TTextValue(double AValue) : Value(AValue)
{
  if(_finite(Value))
    Text = ToWString(AValue);
}
//---------------------------------------------------------------------------
long double FastCalc(const std::wstring &Text, const TData &Data, bool IgnoreErrors=true)
{
  if(Text.empty())
  {
    if(!IgnoreErrors)
      throw Func32::EFuncError(Func32::ecEmptyString);
    return NAN;
  }

  //Optimize for numbers
  //Make sure there is no 'e' (Euler's constant) as it will be interpretted as 'E'
  if(Text.find(L"e") == std::wstring::npos)
  {
    //use wcstod as boost::lexical_cast is slow when it fails
    wchar_t *endptr;
    double Value = wcstod(Text.c_str(), &endptr);
    if(endptr == Text.c_str() + Text.size())
      return Value;
    if(Text == L"INF")
      //wcstod() handles "+INF" and "-INF"
      return INF;
  }
  try
  {
    return Data.Calc(Text);
  }
  catch(Func32::EFuncError &E)
  {
    if(!IgnoreErrors)
      throw;
    return NAN;
  }
}
//---------------------------------------------------------------------------
void TTextValue::Update(const TData &Data)
{
  //Nothing to update. Values is already +/-INF
  if(!Text.empty())
    Value = FastCalc(Text, Data, true);
}
//---------------------------------------------------------------------------
void TTextValue::Set(const std::wstring AText, const TData &Data, bool IgnoreErrors)
{
  Value = FastCalc(AText, Data, IgnoreErrors);
  Text = _finite(Value) ? AText : std::wstring();
}
//---------------------------------------------------------------------------
void TTextValue::Set(double AValue)
{
  Value = AValue;
  Text = _finite(Value) ? ToWString(AValue) : std::wstring();
}
//---------------------------------------------------------------------------
bool TTextValue::IsDependent(const TData &Data, const std::wstring &SymbolName) const
{
  try
  {
    return Func32::TFunc(Text, L"", Data.CustomFunctions.SymbolList).IsDependent(SymbolName);
  }
  catch(Func32::EFuncError &E)
  {
    return false;
  }
}
//---------------------------------------------------------------------------
std::wostream& operator<<(std::wostream &Stream, const TTextValue &TextValue)
{
  if(TextValue.Value == INF)
    return Stream << L"+INF";
  else if(TextValue.Value == -INF)
    return Stream << L"-INF";
  return Stream << TextValue.Text;
}
//---------------------------------------------------------------------------
////////////////
// TGraphElem //
////////////////
TGraphElem::TGraphElem(const TGraphElem &Elem)
  : Visible(Elem.Visible), ShowInLegend(Elem.ShowInLegend), LegendText(Elem.LegendText)
{
  //Do not copy ChildList; It must be copyed from the derived class to be able to call SetParentFunc()
}
//---------------------------------------------------------------------------
void TGraphElem::WriteToIni(TConfigFileSection &Section) const
{
  Section.Write(L"Visible", Visible, true);
  Section.Write(L"ShowInLegend", ShowInLegend, true);
  Section.Write(L"LegendText", LegendText, std::wstring());
  std::map<std::wstring,std::wstring>::const_iterator End = PluginData.end();
  for(std::map<std::wstring,std::wstring>::const_iterator Iter = PluginData.begin(); Iter != End; ++Iter)
    Section.Write(L"$" + Iter->first, Iter->second);
}
//---------------------------------------------------------------------------
void TGraphElem::ReadFromIni(const TConfigFileSection &Section)
{
	Visible = Section.Read(L"Visible", true);
	ShowInLegend = Section.Read(L"ShowInLegend", true);
	LegendText = Section.Read(L"LegendText", L"");
	TConfigFileSection::TIterator End = Section.End();
	for(TConfigFileSection::TIterator Iter = Section.Begin(); Iter != End; ++Iter)
		if(Iter->first[0] == L'$')
			PluginData[Iter->first.substr(1)] = Iter->second;
}
//---------------------------------------------------------------------------
TGraphElemPtr TGraphElem::CloneHelper(TGraphElem *NewElem) const
{
	TGraphElemPtr Elem(NewElem);
	for(unsigned I = 0; I < ChildList.size(); I++)
		Elem->InsertChild(ChildList[I]->Clone());
	return Elem;
}
//---------------------------------------------------------------------------
void TGraphElem::InsertChild(const TGraphElemPtr &Elem, int Index)
{
  TGraphElemPtr Parent = Elem->GetParent();
  if(Parent)
  {
    int ParentIndex = Parent->GetChildIndex(Elem);
    Parent->RemoveChild(ParentIndex);
  }

  if(Index == -1)
    ChildList.push_back(Elem);
  else
    ChildList.insert(ChildList.begin() + Index, Elem);
  Elem->SetParent(shared_from_this());
}
//---------------------------------------------------------------------------
void TGraphElem::ReplaceChild(unsigned Index, const TGraphElemPtr &Elem)
{
  BOOST_ASSERT(Index < ChildList.size());
  if(Elem->GetParent())
    Elem->GetParent()->RemoveChild(Elem->GetParent()->GetChildIndex(Elem));
  ChildList[Index]->SetParent(boost::shared_ptr<TBaseFuncType>());
  ChildList[Index] = Elem;
  Elem->SetParent(shared_from_this());
}
//---------------------------------------------------------------------------
unsigned TGraphElem::GetChildIndex(const TGraphElemPtr &Elem) const
{
  return IndexOf(ChildList, Elem);
}
//---------------------------------------------------------------------------
void TGraphElem::RemoveChild(unsigned Index)
{
  BOOST_ASSERT(Index < ChildList.size());
  ChildList[Index]->SetParent(TGraphElemPtr());
  ChildList.erase(ChildList.begin() + Index);
}
//---------------------------------------------------------------------------
void TGraphElem::ClearCache()
{
  for(unsigned I = 0; I < ChildList.size(); I++)
    ChildList[I]->ClearCache();
}
//---------------------------------------------------------------------------
void TGraphElem::Update()
{
  std::for_each(ChildList.begin(), ChildList.end(), boost::mem_fn(&TGraphElem::Update));
}
//---------------------------------------------------------------------------
///////////////////
// TTopGraphElem //
///////////////////
boost::shared_ptr<TTopGraphElem> TTopGraphElem::Clone(const TData *AData) const
{
  boost::shared_ptr<TTopGraphElem> Result =
		boost::static_pointer_cast<TTopGraphElem>(CloneHelper(new TTopGraphElem(*this)));
  Result->Data = AData;
  return Result;
}
//---------------------------------------------------------------------------
///////////////////
// TBaseFuncType //
///////////////////
TBaseFuncType::TBaseFuncType() :
  Color(clRed), Size(1), Style(psSolid), DrawType(dtAuto),
  StartPointStyle(0), EndPointStyle(0), From(-INF), To(INF)
{
}
//---------------------------------------------------------------------------
//NOTE: This does not copy tangents; Use CopyTangents()
TBaseFuncType::TBaseFuncType(const TBaseFuncType &F) : TGraphElem(F), DrawType(F.DrawType),
  Color(F.Color), Size(F.Size), Style(F.Style),
  From(F.From), To(F.To), Steps(F.Steps), StartPointStyle(F.StartPointStyle), EndPointStyle(F.EndPointStyle)
{
}
//---------------------------------------------------------------------------
void TBaseFuncType::WriteToIni(TConfigFileSection &Section) const
{
  TGraphElem::WriteToIni(Section);

  Section.Write(L"From", From.Text, std::wstring());
  Section.Write(L"To", To.Text, std::wstring());
  Section.Write(L"Steps", Steps.Text, std::wstring());
  Section.Write(L"Style", Style, psSolid);
  Section.Write(L"Color", Color);
	Section.Write(L"Size", Size, 1U);
  Section.Write(L"StartPoint", StartPointStyle, 0U);
  Section.Write(L"EndPoint", EndPointStyle, 0U);
  Section.Write(L"DrawType", DrawType, dtAuto);
}
//---------------------------------------------------------------------------
void TBaseFuncType::ReadFromIni(const TConfigFileSection &Section)
{
  const TData &Data = GetData();
  GetFunc().SetTrigonometry(GetData().Axes.Trigonometry);
  Style = Section.Read(L"Style", psSolid);
  Color = Section.Read(L"Color", clRed);
  Size = Section.Read(L"Size", 1);
  if(Section.KeyExists(L">From"))
    From.Set(Section.Read(L">From", L"-INF"), GetData(), true); //Caused by bug in MS Outlook Express
  else
    From.Set(Section.Read(L"From", L"-INF"), Data, true);
  To.Set(Section.Read(L"To", L"+INF"), Data, true);
  Steps.Set(Section.Read(L"Steps", L""), Data, true);
  StartPointStyle = Section.Read(L"StartPoint", 0);
  EndPointStyle = Section.Read(L"EndPoint", 0);
  DrawType = Section.Read(L"DrawType", dtAuto);

  TGraphElem::ReadFromIni(Section);
}
//---------------------------------------------------------------------------
void TBaseFuncType::ClearCache()
{
  Points.clear();
  PointNum.clear();
	sList.clear();
  TGraphElem::ClearCache();
}
//---------------------------------------------------------------------------
void TBaseFuncType::Update()
{
  GetFunc().SetTrigonometry(GetData().Axes.Trigonometry);
  GetFunc().Update(GetData().CustomFunctions.SymbolList);
  From.Update(GetData());
  To.Update(GetData());
  Steps.Update(GetData());
  Steps.Value = floor(Steps.Value); //Make sure Steps is an integer
  if(Steps.Value < 0)
    Steps.Value = 0;
  TGraphElem::Update();
}
//---------------------------------------------------------------------------
void TBaseFuncType::GetCurrentRange(double &Min, double &Max, double &ds) const
{
  Min = From.Value;
  Max = To.Value;
  ds = Steps.Value == 1 ? 1 : (To.Value - From.Value) / (Steps.Value - 1); //If Steps, From and To are defined, ds must be exact
}
//---------------------------------------------------------------------------
Func32::TCoord<long double> TBaseFuncType::Eval(long double t) const
{
  if(!GetData().Axes.CalcComplex)
    return GetFunc().Calc(t);

	Func32::TCoord<Func32::TComplex> Result = GetFunc().Calc(Func32::TComplex(t));
  if(imag(Result.x) || imag(Result.y))
    throw Func32::ECalcError(Func32::ecComplexError);
  return Func32::TCoord<long double>(real(Result.x), real(Result.y));
}
//---------------------------------------------------------------------------
long double TBaseFuncType::CalcArea(long double From, long double To) const
{
  return GetFunc().CalcArea(From, To, 1E-3);
}
//---------------------------------------------------------------------------
bool TBaseFuncType::IsDependent(const std::wstring &SymbolName) const
{
  return GetFunc().IsDependent(SymbolName) || From.IsDependent(GetData(), SymbolName) ||
    To.IsDependent(GetData(), SymbolName) || Steps.IsDependent(GetData(), SymbolName);
}
//---------------------------------------------------------------------------
//////////////
// TStdFunc //
//////////////
TStdFunc::TStdFunc(const std::wstring &AText, const Func32::TSymbolList &SymbolList, Func32::TTrigonometry Trig)
  : Text(AText), Func(AText, L"x", SymbolList, Trig)
{
}
//---------------------------------------------------------------------------
TStdFunc::TStdFunc(const Func32::TFunc &AFunc)
  : Text(AFunc.MakeText()), Func(AFunc)
{
}
//---------------------------------------------------------------------------
void TStdFunc::WriteToIni(TConfigFileSection &Section) const
{
  Section.Write(L"FuncType", ftStdFunc);
  Section.Write(L"y", Text);

	TBaseFuncType::WriteToIni(Section);
}
//---------------------------------------------------------------------------
void TStdFunc::ReadFromIni(const TConfigFileSection &Section)
{
  TBaseFuncType::ReadFromIni(Section);
  Text = Section.Read(L"y", L"");

  try
  {
    Func.SetFunc(Text, L"x", GetData().CustomFunctions.SymbolList);
  }
  catch(Func32::EParseError &E)
  {
    ShowStatusError(GetErrorMsg(E));
  }
}
//---------------------------------------------------------------------------
std::wstring TStdFunc::MakeText() const
{
  if(Text.size() >= 100)
    return L"f(x)=" + Text.substr(0, 100) + L"...";
  return L"f(x)=" + Text;
}
//---------------------------------------------------------------------------
boost::shared_ptr<TBaseFuncType> TStdFunc::MakeDifFunc()
{
  boost::shared_ptr<TStdFunc> DifFunc(new TStdFunc);
  DifFunc->Func = Func.MakeDif();
  DifFunc->Text = DifFunc->Func.MakeText();
  return DifFunc;
}
//---------------------------------------------------------------------------
void TStdFunc::GetCurrentRange(double &Min, double &Max, double &ds) const
{
  const TAxis &xAxis = GetData().Axes.xAxis;
  Min = std::max(xAxis.Min, From.Value);
  Max = std::min(xAxis.Max, To.Value);
  if(Steps.Value == 1)
    ds = 1;
  else if(From.IsFinite() && To.IsFinite() && Steps.Value > 0)
  {
    ds = (To.Value - From.Value) / (Steps.Value - 1); //If Steps, From and To are defined, ds must be exact
    if(DrawType != dtAuto && Min > From.Value)
      Min = From.Value + floor((Min - From.Value) / ds) * ds;
    if(DrawType != dtAuto && Max < To.Value)
      Max = To.Value - floor((To.Value - Max) / ds) * ds;
  }
  else if(Steps.Value > 0)
    ds = (Max - Min) / (Steps.Value - 1); //If Steps, From and To are defined, ds must be exact
  else
    ds = 0;
}
//---------------------------------------------------------------------------
//////////////
// TParFunc //
//////////////
TParFunc::TParFunc(const std::wstring &AxText, const std::wstring &AyText, const Func32::TSymbolList &SymbolList, Func32::TTrigonometry Trig)
  : xText(AxText), yText(AyText), Func(AxText, AyText, L"t", SymbolList, Trig)
{
  SetSteps(TTextValue(1000));
  From.Set(-10);
  To.Set(10);
}
//---------------------------------------------------------------------------
TParFunc::TParFunc(const Func32::TParamFunc &AFunc)
  : xText(AFunc.MakeXText()), yText(AFunc.MakeYText()), Func(AFunc)
{
}
//---------------------------------------------------------------------------
void TParFunc::WriteToIni(TConfigFileSection &Section) const
{
  Section.Write(L"FuncType", ftParFunc);
  Section.Write(L"x", xText);
  Section.Write(L"y", yText);

  TBaseFuncType::WriteToIni(Section);
}
//---------------------------------------------------------------------------
void TParFunc::ReadFromIni(const TConfigFileSection &Section)
{
  try
  {
    xText = Section.Read(L"x", L"");
    yText = Section.Read(L"y", L"");
    Func.SetFunc(xText, yText, L"t", GetData().CustomFunctions.SymbolList);
  }
  catch(Func32::EParseError &E)
  {
    ShowStatusError(GetErrorMsg(E));
  }
  TBaseFuncType::ReadFromIni(Section);
}
//---------------------------------------------------------------------------
std::wstring TParFunc::MakeText() const
{
  //Ensure that the text is not too long to show in a dialog box.
  std::wstring Str = L"x(t)=";
  if(xText.size() >= 100)
    Str += xText.substr(0, 100) + L"... , y(t)=";
  else
    Str += xText + L", y(t)=";
  if(yText.size() >= 100)
    Str += yText.substr(0, 100) + L"...";
  else
    Str += yText;
  return Str;
}
//---------------------------------------------------------------------------
boost::shared_ptr<TBaseFuncType> TParFunc::MakeDifFunc()
{
  boost::shared_ptr<TParFunc> DifFunc(new TParFunc);
  DifFunc->Func = Func.MakeDif();
  DifFunc->xText = DifFunc->Func.MakeXText();
  DifFunc->yText = DifFunc->Func.MakeYText();
  return DifFunc;
}
//---------------------------------------------------------------------------
//////////////
// TPolFunc //
//////////////
TPolFunc::TPolFunc(const std::wstring &AText, const Func32::TSymbolList &SymbolList, Func32::TTrigonometry Trig)
  : Text(AText), Func(AText, L"t", SymbolList, Trig)
{
  SetSteps(TTextValue(1000));
  From.Set(0);
  To = TTextValue(2*M_PI, L"2pi");
}
//---------------------------------------------------------------------------
void TPolFunc::WriteToIni(TConfigFileSection &Section) const
{
  Section.Write(L"FuncType", ftPolFunc);
  Section.Write(L"r", Text);

  TBaseFuncType::WriteToIni(Section);
}
//---------------------------------------------------------------------------
void TPolFunc::ReadFromIni(const TConfigFileSection &Section)
{
  try
  {
    Text = Section.Read(L"r", L"");
    Func.SetFunc(Text, L"t", GetData().CustomFunctions.SymbolList);
  }
  catch(Func32::EParseError &E)
  {
    ShowStatusError(GetErrorMsg(E));
  }

  TBaseFuncType::ReadFromIni(Section);
}
//---------------------------------------------------------------------------
std::wstring TPolFunc::MakeText() const
{
  if(Text.size() <= 100)
    return L"r(t)=" + Text;
  return L"r(t)=" + Text.substr(0, 100) + L"...";
}
//---------------------------------------------------------------------------
boost::shared_ptr<TBaseFuncType> TPolFunc::MakeDifFunc()
{
  boost::shared_ptr<TPolFunc> DifFunc(new TPolFunc);
  DifFunc->Func = Func.MakeDif();
  DifFunc->Text = DifFunc->Func.MakeText();
  return DifFunc;
}
//---------------------------------------------------------------------------
//////////
// TTan //
//////////
TTan::TTan()
{
  DrawType = dtLines;
}
//---------------------------------------------------------------------------
void TTan::WriteToIni(TConfigFileSection &Section) const
{
  Section.Write(L"t", t.Text);
  Section.Write(L"From", From, TTextValue(-INF));
  Section.Write(L"To", To, TTextValue(+INF));
  Section.Write(L"Style", Style, psSolid);
  Section.Write(L"Color", Color);
  Section.Write(L"Size", Size, 1U);
  Section.Write(L"TangentType", TangentType, ttTangent);

  TBaseFuncType::WriteToIni(Section);
}
//---------------------------------------------------------------------------
void TTan::ReadFromIni(const TConfigFileSection &Section)
{
  t.Text = Section.Read(L"t", L"0");
  t.Value = GetData().Calc(t.Text);
  TangentType = Section.Read(L"TangentType", ttTangent);

  TBaseFuncType::ReadFromIni(Section);
  DrawType = dtLines; //Overwrite the default from TBaseFuncType::ReadFromIni()
}
//---------------------------------------------------------------------------
std::wstring TTan::MakeText() const
{
  if(boost::shared_ptr<TBaseFuncType> Func = boost::dynamic_pointer_cast<TBaseFuncType>(GetParent()))
    return Func->GetVariable() + L"=" + t.Text;
  return L"";
}
//---------------------------------------------------------------------------
std::wstring TTan::MakeLegendText() const
{
  if(!GetLegendText().empty())
    return GetLegendText();

  if(_isnan(a))
    return L"";
  if(!_finite(a))
    return L"x=" + RoundToString(q, GetData());
  return L"y=" + RoundToString(a, GetData()) + L"x" + (q < 0 ? L'-' : L'+') + RoundToString(std::abs(q), GetData());
}
//---------------------------------------------------------------------------
const TTextValue& TTan::GetSteps() const
{
  return boost::math::isfinite(a) ? TTextValue(0, L"") : TTextValue(2, L"");
}
//---------------------------------------------------------------------------
void TTan::UpdateTan(double a1, double q1)
{
  a = a1;
  q = q1;
  if(_finite(a))
    TanFunc.SetFunc(L"t", ToWString(a) + L"t+" + ToWString(q));
  else if(_finite(q))
    TanFunc.SetFunc(ToWString(q), L"t");
  else
    TanFunc.Clear();
  Points.clear();
}
//---------------------------------------------------------------------------
bool TTan::IsValid() const
{
  return !_isnan(a);
}
//---------------------------------------------------------------------------
void TTan::GetCurrentRange(double &Min, double &Max, double &ds) const
{
  Min = _finite(a) ? std::max(GetData().Axes.xAxis.Min, From.Value) : GetData().Axes.yAxis.Min;
  Max = _finite(a) ? std::min(GetData().Axes.xAxis.Max, To.Value) : GetData().Axes.yAxis.Max;
  ds = boost::math::isfinite(a) ? 0 : 2;
}
//---------------------------------------------------------------------------
void TTan::Update()
{
  TBaseFuncType::Update();
  t.Update(GetData());
  CalcTan(); //Update a and q
}
//---------------------------------------------------------------------------
long double TTan::CalcArea(long double From, long double To) const
{
  if(_isnanl(a))
		return NAN; //The tangent is not valid, i.e. it is touching a undefined function
	if(_finitel(a))
		return a*To*To/2 + q*To - a*From*From/2 - q*From;
  return 0;    //The tangent is vertical
}
//---------------------------------------------------------------------------
//Calculate data used to draw a tangent
//(x,y) is the point there the tangent crosses the function and a is the slope
bool TTan::CalcTan()
{
  boost::shared_ptr<TBaseFuncType> F = boost::dynamic_pointer_cast<TBaseFuncType>(GetParent());
  if(!F)
    return false;

  try
  {
    const Func32::TBaseFunc &Func = F->GetFunc();

    double x = Func.CalcX(t.Value); //Find x(t)
    double y = Func.CalcY(t.Value); //Find y(t)

    double a = Func.CalcSlope(t.Value);

    if(TangentType == ttNormal)
      if(_finite(a))
        a = std::abs(a) < MIN_ZERO ? INF : -1/a;
      else
        a = 0;

    double q = _finite(a) ? y - a*x : x;
    UpdateTan(a, q);
    return true;
  }
  catch(Func32::EFuncError&)
  {
    UpdateTan(NAN, NAN);
    return false;
  }
}
//---------------------------------------------------------------------------
bool TTan::IsDependent(const std::wstring &SymbolName) const
{
  return t.IsDependent(GetData(), SymbolName) || TBaseFuncType::IsDependent(SymbolName);
}
//---------------------------------------------------------------------------
//////////////
// TShading //
//////////////
TShading::TShading()
  : ShadeStyle(ssXAxis), BrushStyle(bsFDiagonal), Color(clGreen),
  ExtendMinToIntercept(false), ExtendMaxToIntercept(false), ExtendMin2ToIntercept(false),
  ExtendMax2ToIntercept(false), MarkBorder(true),
  sMin(-INF), sMax(INF), sMin2(-INF), sMax2(INF)
{
  SetLegendText(L"Shading");
}
//---------------------------------------------------------------------------
TGraphElemPtr TShading::Clone() const
{
	TShading *Shading = new TShading(*this);
  //Func2 will be updated later in the copy constructor of TData when all functions
  //has been copied.
	Shading->Func2 = Func2;
	return CloneHelper(Shading);
}
//---------------------------------------------------------------------------
void TShading::WriteToIni(TConfigFileSection &Section) const
{
  TGraphElem::WriteToIni(Section);

  Section.Write(L"ShadeStyle", ShadeStyle);
  Section.Write(L"BrushStyle", BrushStyle);
  Section.Write(L"Color", Color);

  unsigned FuncCount = 0;
  unsigned FuncNo = 0;
	unsigned Func2No = 0;
	unsigned ElemCount = GetData().ElemCount();
	for(unsigned I = 0; I < ElemCount; I++)
	{
		TGraphElemPtr Elem = GetData().GetElem(I);
		if(dynamic_cast<TBaseFuncType*>(Elem.get()))
			FuncCount++;
		if(Elem == GetParent())
			FuncNo = FuncCount;
    if(Elem == Func2)
      Func2No = FuncCount;
  }

  BOOST_ASSERT(!Func2 || Func2No); //If Func2 is defined we must have a number for it

  Section.Write(L"FuncNo", FuncNo);
  Section.Write(L"Func2No", Func2No, 0U);
  Section.Write(L"sMin", sMin, TTextValue(-INF));
  Section.Write(L"sMax", sMax, TTextValue(+INF));
  Section.Write(L"sMin2", sMin2, TTextValue(-INF));
  Section.Write(L"sMax2", sMax2, TTextValue(+INF));
  Section.Write(L"ExtendMinToIntercept", ExtendMinToIntercept, false);
  Section.Write(L"ExtendMaxToIntercept", ExtendMaxToIntercept, false);
  Section.Write(L"ExtendMin2ToIntercept", ExtendMin2ToIntercept, false);
  Section.Write(L"ExtendMax2ToIntercept", ExtendMax2ToIntercept, false);
  Section.Write(L"MarkBorder", MarkBorder, true);
}
//---------------------------------------------------------------------------
void TShading::ReadFromIni(const TConfigFileSection &Section)
{
  TGraphElem::ReadFromIni(Section);

  //For backward compatibility
  if(GetLegendText().empty())
    SetLegendText(LoadString(RES_SHADE));

  ShadeStyle = Section.Read(L"ShadeStyle", ssXAxis);
  BrushStyle = Section.Read(L"BrushStyle", bsBDiagonal);
  Color = Section.Read(L"Color", clGreen);

  int Func2No = Section.Read(L"Func2No", 0) - 1;
  Func2.reset();
  if(Func2No != -1)
    Func2 = GetData().GetFuncFromIndex(Func2No);

  sMin.Set(Section.Read(L"sMin", L"-INF"), GetData(), true);
  sMax.Set(Section.Read(L"sMax", L"+INF"), GetData(), true);
  sMin2.Set(Section.Read(L"sMin2", L"-INF"), GetData(), true);
  sMax2.Set(Section.Read(L"sMax2", L"+INF"), GetData(), true);
  ExtendMinToIntercept = Section.Read(L"ExtendMinToIntercept", false);
  ExtendMaxToIntercept = Section.Read(L"ExtendMaxToIntercept", false);
  ExtendMin2ToIntercept = Section.Read(L"ExtendMin2ToIntercept", false);
  ExtendMax2ToIntercept = Section.Read(L"ExtendMax2ToIntercept", false);
  MarkBorder = Section.Read(L"MarkBorder", true);

  //For backweards compatibility
  if(_isnan(sMin.Value)) sMin.Set(L"-INF", GetData());
  if(_isnan(sMax.Value)) sMax.Set(L"+INF", GetData());
  if(_isnan(sMin2.Value)) sMin2.Set(L"-INF", GetData());
  if(_isnan(sMax2.Value)) sMax2.Set(L"+INF", GetData());

}
//---------------------------------------------------------------------------
std::wstring TShading::MakeText() const
{
  return GetLegendText();
}
//---------------------------------------------------------------------------
void TShading::Update()
{
  sMin.Update(GetData());
  sMax.Update(GetData());
  sMin2.Update(GetData());
  sMax2.Update(GetData());
}
//---------------------------------------------------------------------------
void TShading::ClearCache()
{
  Region.reset();
}
//---------------------------------------------------------------------------
bool TShading::IsDependent(const std::wstring &SymbolName) const
{
  return sMin.IsDependent(GetData(), SymbolName) || sMax.IsDependent(GetData(), SymbolName) ||
    sMin2.IsDependent(GetData(), SymbolName) || sMax2.IsDependent(GetData(), SymbolName);
}
//---------------------------------------------------------------------------
//////////////////
// TPointSeries //
//////////////////
TPointSeries::TPointSeries(TColor AFrameColor, TColor AFillColor, TColor ALineColor, unsigned ASize, unsigned ALineSize,
  unsigned AStyle, TPenStyle ALineStyle, TInterpolationAlgorithm AInterpolation, bool AShowLabels,
  TFont *AFont, TLabelPosition ALabelPosition, TPointType APointType,
  TErrorBarType XErrorBarType, double XErrorValue, TErrorBarType YErrorBarType, double YErrorValue)
  : FrameColor(AFrameColor), FillColor(AFillColor), LineColor(ALineColor), Size(ASize), LineSize(ALineSize),
    Style(AStyle), LineStyle(ALineStyle), Interpolation(AInterpolation), ShowLabels(AShowLabels),
    Font(AFont), LabelPosition(ALabelPosition), PointType(APointType),
    xErrorBarType(XErrorBarType), xErrorValue(XErrorValue), yErrorBarType(YErrorBarType), yErrorValue(YErrorValue)
{
  SetLegendText(L"Point series");
}
//---------------------------------------------------------------------------
void TPointSeries::InsertDblPoint(const Func32::TDblPoint &Point, int Index)
{
  TPointSeriesPoint P;
  if(PointType == ptPolar)
  {
    std::pair<long double,long double> Polar = GetPolarCoord(Point, GetData().Axes.Trigonometry);
    P.First = RoundToString(Polar.second, 6);
    P.Second = RoundToString(Polar.first, 6);
  }
  else
  {
    P.First = RoundToString(Point.x, 6);
    P.Second = RoundToString(Point.y, 6);
  }

  if(Index == -1)
  {
    PointData.push_back(P);
    PointList.push_back(Point);
  }
  else
  {
    PointData.insert(PointData.begin() + Index, P);
    PointList.insert(PointList.begin() + Index, Point);
  }
}
//---------------------------------------------------------------------------
void TPointSeries::InsertPoint(const TPointSeriesPoint &Point, int Index, bool AutoUpdate)
{
  if(Index >= static_cast<int>(PointData.size()) || Index < -1)
    throw std::out_of_range("Index out of range.");
  PointData.insert(Index == -1 ? PointData.end() : PointData.begin() + Index, Point);
  if(AutoUpdate)
    PointList.insert(Index == -1 ? PointList.end() : PointList.begin() + Index, ConvertPoint(Point));
}
//---------------------------------------------------------------------------
void TPointSeries::ReplaceDblPoint(const Func32::TDblPoint &Point, unsigned Index)
{
  TPointSeriesPoint P;
  if(PointType == ptPolar)
  {
    std::pair<long double,long double> Polar = GetPolarCoord(Point, GetData().Axes.Trigonometry);
    P.First = RoundToString(Polar.second, 2);
    P.Second = RoundToString(Polar.first, 2);
  }
  else
  {
    P.First = RoundToString(Point.x, 3);
    P.Second = RoundToString(Point.y, 3);
  }
  PointData.at(Index) = P;
  PointList.at(Index) = Point;
}
//---------------------------------------------------------------------------
void TPointSeries::ReplacePoint(const TPointSeriesPoint &Point, unsigned Index)
{
  PointData.at(Index) = Point;
  PointList.at(Index) = ConvertPoint(Point);
}
//---------------------------------------------------------------------------
void TPointSeries::DeletePoint(unsigned Index, unsigned Count)
{
  if(Index >= PointData.size())
    throw std::out_of_range("Index out of range.");
  PointData.erase(PointData.begin() + Index, PointData.begin() + Index + Count);
  PointList.erase(PointList.begin() + Index, PointList.begin() + Index + Count);
}
//---------------------------------------------------------------------------
void TPointSeries::WriteToIni(TConfigFileSection &Section) const
{
  Section.Write(L"FrameColor", FrameColor, clBlack);
  Section.Write(L"FillColor", FillColor);
  Section.Write(L"LineColor", LineColor);
  Section.Write(L"Size", Size);
  Section.Write(L"Style", Style);
  Section.Write(L"LineSize", LineSize, 1U);
  Section.Write(L"LineStyle", LineStyle, psClear);
  Section.Write(L"Interpolation", Interpolation, iaLinear);
  Section.Write(L"ShowLabels", ShowLabels, false);
  Section.Write(L"Font", FontToStr(Font), DEFAULT_POINT_FONT);
  Section.Write(L"LabelPosition", LabelPosition);
  Section.Write(L"PointType", PointType, ptCartesian);
  Section.Write(L"PointCount", PointList.size());

  std::wostringstream Str;
  for(unsigned N = 0; N < PointList.size(); N++)
  {
    if(PointData[N].First.find(',') == std::string::npos)
      Str << PointData[N].First << ',';
    else
      Str << '"' << PointData[N].First << "\",";

    if(PointData[N].Second.find(',') == std::string::npos)
      Str << PointData[N].Second << ';';
    else
      Str << '"' << PointData[N].Second << "\";";
  }
  Section.Write(L"Points", Str.str());

  Section.Write(L"xErrorBarType", xErrorBarType, ebtNone);
  Section.Write(L"xErrorBarValue", xErrorValue, 0.0);
  if(xErrorBarType == ebtCustom)
  {
    Str.str(L"");
    for(unsigned N = 0; N < PointList.size(); N++)
      Str << PointData[N].xError << ';';
    Section.Write(L"xErrorBarData", Str.str(), std::wstring());
  }

  Section.Write(L"yErrorBarType", yErrorBarType, ebtNone);
  Section.Write(L"yErrorBarValue", yErrorValue, 0.0);
  if(yErrorBarType == ebtCustom)
  {
    Str.str(L"");
    for(unsigned N = 0; N < PointList.size(); N++)
        Str << PointData[N].yError << ';';
    Section.Write(L"yErrorBarData", Str.str(), std::wstring());
  }

  TGraphElem::WriteToIni(Section);
}
//---------------------------------------------------------------------------
void TPointSeries::ReadFromIni(const TConfigFileSection &Section)
{
  TGraphElem::ReadFromIni(Section);
  //For backward compatibility
  if(GetLegendText().empty())
    SetLegendText(Section.Read(L"Text", Section.GetName()));

  FrameColor = Section.Read(L"FrameColor", clBlack);
  FillColor = Section.Read(L"FillColor", clRed);
  LineColor = Section.Read(L"LineColor", clBlue);
  Size = Section.Read(L"Size", 5);
  Style = Section.Read(L"Style", 0);
  LineSize = Section.Read(L"LineSize", 1);
  LineStyle = Section.Read(L"LineStyle", psClear);
  if(Section.KeyExists(L"SmoothLine"))
    Interpolation = Section.Read(L"SmoothLine", iaLinear);
  else
    Interpolation = Section.Read(L"Interpolation", iaLinear);
  ShowLabels = Section.Read(L"ShowLabels", false);
  StrToFont(Section.Read(L"Font", DEFAULT_POINT_FONT), Font);
  LabelPosition = Section.Read(L"LabelPosition", lpBelow);
  PointType = Section.Read(L"PointType", ptCartesian);

  TTokenizer Tokens(Section.Read(L"Points", L""), L';');
  std::wistringstream xStream(Section.Read(L"xErrorBarData", L""));
  std::wistringstream yStream(Section.Read(L"yErrorBarData", L""));
  TTokenizer Token(L"", L',', L'"');
  unsigned PointCount = Section.Read(L"PointCount", 0U);
  PointData.reserve(PointCount);

  while(Token = Tokens.Next(), Tokens)
  {
    std::wstring x, y, xError, yError;
    Token >> x >> y;

    getline(xStream, xError, L';');
    getline(yStream, yError, L';');
    TTextValue XError, YError;

    try
    {
      if(!xError.empty())
        XError.Set(xError, GetData());
      if(!yError.empty())
        YError.Set(yError, GetData());
    }
    catch(Func32::EParseError &E)
    {
      ShowStatusError(GetErrorMsg(E));
      XError.Set(xError, GetData(), true);
      YError.Set(yError, GetData(), true);
    }
    PointData.push_back(TPointSeriesPoint(x, y, XError, YError));
  }

  xErrorBarType = Section.Read(L"xErrorBarType", ebtNone);
  xErrorValue = Section.Read(L"xErrorBarValue", 0.0);

  yErrorBarType = Section.Read(L"yErrorBarType", ebtNone);
  yErrorValue = Section.Read(L"yErrorBarValue", 0.0);
}
//---------------------------------------------------------------------------
double TPointSeries::GetXError(unsigned Index) const
{
  switch(xErrorBarType)
  {
    case ebtFixed:
      return xErrorValue;

    case ebtRelative:
      return xErrorValue * PointList[Index].x / 100;

    case ebtCustom:
      return PointData[Index].xError.Value;
  }
  return 0;
}
//---------------------------------------------------------------------------
double TPointSeries::GetYError(unsigned Index) const
{
  switch(yErrorBarType)
  {
    case ebtFixed:
      return yErrorValue;

    case ebtRelative:
      return yErrorValue * PointList[Index].y / 100;

    case ebtCustom:
      return PointData[Index].yError.Value;
  }
  return 0;
}
//---------------------------------------------------------------------------
Func32::TDblPoint FindCoord(TPointSeries::TPointList::const_iterator Iter, double x)
{
//  if(Interpolation == iaLinear)
  {
    double a = ((Iter+1)->y - Iter->y) / ((Iter+1)->x - Iter->x);
    double b = Iter->y - a * Iter->x;
    return Func32::TDblPoint(x, a*x+b);
  }

  throw EAbort("");
}
//---------------------------------------------------------------------------
TPointSeries::TPointList::const_iterator TPointSeries::FindPoint(double x) const
{
  for(unsigned I = 0; I < PointList.size() - 1; I++)
    if((PointList[I].x <= x && PointList[I+1].x >= x) ||
       (PointList[I].x >= x && PointList[I+1].x <= x))
      return PointList.begin() + I;

  throw EAbort("");
}
//---------------------------------------------------------------------------
Func32::TDblPoint TPointSeries::ConvertPoint(const TPointSeriesPoint &P) const
{
  Func32::TDblPoint Point;
  long double a = FastCalc(P.First, GetData()); //x or theta
  long double b = FastCalc(P.Second, GetData()); //y or r
  if(PointType == ptPolar)
  {
    Func32::TTrigonometry Trig = GetData().Axes.Trigonometry;
    Point.x = b * std::cos(Trig == Func32::Degree ? a*M_PI/180 : a);
    Point.y = b * std::sin(Trig == Func32::Degree ? a*M_PI/180 : a);
  }
  else
    Point.x = a, Point.y = b;
  return Point;
}
//---------------------------------------------------------------------------
void TPointSeries::Update()
{
  PointList.resize(PointData.size());
  for(unsigned I = 0; I < PointList.size(); I++)
  {
    PointList[I] = ConvertPoint(PointData[I]);
    PointData[I].xError.Update(GetData());
    PointData[I].yError.Update(GetData());
  }
}
//---------------------------------------------------------------------------
bool TPointSeries::IsDependent(const std::wstring &SymbolName) const
{
  for(unsigned I = 0; I < PointList.size(); I++)
  {
    if(GetData().IsDependent(PointData[I].First, SymbolName) ||
      GetData().IsDependent(PointData[I].Second, SymbolName) ||
      PointData[I].xError.IsDependent(GetData(), SymbolName) ||
      PointData[I].yError.IsDependent(GetData(), SymbolName))
        return true;
  }
  return false;
}
//---------------------------------------------------------------------------
////////////////
// TTextLabel //
////////////////
TTextLabel::TTextLabel()
  : LabelPlacement(lpUserTopLeft), Rect(0,0,0,0), Rotation(0), xPos(0), yPos(0),
    BackgroundColor(clDefault), ContainsOleLink(false)
{
}
//---------------------------------------------------------------------------
TTextLabel::TTextLabel(const std::string &Str, TLabelPlacement Placement, const TTextValue &AxPos, const TTextValue &AyPos, TColor Color, unsigned ARotation, bool OleLink)
  : Text(Str), LabelPlacement(Placement), xPos(AxPos), yPos(AyPos),
    BackgroundColor(Color), Rotation(ARotation), ContainsOleLink(OleLink)
{
  SetShowInLegend(false);
  StatusText = RtfToPlainText(Str);
  //Update() must be called after Label is added to Data
}
//---------------------------------------------------------------------------
void TTextLabel::WriteToIni(TConfigFileSection &Section) const
{
  Section.Write(L"Placement", LabelPlacement);
  if(LabelPlacement == lpUserTopLeft || LabelPlacement >= lpUserTopRight)
    Section.Write(L"Pos", xPos.Text + L";" + yPos.Text);
  Section.Write(L"Rotation", Rotation, 0U);
  Section.Write(L"Text", ToWString(EncodeEscapeSequence(Text)));
  Section.Write(L"BackgroundColor", BackgroundColor);
  Section.Write(L"OleLink", ContainsOleLink, false);

  TGraphElem::WriteToIni(Section);
}
//---------------------------------------------------------------------------
void TTextLabel::ReadFromIni(const TConfigFileSection &Section)
{
  LabelPlacement = Section.Read(L"Placement", lpUserTopLeft);
  std::wstring Temp = Section.Read(L"Pos", L"0;0");
  size_t n = Temp.find(L";");
  if(n == std::string::npos)
  {
    //For backwards compatibility
    Func32::TDblPoint Pos = Section.Read(L"Pos", Func32::TDblPoint(0,0));
    xPos = TTextValue(Pos.x);
    yPos = TTextValue(Pos.y);
  }
  else
  {
    xPos.Set(Temp.substr(0, n), GetData(), true);
    yPos.Set(Temp.substr(n+1), GetData(), true);
  }

  Rotation = Section.Read(L"Rotation", 0U);
  Text = DecodeEscapeSequence(ToString(Section.Read(L"Text", L"ERROR")));
  BackgroundColor = Section.Read(L"BackgroundColor", clNone);
  ContainsOleLink = Section.Read(L"OleLink", false);
  if(ContainsOleLink)
    Text = ToString(UpdateRichText(ToUString(Text)));

  StatusText = RtfToPlainText(Text);

  TGraphElem::ReadFromIni(Section);
  SetShowInLegend(false); //Overwrite data; Label is never shown in legend
}
//---------------------------------------------------------------------------
std::wstring TTextLabel::MakeText() const
{
  return StatusText.empty() ? LoadString(RES_LABEL) : StatusText;
}
//---------------------------------------------------------------------------
void TTextLabel::Scale(double xSizeMul, double ySizeMul)
{
  Rect.Right = Rect.Left + Rect.Width() * xSizeMul;
  Rect.Bottom = Rect.Top + Rect.Height() * ySizeMul;
  Metafile->MMWidth = Metafile->MMWidth * xSizeMul;
  Metafile->MMHeight = Metafile->MMHeight * ySizeMul;
}
//---------------------------------------------------------------------------
void TTextLabel::Update()
{
  TPoint Size = RichTextSize(Text, &GetData());
  Metafile->Width = Size.x;
  Metafile->Height = Size.y;
  std::auto_ptr<TMetafileCanvas> Canvas(new TMetafileCanvas(Metafile, 0));

  RenderRichText(Text.c_str(), Canvas.get(), TPoint(0, 0), Size.x, BackgroundColor, &GetData());
  Canvas.reset();
  Rect = Rotate(Metafile, Rotation);

  xPos.Update(GetData());
  yPos.Update(GetData());
}
//---------------------------------------------------------------------------
bool TTextLabel::IsDependent(const std::wstring &SymbolName) const
{
  size_t Pos = std::wstring::npos;
  while((Pos = Text.rfind("%(", Pos-1)) != std::wstring::npos)
    try
		{
      size_t Pos2 = FindEndPar(Text, Pos);
      if(Pos2 == std::string::npos)
        break;
      unsigned Length = Pos2 - Pos + 1;
      if(Func32::TFunc(ToWString(Text.substr(Pos+2, Pos2-Pos-2)), L"",
        GetData().CustomFunctions.SymbolList).IsDependent(SymbolName))
        return true;
    }
    catch(Func32::EFuncError &Error)
    {
    }
  return xPos.IsDependent(GetData(), SymbolName) || yPos.IsDependent(GetData(), SymbolName);
}
//---------------------------------------------------------------------------
///////////////
// TRelation //
///////////////
TRelation::TRelation()
  : Color(clGreen), BrushStyle(bsBDiagonal), Size(1)
{
}
//---------------------------------------------------------------------------
TRelation::TRelation(const std::wstring &AText, const std::wstring &AConstraints, const Func32::TSymbolList &SymbolList, Func32::TTrigonometry Trig)
  : Text(AText), Color(clGreen), BrushStyle(bsFDiagonal), Size(1)
{
  std::vector<std::wstring> Args;
  Args.push_back(L"x");
  Args.push_back(L"y");
  Func.SetTrigonometry(Trig);
  Constraints.SetTrigonometry(Trig);
  Func.SetFunc(Text, Args, SymbolList);
  if(Func.GetFunctionType() != Func32::ftInequality && Func.GetFunctionType() != Func32::ftEquation)
    throw EGraphError(LoadString(RES_INVALID_RELATION));

  RelationType = Func.GetFunctionType() == Func32::ftInequality ? rtInequality : rtEquation;
  if(RelationType == rtEquation && Size == 0)
    Size = 1;

  if(Func.GetFunctionType() == Func32::ftEquation)
    Func.RemoveRelation();

  if(!AConstraints.empty())
  {
    Constraints.SetFunc(AConstraints, Args, SymbolList);
    ConstraintsText = AConstraints;
  }
}
//---------------------------------------------------------------------------
TRelation::TRelation(const TRelation &Relation)
  : TGraphElem(Relation), Text(Relation.Text), ConstraintsText(Relation.ConstraintsText),
    Func(Relation.Func), Constraints(Relation.Constraints), RelationType(Relation.RelationType),
    Color(Relation.Color), BrushStyle(Relation.BrushStyle), Size(Relation.Size)
{
}
//---------------------------------------------------------------------------
void TRelation::WriteToIni(TConfigFileSection &Section) const
{
  Section.Write(L"Relation", Text);
  Section.Write(L"Constraints", ConstraintsText, std::wstring());
  Section.Write(L"Style", BrushStyle);
  Section.Write(L"Color", Color);
  Section.Write(L"Size", Size, 1U);

  TGraphElem::WriteToIni(Section);
}
//---------------------------------------------------------------------------
void TRelation::ReadFromIni(const TConfigFileSection &Section)
{
  Func.SetTrigonometry(GetData().Axes.Trigonometry);
  Constraints.SetTrigonometry(GetData().Axes.Trigonometry);
  Text = Section.Read(L"Relation", L"");
  ConstraintsText = Section.Read(L"Constraints", L"");
  BrushStyle = Section.Read(L"Style", bsBDiagonal);
  Color = Section.Read(L"Color", clGreen);
  Size = Section.Read(L"Size", 1);
  std::vector<std::wstring> Args;
  Args.push_back(L"x");
  Args.push_back(L"y");
  try
  {
    Func.SetFunc(Text, Args, GetData().CustomFunctions.SymbolList );
    if(!ConstraintsText.empty())
      Constraints.SetFunc(ConstraintsText, Args, GetData().CustomFunctions.SymbolList);
  }
  catch(Func32::EParseError &E)
  {
    ShowStatusError(GetErrorMsg(E));
  }
  RelationType = Func.GetFunctionType() == Func32::ftInequality ? rtInequality : rtEquation;
  if(Func.GetFunctionType() == Func32::ftEquation)
    Func.RemoveRelation();

  TGraphElem::ReadFromIni(Section);
}
//---------------------------------------------------------------------------
std::wstring TRelation::MakeText() const
{
  return ConstraintsText.empty() ? Text : Text + L"; " + ConstraintsText;
}
//---------------------------------------------------------------------------
void TRelation::SetConstraints(const std::wstring &AConstraintsText, const Func32::TSymbolList &SymbolList)
{
  std::vector<std::wstring> Args;
  Args.push_back(L"x");
  Args.push_back(L"y");
  Constraints.SetFunc(AConstraintsText, Args, SymbolList);
  ConstraintsText = AConstraintsText;
}
//---------------------------------------------------------------------------
long double TRelation::Eval(const std::vector<long double> &Args, Func32::ECalcError &E)
{
  if(!Constraints.IsEmpty())
  {
    long double Valid = Constraints.Calc(Args, E);
    if(E.ErrorCode != Func32::ecNoError || !Valid)
      return NAN;
  }

  long double Result = Func.Calc(Args, E);
  if(E.ErrorCode != Func32::ecNoError)
    return NAN;
  return Result;
}
//---------------------------------------------------------------------------
long double TRelation::Eval(long double x, long double y)
{
  std::vector<long double> Args;
  Args.push_back(x);
  Args.push_back(y);
  if(!Constraints.IsEmpty())
  {
    long double Valid = Constraints.Calc(Args);
    if(!Valid)
      return NAN;
  }

  return Func.Calc(Args);
}
//---------------------------------------------------------------------------
void TRelation::ClearCache()
{
  Region.reset();
  BoundingRegion.reset();
}
//---------------------------------------------------------------------------
void TRelation::Update()
{
  Func.SetTrigonometry(GetData().Axes.Trigonometry);
  Constraints.SetTrigonometry(GetData().Axes.Trigonometry);
  Func.Update(GetData().CustomFunctions.SymbolList);
  Constraints.Update(GetData().CustomFunctions.SymbolList);
}
//---------------------------------------------------------------------------
bool TRelation::IsDependent(const std::wstring &SymbolName) const
{
  return Func.IsDependent(SymbolName) || Constraints.IsDependent(SymbolName);
}
//---------------------------------------------------------------------------
///////////////
// TAxesView //
///////////////
void TAxesView::ReadFromIni(const TConfigFileSection &Section)
{
  SetShowInLegend(false);
}
//---------------------------------------------------------------------------
void TAxesView::WriteToIni(TConfigFileSection &Section) const
{
  GetData().Axes.WriteToIni(Section);
}
//---------------------------------------------------------------------------
std::wstring TAxesView::MakeText() const
{
  return LoadString(RES_AXES);
}
//---------------------------------------------------------------------------
int TAxesView::GetVisible() const
{
  switch(GetData().Axes.AxesStyle)
  {
    case asCrossed:
      return 1;
    case asBoxed:
      return -1;
    default:
      return 0;
  }
}
//---------------------------------------------------------------------------
void TAxesView::ChangeVisible()
{
  TData &Data = const_cast<TData&>(GetData());
  switch(Data.Axes.AxesStyle)
  {
    case asCrossed:
      Data.Axes.AxesStyle = asBoxed;
      Data.ClearCache();
      break;

    case asBoxed:
      Data.Axes.AxesStyle = ::asNone;
      Data.ClearCache();
      break;

    default:
      Data.Axes.AxesStyle = asCrossed;
      break;
  }
}
//---------------------------------------------------------------------------
const TAxes& TAxesView::GetAxes() const
{
  return GetData().Axes;
}
//---------------------------------------------------------------------------
bool TAxesView::IsDependent(const std::wstring &SymbolName) const
{
  return false;
}
//---------------------------------------------------------------------------
} //namespace Graph
