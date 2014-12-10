//===========================================================================
// Copyright © 2009 Ivan Johansen
// RadioGroupEx.cpp
//===========================================================================
#include <vcl.h>
#pragma hdrstop
#include "RadioGroupEx.h"
#include "ICompCommon.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
static TRegisterClass Dummy(__classid(TRadioGroupEx));
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//
static inline void ValidCtrCheck(TRadioGroupEx *)
{
  new TRadioGroupEx(NULL);
}
//---------------------------------------------------------------------------
namespace Radiogroupex
{
  void __fastcall PACKAGE Register()
  {
     TComponentClass classes[1] = {__classid(TRadioGroupEx)};
     RegisterComponents(L"IComp", classes, 0);
  }
}
//---------------------------------------------------------------------------
__fastcall TRadioGroupEx::TRadioGroupEx(TComponent* Owner)
  : TRadioGroup(Owner), FAutoSize(true)
{
  OldItemsChange = static_cast<TStringList*>(Items)->OnChange;
  static_cast<TStringList*>(Items)->OnChange = ItemsChange;
}
//---------------------------------------------------------------------------
void __fastcall TRadioGroupEx::CMTextChanged(TMessage &Message)
{
  TRadioGroup::Dispatch(&Message);
  AdjustBounds();
}
//---------------------------------------------------------------------------
void __fastcall TRadioGroupEx::CMFontChanged(TMessage &Message)
{
  TRadioGroup::Dispatch(&Message);
  AdjustBounds();
}
//---------------------------------------------------------------------------
void __fastcall TRadioGroupEx::Loaded()
{
  TRadioGroup::Loaded();
  AdjustBounds();
}
//---------------------------------------------------------------------------
void TRadioGroupEx::AdjustBounds()
{
  if(!ComponentState.Contains(csReading) && !ComponentState.Contains(csLoading)&& AutoSize)
  {
    int ButtonWidth = GetSystemMetrics(SM_CXMENUCHECK) + 6;
    HDC DC = GetDC(0);
    SelectObject(DC, Font->Handle);
    SIZE Size;
    GetTextExtentPoint32(DC, Caption.c_str(), Caption.Length(), &Size);
    int MaxSize = Size.cx;
    for(int I = 0; I < Items->Count; I++)
    {
      GetTextExtentPoint32(DC, Items->Strings[I].c_str(), Items->Strings[I].Length(), &Size);
      if(Size.cx + ButtonWidth > MaxSize)
        MaxSize = Size.cx + ButtonWidth;
    }
    ReleaseDC(0, DC);
    Width = MaxSize * Columns + 15;
  }
}
//---------------------------------------------------------------------------
void __fastcall TRadioGroupEx::ItemsChange(TObject *Sender)
{
  AdjustBounds();
  OldItemsChange(Sender);
}
//---------------------------------------------------------------------------

