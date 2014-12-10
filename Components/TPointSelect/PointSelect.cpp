//===========================================================================
// Copyright © 2003 Ivan Johansen
// PointSelect.cpp
//===========================================================================
#include <vcl.h>
#pragma hdrstop
#include "PointSelect.h"
#include "ICompCommon.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
static TRegisterClass Dummy(__classid(TPointSelect));
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//
static inline void ValidCtrCheck(TPointSelect *)
{
  new TPointSelect(NULL);
}
//---------------------------------------------------------------------------
namespace Pointselect
{
  void __fastcall PACKAGE Register()
  {
    TComponentClass classes[1] = {__classid(TPointSelect)};
    RegisterComponents("IComp", classes, 0);
  }
}
//---------------------------------------------------------------------------
__fastcall TPointSelect::TPointSelect(TComponent* Owner)
        : TCustomComboBox(Owner), FFrameColor(clBlack), FFillColor(clRed)
{
  Style = csOwnerDrawFixed;
  Width = 81;
}
//---------------------------------------------------------------------------
void __fastcall TPointSelect::SetFrameColor(TColor Value)
{
  FFrameColor = Value;
  Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TPointSelect::SetFillColor(TColor Value)
{
  FFillColor = Value;
  Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TPointSelect::SetParent(TWinControl* AParent)
{
  TControl::SetParent(AParent);
  if(AParent)
    if(!Items->Count)
    {
      for(int I = 0; I < 8; I++)
        Items->Add("X");
      ItemIndex = 0;
    }
}
//---------------------------------------------------------------------------
void __fastcall TPointSelect::DrawItem(int Index, const TRect &Rect, TOwnerDrawState State)
{
  //Clear drawing area
  Canvas->Brush->Color = clWhite;
  Canvas->FillRect(Rect);

  TPoint Pos((Rect.Left+Rect.Right)/2, (Rect.Top+Rect.Bottom)/2);
  DrawPoint(Canvas, Pos, Index, FrameColor, FillColor, 5);

  //This is necesarry to use white as brush before the VCL can draw the focus
  //rectangle correct; I don't know why.
  TRect Rect1(0,0,0,0);
  Canvas->Brush->Color = clWhite;
  Canvas->FillRect(Rect1);
}
//---------------------------------------------------------------------------
void TPointSelect::DrawPoint(TCanvas *Canvas, TPoint Pos, int Style, TColor FrameColor, TColor FillColor, unsigned Size)
{
	if(Size == 0)
	  return;
  Canvas->Pen->Color = Size > 2 ? FrameColor : FillColor;
  Canvas->Pen->Width = Size/8 + 1;
  Canvas->Pen->Style = psSolid;
  Canvas->Brush->Color = FillColor;
  Canvas->Brush->Style = bsSolid;

  TRect Rect(Pos.x-Size, Pos.y-Size, Pos.x+Size+1, Pos.y+Size+1);

  switch(Style)
  {
    case 0: //Draw circle
      Canvas->Ellipse(Rect);
      break;

    case 1: //Draw square
      Canvas->Rectangle(Rect);
      break;

    case 2: //Draw square rotated 45 degrees
    {
      TPoint Points[] = {TPoint(Pos.x-Size, Pos.y), TPoint(Pos.x, Pos.y-Size),
                         TPoint(Pos.x+Size, Pos.y), TPoint(Pos.x, Pos.y+Size)};
      Canvas->Polygon(Points, sizeof(Points)/sizeof(TPoint)-1);
      break;
    }

    case 3: //Draw triangle
    {
      TPoint Points[] = {TPoint(Pos.x-Size, Pos.y+Size), TPoint(Pos.x, Pos.y-Size),
                         TPoint(Pos.x+Size, Pos.y+Size)};
      Canvas->Polygon(Points, sizeof(Points)/sizeof(TPoint)-1);
      break;
    }

    case 4: //Draw horizontal line
      Canvas->Pen->Width = Size;
      Canvas->Pen->Color = FillColor;
      Canvas->MoveTo(Pos.x-Size, Pos.y);
      Canvas->LineTo(Pos.x+Size, Pos.y);
      break;

    case 5: //Draw square with a cross
    case 6: //Draw cross
      if(Style == 5)
        Canvas->FillRect(Rect);
      else
      {
        Canvas->Pen->Width = Size / 2;
        Canvas->Pen->Color = FillColor;
      }
      Canvas->MoveTo(Pos.x, Pos.y);
      Canvas->LineTo(Pos.x - Size, Pos.y - Size);
      Canvas->MoveTo(Pos.x, Pos.y);
      Canvas->LineTo(Pos.x + Size, Pos.y - Size);
      Canvas->MoveTo(Pos.x, Pos.y);
      Canvas->LineTo(Pos.x - Size, Pos.y + Size);
      Canvas->MoveTo(Pos.x, Pos.y);
      Canvas->LineTo(Pos.x + Size, Pos.y + Size);
      break;

    case 7: //Draw arrow
			Canvas->Pen->Width = 1;
			Canvas->Pen->Color = FillColor;
			Canvas->Brush->Color = FillColor;
			int Length = 5 + 2*Size;
			int dX = Length * 0.9422;
			int dY = Length * 0.3350;
			Pos.x += dX / 2;
			TPoint Arrow[] = {TPoint(Pos.x - dX, Pos.y - dY), Pos, TPoint(Pos.x - dX, Pos.y + dY)};
			Canvas->Polygon(Arrow, 2);
			break;
  }
}
//---------------------------------------------------------------------------
void __fastcall TPointSelect::ChangeScale(int M, int D)
{
  TCustomComboBox::ChangeScale(M, D); // Call inherited
  ItemHeight = (ItemHeight * M) / D;
}
//---------------------------------------------------------------------------

