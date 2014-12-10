//===========================================================================
// Copyright © 2003 Ivan Johansen
// LineSelect.h
//===========================================================================
#ifndef LineSelectH
#define LineSelectH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Controls.hpp>
#include <Classes.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TLineSelect : public TCustomComboBox
{
private:
  TColor FColor;
  bool FEnableClear;
  void __fastcall DrawItem(int Index, const TRect &Rect, TOwnerDrawState State);
  void __fastcall SetLineStyle(TPenStyle Value);
  TPenStyle __fastcall GetLineStyle();
  void __fastcall SetColor(TColor Value);
  void __fastcall SetEnableClear(bool Value);
  void __fastcall SetParent(TWinControl* AParent);
  DYNAMIC void __fastcall ChangeScale(int M, int D);

protected:
public:
  __fastcall TLineSelect(TComponent* Owner);
__published:
  //Note: EnableClear needs to be loaded before LineStyle
  __property bool EnableClear = {read=FEnableClear, write=SetEnableClear, default=false};
  __property TPenStyle LineStyle = {read=GetLineStyle, write=SetLineStyle};
  __property TColor Color = {read=FColor, write=SetColor, default=clBlack};
  __property OnChange;
  __property OnDropDown;
  __property OnClick;
  __property OnDblClick;
  __property OnEnter;
  __property OnExit;
  __property OnContextPopup;
  __property TabOrder;
  __property Anchors;
  __property ItemHeight;
  __property ShowHint;
  __property ParentShowHint; 
};
//---------------------------------------------------------------------------
#endif
