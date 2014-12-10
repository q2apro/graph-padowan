//===========================================================================
// Copyright © 2003 Ivan Johansen
// ShadeSelect.h
//===========================================================================
#ifndef ShadeSelectH
#define ShadeSelectH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Controls.hpp>
#include <Classes.hpp>
#include <Forms.hpp>
#include <StdCtrls.hpp>
//---------------------------------------------------------------------------
class TShadeSelect : public TCustomComboBox
{
private:
  TColor FColor;
  void __fastcall SetColor(TColor Value);
  TBrushStyle __fastcall GetShadeStyle();
  void __fastcall SetShadeStyle(TBrushStyle Value);
  void __fastcall DrawItem(int Index, const TRect &Rect, TOwnerDrawState State);
  void __fastcall SetParent(TWinControl* AParent);
  DYNAMIC void __fastcall ChangeScale(int M, int D);

protected:
public:
  __fastcall TShadeSelect(TComponent* Owner);
__published:
  __property TColor Color = {read=FColor,write=SetColor,default=clGreen};
  __property TBrushStyle ShadeStyle = {read=GetShadeStyle,write=SetShadeStyle,default=bsBDiagonal};
  __property Align;
  __property OnChange;
  __property OnDropDown;
  __property OnClick;
  __property OnDblClick;
  __property OnEnter;
  __property OnExit;
  __property OnContextPopup;
  __property TabOrder;
  __property Anchors;
  __property ShowHint;
  __property ParentShowHint;
};
//---------------------------------------------------------------------------
#endif
