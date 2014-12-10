//===========================================================================
// Copyright © 2003 Ivan Johansen
// PointSelect.h
//===========================================================================
#ifndef PointSelectH
#define PointSelectH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Controls.hpp>
#include <Classes.hpp>
#include <Forms.hpp>
#include <StdCtrls.hpp>
//---------------------------------------------------------------------------
class TPointSelect : public TCustomComboBox
{
private:
  TColor FFrameColor;
  TColor FFillColor;

  void __fastcall SetFrameColor(TColor Value);
  void __fastcall SetFillColor(TColor Value);
  void __fastcall SetParent(TWinControl* AParent);
  void __fastcall DrawItem(int Index, const TRect &Rect, TOwnerDrawState State);
  DYNAMIC void __fastcall ChangeScale(int M, int D);

protected:
public:
  __fastcall TPointSelect(TComponent* Owner);
  static void DrawPoint(TCanvas *Canvas, TPoint Pos, int Style, TColor FrameColor, TColor FillColor, unsigned Size);
__published:
  __property TColor FrameColor = {read=FFrameColor, write=SetFrameColor, default=clBlack};
  __property TColor FillColor = {read=FFillColor, write=SetFillColor, default=clRed};
  __property ItemIndex;
  __property OnChange;
  __property OnDropDown;
  __property OnClick;
  __property OnDblClick;
  __property OnEnter;
  __property OnExit;
  __property OnContextPopup;
  __property TabOrder;
  __property ShowHint;
  __property ParentShowHint;
  __property Anchors;
};
//---------------------------------------------------------------------------
#endif
