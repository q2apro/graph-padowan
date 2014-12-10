//===========================================================================
// Copyright © 2003 Ivan Johansen
// IColorBox.h
//===========================================================================
#ifndef ColorBoxH
#define ColorBoxH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Controls.hpp>
#include <Classes.hpp>
#include <Forms.hpp>
#include "..\TIColorSelect\IColorSelect.h"
//---------------------------------------------------------------------------
class TIColorBox : public TCustomControl
{
private:
  TColor FColor;//The choosen color
  bool FCtl3D;
  TStrings *FCustomColors;
  TIColorSelect *FColorSelect;
  TNotifyEvent FOnChange;
  TNotifyEvent FBeforeChange;
  String FCaption;
  void __fastcall SetColor(TColor Value);
  void __fastcall Paint();
  DYNAMIC void __fastcall DoEnter();
  DYNAMIC void __fastcall DoExit();
  DYNAMIC void __fastcall KeyPress(Char &Key);
  DYNAMIC void __fastcall Click();
  DYNAMIC void __fastcall DblClick();
  void __fastcall ShowFocus();
  void __fastcall SetCtl3D(bool Value);
protected:
  bool Active;//Indicate if the box has focus
public:
  __fastcall TIColorBox(TComponent* Owner);
__published:
  __property Align;
  __property TabStop;
  __property TabOrder;
  __property Enabled;
  __property ShowHint;
  __property ParentShowHint;
  __property Anchors;

  __property TColor Color = {read=FColor, write=SetColor, default=clRed};
  __property bool Ctl3D = {read=FCtl3D, write=SetCtl3D, default=true};
  __property TIColorSelect* ColorSelect={read=FColorSelect,write=FColorSelect, default=NULL};
  __property String Caption = {read=FCaption, write=FCaption};

  __property OnEnter;
  __property OnExit;

  __property TNotifyEvent OnChange = {read=FOnChange, write=FOnChange};
  __property TNotifyEvent OnBeforeChange = {read=FBeforeChange, write=FBeforeChange};
};
//---------------------------------------------------------------------------
#endif

