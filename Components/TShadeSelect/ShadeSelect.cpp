//===========================================================================
// Copyright © 2003 Ivan Johansen
// ShadeSelect.cpp
//===========================================================================
#include <vcl.h>
#pragma hdrstop
#include "ShadeSelect.h"
#include "ICompCommon.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
static TRegisterClass Dummy(__classid(TShadeSelect));
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
static inline void ValidCtrCheck(TShadeSelect *)
{
  new TShadeSelect(NULL);
}
//---------------------------------------------------------------------------
namespace Shadeselect
{
  void __fastcall PACKAGE Register()
  {
     TComponentClass classes[1] = {__classid(TShadeSelect)};
     RegisterComponents("IComp", classes, 0);
  }
}
//---------------------------------------------------------------------------
__fastcall TShadeSelect::TShadeSelect(TComponent* Owner)
  : TCustomComboBox(Owner), FColor(clGreen)
{
  Style = csOwnerDrawFixed;
  Width = 81;
}
//---------------------------------------------------------------------------
void __fastcall TShadeSelect::SetColor(TColor Value)
{
  FColor = Value;
  Invalidate();
}
//---------------------------------------------------------------------------
TBrushStyle __fastcall TShadeSelect::GetShadeStyle()
{
  return ItemIndex ? static_cast<TBrushStyle>(ItemIndex+1) : bsSolid;
}
//---------------------------------------------------------------------------
void __fastcall TShadeSelect::SetShadeStyle(TBrushStyle Value)
{
  ItemIndex = Value ? Value-1 : 0;
  Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TShadeSelect::DrawItem(int Index, const TRect &Rect, TOwnerDrawState State)
{
  //There seems to be a bug in TBrush, so CreateHatchBrush and CreateSolidBrush
  //are used directly to get a handle
  switch(Index)
  {
    case 0: Canvas->Brush->Handle = CreateSolidBrush(Color); break;
    case 1: Canvas->Brush->Handle = CreateHatchBrush(HS_HORIZONTAL, Color); break;
    case 2: Canvas->Brush->Handle = CreateHatchBrush(HS_VERTICAL, Color); break;
    case 3: Canvas->Brush->Handle = CreateHatchBrush(HS_FDIAGONAL, Color); break;
    case 4: Canvas->Brush->Handle = CreateHatchBrush(HS_BDIAGONAL, Color); break;
    case 5: Canvas->Brush->Handle = CreateHatchBrush(HS_CROSS, Color); break;
    case 6: Canvas->Brush->Handle = CreateHatchBrush(HS_DIAGCROSS, Color); break;
  }
  unsigned OldColor = GetBkColor(Canvas->Handle);
  SetBkColor(Canvas->Handle, clWhite);
  Canvas->FillRect(Rect);
  SetBkColor(Canvas->Handle, OldColor);
}
//---------------------------------------------------------------------------
void __fastcall TShadeSelect::SetParent(TWinControl* AParent)
{
  TControl::SetParent(AParent);
  //Items can first be added when the control has a parent
  if(AParent)
  {
    while(Items->Count < 7)
      Items->Add("X");
    ShadeStyle = bsBDiagonal;
  }
}
//---------------------------------------------------------------------------
void __fastcall TShadeSelect::ChangeScale(int M, int D)
{
  TCustomComboBox::ChangeScale(M, D); // Call inherited
  ItemHeight = (ItemHeight * M) / D;
}
//---------------------------------------------------------------------------

