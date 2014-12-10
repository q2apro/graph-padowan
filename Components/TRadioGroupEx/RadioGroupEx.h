//===========================================================================
// Copyright © 2009 Ivan Johansen
// RadioGroupEx.h
//===========================================================================
#ifndef RadioGroupExH
#define RadioGroupExH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include <StdCtrls.hpp>
//---------------------------------------------------------------------------
class PACKAGE TRadioGroupEx : public TRadioGroup
{
private:
  TNotifyEvent OldItemsChange;
  bool FAutoSize;

  void __fastcall CMTextChanged(TMessage &Message);
  void __fastcall CMFontChanged(TMessage &Message);
  void __fastcall ItemsChange(TObject *Sender);

  BEGIN_MESSAGE_MAP
    VCL_MESSAGE_HANDLER(CM_TEXTCHANGED, TMessage, CMTextChanged)
    VCL_MESSAGE_HANDLER(CM_FONTCHANGED, TMessage, CMFontChanged)
  END_MESSAGE_MAP(TRadioGroup)

protected:
  void AdjustBounds();
  void __fastcall Loaded();

public:
  __fastcall TRadioGroupEx(TComponent* Owner);

__published:
  bool __property AutoSize = {read=FAutoSize, write=FAutoSize, default=true};
};
//---------------------------------------------------------------------------
#endif
