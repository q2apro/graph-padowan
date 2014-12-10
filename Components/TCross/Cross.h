//===========================================================================
// Copyright © 2003 Ivan Johansen
// Cross.h
//===========================================================================
#ifndef CrossH
#define CrossH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Controls.hpp>
#include <Classes.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TCross : public TGraphicControl
{
private:
  int FX;
  int FY;
  void __fastcall SetX(int Value) {FX = Value; Invalidate();}
  void __fastcall SetY(int Value) {FY = Value; Invalidate();}
  void __fastcall Paint();
  void __fastcall SetStyle(TPenStyle Value){Canvas->Pen->Style = Value; Invalidate();};
  TPenStyle __fastcall GetStyle(){return Canvas->Pen->Style;};
  void __fastcall SetColor(TColor Value){Canvas->Pen->Color = Value; Invalidate();};
  TColor __fastcall GetColor(){return Canvas->Pen->Color;};
  DYNAMIC void __fastcall VisibleChanging(){Invalidate();};
  void __fastcall SetMode(TPenMode Value){Canvas->Pen->Mode = Value; Invalidate();};
  TPenMode __fastcall GetMode(){return Canvas->Pen->Mode;};

protected:
public:
  __fastcall TCross(TComponent* Owner);
  void SetPos(int x, int y) { FX = x; FY = y; Invalidate();}

  __published:
  __property TPenStyle Style = {read=GetStyle, write=SetStyle};
  __property TColor Color = {read=GetColor, write=SetColor};
  __property TPenMode Mode = {read=GetMode, write=SetMode};
  __property int X = {read=FX, write=SetX};
  __property int Y = {read=FY, write=SetY};
  __property Align;
  __property Visible;
  __property ShowHint;
  __property ParentShowHint;
  __property OnMouseDown;
  __property OnMouseUp;
  __property OnMouseMove;
  __property OnClick;
  __property OnDblClick;
};
//---------------------------------------------------------------------------
#endif
 