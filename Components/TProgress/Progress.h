//===========================================================================
// Copyright © 2003 Ivan Johansen
// Progress.h
//===========================================================================
#ifndef ProgressH
#define ProgressH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Controls.hpp>
#include <Classes.hpp>
#include <Forms.hpp>

#define stWidth 100//Standard Width of control
#define stHeight 25//Standard Height of control
//---------------------------------------------------------------------------
class TProgress : public TGraphicControl
{
private:
  bool FCtl3D;
  int FMin;
  int FMax;
  int FPosition;
  TColor FBackground;
  bool FShowText;
  int FStep;
  TRect ClientArea;//Area inside control exlusive frame
  int BarPos;//The right position of the bar
  Graphics::TBitmap *Bitmap;//used to write text on
  void __fastcall SetCtl3D(bool Value);
  void __fastcall SetMin(int Value);
  void __fastcall SetMax(int Value);
  void __fastcall SetPosition(int Value);
  void __fastcall SetBackground(TColor Value);
  void __fastcall SetShowText(bool Value);
  void __fastcall Paint(void);
  void __fastcall Update(void);

protected:
public:
  __fastcall TProgress(TComponent* Owner);
  __fastcall ~TProgress();
  void __fastcall StepIt(void);

__published:
  __property bool Ctl3D = {read=FCtl3D, write=SetCtl3D, default=true};
  __property int Min = {read=FMin, write=SetMin, default=0};
  __property int Max = {read=FMax, write=SetMax, default=100};
  __property int Position = {read=FPosition, write=SetPosition, default=0};
  __property TColor Background = {read=FBackground, write=SetBackground, default=clWhite};
  __property Color;
  __property Visible;
  __property ShowHint;
  __property Font;
  __property bool ShowText={read=FShowText,write=SetShowText,default=true};
  __property int Step={read=FStep,write=FStep,default=1};
};
//---------------------------------------------------------------------------
#endif
