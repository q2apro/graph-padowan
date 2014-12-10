//===========================================================================
// Copyright © 2003 Ivan Johansen
// LineSelect.cpp
//===========================================================================
#include <vcl.h>
#pragma hdrstop
#include "LineSelect.h"
#include "ICompCommon.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
static TRegisterClass Dummy(__classid(TLineSelect));
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
static inline void ValidCtrCheck(TLineSelect *)
{
  new TLineSelect(NULL);
}
//---------------------------------------------------------------------------
namespace Lineselect
{
  void __fastcall PACKAGE Register()
  {
    TComponentClass classes[1] = {__classid(TLineSelect)};
    RegisterComponents("IComp", classes, 0);
  }
}
//---------------------------------------------------------------------------
__fastcall TLineSelect::TLineSelect(TComponent* Owner)
		: TCustomComboBox(Owner), FColor(clBlack), FEnableClear(false)
{
  Style = csOwnerDrawFixed;
  Width = 81;
}
//---------------------------------------------------------------------------
void __fastcall TLineSelect::DrawItem(int Index, const TRect &Rect, TOwnerDrawState State)
{
  Canvas->Pen->Style = (TPenStyle)Index;
  Canvas->Pen->Color = FColor;
  Canvas->Brush->Color = clWhite;
  Canvas->FillRect(Rect);
  Canvas->MoveTo(Rect.Left, (Rect.Top + Rect.Bottom)/2);
  Canvas->LineTo(Rect.Right,(Rect.Top + Rect.Bottom)/2);
  Canvas->MoveTo(Rect.Left, (Rect.Top + Rect.Bottom)/2+1);
  Canvas->LineTo(Rect.Right,(Rect.Top + Rect.Bottom)/2+1);
}
//---------------------------------------------------------------------------
void __fastcall TLineSelect::SetLineStyle(TPenStyle Value)
{
  if(EnableClear || Value < psClear)
    ItemIndex = Value;
}
//---------------------------------------------------------------------------
TPenStyle __fastcall TLineSelect::GetLineStyle()
{
  return ItemIndex>= 0 ? (TPenStyle)ItemIndex : psClear;
}
//---------------------------------------------------------------------------
void __fastcall TLineSelect::SetColor(TColor Value)
{
  FColor = Value;
  Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TLineSelect::SetParent(TWinControl* AParent)
{
  TControl::SetParent(AParent);
  if(AParent)
    if(!Items->Count)
    {
      for(int I = 0; I< 5; I++)
        Items->Add("X");
      ItemIndex = 0;
    }
}
//---------------------------------------------------------------------------
void __fastcall TLineSelect::SetEnableClear(bool Value)
{
  if(EnableClear && !Value)
  {
    Items->Delete(5);
    if(LineStyle == psClear)
      LineStyle = psSolid;
  }
  else if(!EnableClear && Value)
    Items->Add("X");
  FEnableClear = Value;
}
//---------------------------------------------------------------------------
void __fastcall TLineSelect::ChangeScale(int M, int D)
{
  TCustomComboBox::ChangeScale(M, D); // Call inherited
  ItemHeight = (ItemHeight * M) / D; 
}
//---------------------------------------------------------------------------









