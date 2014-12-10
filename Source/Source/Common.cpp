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
#include "ConfigRegistry.h"
#include <complex>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include "VersionInfo.h"
#include "ConfigFile.h"
#include <ComObj.hpp>

//Do not use std::numeric_limits<double>::quiet_NaN(), which makes the program crash
const double NAN = 0.0/0.0;
const double INF = 1.0/0.0;
//---------------------------------------------------------------------------
//Centers a form on the working area
void CenterForm(TForm *Form)
{
  TRect WorkArea;
  if(!SystemParametersInfo(SPI_GETWORKAREA,0,&WorkArea,0))
    return;
  Form->Left=WorkArea.Left+(WorkArea.Width()-Form->Width)/2;
  Form->Top=WorkArea.Top+(WorkArea.Height()-Form->Height)/2;
}
//---------------------------------------------------------------------------
//This function makes an association between a file type and a program
//Ext:          The file type. Ex. ".doc"
//ProgramName:  The file path and name of the program file. Ex. "D:\\WINWORD.EXE"
//              If ProgramName is empty when the applications name is used.
//Ident:        An identifier used to create the asociation. Ex. "docfile"
//Description:  A description of the file type
//Icon:         Path and name of icon file and the number of icon in the file
//              Ex. "C:\\WINWORD.EXE,0"
void AssociateExt(std::wstring Ext, std::wstring ProgramName, std::wstring Ident, std::wstring Description, std::wstring Icon, bool AllUsers)
{
  try
  {
    //Make sure there is a dot before the extention
    if(Ext[0] != L'.')
      Ext = L'.' + Ext;
    //If no program name specified then use the application name
    if(ProgramName.empty())
      ProgramName = ToWString(Application->ExeName);
    //If no identifier when use the exe name without extention
    if(Ident.empty())
      Ident = ToWString(Application->ExeName.SubString(1, Application->ExeName.Pos(".")));
    //If no icon when use the icon from ProgramName
    if(Icon.empty())
      Icon = ProgramName + L",0";
    //Change ProgramName to format: "Name.ext" "%1"
    ProgramName = L"\"" + ProgramName + L"\" \"%1\"";

    HKEY RootKey = AllUsers ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER;

    RemoveRegistryKey(L"Software\\Classes\\" + Ext, HKEY_CURRENT_USER);
    SetRegValue(L"Software\\Classes\\" + Ext, L"", RootKey, Ident);
    SetRegValue(L"Software\\Classes\\" + Ident, L"", RootKey, Description);
    SetRegValue(L"Software\\Classes\\" + Ident + L"\\DefaultIcon", L"", RootKey, Icon);
    SetRegValue(L"Software\\Classes\\" + Ident + L"\\shell\\open\\command", L"", RootKey, ProgramName);

    //Tell the shell that a file association has been changed
    SHChangeNotify(SHCNE_ASSOCCHANGED, 0, NULL, NULL);
  }
  catch(...)
  {
  }
}
//---------------------------------------------------------------------------
//This function removes an association between a file type and a program
//Ext is the file type. Ex. ".doc"
//Ident is the identifier used when the association was made. Ex. "docfile"
void RemoveAsociation(const std::wstring &Ext, const std::wstring &Ident, bool RemoveIdent)
{
  //Make sure there is a dot before the extention
  if(GetRegValue(Ext[0] != L'.' ? L'.' + Ext : Ext, L"", HKEY_CLASSES_ROOT, L"") == Ident)
    RemoveRegistryKey(Ext[0] != L'.' ? L'.' + Ext : Ext, HKEY_CLASSES_ROOT);

  if(RemoveIdent)
    RemoveRegistryKey(Ident, HKEY_CLASSES_ROOT);

  //Tell the shell that a file association has been changed
  SHChangeNotify(SHCNE_ASSOCCHANGED, 0, NULL, NULL);
}
//---------------------------------------------------------------------------
//This function checks if a file type is associated with a program
//Ext:   The file extention. Ex. ".doc"
//Ident: The identifier used when the asocation was made. Ex. "docfile"
bool CheckAssocation(const std::wstring &Ext, const std::wstring &Ident)
{
  return GetRegValue(Ext, L"", HKEY_CLASSES_ROOT, L"") == Ident;
}
//---------------------------------------------------------------------------
String GetErrorMsg(const Func32::EFuncError &Error)
{
  String ErrorStr = LoadRes(Error.ErrorCode+100, ToUString(Error.Str));
  if(ErrorStr.IsEmpty())
    ErrorStr = "Unknown error code.\nCould not find error message for this error.";
  return ErrorStr;
}
//---------------------------------------------------------------------------
String GetErrorMsg(Func32::TErrorCode ErrorCode)
{
  return GetErrorMsg(Func32::EFuncError(ErrorCode));
}
//---------------------------------------------------------------------------
//Draws a curved line between a series of points
void DrawCurve(TCanvas *Canvas, const std::vector<TPoint> &Points)
{
  std::vector<TPoint> List;
  List.reserve(Points.size()*3+1);
  List.push_back(Points.front());
  for(unsigned I = 1; I < Points.size()-2; I++)
  {
    List.push_back(Points[I]);
    List.push_back(Points[I]);
    List.push_back(TPoint((Points[I].x + Points[I+1].x)/2, (Points[I].y + Points[I+1].y)/2));
  }
  List.push_back(Points[Points.size()-2]);
  List.push_back(Points[Points.size()-2]);
  List.push_back(Points.back());
  Canvas->PolyBezier(&List[0], List.size()-1);
}
//---------------------------------------------------------------------------
void InvertBitmap(Graphics::TBitmap *Bitmap)
{
  InvertRect(Bitmap->Canvas->Handle, &TRect(0, 0, Bitmap->Width, Bitmap->Height));
}
//---------------------------------------------------------------------------
//Returns number rounded up to nearest a*10^k, where a={1,2,5} and k is an integer
double AdjustUnit(double Number)
{
  double Exponent = std::floor(std::log10(Number));
  double Factor = std::pow10(Exponent);

  //Normalise; Number = [1;10[
  Number /= Factor;
  if(Number > 5)
    Number = 10;
  else if(Number > 2)
    Number = 5;
  else
    Number = 2;
  return Number * Factor;
}
//---------------------------------------------------------------------------
//Set focus to the control, even though the parent doesn't have focus
void SetGlobalFocus(TWinControl *Control)
{
  if(Control == NULL || dynamic_cast<TForm*>(Control))
    return;

  if(TTabSheet *TabSheet = dynamic_cast<TTabSheet*>(Control))
    TabSheet->PageControl->ActivePage = TabSheet;

  SetGlobalFocus(Control->Parent);
  if(Control->CanFocus())
    Control->SetFocus();
}
//---------------------------------------------------------------------------
template<typename T1, typename T2>
LessFirstPair(const std::pair<T1, T2> &Pair1, const std::pair<T1, T2> &Pair2)
{
  return Pair1.first < Pair2.first;
}
//---------------------------------------------------------------------------
//List is filled with langugae names, which are the same as the file names
void GetLanguageList(TStrings *List)
{
  std::auto_ptr<TStringList> TempList(new TStringList);
  TempList->Sorted = true;
  TempList->Add("English"); //We always have English

  TSearchRec SearchRec;
  String Path = ExtractFilePath(Application->ExeName) + "locale\\";
  int Result = FindFirst(Path + "*.mo", faReadOnly | faArchive, SearchRec);
  while(Result == 0)
  {
    TempList->Add(SearchRec.Name.SubString(1, SearchRec.Name.Length() - 3));
    Result = FindNext(SearchRec);
  }
  FindClose(SearchRec);
  List->Assign(TempList.get());
}
//---------------------------------------------------------------------------
//WARNING don't inline this function. It gives problems in release mode
std::wistream& operator>>(std::wistream &Stream, TColor &Color)
{
  std::wstring Str;
  Stream >> Str;
  Color = StringToColor(Str.c_str());
  return Stream;
}
//---------------------------------------------------------------------------
std::wstring ReduceString(const std::wstring &Str, unsigned MaxLength)
{
  if(Str.size() > MaxLength)
    return Str.substr(0, MaxLength-4) + L" ...";
  return Str;
}
//---------------------------------------------------------------------------
TMaxWidth::TMaxWidth(TControl *Control)
{
  TCanvas *Canvas = Application->MainForm->Canvas;
  if(TCustomLabel *Label = dynamic_cast<TCustomLabel*>(Control))
    Canvas = Label->Canvas;
  else if(TForm *Form = dynamic_cast<TForm*>(Control->Owner))
    Canvas = Form->Canvas;
  Value = Canvas->TextWidth(GetControlText(Control));
}
//---------------------------------------------------------------------------
String GetTempPath()
{
  unsigned Length = GetTempPathW(0, NULL);
  std::vector<wchar_t> Result(Length);
  Win32Check(GetTempPathW(Length, &Result[0]));
  return &Result[0];
}
//---------------------------------------------------------------------------
String GetTempFileName(const String &Prefix, const String &Ext)
{
  String FileName;
  int Count = 0;
  do
  {
    FileName = GetTempPath() + Prefix + Count + "." + Ext;
    Count++;
  }
  while(FileExists(FileName));

  return FileName;
}
//---------------------------------------------------------------------------
void LoadLanguage(const String &Lang)
{
  String Path = ExtractFilePath(Application->ExeName);
  UseLanguage(Lang); //dxGetText will not update translation unless language has been changed
  DefaultInstance->bindtextdomainToFile("default", Path + "locale\\" + Lang + ".mo");
	String HelpFile = DefaultInstance->GetTranslationProperty("Help-file");
	if(HelpFile.IsEmpty() )
		HelpFile = Path + "Help\\Graph-" + Lang + ".chm";
	else
	  HelpFile = Path + "Help\\" + HelpFile;
	if(FileExists(HelpFile))
    Application->HelpFile = HelpFile;
  else
    Application->HelpFile = Path + "Help\\Graph-English.chm";
}
//---------------------------------------------------------------------------
struct TTimerClass
{
  TNotifyEvent CallBack;
  void __fastcall OnTimer(TObject *Sender)
  {
    CallBack(Sender);
    delete Sender;
    delete this;
  }
};
void StartTimer(unsigned Time, TNotifyEvent CallBack)
{
  TTimer *Timer = new TTimer(NULL);
  Timer->Interval = Time;
  Timer->Enabled = true;
  TTimerClass *TimerClass = new TTimerClass;
  TimerClass->CallBack = CallBack;
  Timer->OnTimer = TimerClass->OnTimer;
}
//---------------------------------------------------------------------------







