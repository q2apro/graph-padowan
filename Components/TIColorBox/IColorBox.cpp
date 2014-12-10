//===========================================================================
// Copyright © 2003 Ivan Johansen
// IColorBox.cpp
//===========================================================================
#include "Config.h"
#include <vcl.h>
#pragma hdrstop
#include "IColorBox.h"
#include "ICompCommon.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
#define stdWidth 81   //The default width of control
#define stdHeight 27  //The default height of control
#define BORDER 3     //The number of pixels between box and focus rectangle

static TRegisterClass Dummy(__classid(TIColorBox));
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//
static inline void ValidCtrCheck(TIColorBox *)
{
  new TIColorBox(NULL);
}
//---------------------------------------------------------------------------
namespace Icolorbox
{
  void __fastcall PACKAGE Register()
  {
           TComponentClass classes[1] = {__classid(TIColorBox)};
           RegisterComponents("IComp", classes, 0);
  }
}
//---------------------------------------------------------------------------
//Constructor initializes variables
__fastcall TIColorBox::TIColorBox(TComponent* Owner)
: TCustomControl(Owner), FCtl3D(true), FColorSelect(NULL), FOnChange(NULL), OnBeforeChange(NULL)
{
  Width=stdWidth;
  Height=stdHeight;
  FColor=clRed;
  TabStop=true;
  Active=false;
}
//---------------------------------------------------------------------------
//This function is called then the color should be changed
void __fastcall TIColorBox::SetColor(TColor Value)
{
  FColor=Value;//Set color variable
  Paint();//Repaint box
}
//---------------------------------------------------------------------------
//This function is called then be control should be repainted
void __fastcall TIColorBox::Paint()
{
  if(Ctl3D)
  {
    //Draw frame in 3D
    Canvas->Pen->Color=clGray;
    Canvas->MoveTo(BORDER,Height-1-BORDER);
    Canvas->LineTo(BORDER,BORDER);
    Canvas->LineTo(Width-1-BORDER,BORDER);
    Canvas->Pen->Color=clWhite;
    Canvas->LineTo(Width-1-BORDER,Height-1-BORDER);
    Canvas->LineTo(BORDER-1,Height-1-BORDER);
    Canvas->Pen->Color=clBlack;
    Canvas->MoveTo(1+BORDER,Height-3-BORDER);
    Canvas->LineTo(1+BORDER,1+BORDER);
    Canvas->LineTo(Width-2-BORDER,1+BORDER);
    Canvas->Pen->Color=(TColor)0x00DFDFDF;
    Canvas->LineTo(Width-2-BORDER,Height-2-BORDER);
    Canvas->LineTo(BORDER,Height-2-BORDER);
    //Fill the rectangle with the choosen color
    Canvas->Brush->Color=FColor;
    Canvas->FillRect(Rect(BORDER+2,BORDER+2,Width-BORDER-2,Height-BORDER-2));
  }
  else
  {//Don't show frame in 3D
    //Draw a black rectangle
    Canvas->Brush->Color=clBlack;
    Canvas->FrameRect(Rect(BORDER,BORDER,Width-BORDER,Height-BORDER));
    //Fill the rectangle with the choosen color
    Canvas->Brush->Color=FColor;
    Canvas->FillRect(Rect(BORDER+1,BORDER+1,Width-BORDER-1,Height-BORDER-1));
  }
  //Show the box
  ShowFocus();
}
//---------------------------------------------------------------------------
//This function is called then the control receives focus
void __fastcall TIColorBox::DoEnter()
{
  //Set variable to indicate that control got focus
  Active=true;
  //Show focus rectangle
  ShowFocus();
}
//---------------------------------------------------------------------------
//This function is called then the control looses focus
void __fastcall TIColorBox::DoExit()
{
  //Set variable to indicate that control not have focus
  Active=false;
  //Remove focus rectangle
  ShowFocus();
}
//---------------------------------------------------------------------------
//This function is called then a key is pressed
void __fastcall TIColorBox::KeyPress(Char &Key)
{
  //If space is pressed
  if(Key==' ')
    DblClick();//Simulate dobbelt click
}
//---------------------------------------------------------------------------
//This function is called then the control is dobbelt
//clicked or the space key was pressed
void __fastcall TIColorBox::DblClick()
{
  if(FBeforeChange)
    FBeforeChange(this);  
  if(FColorSelect)
  {
    ColorSelect->Color=FColor;
    if(ColorSelect->Execute())
      Color=ColorSelect->Color;
  }
  else
  {
    TIColorSelect *Temp =  new TIColorSelect(this);
    if(!FCaption.IsEmpty())
      Temp->Caption = FCaption;
    Temp->Color = Color;
    if(Temp->Execute())
      Color = Temp->Color;
    delete Temp;
  }
  if(FOnChange)
    FOnChange(this);
}
//---------------------------------------------------------------------------
//This function is called the control has ben clicked
void __fastcall TIColorBox::Click()
{
  //Move focus to control
  SetFocus();
}
//---------------------------------------------------------------------------
//This function shows or removes the focus rectangle
void __fastcall TIColorBox::ShowFocus()
{
  //Draw a rectangle with the background color
  //BORDER pixels from the color box
  Canvas->Brush->Color=TCustomControl::Color;
  Canvas->FrameRect(Rect(0,0,Width,Height));
  //If component has focus
  if(Active)
  {
    //Draw a focus rectangle around the color box
    Canvas->Brush->Color=clBtnFace;
    Canvas->DrawFocusRect(Rect(0,0,Width,Height));
  }
}
//---------------------------------------------------------------------------
//This function is called then the property Ctl3D is changed
void __fastcall TIColorBox::SetCtl3D(bool Value)
{
  FCtl3D=Value;
  Paint();
}
//---------------------------------------------------------------------------



