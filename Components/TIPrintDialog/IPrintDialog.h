//===========================================================================
// Copyright © 2004 Ivan Johansen
// IPrintdialog.h
//===========================================================================
#ifndef IPrintDialogH
#define IPrintDialogH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Classes.hpp>
//---------------------------------------------------------------------------
class TIPrintDialog;
typedef void __fastcall (__closure *TPaintSampleEvent)(TIPrintDialog *Sender, TCanvas *PageCanvas, const TRect &PaintRect, const TRect &PaperSize);
enum TPrintOutput {poColor, poBlackWhite};

class PACKAGE TIPrintDialog : public TComponent
{
private:
  TNotifyEvent FOnShow;
  TNotifyEvent FOnClose;
  TPaintSampleEvent FOnPaintSample;
  int FLeftMargin;
  int FTopMargin;
  int FScale;
  bool FCenterOnPage;
  TPrintOutput FPrintOutput;
  int FImageWidth;
  int FImageHeight;
  int FCopies;
  Printers::TPrinterOrientation FOrientation;
  class TPrintFrm *PrintFrm;
  
  TRect GetDrawingArea();

protected:
public:
  __fastcall TIPrintDialog(TComponent* Owner);
  bool Execute();

  __property TRect DrawingArea = {read = GetDrawingArea};
  __property TPrintFrm *PrintForm = {read=PrintFrm};

__published:
  __property TNotifyEvent OnShow = {read=FOnShow, write=FOnShow, default=NULL};
  __property TNotifyEvent OnClose = {read=FOnClose, write=FOnClose, default=NULL};
  __property TPaintSampleEvent OnPaintSample = {read=FOnPaintSample, write=FOnPaintSample, default=NULL};
  __property int LeftMargin = {read=FLeftMargin, write=FLeftMargin, default=25};
  __property int TopMargin = {read=FTopMargin, write=FTopMargin, default=75};
  __property int Scale = {read=FScale, write=FScale, default=100};
  __property bool CenterOnPage = {read=FCenterOnPage, write=FCenterOnPage, default=false};
  __property TPrintOutput PrintOutput = {read=FPrintOutput, write=FPrintOutput, default=poColor};
  __property int ImageWidth = {read=FImageWidth, write=FImageWidth, default=100};
  __property int ImageHeight = {read=FImageHeight, write=FImageHeight, default=100};
  __property int Copies = {read=FCopies, write=FCopies, default=1};
  __property Printers::TPrinterOrientation Orientation = {read=FOrientation, write=FOrientation, default=poPortrait};
};
//---------------------------------------------------------------------------
#endif
