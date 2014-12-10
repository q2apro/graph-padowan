//===========================================================================
// Copyright © 2009 Ivan Johansen
// LinkLabelEx.h
//===========================================================================
#ifndef LinkLabelExH
#define LinkLabelExH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <Controls.hpp>
//---------------------------------------------------------------------------
class PACKAGE TLinkLabelEx : public TWinControl
{
private:
  bool FAutoSize;
  bool FWordWrap;
  TSysLinkEvent FOnLinkClick;

  void AdjustBounds();
  void DoOnLinkClick(const String &Link, TSysLinkType LinkType);
  String ParseLinks();
  void CMFontChanged(TMessage &Message);
  void CMTextChanged(TMessage &Message);
  void CMNotify(TWMNotify &Message);

BEGIN_MESSAGE_MAP
  VCL_MESSAGE_HANDLER(CM_TEXTCHANGED, TMessage, CMTextChanged)
  VCL_MESSAGE_HANDLER(CM_FONTCHANGED, TMessage, CMFontChanged)
  VCL_MESSAGE_HANDLER(CN_NOTIFY, TWMNotify, CMNotify)
END_MESSAGE_MAP(TWinControl)

protected:
  void __fastcall CreateParams(TCreateParams &Params);
  void __fastcall CreateWnd();
  void __fastcall SetAutoSize(bool Value);
  void __fastcall SetWordWrap(bool Value);

public:
  __fastcall TLinkLabelEx(TComponent* Owner);

__published:
  __property AutoSize = {read=FAutoSize, write=SetAutoSize, default=true};
  __property bool WordWrap = {read=FWordWrap, write=SetWordWrap, default=false};
  __property TSysLinkEvent OnLinkClick = {read=FOnLinkClick, write=FOnLinkClick, default=NULL};

  __property Align;
  __property Anchors;
  __property Caption;
  __property Color;
  __property Constraints;
  __property Font;
  __property ParentColor;
  __property ParentFont;
  __property Visible;
  __property ShowHint;
  __property ParentShowHint;
  __property OnClick;
  __property OnDblClick;
  __property OnMouseActivate;
  __property OnMouseDown;
  __property OnMouseEnter;
  __property OnMouseLeave;
  __property OnMouseMove;
  __property OnMouseUp;
};
//---------------------------------------------------------------------------
#endif
