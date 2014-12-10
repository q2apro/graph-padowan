//===========================================================================
// Copyright © 2009 Ivan Johansen
// SymbolDialog.cpp
//===========================================================================
#include <vcl.h>
#pragma hdrstop
#include "SymbolDialog.h"
#include "SymbolForm.h"
#include <consts.hpp>
#include "ICompCommon.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
static TRegisterClass Dummy(__classid(TSymbolDialog));
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//
static inline void ValidCtrCheck(TSymbolDialog *)
{
  new TSymbolDialog(NULL);
}
//---------------------------------------------------------------------------
namespace Symboldialog
{
  void __fastcall PACKAGE Register()
  {
     TComponentClass classes[1] = {__classid(TSymbolDialog)};
     RegisterComponents("IComp", classes, 0);
  }
}
//---------------------------------------------------------------------------
__fastcall TSymbolDialog::TSymbolDialog(TComponent* Owner)
  : TComponent(Owner), FFontName("Symbol"), FCharacterSet(csAnsiSet),
    FOnShow(NULL), FOnClose(NULL), FOnInsertAnsiChar(NULL), FOnInsertWideChar(NULL), FSymbolFrm(NULL), FSymbol(0x20)
{
}
//---------------------------------------------------------------------------
__fastcall TSymbolDialog::~TSymbolDialog()
{
  Close();
}
//---------------------------------------------------------------------------
void TSymbolDialog::Execute()
{
  if(FSymbolFrm == NULL)
  {
    FSymbolFrm = new TSymbolFrm(NULL, CharacterSet == csUnicodeSet && Win32Platform == VER_PLATFORM_WIN32_NT, FSymbol);
    DoShow();
  }
  FSymbolFrm->ShowModal();
}
//---------------------------------------------------------------------------
void TSymbolDialog::Show()
{
  if(FSymbolFrm == NULL)
  {
    FSymbolFrm = new TSymbolFrm(NULL, CharacterSet == csUnicodeSet && Win32Platform == VER_PLATFORM_WIN32_NT, FSymbol);
    DoShow();
    FSymbolFrm->Show();
  }
}
//---------------------------------------------------------------------------
void TSymbolDialog::Close()
{
  if(FSymbolFrm)
    FSymbolFrm->Close();
}
//---------------------------------------------------------------------------
void __fastcall TSymbolDialog::Button2Click(TObject *Sender)
{
  if(CharacterSet == csUnicodeSet && Win32Platform == VER_PLATFORM_WIN32_NT)
  {
    if(OnInsertWideChar)
      OnInsertWideChar(this, FSymbolFrm->GetSelected(), FSymbolFrm->ComboBox1->Text);
  }
  else
  {
    if(OnInsertAnsiChar)
      OnInsertAnsiChar(this, FSymbolFrm->GetSelected(), FSymbolFrm->ComboBox1->Text);
  }
}
//---------------------------------------------------------------------------
void __fastcall TSymbolDialog::FormClose(TObject *Sender, TCloseAction &Action)
{
  FFontName = FSymbolFrm->ComboBox1->Text;
  FSymbol = FSymbolFrm->GetSelected();
  if(OnClose)
    OnClose(this);
  FSymbolFrm = NULL;
  Action = caFree;
}
//---------------------------------------------------------------------------
void TSymbolDialog::DoShow()
{
  FSymbolFrm->Button2->OnClick = Button2Click;
  FSymbolFrm->OnClose = FormClose;
  FSymbolFrm->ComboBox1->ItemIndex = FSymbolFrm->ComboBox1->Items->IndexOf(FontName);
  if(OnShow)
    OnShow(this);
}
//---------------------------------------------------------------------------

