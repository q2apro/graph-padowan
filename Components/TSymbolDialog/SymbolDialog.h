//===========================================================================
// Copyright © 2009 Ivan Johansen
// SymbolDialog.h
//===========================================================================
#ifndef SymbolDialogH
#define SymbolDialogH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Classes.hpp>
//---------------------------------------------------------------------------
class TSymbolDialog;
typedef void __fastcall (__closure *TInsertAnsiCharEvent)(TSymbolDialog *Sender, char Symbol, const String &FontName);
typedef void __fastcall (__closure *TInsertWideCharEvent)(TSymbolDialog *Sender, wchar_t Symbol, const String &FontName);
enum TCharacterSet {csAnsiSet, csUnicodeSet};

class PACKAGE TSymbolDialog : public TComponent
{
private:
  TFontName FFontName;
  TNotifyEvent FOnShow;
  TNotifyEvent FOnClose;
  TInsertAnsiCharEvent FOnInsertAnsiChar;
  TInsertWideCharEvent FOnInsertWideChar;
  TCharacterSet FCharacterSet;
  wchar_t FSymbol;
  class TSymbolFrm *FSymbolFrm;

  void __fastcall Button2Click(TObject *Sender);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void DoShow();
  void DoClose();

protected:
public:
  __fastcall TSymbolDialog(TComponent* Owner);
  __fastcall ~TSymbolDialog();
  void Execute();
  void Show();
  void Close();

  __property wchar_t Symbol = {read=FSymbol, write=FSymbol};
  __property TSymbolFrm *SymbolForm = {read=FSymbolFrm};

__published:
  __property TFontName FontName = {read=FFontName, write=FFontName};
  __property TCharacterSet CharacterSet = {read=FCharacterSet, write=FCharacterSet};
  __property TNotifyEvent OnShow = {read=FOnShow, write=FOnShow, default=NULL};
  __property TNotifyEvent OnClose = {read=FOnClose, write=FOnClose, default=NULL};
  __property TInsertAnsiCharEvent OnInsertAnsiChar = {read=FOnInsertAnsiChar, write=FOnInsertAnsiChar, default=NULL};
  __property TInsertWideCharEvent OnInsertWideChar = {read=FOnInsertWideChar, write=FOnInsertWideChar, default=NULL};
};
//---------------------------------------------------------------------------
#endif
