//===========================================================================
// Copyright © 2009 Ivan Johansen
// UpDownEx.h
//===========================================================================
#ifndef UpDownExH
#define UpDownExH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <ComCtrls.hpp>
#include <Controls.hpp>
//---------------------------------------------------------------------------
class PACKAGE TUpDownEx : public TCustomUpDown
{
private:
  TUDClickEvent FOnUpDown;
  TWinControl *FAssociate;
  void __fastcall SetAssociate(TWinControl *Value);
  void UndoAutoResizing(TWinControl *Value);
  void CNNotify(TWMNotifyUD &Message);

BEGIN_MESSAGE_MAP
  VCL_MESSAGE_HANDLER(CN_NOTIFY, TWMNotifyUD, CNNotify);
END_MESSAGE_MAP(TCustomUpDown)

protected:
  void __fastcall CreateWnd();

public:
  __fastcall TUpDownEx(TComponent* Owner);

__published:
  __property TWinControl *Associate = {read=FAssociate, write=SetAssociate, default=NULL};
  __property TUDClickEvent OnUpDown = {read=FOnUpDown, write=FOnUpDown, default=NULL};

  __property Orientation;
  __property ArrowKeys;
  __property AlignButton;
  __property Anchors;
};
//---------------------------------------------------------------------------
#endif
