//===========================================================================
// Copyright © 2003 Ivan Johansen
// MyEdit.h
//===========================================================================
#ifndef MyEditH
#define MyEditH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Controls.hpp>
#include <Classes.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TMyEdit : public TEdit
{
private:
  DYNAMIC void __fastcall KeyPress(Char &Key);

  void __fastcall WMDeadChar(TMessage &Message);

  BEGIN_MESSAGE_MAP
    VCL_MESSAGE_HANDLER(WM_DEADCHAR, TMessage, WMDeadChar)
  END_MESSAGE_MAP(TEdit)
protected:
  String ConvertToGreek(String Str);
  wchar_t ConvertToGreek(wchar_t Ch);
public:
  __fastcall TMyEdit(TComponent* Owner);
__published:
};
//---------------------------------------------------------------------------
#endif
