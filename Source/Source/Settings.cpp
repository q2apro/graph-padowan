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
#include "Settings.h"
#include "ConfigFile.h"
#include "ConfigRegistry.h"
#include "VersionInfo.h"
//---------------------------------------------------------------------------
namespace Graph
{
TProperty Property;
TGuiFormatSettings GuiFormatSettings;
TPlotSettings PlotSettings;
TGuiSettings GuiSettings;
Func32::TSymbolList TCustomFunctions::GlobalSymbolList;
//---------------------------------------------------------------------------
///////////
// TAxes //
///////////
TAxis::TAxis() : Min(-10), Max(10), LogScl(false), ShowGrid(false), MultipleOfPi(false),
  ShowLabel(true), ShowNumbers(true), ShowTicks(true), AutoTick(true), AutoGrid(true),
  AxisCross(0), TickUnit(1), GridUnit(1), NumberPlacement(npCenter),
  Visible(true), ShowPositiveArrow(true), ShowNegativeArrow(false)
{
}
//---------------------------------------------------------------------------
TAxes::TAxes() : ShowLegend(true), AxesColor(clBlue),
  GridColor(static_cast<TColor>(0x00FF9999)), BackgroundColor(clWhite), Trigonometry(Func32::Radian),
  AxesStyle(asCrossed), LegendPlacement(lpTopRight), CalcComplex(false), //ZoomSquare(false),
  LegendPos(0,0), GridStyle(gsLines)
{
}
//---------------------------------------------------------------------------
void TAxis::WriteToIni(TConfigFileSection &Section, const std::wstring &Prefix) const
{
  Section.Write(Prefix + L"Min", Min);
  Section.Write(Prefix + L"Max", Max);
  Section.Write(Prefix + L"TickUnit", TickUnit);
  Section.Write(Prefix + L"GridUnit", GridUnit);
  Section.Write(Prefix + L"LogScl", LogScl, false);
  Section.Write(Prefix + L"ShowTicks", ShowTicks, true);
  Section.Write(Prefix + L"ShowGrid", ShowGrid, false);
  Section.Write(Prefix + L"ShowLabel", ShowLabel, true);
  Section.Write(Prefix + L"AutoTick", AutoTick, true);
  Section.Write(Prefix + L"AutoGrid", AutoGrid, true);
  Section.Write(Prefix + L"Label", Label, Prefix);
  Section.Write(Prefix + L"ShowNumbers", ShowNumbers, true);
  Section.Write(Prefix + L"AxisCross", AxisCross, 0.0);
	Section.Write(Prefix + L"MultiplyOfPi", MultipleOfPi, false);
  Section.Write(Prefix + L"Visible", Visible, true);
  Section.Write(Prefix + L"ShowPositiveArrow", ShowPositiveArrow, true);
  Section.Write(Prefix + L"ShowNegativeArrow", ShowNegativeArrow, false);
  Section.Write(Prefix + L"NumberPlacement", NumberPlacement, npCenter);
}
//---------------------------------------------------------------------------
void TAxes::WriteToIni(TConfigFileSection &Section) const
{
  xAxis.WriteToIni(Section, L"x");
  yAxis.WriteToIni(Section, L"y");

  Section.Write(L"AxesColor", AxesColor);
  Section.Write(L"GridColor", GridColor);
  Section.Write(L"BackgroundColor", BackgroundColor, clWhite);
  Section.Write(L"NumberFont", FontToStr(NumberFont), DEFAULT_NUMBER_FONT);
  Section.Write(L"LabelFont", FontToStr(LabelFont), DEFAULT_LABEL_FONT);
  Section.Write(L"LegendFont", FontToStr(LegendFont), DEFAULT_LEGEND_FONT);
  Section.Write(L"ShowLegend", ShowLegend);
  Section.Write(L"Radian", Trigonometry == Func32::Radian);
  Section.Write(L"Title", Title, std::wstring());
  Section.Write(L"TitleFont", FontToStr(TitleFont), DEFAULT_TITLE_FONT);
  Section.Write(L"AxesStyle", AxesStyle, asCrossed);
  Section.Write(L"LegendPlacement", LegendPlacement, lpTopRight);
  if(LegendPlacement == lpCustom)
    Section.Write(L"LegendPos", LegendPos);
  Section.Write(L"CalcComplex", CalcComplex, false);
  Section.Write(L"GridStyle", GridStyle, gsLines);
}
//---------------------------------------------------------------------------
void TAxis::ReadFromIni(const TConfigFileSection &Section, const std::wstring &Prefix)
{
  Min = Section.Read(Prefix + L"Min", -10.0);
  Max = Section.Read(Prefix + L"Max", 10.0);

  LogScl = Section.Read(Prefix + L"LogScl", false);
  TickUnit = Section.Read(Prefix + L"TickUnit", 1.0);
  GridUnit = Section.Read(Prefix + L"GridUnit", 1.0);
  ShowTicks = Section.Read(Prefix + L"ShowTicks", true);
  ShowGrid = Section.Read(Prefix + L"ShowGrid", false);
  ShowLabel = Section.Read(Prefix + L"ShowLabel", true);
  AutoTick = Section.Read(Prefix + L"AutoTick", true);
  AutoGrid = Section.Read(Prefix + L"AutoGrid", true);
  ShowNumbers = Section.Read(Prefix + L"ShowNumbers", true);
  Label = Section.Read(Prefix + L"Label", Prefix);
  AxisCross = Section.Read(Prefix + L"AxisCross", 0.0);
  MultipleOfPi = Section.Read(Prefix + L"MultiplyOfPi", false);
  Visible = Section.Read(Prefix + L"Visible", true);
  ShowPositiveArrow = Section.Read(Prefix + L"ShowPositiveArrow", true);
  ShowNegativeArrow = Section.Read(Prefix + L"ShowNegativeArrow", false);
  NumberPlacement = Section.Read(Prefix + L"NumberPlacement", npCenter);
}
//---------------------------------------------------------------------------
void TAxes::ReadFromIni(const TConfigFileSection &Section)
{
  xAxis.ReadFromIni(Section, L"x");
  yAxis.ReadFromIni(Section, L"y");

  AxesColor = Section.Read(L"AxesColor", clBlue);
  GridColor = Section.Read(L"GridColor", static_cast<TColor>(0x00FF9999));
  BackgroundColor = Section.Read(L"BackgroundColor", clWhite);
  StrToFont(Section.Read(L"NumberFont", DEFAULT_NUMBER_FONT), NumberFont);
  StrToFont(Section.Read(L"LabelFont", DEFAULT_LABEL_FONT), LabelFont);
  StrToFont(Section.Read(L"LegendFont", DEFAULT_LEGEND_FONT), LegendFont);
  StrToFont(Section.Read(L"TitleFont", DEFAULT_TITLE_FONT),TitleFont);
  ShowLegend = Section.Read(L"ShowLegend", true);
  Title = Section.Read(L"Title", std::wstring());
  Trigonometry = Section.Read(L"Radian", true) ? Func32::Radian : Func32::Degree;
  AxesStyle = Section.Read(L"AxesStyle", asCrossed);
  LegendPlacement = Section.Read(L"LegendPlacement", lpTopRight);
  LegendPos = Section.Read(L"LegendPos", Func32::TDblPoint(0, 0));
  CalcComplex = Section.Read(L"CalcComplex", false);
  GridStyle = Section.Read(L"GridStyle", gsLines);
}
//---------------------------------------------------------------------------
void TAxes::HandleZoomSquare(double xyScale)
{
  if(/*ZoomSquare &&*/ xAxis.LogScl == yAxis.LogScl)
  {
    if(xAxis.LogScl)
    {
      double yMiddle = std::exp((std::log(yAxis.Max) + std::log(yAxis.Min)) / 2);
      double dy = std::exp(xyScale * (std::log(xAxis.Max) - std::log(xAxis.Min)) / 2);
      yAxis.Min = yMiddle / dy;
      yAxis.Max = yMiddle * dy;
    }
    else
    {
      double yMiddle = (yAxis.Min + yAxis.Max) /2;
      double dy = xyScale * (xAxis.Max - xAxis.Min);
      yAxis.Min = yMiddle - dy / 2;
      yAxis.Max = yMiddle + dy / 2;
    }
  }
//  else
//    ZoomSquare = false;
}
//---------------------------------------------------------------------------
///////////////
// TProperty //
///////////////
TProperty::TProperty() : RoundTo(4), SavePos(true), ComplexFormat(cfRectangular),
  CheckForUpdate(true), ShowTipsAtStartup(true), NextTip(RES_FIRST_TIP), Language(L"English"),
  FontScale(100)
{
  //Initialize with default settings
  DefaultFunction.FromString(L"", psSolid, clRed, 1);
  DefaultPoint.FromString(L"", 0, clRed, 5);
  DefaultTrendline.FromString(L"", psSolid, clBlue, 1);
  DefaultPointLine.FromString(L"", psClear, clBlue, 1);
  DefaultShade.FromString(L"", bsBDiagonal, clGreen, 0);
  DefaultRelation.FromString(L"", bsBDiagonal, clGreen, 1);
  DefaultTangent.FromString(L"", psSolid, clRed, 1);
  DefaultDif.FromString(L"", psSolid, clGreen, 1);
  StrToFont(DEFAULT_POINT_FONT, DefaultPointLabelFont);
  StrToFont(DEFAULT_TEXT_LABEL_FONT, DefaultLabelFont);
}
//---------------------------------------------------------------------------
void TProperty::Read(const TConfigRegistry &Registry)
{
  //Use current value as default
  RoundTo = Registry.Read("RoundTo", RoundTo);
  ComplexFormat = static_cast<TComplexFormat>(Registry.Read("ComplexFormat", ComplexFormat));
  SavePos = Registry.Read("SavePosition", SavePos);
  CheckForUpdate = Registry.Read("CheckForUpdate", CheckForUpdate);

  DefaultFunction.FromString(Registry.Read(L"DefaultFunction", L""), psSolid, clRed, 1);
  DefaultPoint.FromString(Registry.Read(L"DefaultPoint", L""), 0, clRed, 5);
  DefaultTrendline.FromString(Registry.Read(L"DefaultTrendline", L""), psSolid, clRed, 1);
  DefaultPointLine.FromString(Registry.Read(L"DefaultPointLine", L""), psClear, clBlue, 1);
  DefaultShade.FromString(Registry.Read(L"DefaultShade", L""), bsBDiagonal, clGreen, 0);
  DefaultRelation.FromString(Registry.Read(L"DefaultRelation", L""), bsBDiagonal, clGreen, 1);
  DefaultTangent.FromString(Registry.Read(L"DefaultTangent", L""), psSolid, clRed, 1);
  DefaultDif.FromString(Registry.Read(L"DefaultDif", L""), psSolid, clRed, 1);
  StrToFont(Registry.Read(L"DefaultPointLabelFont", L""), DefaultPointLabelFont);
  StrToFont(Registry.Read(L"DefaultLabelFont", L""), DefaultLabelFont);

  ShowTipsAtStartup = Registry.Read(L"ShowTipsAtStartup", ShowTipsAtStartup);
  NextTip = Registry.Read(L"NextTip", NextTip) + RES_FIRST_TIP; //Tips in registry count from 0
  FontScale = Registry.Read(L"FontScale", FontScale);

  DecimalSeparator = Registry.Read(L"DecimalSeparator", L'\0');
  CustomDecimalSeparator = DecimalSeparator != 0;
  if(!CustomDecimalSeparator)
    DecimalSeparator = GetLocaleChar(GetThreadLocale(), LOCALE_SDECIMAL, '.');
}
//---------------------------------------------------------------------------
void TProperty::Write(TConfigRegistry &Registry)
{
  Registry.Write("RoundTo", RoundTo);
  Registry.Write("ComplexFormat", ComplexFormat);
  Registry.Write("SavePosition", SavePos);
  Registry.Write("CheckForUpdate", CheckForUpdate);

  Registry.Write(L"DefaultFunction", DefaultFunction.ToString());
  Registry.Write(L"DefaultPoint", DefaultPoint.ToString());
  Registry.Write(L"DefaultPointLine", DefaultPointLine.ToString());
  Registry.Write(L"DefaultTrendline", DefaultTrendline.ToString());
  Registry.Write(L"DefaultShade", DefaultShade.ToString());
  Registry.Write(L"DefaultRelation", DefaultRelation.ToString());
  Registry.Write(L"DefaultTangent", DefaultTangent.ToString());
  Registry.Write(L"DefaultDif", DefaultDif.ToString());
  Registry.Write(L"DefaultPointLabelFont", FontToStr(DefaultPointLabelFont));
  Registry.Write(L"DefaultLabelFont", FontToStr(DefaultLabelFont));

  Registry.Write("ShowTipsAtStartup", ShowTipsAtStartup);
  Registry.Write("NextTip", NextTip - RES_FIRST_TIP); //Tips in registry count from 0

  Registry.Write("FontScale", FontScale);
  Registry.Write("DecimalSeparator", CustomDecimalSeparator ? DecimalSeparator : 0);
}
//---------------------------------------------------------------------------
/////////////////////
// TCustomFunction //
/////////////////////
TCustomFunction::TCustomFunction(const std::wstring &Str, const std::wstring &AText)
  : Text(AText)
{
  size_t Begin = Str.find('(');
  if(Begin == std::wstring::npos)
    Name = CheckAndTrimName(Str, 0);
  else
  {
    Name = CheckAndTrimName(Str.substr(0, Begin), 0);

    ++Begin;
    size_t End;
    while((End = Str.find(',', Begin)) != std::string::npos)
    {
      Arguments.push_back(CheckAndTrimName(Str.substr(Begin, End - Begin), Begin));
      Begin = End + 1;
    }

    End = Str.find(')', Begin);
    if(End == std::wstring::npos)
      throw ECustomFunctionError(cfeEndParMissing, End);

    if(End != Str.size() - 1)
      throw ECustomFunctionError(cfeCharAfterEndPar, End);
    Arguments.push_back(CheckAndTrimName(Str.substr(Begin, End - Begin), Begin));
  }
}
//---------------------------------------------------------------------------
std::wstring TCustomFunction::GetName() const
{
  std::wstring Str = Name;
  if(!Arguments.empty())
  {
    Str += '(';
    for(unsigned I = 0; I < Arguments.size() - 1; I++)
      Str += Arguments[I] + L',';
    Str += Arguments.back() + L')';
  }
  return Str;
}
//---------------------------------------------------------------------------
std::wstring TCustomFunction::CheckAndTrimName(const std::wstring &Str, unsigned Offset)
{
  size_t Begin = Str.find_first_not_of(L" ");
  size_t End = Str.find_last_not_of(L" ");
  if(Begin == std::wstring::npos)
    throw ECustomFunctionError(Offset ? cfeEmptyArg : cfeEmptyName, Offset);

  std::wstring Name = Str.substr(Begin, End - Begin + 1);
  if(Name[0] == '$')
    throw ECustomFunctionError(Offset ? cfeInvalidArg : cfeInvalidName, Begin + Offset, Name);

  if(!Func32::IsValidName(Name))
    throw ECustomFunctionError(Offset ? cfeInvalidArg : cfeInvalidName, Begin + Offset, Name);

  return Name;
}
//---------------------------------------------------------------------------
//////////////////////
// TCustomFunctions //
//////////////////////
TCustomFunctions::TCustomFunctions()
  : InternalSymbolList(GlobalSymbolList), SymbolList(GlobalSymbolList)
{
}
//---------------------------------------------------------------------------
TCustomFunctions::TCustomFunctions(const TCustomFunctions &Other)
  : InternalSymbolList(GlobalSymbolList), SymbolList(GlobalSymbolList)
{
  for(unsigned I = 0; I < Other.Functions.size(); I++)
  {
    const TCustomFunction &Function = Other.Functions[I];
    TCustomFunction NewFunc(Function);
    NewFunc.InternalFunc.reset(new Func32::TCustomFunc(*Function.InternalFunc));
    if(Function.Func)
      NewFunc.Func.reset(new Func32::TCustomFunc(*Function.Func));
    Functions.push_back(NewFunc);
    InternalSymbolList.Add(NewFunc.Name, NewFunc.InternalFunc);
    SymbolList.Add(NewFunc.Name, NewFunc.Func ? NewFunc.Func : NewFunc.InternalFunc);
  }

  for(unsigned I = 0; I < Functions.size(); I++)
  {
    const TCustomFunction &Function = Functions[I];
    Function.InternalFunc->Update(InternalSymbolList);
    if(Function.Func)
      Function.Func->Update(SymbolList);
  }
}
//---------------------------------------------------------------------------
void TCustomFunctions::Add(const std::wstring &Str, const std::wstring &Value)
{
  TCustomFunction Function(Str, Value);
  if(InternalSymbolList.Exists(Function.Name))
    throw ECustomFunctionError(cfeDoubleDefinedSymbol, 0, Str);
  Function.InternalFunc = InternalSymbolList.Add(Function.Name, L"0", Function.Arguments);
  Functions.push_back(Function);
}
//---------------------------------------------------------------------------
void TCustomFunctions::Add(const std::wstring &Name, const Func32::TArgType &Args, const std::wstring &Text)
{
  TCustomFunction CustomFunction(Name, Args, Text);
  std::wstring LowerName = ToLower(Name);
  for(TIterator Iter = Functions.begin(); Iter != Functions.end(); ++Iter)
    if(ToLower(Iter->Name) == LowerName)
    {
      CustomFunction.InternalFunc = Iter->InternalFunc;
      CustomFunction.InternalFunc->SetFunc(Text, CustomFunction.Arguments, SymbolList);
      *Iter = CustomFunction;
      return;
    }
  CustomFunction.InternalFunc = InternalSymbolList.Add(Name, Text, CustomFunction.Arguments);
  SymbolList.Add(Name, CustomFunction.InternalFunc);
  Functions.push_back(CustomFunction);
}
//---------------------------------------------------------------------------
void TCustomFunctions::Replace(const std::wstring &Name, const std::wstring &Value)
{
  for(TIterator Iter = Functions.begin(); Iter != Functions.end(); ++Iter)
    if(Iter->Name == Name)
    {
      Iter->Text = Value;
      Iter->Arguments.clear();
      Iter->InternalFunc->SetFunc(Value, Iter->Arguments, InternalSymbolList);
      return;
    }
  throw ECustomFunctionError(cfeSymbolUndefined, 0, Name);
}
//---------------------------------------------------------------------------
void TCustomFunctions::Replace(const std::wstring &Name, long double Value)
{
  for(TIterator Iter = Functions.begin(); Iter != Functions.end(); ++Iter)
    if(Iter->Name == Name)
    {
      Iter->Text = ToWString(Value);
      Iter->Arguments.clear();
      Iter->InternalFunc->SetFunc(Value);
      return;
    }
  throw ECustomFunctionError(cfeSymbolUndefined, 0, Name);
}
//---------------------------------------------------------------------------
void TCustomFunctions::Delete(const std::wstring &Name)
{
  for(TIterator Iter = Functions.begin(); Iter != Functions.end(); ++Iter)
    if(Iter->Name == Name)
    {
      Functions.erase(Iter);
      InternalSymbolList.Erase(Name);
      SymbolList.Erase(Name);
      return;
    }
  throw ECustomFunctionError(cfeSymbolUndefined, 0, Name);
}
//---------------------------------------------------------------------------
const TCustomFunction& TCustomFunctions::GetValue(const std::wstring &Name) const
{
  for(TConstIterator Iter = Functions.begin(); Iter != Functions.end(); ++Iter)
    if(Iter->Name == Name)
      return *Iter;
  throw ECustomFunctionError(cfeSymbolUndefined, 0, Name);
}
//---------------------------------------------------------------------------
void TCustomFunctions::UpdateAll(bool IgnoreErrors)
{
  unsigned I;
  for(I = 0; I < Functions.size(); I++)
    try
    {
      TCustomFunction &Function = Functions[I];
      std::wstring Expression = Function.Text.substr(0, Function.Text.find(L"#"));
      Function.InternalFunc->SetFunc(Expression, Function.Arguments, InternalSymbolList);
      SymbolList.Add(Function.Name, Function.InternalFunc);
    }
    catch(const Func32::EParseError &E)
    {
      if(IgnoreErrors)
        ShowStatusError(GetErrorMsg(E));
      else
        throw ECustomFunctionError(cfeParseError, I, E);
    }
}
//---------------------------------------------------------------------------
void TCustomFunctions::Update(const TData &Data)
{
  //Replace all constant expressions with their value
  std::vector<Func32::TComplex> DummyArgs;
  for(unsigned I = 0; I < Functions.size(); I++)
  {
    TCustomFunction &Function = Functions[I];
    if(Function.Arguments.size() == 0)
      try
      {
        Function.InternalFunc->SetTrigonometry(Data.Axes.Trigonometry);
        Func32::TComplex Value = Function.InternalFunc->Calc(DummyArgs);
        if(Function.Func)
          Function.Func->SetFunc(Value);
        else
          Function.Func = SymbolList.Add(Function.Name, Value);
      }
      catch(Func32::ECalcError &E)
      {
        if(Function.Func)
          *Function.Func = *Function.InternalFunc;
        SymbolList.Add(Function.Name, Function.InternalFunc);
      }
  }
}
//---------------------------------------------------------------------------
void TCustomFunctions::Swap(TCustomFunctions &Other)
{
  //Do not swap GlobalSymbolList. It must stay in the global TData
  Functions.swap(Other.Functions);
  SymbolList.Swap(Other.SymbolList);
  InternalSymbolList.Swap(Other.InternalSymbolList);
}
//---------------------------------------------------------------------------
void TCustomFunctions::WriteToIni(TConfigFileSection &Section) const
{
  for(TConstIterator Iter = Functions.begin(); Iter != Functions.end(); ++Iter)
    Section.Write(Iter->GetName(), Iter->Text);
}
//---------------------------------------------------------------------------
void TCustomFunctions::ReadFromIni(const TConfigFileSection &Section)
{
  if(!Section.Empty())
  {
    for(TConfigFileSection::TIterator Iter = Section.Begin(); Iter != Section.End(); ++Iter)
      Add(Iter->first, Iter->second);
    UpdateAll(true);
  }
}
//---------------------------------------------------------------------------
void TCustomFunctions::Clear()
{
  Functions.clear();
  InternalSymbolList = GlobalSymbolList;
  SymbolList = GlobalSymbolList;
}
//---------------------------------------------------------------------------
void TCustomFunctions::Reset()
{
  SymbolList = InternalSymbolList;
  for(unsigned I = 0; I < Functions.size(); I++)
    Functions[I].Func.reset();
}
//---------------------------------------------------------------------------
//////////////////
// TDefaultData //
//////////////////
std::wstring TDefaultData::ToString() const
{
  return ToWString(Style) + L"," + ToWString(ColorToString(Color)) + L"," + ToWString(Size);
}
//---------------------------------------------------------------------------
void TDefaultData::FromString(const std::wstring &Str, unsigned AStyle, TColor AColor, unsigned ASize)
{
  Style = AStyle;
  Color = AColor;
  Size = ASize;

  if(Str.empty())
    return;

  try
  {
    size_t Pos = Str.find(',');
    Style = ToIntDef(Str.substr(0, Pos), AStyle);
    size_t Pos2 = Str.find(',', Pos + 1);
    Color = StringToColor(Str.substr(Pos + 1, Pos2 - Pos - 1).c_str());
    Size = ToIntDef(Str.substr(Pos2 + 1), ASize);
  }
  catch(...)
  {
  }
}
//---------------------------------------------------------------------------
void TDefaultData::Set(unsigned AStyle, TColor AColor, unsigned ASize)
{
  Style = AStyle;
  Color = AColor;
  Size = ASize;
}
//---------------------------------------------------------------------------
////////////////////
// TAnimationInfo //
////////////////////
void TAnimationInfo::WriteToIni(TConfigFileSection &Section) const
{
  if(!Constant.empty())
  {
    Section.Write(L"Constant", Constant);
    Section.Write(L"FramesPerSecond", FramesPerSecond);
    Section.Write(L"Width", Width, 0U);
    Section.Write(L"Height", Height, 0U);
  }
  for(std::map<std::wstring, TAnimationConstant>::const_iterator Iter = ConstantList.begin(); Iter != ConstantList.end(); ++Iter)
    Section.Write(L'%' + Iter->first, Iter->second.Min + L';' + Iter->second.Max + L';' + Iter->second.Step);
}
//---------------------------------------------------------------------------
void TAnimationInfo::ReadFromIni(const TConfigFileSection &Section)
{
  if(!Section.Empty())
  {
    Constant = Section.Read(L"Constant", L"");
    FramesPerSecond = Section.Read(L"FramesPerSecond", 1);
    Width = Section.Read(L"Width", 0);
    Height = Section.Read(L"Height", 0);
    for(TConfigFileSection::TIterator Iter = Section.Begin(); Iter != Section.End(); ++Iter)
      if(Iter->first[0] == '%')
      {
        TAnimationConstant &AnimationConstant = ConstantList[Iter->first.substr(1)];
        int Pos = Iter->second.find(';');
        AnimationConstant.Min = Iter->second.substr(0, Pos);
        int Pos2 = Iter->second.find(';', Pos+1);
        AnimationConstant.Max = Iter->second.substr(Pos+1, Pos2 - Pos - 1);
        AnimationConstant.Step = Iter->second.substr(Pos2+1);
      }
  }
}
//---------------------------------------------------------------------------
void TAnimationInfo::Clear()
{
  Constant.clear();
  ConstantList.clear();
}
//---------------------------------------------------------------------------
/////////////////////
// TGuiFormatSettings //
/////////////////////
TGuiFormatSettings::TGuiFormatSettings()
  : CartesianPointFormat(L"(%1%,%2%)"),
	DegreePointFormat(L"(%2%,%1%\xB0)"),
    RadianPointFormat(L"(%2%,%1%)")
{
}
//---------------------------------------------------------------------------
/////////////////////
// TFormatSettings //
/////////////////////
TPlotSettings::TPlotSettings()
  : AxisWidth(2), GridWidth(1), xNumberDist(4), yNumberDist(7), TickWidth(1),
    TickLength(5)
{
}
//---------------------------------------------------------------------------
//////////////////
// TGuiSettings //
//////////////////
TGuiSettings::TGuiSettings()
	: MajorZoomIn(0.5), MinorZoomIn(0.9), MajorZoomOut(2), MinorZoomOut(20.0/18.0),
    MajorStepSize(0.1), MinorStepSize(0.01), MouseZoomIn(0.9), MouseZoomOut(1/0.9)
{
}
//---------------------------------------------------------------------------
} //namespace Graph
