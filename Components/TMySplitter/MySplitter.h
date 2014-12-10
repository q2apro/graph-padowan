//===========================================================================
// Copyright © 2003 Ivan Johansen
// MySplitter.h
//===========================================================================
#ifndef MySplitterH
#define MySplitterH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Controls.hpp>
#include <Classes.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TMySplitter : public TSplitter
{
private:
  bool DblClicked;
protected:
  DYNAMIC void __fastcall DblClick(void);
  DYNAMIC void __fastcall MouseDown(TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
public:
  __fastcall TMySplitter(TComponent* Owner);
__published:
  __property OnMouseDown;
  __property OnMouseUp;
  __property OnMouseMove;
  __property OnClick;
  __property OnDblClick;
};
//---------------------------------------------------------------------------
#endif
