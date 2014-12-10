//===========================================================================
// Copyright © 2003 Ivan Johansen
// Progress.cpp
//===========================================================================
#include <vcl.h>
#pragma hdrstop
#include "Config.h"
#include <math.h>
#include "Progress.h"
#include "ICompCommon.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
static TRegisterClass Dummy(__classid(TProgress));
static inline void ValidCtrCheck(TProgress *)
{
	new TProgress(NULL);
}
//---------------------------------------------------------------------------
namespace Progress
{
	void __fastcall PACKAGE Register()
	{
		TComponentClass classes[1] = {__classid(TProgress)};
		RegisterComponents("IComp", classes, 0);
	}
}
//---------------------------------------------------------------------------
__fastcall TProgress::TProgress(TComponent* Owner)
	: TGraphicControl(Owner), Bitmap(new Graphics::TBitmap), FCtl3D(true), FMin(0), FMax(100), FPosition(0),
    FBackground(clWhite), FShowText(true), FStep(1), ClientArea(2, 2, Width-2, Height-2)
{
  Bitmap->Canvas->Brush->Color = clBlack;//Brush color must allways be black
  Bitmap->Canvas->Font->Color = clWhite;//Font color must be white
  Height = stHeight;//Initialize variables to default
  Width = stWidth;
  Color = clBlue;
}
//---------------------------------------------------------------------------
__fastcall TProgress::~TProgress()
{
  delete Bitmap;
}
//---------------------------------------------------------------------------
void __fastcall TProgress::SetCtl3D(bool Value)
{
  FCtl3D = Value;//Set variable hidden from user
  Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TProgress::SetMin(int Value)
{
  if(Value >= FMax)
    return;//Min must allways be less than Max
  FMin = Value;//Set value and update
  Update();
}
//---------------------------------------------------------------------------
void __fastcall TProgress::SetMax(int Value)
{
  if(Value <= FMin)
    return;//Max must allways be greater than Min
  FMax = Value;//Set value and update
  Update();
}
//---------------------------------------------------------------------------
void __fastcall TProgress::SetPosition(int Value)
{
  FPosition = Value;//Set value and update
  Update();
}
//---------------------------------------------------------------------------
void __fastcall TProgress::SetBackground(TColor Value)
{
  FBackground = Value;
  BarPos = -1;//Invalidate update
  Update();
}
//---------------------------------------------------------------------------
void __fastcall TProgress::SetShowText(bool Value)
{
  FShowText=Value;
  BarPos = -1;//Invalidate update
  Update();
}
//---------------------------------------------------------------------------
void __fastcall TProgress::StepIt(void)
{
  FPosition += FStep;
  Update();
}
//---------------------------------------------------------------------------
void __fastcall TProgress::Paint(void)
{
  if(FCtl3D)
  {//Show frame in 3D
    Canvas->Pen->Color = clGray;
    Canvas->MoveTo(0, Height-1);
    Canvas->LineTo(0, 0);
    Canvas->LineTo(Width - 1, 0);
    Canvas->Pen->Color = clWhite;
    Canvas->LineTo(Width-1, Height-1);
    Canvas->LineTo(-1, Height-1);
    Canvas->Pen->Color = clBlack;
    Canvas->MoveTo(1, Height-3);
    Canvas->LineTo(1, 1);
    Canvas->LineTo(Width-2, 1);
    Canvas->Pen->Color = (TColor)0x00DFDFDF;
    Canvas->LineTo(Width-2, Height-2);
    Canvas->LineTo(0, Height-2);
    ClientArea = Rect(2, 2, Width-2, Height-2);
  }
  else
  {//Show frame not in 3D
    Canvas->Pen->Color = clBlack;
    Canvas->Rectangle(0, 0, Width, Height);
    ClientArea = Rect(1, 1, Width-1, Height-1);
  }
  Bitmap->Height = Height;//The size of Bitmap must be the same as the control
  Bitmap->Width = Width;
  BarPos = -1;//Make sure bar is updated
  Update();//Show bar
}
//---------------------------------------------------------------------------
void __fastcall TProgress::Update(void)
{
  double Scale = double(Position - FMin) / (FMax - FMin);//Multiply by 100 to get the scale in percent
  int I = Scale * (ClientArea.Right - ClientArea.Left) + ClientArea.Left;//Find right position of progressbar
  if(I > ClientArea.Right)
    I = ClientArea.Right;
  if(I != BarPos)
  {//If position changed
    BarPos = I;//Set right position for bar
    Canvas->Brush->Color = FBackground;//Set background color
    Canvas->FillRect(Rect(BarPos, ClientArea.Top, ClientArea.Right, ClientArea.Bottom));//Clear area inside frame
    Canvas->Brush->Color = Color;//Set bar color
    Canvas->FillRect(Rect(ClientArea.Left, ClientArea.Top, BarPos, ClientArea.Bottom));//Draw bar
    if(!FShowText)
      return;//Return if progress shout not be shown in percent

    int Percent = floor(Scale * 100);
    if(Percent < 0)
      Percent = 0;
    if(Percent == 0 && Position > FMin)
      Percent = 1;
    String Str = IntToStr(Percent) + "%";

    Bitmap->Canvas->FillRect(ClientRect);//Bitmap is filled with the color clBlack
    Bitmap->Canvas->Font = Font;//Set font
    Bitmap->Canvas->Font->Color = clWhite;//Font color must be white
    int x = (ClientWidth-Bitmap->Canvas->TextWidth(Str))/2;//Position to write text
    int y = (ClientHeight-Bitmap->Canvas->TextHeight(Str))/2;
//    Bitmap->Canvas->Brush->Style=bsClear;
    Bitmap->Canvas->TextRect(ClientArea, x, y, Str);//Write Str in clWhite
    Canvas->CopyMode = cmSrcInvert;
    Canvas->Draw(0, 0, Bitmap);//Draw text invertet
  }
}
//---------------------------------------------------------------------------

