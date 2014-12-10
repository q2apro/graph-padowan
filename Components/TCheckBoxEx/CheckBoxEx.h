//===========================================================================
// Copyright © 2009 Ivan Johansen
// CheckBoxEx.h
//===========================================================================
#ifndef CheckBoxExH
#define CheckBoxExH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
//---------------------------------------------------------------------------
class PACKAGE TCheckBoxEx : public TCheckBox
{
private:
  bool FAutoSize;
  void __fastcall CMTextChanged(TMessage &Message);
  void __fastcall CMFontChanged(TMessage &Message);

BEGIN_MESSAGE_MAP
  VCL_MESSAGE_HANDLER(CM_TEXTCHANGED, TMessage, CMTextChanged)
  VCL_MESSAGE_HANDLER(CM_FONTCHANGED, TMessage, CMFontChanged)
END_MESSAGE_MAP(TCheckBox)

protected:
  void AdjustBounds();
  void __fastcall Loaded();

public:
  __fastcall TCheckBoxEx(TComponent* Owner);
__published:
  __property bool AutoSize = {read=FAutoSize, write=FAutoSize, default=true};
};
//---------------------------------------------------------------------------
#endif
