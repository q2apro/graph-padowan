//===========================================================================
// Copyright © 2005 Ivan Johansen
// SaveDialogEx.cpp
//===========================================================================
#include "Config.h"
#pragma hdrstop
#include "SaveDialogEx.h"
#include "Dlgs.h"
#include <vector>
#include "Tokenizer.h"
#include "ICompCommon.h"
#pragma package(smart_init)
#pragma link "Tokenizer"
//---------------------------------------------------------------------------
static TRegisterClass Dummy(__classid(TSaveDialogEx));
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//
static inline void ValidCtrCheck(TSaveDialogEx *)
{
  new TSaveDialogEx(NULL);
}
//---------------------------------------------------------------------------
namespace Savedialogex
{
  void __fastcall PACKAGE Register()
  {
     TComponentClass classes[1] = {__classid(TSaveDialogEx)};
     RegisterComponents("IComp", classes, 0);
  }
}
//---------------------------------------------------------------------------
__fastcall TSaveDialogEx::TSaveDialogEx(TComponent* Owner)
  : TSaveDialog(Owner), FOnHelp(NULL), FAutoExt(true)
{
}
//---------------------------------------------------------------------------
void __fastcall TSaveDialogEx::WndProc(TMessage &Message)
{
  if(Message.Msg == WM_NOTIFY && reinterpret_cast<POFNotify>(Message.LParam)->hdr.code == CDN_HELP)
    if(FOnHelp)
      FOnHelp(this);
  TSaveDialog::WndProc(Message);
}
//---------------------------------------------------------------------------
void __fastcall TSaveDialogEx::DoShow()
{
  if(!HelpCaption.IsEmpty())
    SendMessage(GetParent(Handle), CDM_SETCONTROLTEXT, pshHelp, reinterpret_cast<long>(HelpCaption.c_str()));
}
//---------------------------------------------------------------------------
void __fastcall TSaveDialogEx::SetFileName(const String &AFileName)
{
  if(Handle == NULL)
    inherited::FileName = AFileName;
  else
    SendMessage(GetParent(Handle), CDM_SETCONTROLTEXT, 1152, reinterpret_cast<DWORD>(AFileName.c_str()));
}
//---------------------------------------------------------------------------
String __fastcall TSaveDialogEx::GetFileName()
{
  if(Handle == NULL)
    return inherited::FileName;
  else
  {
    int Length = SendMessage(GetParent(Handle), CDM_GETSPEC, 0, 0);
    if(Length == 0)
      return "";
    std::wstring FileName(Length, 0);
    SendMessage(GetParent(Handle), CDM_GETSPEC, Length, reinterpret_cast<DWORD>(&FileName[0]));
    return &FileName[0];
  }
}
//---------------------------------------------------------------------------
void __fastcall TSaveDialogEx::DoTypeChange()
{
  if(FAutoExt && !FileName.IsEmpty())
  {
    TTokenizer Tokenizer(Filter.c_str(), L'|');
    Tokenizer.Ignore(FilterIndex*2-1);
    String Extension = Tokenizer.Next().c_str();
    Extension = Extension.SubString(Extension.LastDelimiter("."), MaxInt);
    FileName = ChangeFileExt(FileName, Extension);
  }
}
//---------------------------------------------------------------------------








