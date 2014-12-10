//===========================================================================
// Copyright © 2003 Ivan Johansen
// IPageSetup.h
//===========================================================================
#ifndef PageSetupH
#define PageSetupH

#include <SysUtils.hpp>
#include <Controls.hpp>
#include <Classes.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
#include <Printers.hpp>
//---------------------------------------------------------------------------
typedef enum {psoDefaultMinMargins, psoDisableMargins, psoDisableOrientation,
	psoDisablePagePainting, psoDisablePaper, psoDisablePrinter, psoMargins,
	psoMinMargins, psoNoWarning, psoShowHelp
	} TIPageSetupDialogOptions;

typedef Set<TIPageSetupDialogOptions, ::psoDefaultMinMargins, ::psoShowHelp> TPageSetupOptionsSet;

typedef enum {psuMillimeters, psuInches} TIPageSetupDialogUnits;

class TIPageSetupDialog;

typedef void __fastcall (__closure *TPaintEvent)(TIPageSetupDialog *Sender,TCanvas *PageCanvas,const TRect &UpdateRect,const TRect &PaperSize);

class TPageSetupRect : public TPersistent
{
private:
  void __fastcall Change(int Index, long Value);
public:
  TRect Rect;
  __fastcall TPageSetupRect(TComponent* Owner):Rect(0,0,0,0){};
  __fastcall TPageSetupRect(const TRect &R):Rect(R.Left,R.Top,R.Right,R.Bottom){};
__published:
  __property long Left = { read = Rect.left, write = Change, index = 0, default = 0 };
  __property long Top = { read = Rect.top, write = Change, index = 1, default = 0 };
  __property long Right = { read = Rect.right, write = Change, index = 2, default = 0 };
  __property long Bottom = { read = Rect.bottom, write = Change, index = 3, default = 0 };
};

class TIPageSetupDialog : public TCommonDialog
{
private:
  static TIPageSetupDialog *Dialog;
  static  UINT APIENTRY DialogHook(HWND hdlg,UINT uiMsg,WPARAM wParam,LPARAM lParam);
  static int Count;
  HWND FHandle;
protected:
  TPageSetupRect* FMinMargins;
  TPageSetupRect* FMargins;
  int FPaperWidth;
  int FPaperHeight;
  TPageSetupOptionsSet FOptions;
  TIPageSetupDialogUnits FUnits;
  TPaintEvent FOnPaintMargins;
  TPaintEvent FOnPaintText;
  TPaintEvent FOnPaintStamp;
  TPaintEvent FOnPaintAdress;
  TPoint GetResolution();
  TRect GetPrinterMargins();
  TRect GetDrawingArea();

  unsigned int __fastcall GetOptionFlags(void);
  void __fastcall SetOptionFlags(int flags);
public:
  __property int PaperWidth = {read = FPaperWidth};
  __property int PaperHeight = {read = FPaperHeight};
  __property HWND Handle = {read = FHandle};
  __property TPoint Resolution = {read = GetResolution};
  __property TRect PrinterMargins = {read=GetPrinterMargins};
  __property TRect DrawingArea = {read = GetDrawingArea};

  __fastcall TIPageSetupDialog(TComponent* Owner);
  __fastcall ~TIPageSetupDialog();
  bool __fastcall Execute(HWND ParentWnd = NULL);
  bool __fastcall Execute() {return Execute(NULL);}
  void __fastcall Error(TPageSetupRect *R){throw Exception("Error!");};

__published:
  __property TIPageSetupDialogUnits Units = {read = FUnits, write = FUnits,default = psuMillimeters};
  __property TPageSetupRect* MinMargins = {read = FMinMargins, write=FMinMargins};
  __property TPageSetupRect* Margins = {read = FMargins, write = FMargins};
  __property TPageSetupOptionsSet Options = {read = FOptions, write = FOptions};
  __property TPaintEvent OnPaintMargins = {read=FOnPaintMargins, write=FOnPaintMargins};
  __property TPaintEvent OnPaintText = {read=FOnPaintText, write=FOnPaintText};
  __property TPaintEvent OnPaintStamp = {read=FOnPaintStamp, write=FOnPaintStamp};
  __property TPaintEvent OnPaintAdress = {read=FOnPaintAdress, write=FOnPaintAdress};
};

//Exception class thrown by TIPageSetupDialog::Execute when an error occour
//ErrorCode contain the value returned from CommDlgExtendedError
class EPageSetup : public Exception
{
  int FErrorCode;
public:
  EPageSetup(String Msg, int Error);
  __property int ErrorCode = {read=FErrorCode};
};
//---------------------------------------------------------------------------
#endif

