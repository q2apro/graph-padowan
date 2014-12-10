//===========================================================================
// Copyright © 2004 Ivan Johansen
// IPrintDialog.cpp
//===========================================================================
#include <vcl.h>
#pragma hdrstop
#include "IPrintDialog.h"
#include "PrintForm.h"
#include <memory>
#include "ICompCommon.h"

#pragma package(smart_init)

const double InchToMillimeter = 25.4;   //Constant to multiply a lenth in inches with to get millimetres
const double MillimeterToInch = 1/25.4; //Constant to multiply a length in millimetres to get inches
static TRegisterClass Dummy(__classid(TIPrintDialog));
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//
static inline void ValidCtrCheck(TIPrintDialog *)
{
  new TIPrintDialog(NULL);
}
//---------------------------------------------------------------------------
namespace Iprintdialog
{
  void __fastcall PACKAGE Register()
  {
     TComponentClass classes[1] = {__classid(TIPrintDialog)};
     RegisterComponents("IComp", classes, 0);
  }
}
//---------------------------------------------------------------------------
__fastcall TIPrintDialog::TIPrintDialog(TComponent* Owner)
  : TComponent(Owner), FOnShow(NULL), FOnClose(NULL), FOnPaintSample(NULL),
    FLeftMargin(25), FTopMargin(75), FScale(100), FCenterOnPage(false), FPrintOutput(poColor),
    FImageWidth(100), FImageHeight(100), PrintFrm(NULL), FCopies(1), FOrientation(poPortrait)
{
}
//---------------------------------------------------------------------------
bool TIPrintDialog::Execute()
{
  Printer()->PrinterIndex; //This will throw EPrinter if no default printer exists

  PrintFrm = new TPrintFrm(NULL, this);
  PrintFrm->Edit1->Text = String(FLeftMargin) / 100.0;
  PrintFrm->Edit2->Text = String(FTopMargin) / 100.0;
  PrintFrm->UpDown3->Position = std::max(1, FCopies);
  PrintFrm->UpDown4->Position = FScale;
  PrintFrm->CheckBox1->Checked = FCenterOnPage;
  PrintFrm->RadioGroup1->ItemIndex = FOrientation == poLandscape;
  PrintFrm->RadioGroup2->ItemIndex = FPrintOutput == poBlackWhite;
  Printer()->Orientation = FOrientation;

  if(PrintFrm->ShowModal() == mrOk)
  {
    FLeftMargin = PrintFrm->Edit1->Text.ToIntDef(0) * 100;
    FTopMargin = PrintFrm->Edit2->Text.ToIntDef(0) * 100;
    FCopies = PrintFrm->Edit3->Text.ToIntDef(1);
    FScale = PrintFrm->Edit4->Text.ToIntDef(100);
    FCenterOnPage = PrintFrm->CheckBox1->Checked;
    FPrintOutput = PrintFrm->RadioGroup2->ItemIndex ? poBlackWhite : poColor;
    FOrientation = PrintFrm->RadioGroup1->ItemIndex ? poLandscape  : poPortrait;

    delete PrintFrm;
    PrintFrm = NULL;
    return true;
  }

  delete PrintFrm;
  PrintFrm = NULL;
  return false;
}
//---------------------------------------------------------------------------
TRect TIPrintDialog::GetDrawingArea()
{
  //Calculate margins in inches;
  double LeftMargin   = FLeftMargin /100.0;
  double TopMargin    = FTopMargin / 100.0;

  HDC hPrnDC = Printer()->Handle;
  //Get x-resolution and y-resolution
  double PrnLogPixelsX = GetDeviceCaps(hPrnDC, LOGPIXELSX) / InchToMillimeter;
  double PrnLogPixelsY = GetDeviceCaps(hPrnDC, LOGPIXELSY) / InchToMillimeter;

  //LOGPIXELSX doesn't work. Always returns 96 for screen
  HDC ScreenDC = GetDC(NULL);
  double ScreenLogPixelsX = (double)GetDeviceCaps(ScreenDC, HORZRES) / GetDeviceCaps(ScreenDC, HORZSIZE);
  double ScreenLogPixelsY = (double)GetDeviceCaps(ScreenDC, VERTRES) / GetDeviceCaps(ScreenDC, VERTSIZE);
  ReleaseDC(NULL, ScreenDC);

  double xScale = PrnLogPixelsX / ScreenLogPixelsX;
  double yScale = PrnLogPixelsY / ScreenLogPixelsY;

  // Locate the upper left corner of the printable area
  TPoint pt(0,0);
  pt.x = GetDeviceCaps(hPrnDC, PHYSICALOFFSETX);
  pt.y = GetDeviceCaps(hPrnDC, PHYSICALOFFSETY);

  double Left = PrnLogPixelsX * LeftMargin - pt.x;
  double Top = PrnLogPixelsY * TopMargin - pt.y;
  double Right = Left + xScale * FImageWidth * Scale / 100.0;
  double Bottom = Top + yScale * FImageHeight * Scale / 100.0;

  //Return drawing area
  //(0,0) is the top left corner of the printable area
  return TRect(Left, Top, Right, Bottom);
}
//---------------------------------------------------------------------------

