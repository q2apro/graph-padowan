//===========================================================================
// Copyright © 2003 Ivan Johansen
// MyRadioButton.cpp
//===========================================================================
#include <vcl.h>
#pragma hdrstop
#include "MyRadioButton.h"
#include "ICompCommon.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
static TRegisterClass Dummy(__classid(TMyRadioButton));
static inline void ValidCtrCheck(TMyRadioButton *)
{
  new TMyRadioButton(NULL);
}
//---------------------------------------------------------------------------
namespace Myradiobutton
{
  void __fastcall PACKAGE Register()
  {
    TComponentClass classes[1] = {__classid(TMyRadioButton)};
    RegisterComponents("IComp", classes, 0);
  }
}
//---------------------------------------------------------------------------
__fastcall TMyRadioButton::TMyRadioButton(TComponent* Owner)
        : TRadioButton(Owner), FOnCheckedChange(NULL), FMultiLine(false)
{
}
//---------------------------------------------------------------------------
void __fastcall TMyRadioButton::KeyDown(Word &Key, TShiftState Shift)
{
  TRadioButton::KeyDown(Key, Shift);
  if(Key == VK_LEFT || Key == VK_RIGHT || Key == VK_UP || Key == VK_DOWN)
  {
    TList *List = new TList;
    Parent->GetTabOrderList(List);
    int Index = List->IndexOf(this);
    TWinControl *NextControl;
    do
    {
      if(Key == VK_RIGHT || Key == VK_DOWN)
        Index = Index+1 == List->Count ? 0 : Index+1;
      else
        Index = Index == 0 ? List->Count-1 : Index-1;
      NextControl = static_cast<TWinControl*>(List->Items[Index]);
    }
    while(dynamic_cast<TRadioButton*>(NextControl) == NULL || !NextControl->CanFocus());
    NextControl->SetFocus();
  }
}
//---------------------------------------------------------------------------
void __fastcall TMyRadioButton::WMGetDlgCode(TMessage & Message)
{
  Message.Result = DLGC_WANTARROWS;
}
//---------------------------------------------------------------------------
void __fastcall TMyRadioButton::SetChecked(bool Value)
{
  TRadioButton::SetChecked(Value);
  if(OnCheckedChange)
    OnCheckedChange(this);
}
//---------------------------------------------------------------------------
void __fastcall TMyRadioButton::SetMultiLine(bool Value)
{
  FMultiLine = Value;
  LONG dwStyle = GetWindowLong(Handle, GWL_STYLE);
 	SetWindowLong(Handle, GWL_STYLE, Value ? dwStyle | BS_MULTILINE : dwStyle & ~BS_MULTILINE);
  Invalidate();
}
//---------------------------------------------------------------------------

