//===========================================================================
// Copyright © 2003 Ivan Johansen
// MyRadioButton.h
//===========================================================================
#ifndef MyRadioButtonH
#define MyRadioButtonH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Controls.hpp>
#include <Classes.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TMyRadioButton : public TRadioButton
{
private:
  TNotifyEvent FOnCheckedChange;
  bool FMultiLine;

  void __fastcall SetMultiLine(bool Value);
  DYNAMIC void __fastcall KeyDown(Word &Key, TShiftState Shift);
  void __fastcall SetChecked(bool Value);
  void __fastcall WMGetDlgCode(TMessage &Message);

BEGIN_MESSAGE_MAP
  VCL_MESSAGE_HANDLER(WM_GETDLGCODE, TMessage, WMGetDlgCode)
END_MESSAGE_MAP(TControl)

protected:
public:
  __fastcall TMyRadioButton(TComponent* Owner);
__published:
  __property bool MultiLine={read=FMultiLine,write=SetMultiLine,default=false};
  __property TNotifyEvent OnCheckedChange={read=FOnCheckedChange,write=FOnCheckedChange,default=NULL};
};
//---------------------------------------------------------------------------
#endif
