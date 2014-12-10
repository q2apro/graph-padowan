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
#include "Unit1.h"
#include "Convert.h"
#include "Grid.h"
#include "GuiUtil.h"
#include <boost/spirit/phoenix/operators.hpp>
#include <boost/spirit/phoenix/primitives.hpp>
using phoenix::arg1;
//---------------------------------------------------------------------------
double MakeFloat(TCustomEdit *Edit, const String &ErrorStr, const boost::function1<bool, double> &Interval)
{
  String Text = Edit->Text;
  if(Text.IsEmpty())
  {
    SetGlobalFocus(Edit);
    MessageBox(ErrorStr.IsEmpty() ? LoadRes(RES_VALUE_NEEDED) : ErrorStr, LoadRes(RES_ERROR));
    throw EAbort("");
  }

  try
  {
    double Number = Form1->Data.Calc(ToWString(Text));

    if(!boost::math::isfinite(Number))
    {
      SetGlobalFocus(Edit);
      MessageBox(LoadRes(RES_INVALID_USE_OF_INF), LoadRes(RES_ERROR_IN_VALUE));
      throw EAbort("");
    }

    if(!Interval(Number))
    {
      SetGlobalFocus(Edit);
      if(ErrorStr.IsEmpty())
        MessageBox("This error should not occur. Please report this to the programmer.", LoadRes(RES_ERROR), MB_ICONSTOP);
      else
        MessageBox(ErrorStr, LoadRes(RES_ERROR_IN_VALUE));
      throw EAbort("");
    }
    return Number;
  }
  catch(Func32::EFuncError &Error)
  {
    SetGlobalFocus(Edit);
    ShowErrorMsg(Error, Edit);
    throw EAbort("");
  }
}
//---------------------------------------------------------------------------
double MakeFloat(TCustomEdit *Edit, const String &ErrorStr, double Min, double Max)
{
  return MakeFloat(Edit, ErrorStr, arg1 >= Min && arg1 <= Max);
}
//---------------------------------------------------------------------------
int MakeInt(TCustomEdit *Edit, const String &Caption)
{
  String Text = Edit->Text;
  if(Text.IsEmpty())
  {
    MessageBox(LoadRes(RES_VALUE_NEEDED), LoadRes(RES_ERROR));
    Edit->SetFocus();
    throw EAbort("");
  }

  try
  {
    double Number = Form1->Data.Calc(Text.c_str());
    int Value = Number;
    if(Number <= 0 || static_cast<double>(Value) != Number)
    {
      MessageBox(LoadRes(RES_INT_GREATER_ZERO, Caption), LoadRes(RES_ERROR));
      SetGlobalFocus(Edit);
      throw EAbort("");
    }
    return Number;
  }
  catch(Func32::EFuncError &Error)
  {
    SetGlobalFocus(Edit);
    ShowErrorMsg(Error, Edit);
    throw EAbort("");
  }
}
//---------------------------------------------------------------------------
String ComplexToString(const Func32::TComplex &C, unsigned Decimals, TComplexFormat ComplexFormat)
{
  String Str;

  switch(ComplexFormat)
  {
    case cfReal:
      //Check for an imaginary part
      if(std::abs(C.imag()) < MIN_ZERO)
        Str += RoundToStr(C.real(), Decimals);
      break;

    case cfRectangular:
    {
      //Round numbers to chosen number of decimals
      String Real = RoundToStr(C.real(), Decimals);
      String Imag = RoundToStr(std::abs(C.imag()), Decimals);

      if(Imag == "0" || std::abs(C.imag()) < MIN_ZERO)  //(-1.50)^2 = 2.25+2.4395E-19i
        Str += Real;
      else //If there is a imaginary part
      {
        if(Real != "0")
          Str += Real + (C.imag() < 0 ? '-' : '+');
        //Add imaginary part to string
        if(Imag != "1")
          Str += Imag;
        Str += "i";
      }
      break;
    }
    case cfPolar: //Polar format
      //The complex number 0 does not have an angle
      if(C.real() == 0 && C.imag() == 0)
      {
        Str += "0";
        break;
      }

      Str += RoundToStr(abs(C), Decimals); //Get numeric value as a string
      //Add angle symbol to text in Symbol font
      Str += L'\x2220';
      const TAxes Axes = Form1->Data.Axes;
      if(Axes.Trigonometry == Func32::Degree)
        //Add degree symbol, if angle is in degree
        Str += RoundToStr(std::arg(C) * 180 / M_PI, Decimals) + L'\xB0';
      else
        Str += RoundToStr(arg(C), Decimals);
  }

  return Str;
}
//---------------------------------------------------------------------------
String ComplexToString(const Func32::TComplex &C)
{
  return ComplexToString(C, Property.RoundTo, Property.ComplexFormat);
}
//---------------------------------------------------------------------------
//This function converts a floating point number to a string
//The number of decimals will be determined by Property.RoundTo
String DoubleToStr(long double Number)
{
  return DoubleToStr(Number, Property.RoundTo);
}
//---------------------------------------------------------------------------
//This function converts a floating point number to a string
//Decimals indicate the fixed number of decimals in the string
String DoubleToStr(long double Number,int Decimals)
{
  if(std::abs(Number) < MIN_ZERO)
    return 0;
  if(Number >= 10000 || Number <= -10000)
    return FloatToStrF(Number,ffExponent,Decimals,9999);
  return FloatToStrF(Number,ffFixed,9999,Decimals);
}
//---------------------------------------------------------------------------
//This function returns the number of decimals in a number
int GetDecimals(double Number)
{
  double Dummy;
  int Decimals;
  for(Decimals = 0; std::modf(Number,&Dummy); Decimals++)
    Number *= 10;
  return Decimals;
}
//---------------------------------------------------------------------------
//Coverts a TFont objet to a String
std::wstring FontToStr(TFont *Font)
{
  //String with Name,Size,Color
  std::wstring Str = (Font->Name + "," + Font->Size + "," + ColorToString(Font->Color)).c_str();

  if(!Font->Style.Empty())
    Str += L",";

  //Add B, I or U acording to the font style
  if(Font->Style.Contains(fsBold))
    Str += L"B";
  if(Font->Style.Contains(fsItalic))
    Str += L"I";
  if(Font->Style.Contains(fsUnderline))
    Str += L"U";
  return Str;
}
//---------------------------------------------------------------------------
//Converts a String into a TFont object
//Format: Name,Size,Color,Style
void StrToFont(const std::wstring &Str, TFont *Font)
{
  if(Str.empty())
    return;

  size_t Pos = Str.find(L',');
  //Get font name from string
  Font->Name = Str.substr(0, Pos).c_str();

  size_t Pos2 = Str.find(L',', Pos+1);
  //Get font size from string
  Font->Size = ToIntDef(Str.substr(Pos+1, Pos2-Pos-1), 10);

  size_t Pos3 = Str.find(L',', Pos2+1);
  //Get font color from string
  Font->Color = StringToColor(Str.substr(Pos2+1, Pos3-Pos2-1).c_str());
  Font->Style = Font->Style.Clear(); //Clear font style

  //Loop through the rest of the characters in string
  if(Pos3 != std::wstring::npos)
    for(unsigned N = Pos3+1; N < Str.size(); N++)
      switch(Str[N])
      {
        case L',': return; //Return if a ',' is found
        case L'B': //Add Bold to font style
        case L'b': Font->Style = Font->Style << fsBold; break;
        case L'I': //Add Italic to font style
        case L'i': Font->Style = Font->Style << fsItalic; break;
        case L'U': //Add Underlined to font style
        case L'u': Font->Style = Font->Style << fsUnderline;
      }

  //If font does not exists
  if(Screen->Fonts->IndexOf(Font->Name) == -1 && Font->Name.CompareIC(DEFAULT_FONT) != 0)
  {
    Form1->ShowStatusError(LoadRes(RES_UNKNOWN_FONT, Font->Name));
    Font->Name = DEFAULT_FONT;
  }
}
//---------------------------------------------------------------------------
double ToDouble(const String &Str)
{
  return Str.ToDouble();
}
//---------------------------------------------------------------------------
double ToDouble(const std::wstring &Str)
{
  return ToUString(Str).ToDouble();
}
//---------------------------------------------------------------------------
bool CheckLimit(TWinControl *Control, String Str, int Min, int Max)
{
  bool Error = false;
  try
  {
    int Value = GetControlText(Control).ToInt();
    if(Value < Min || Value > Max)
      Error = true;
  }
  catch(...)
  {
    Error = true;
  }
  if(Error)
  {
    MessageBox(Str, LoadRes(RES_ERROR_IN_VALUE));
    Control->SetFocus();
    return false;
  }
  return true;
}
//---------------------------------------------------------------------------
String RoundToStr(long double Number, unsigned Decimals)
{
  if(boost::math::isnan(Number))
    return "NAN";
  if(!boost::math::isfinite(Number))
    return "INF";
  if(std::abs(Number) < MIN_ZERO)
    return 0;

  String Str;
  if(std::abs(Number) >= 10000 || std::abs(Number) <= 1E-4)
  {
    Str = FloatToStrF(Number, ffExponent, Decimals + 1, 0);
    int N = Str.Pos('E');
    while(Str[N-1] == '0' && Str[N-2] != '.')
      Str.Delete(--N, 1);
    return Str.c_str();
  }
  Str = FloatToStrF(Number,ffFixed, 9999, Decimals);

  //Returns if no decimal separator found
  if(!Str.Pos(FormatSettings.DecimalSeparator))
    return Str.c_str();

  //Remove trailing zeros
  while(Str[Str.Length()] == '0')
    Str.Delete(Str.Length(), 1);
  if(Str[Str.Length()] == FormatSettings.DecimalSeparator)
    Str.Delete(Str.Length(), 1);
  return Str;
}
//---------------------------------------------------------------------------
String RoundToStr(long double Number)
{
  return RoundToStr(Number, Property.RoundTo);
}
//---------------------------------------------------------------------------
double CellToDouble(TGrid *Grid, int Col, int Row)
{
  try
  {
    //First try to convert as a number as optimization. If it fails parse using Calc()
    double Result;
    String Str = Grid->Cells[Col][Row];
    if(Str.Pos("e") == -1 && TryStrToFloat(Str, Result))
      return Result;
    return Form1->Data.Calc(ToWString(Str));
  }
  catch(Func32::EParseError &E)
  {
    Grid->SetFocus();
    Grid->Col = Col;
    Grid->Row = Row;
    Grid->SetCursorPos(E.ErrorPos);
    ShowErrorMsg(E);
    throw EAbort("");
  }
  catch(Func32::EFuncError &E)
  {
    return NAN;
  }
}
//---------------------------------------------------------------------------
//Converts a float number to a fraction. F.ex: 0.66666666 = 2/3
std::pair<int, int> FloatToFract(double f, double Epsilon)
{
  double t = f;
  while(std::abs(t - static_cast<int>(t + (t < 0 ? -0.5 : 0.5))) > Epsilon)
    t += f;
  int n = t + (t < 0 ? -0.5 : 0.5);
  int d = n/f + (n/f < 0 ? -0.5 : 0.5);
  return std::pair<int, int>(n, d);
}
//---------------------------------------------------------------------------
std::wstring RtfToPlainText(const std::string &Str)
{
  //We need a parent window; just use main form
  std::auto_ptr<TIRichEdit> RichEdit(new TIRichEdit(Application->MainForm));
  RichEdit->Visible = false;
  RichEdit->Parent = Application->MainForm;
  RichEdit->WrapType = wtNone;
  RichEdit->SetRichText(Str.c_str());
  String Text = RichEdit->GetPlainText();
  String Result;
  for(int I = 1; I <= Text.Length(); I++)
    if(Text[I] == '\n')
      Result += ' ';
    else if(Text[I] != '\r' && Text[I] != '\v')
      Result += Text[I];
  return ToWString(Result.Trim());
}
//---------------------------------------------------------------------------
std::string ToString(int Value)
{
  return ToString(AnsiString(Value));
}
//---------------------------------------------------------------------------
std::string ToString(unsigned Value)
{
  return ToString(AnsiString(Value));
}
//---------------------------------------------------------------------------
std::string ToString(long double Value)
{
  return ToString(FloatToStr(Value));
}
//---------------------------------------------------------------------------
std::string ToString(const std::wstring &Str)
{
  AnsiString Str2(Str.c_str(), Str.size());
  return std::string(Str2.c_str(), Str2.Length());
}
//---------------------------------------------------------------------------
std::string ToString(const String &Str)
{
  AnsiString Str2(Str);
  return std::string(Str2.c_str(), Str2.Length());
}
//---------------------------------------------------------------------------
std::string ToString(const wchar_t *Str)
{
  AnsiString Str2(Str);
  return std::string(Str2.c_str(), Str2.Length());
}
//---------------------------------------------------------------------------
std::wstring ToWString(int Value)
{
  return ToWString(String(Value));
}
//---------------------------------------------------------------------------
std::wstring ToWString(unsigned Value)
{
  return ToWString(String(Value));
}
//---------------------------------------------------------------------------
std::wstring ToWString(long double Value)
{
  return ToWString(FloatToStr(Value));
}
//---------------------------------------------------------------------------
std::wstring ToWString(const std::string &Str)
{
  String Str2(Str.c_str(), Str.size());
  return std::wstring(Str2.c_str(), Str2.Length());
}
//--------------------------------------------------------------------------
std::wstring ToWString(const String &Str)
{
  return std::wstring(Str.c_str(), Str.Length());
}
//--------------------------------------------------------------------------
std::wstring ToWString(const char *Str)
{
  String Str2(Str);
  return std::wstring(Str2.c_str(), Str2.Length());
}
//--------------------------------------------------------------------------
String ToUString(const std::wstring &Str)
{
  return String(Str.c_str(), Str.size());
}
//--------------------------------------------------------------------------
String ToUString(const std::string &Str)
{
  return String(Str.c_str(), Str.size());
}
//--------------------------------------------------------------------------
String ToUString(long double Value)
{
  return FloatToStr(Value);
}
//---------------------------------------------------------------------------
std::wstring ToLower(const std::wstring &Str)
{
  std::wstring Result;
  for(unsigned I = 0; I < Str.size(); I++)
    Result += std::tolower(Str[I]);
  return Result;
}
//---------------------------------------------------------------------------
std::pair<long double,long double> GetPolarCoord(const Func32::TDblPoint &P, Func32::TTrigonometry Trig)
{
  if(P.x == 0 && P.y == 0)
    return std::make_pair(0.0L, 0.0L);
  long double r = std::sqrt(P.x*P.x + P.y*P.y);
  long double a = std::atan2(P.y, P.x);
  if(a < 0)
    a += 2*M_PI;
  if(Trig == Func32::Degree)
    a *= 180 / M_PI;
  return std::make_pair(r, a);
}
//---------------------------------------------------------------------------
size_t FindEndPar(const std::string &Str, unsigned Pos)
{
  unsigned Count = 0;
  for(size_t I = Pos; I <= Str.size(); I++)
    if(Str[I] == '(')
      Count++;
    else if(Str[I] == ')')
      if(--Count == 0)
        return I;
  return std::string::npos;
}
//---------------------------------------------------------------------------
size_t FindEndPar(const std::wstring &Str, unsigned Pos)
{
  unsigned Count = 0;
  for(size_t I = Pos; I <= Str.size(); I++)
    if(Str[I] == '(')
      Count++;
    else if(Str[I] == ')')
      if(--Count == 0)
        return I;
  return std::wstring::npos;
}
//---------------------------------------------------------------------------



