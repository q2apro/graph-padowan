//===========================================================================
// Copyright © 2005 Ivan Johansen
// SaveDialogEx.h
//===========================================================================
#ifndef SaveDialogExH
#define SaveDialogExH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
class PACKAGE TSaveDialogEx : public TSaveDialog
{
  typedef TSaveDialog inherited;
private:
  String FHelpCaption;
  TNotifyEvent FOnHelp;
  bool FAutoExt;

  void __fastcall WndProc(TMessage &Message);
  DYNAMIC void __fastcall DoShow();
  void __fastcall SetFileName(const String &AFileName);
  String __fastcall GetFileName();

protected:
  DYNAMIC void __fastcall DoTypeChange();
public:
  __fastcall TSaveDialogEx(TComponent* Owner);

__published:
  __property String HelpCaption = {read=FHelpCaption, write=FHelpCaption};
  __property TNotifyEvent OnHelp = {read=FOnHelp, write=FOnHelp, default=NULL};
  __property TFileName FileName = {read=GetFileName, write=SetFileName};
  __property bool AutoExt = {read=FAutoExt, write=FAutoExt, default=true};
};
//---------------------------------------------------------------------------
#endif

