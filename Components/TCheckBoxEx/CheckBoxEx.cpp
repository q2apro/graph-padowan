//===========================================================================
// Copyright © 2009 Ivan Johansen
// CheckBoxEx.cpp
//===========================================================================
#include <vcl.h>
#pragma hdrstop
#include "CheckBoxEx.h"
#include "ICompCommon.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
static TRegisterClass Dummy(__classid(TCheckBoxEx));
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//
static inline void ValidCtrCheck(TCheckBoxEx *)
{
  new TCheckBoxEx(NULL);
}
//---------------------------------------------------------------------------
namespace Checkboxex
{
  void __fastcall PACKAGE Register()
  {
     TComponentClass classes[1] = {__classid(TCheckBoxEx)};
     RegisterComponents(L"IComp", classes, 0);
  }
}
//---------------------------------------------------------------------------
__fastcall TCheckBoxEx::TCheckBoxEx(TComponent* Owner)
  : TCheckBox(Owner), FAutoSize(true)
{
}
//---------------------------------------------------------------------------
void TCheckBoxEx::AdjustBounds()
{
  if(!ComponentState.Contains(csReading) && !ComponentState.Contains(csLoading)&& AutoSize)
  {
    HDC DC = GetDC(0);
    SelectObject(DC, Font->Handle);
    SIZE Size;
    GetTextExtentPoint32(DC, Caption.c_str(), Caption.Length(), &Size);
    ReleaseDC(0, DC);
    int CheckWidth = GetSystemMetrics(SM_CXMENUCHECK);
    int NewWidth = Size.cx + CheckWidth + 6;
    Width = NewWidth;
  }
}
//---------------------------------------------------------------------------
void __fastcall TCheckBoxEx::CMTextChanged(TMessage &Message)
{
  TCheckBox::Dispatch(&Message);
//  Invalidate();
  AdjustBounds();
}
//---------------------------------------------------------------------------
void __fastcall TCheckBoxEx::CMFontChanged(TMessage &Message)
{
  TCheckBox::Dispatch(&Message);
  AdjustBounds();
}
//---------------------------------------------------------------------------
void __fastcall TCheckBoxEx::Loaded()
{
  TCheckBox::Loaded();
  AdjustBounds();
}
//---------------------------------------------------------------------------

