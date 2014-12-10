//===========================================================================
// Copyright © 2003 Ivan Johansen
// IPageSetup.cpp
//===========================================================================
#include <vcl.h>
#pragma hdrstop
#include "IPageSetup.h"
#include <Dlgs.h> //Contains dialog header information
#include <algorithm>
#include "ICompCommon.h"
#pragma package(smart_init)
#pragma warn -8104

#define INCH_TO_MM 25.4   //Constant to multiply a lenth in inches with to get millimetres
#define MM_TO_INCH (1/25.4) //Constant to multiply a length in millimetres to get inches

//Static varibles containing pointer to activated dialog and its canvas
TIPageSetupDialog *TIPageSetupDialog::Dialog=NULL;
static TRegisterClass Dummy(__classid(TIPageSetupDialog));
//---------------------------------------------------------------------------
//Checks if there is no pure virtual functions; Never called
static inline TIPageSetupDialog *ValidCtrCheck()
{
  return new TIPageSetupDialog(NULL);
}
//---------------------------------------------------------------------------
namespace Ipagesetup
{
  void __fastcall PACKAGE Register()
  {
    TComponentClass classes[1] = {__classid(TIPageSetupDialog)};
    RegisterComponents("IComp", classes, 0);
  }
}
//---------------------------------------------------------------------------
//Constructor allocates memory and initializes variables
__fastcall TIPageSetupDialog::TIPageSetupDialog(TComponent* Owner)
	: TCommonDialog(Owner)
{
  FMinMargins = new TPageSetupRect(Owner);
  FMargins = new TPageSetupRect(Owner);
  FUnits = psuMillimeters;
  FOptions.Clear();
  FOptions << ::psoDefaultMinMargins << ::psoMargins;
  FOnPaintMargins=NULL;
  FOnPaintText=NULL;
  FOnPaintStamp=NULL;
	FOnPaintAdress=NULL;
  FPaperWidth =0;
  FPaperHeight = 0;
  FHandle = NULL;
}
//---------------------------------------------------------------------------
//Destructor deallocates memory allocated by constructor
__fastcall TIPageSetupDialog::~TIPageSetupDialog()
{
  // release dynamic allocations
  delete FMinMargins;
  delete FMargins;
}
//---------------------------------------------------------------------------
//This function shows the dialog
//Returns true if the user presses OK and false if user presses Cancel
bool __fastcall TIPageSetupDialog::Execute(HWND ParentWnd)
{
  PAGESETUPDLG psd;
  wchar_t device[255];
  wchar_t driver[255];
  wchar_t port[255];
  unsigned devmode;	// help file is wrong (it says "int")

  //Check if a dialog is already in use
  if(Dialog)
	throw EPageSetup("PageSetupDialog is already in use!", 0);

  if(Printer()->Printing)
	throw EPageSetup("Printing in progress!", 0);

  try
  {
	//Set static variable to point to this object and allocate memory for canvas
	Dialog=this;

	//Get printer information
	Printer()->GetPrinter(device, driver, port, devmode);

	//Initialize the print setup dialog structure
	psd.lStructSize = sizeof(psd);
	if(ParentWnd)
	  psd.hwndOwner = ParentWnd;
	else
	  psd.hwndOwner = Application->MainForm ? Application->Handle : NULL;
	//hDevMode and hDevNames have to be 0 when default should be returned
	psd.hDevMode = (HGLOBAL)devmode;
	psd.hDevNames = 0;
	psd.Flags = GetOptionFlags();
	psd.ptPaperSize = TPoint(FPaperWidth,FPaperHeight);
	psd.rtMinMargin = FMinMargins->Rect;
	psd.rtMargin = FMargins->Rect;
	psd.hInstance = 0;
	psd.lCustData = 0;
	psd.lpfnPageSetupHook = 0;
	psd.lpfnPagePaintHook = 0;
	psd.lpPageSetupTemplateName = 0;
	psd.hPageSetupTemplate = 0;

	//Check if the printers minimum margins should be used as the minimum
	//margins the user can enter in the dialog
	if(FOptions.Contains(::psoDefaultMinMargins) && !FOptions.Contains(::psoMinMargins))
	{
	  psd.Flags |= PSD_MINMARGINS; //Make the dialog use MinMargins
	  psd.rtMinMargin = GetPrinterMargins();
	}

	//If minimum margins are used
	//Make sure that the shown margins are not less than the minimum margins.
	//Else some printer drivers may give an error (1006h)
    if(psd.Flags & ~PSD_MINMARGINS)
    {
	  if(psd.rtMargin.left < psd.rtMinMargin.left)
        psd.rtMargin.left = psd.rtMinMargin.left;
      if(psd.rtMargin.top < psd.rtMinMargin.top)
        psd.rtMargin.top = psd.rtMinMargin.top;
      if(psd.rtMargin.right < psd.rtMinMargin.right)
        psd.rtMargin.right = psd.rtMinMargin.right;
      if(psd.rtMargin.bottom < psd.rtMinMargin.bottom)
        psd.rtMargin.bottom = psd.rtMinMargin.bottom;
    }

    //Set the hook procedure witch should be called when any window message
    //should be handled
    psd.lpfnPagePaintHook=&DialogHook;
    psd.lpfnPageSetupHook=&DialogHook;

    //Execute the dialog (returns false on cancel)
    //TaskModaldDialog() makes sure that the help key is handled correct

    if(!TaskModalDialog(&PageSetupDlg,&psd))
    {
      //Get error code
	  int Error = CommDlgExtendedError();
      if(Error)
        throw EPageSetup("Error while showing page setup dialog box.", Error);
      Dialog = NULL;
      FHandle = NULL;
      return false;
    }

    //Get the device, driver, and port from the returned data
    if(psd.hDevNames)
    {
      DEVNAMES *dn = (DEVNAMES*) ::GlobalLock(psd.hDevNames);
	  String Driver = (char*)dn + dn->wDriverOffset;
	  String Device = (char*)dn + dn->wDeviceOffset;
	  String Port = (char*)dn + dn->wOutputOffset;

	  //Set the printer to the device, driver, port, and device mode returned
	  Printer()->SetPrinter(Device.c_str(), Driver.c_str(), Port.c_str(), (int) psd.hDevMode);

      //TPrinter takes care of freeing the hDevMode global memory (or
      //so I believe and very much hope) but we still have to free
      //the device names memory
      ::GlobalUnlock(psd.hDevNames);
      ::GlobalFree(psd.hDevNames);
    }

    //Save the returned values back into the properties
    SetOptionFlags(psd.Flags);
    FMinMargins->Rect = psd.rtMinMargin;
    FPaperWidth = psd.ptPaperSize.x;
    FPaperHeight = psd.ptPaperSize.y;
    FMargins->Rect = psd.rtMargin;
  }
  catch(...)
  {
    //Reset pointers
    Dialog = NULL;
    FHandle = NULL;
    throw;
  }

  //Reset pointers
  Dialog = NULL;
  FHandle = NULL;

  // Return success
  return true;
}
//---------------------------------------------------------------------------
//This function converts the options from the user format set by the
//Options property to the format used by the TPageSetupDlg structure
//passed to PageSetupDlg()
//It also enables the hook to be called on painting and other messages
unsigned int __fastcall TIPageSetupDialog::GetOptionFlags(void)
{
  unsigned int Flags = 0;
  if (FOptions.Contains(::psoDisableMargins)) Flags |= PSD_DISABLEMARGINS;
  if (FOptions.Contains(::psoDisableOrientation)) Flags |= PSD_DISABLEORIENTATION;
  if (FOptions.Contains(::psoDisablePagePainting)) Flags |= PSD_DISABLEPAGEPAINTING;
  if (FOptions.Contains(::psoDisablePaper)) Flags |= PSD_DISABLEPAPER;
  if (FOptions.Contains(::psoDisablePrinter)) Flags |= PSD_DISABLEPRINTER;
  if (FOptions.Contains(::psoMargins)) Flags |= PSD_MARGINS;
  if (FOptions.Contains(::psoMinMargins)) Flags |= PSD_MINMARGINS;
  if (FOptions.Contains(::psoNoWarning)) Flags |= PSD_NOWARNING;
  if (FOptions.Contains(::psoShowHelp)) Flags |= PSD_SHOWHELP;
  if (FUnits == psuMillimeters) Flags |= PSD_INHUNDREDTHSOFMILLIMETERS;
  if (FUnits == psuInches) Flags |= PSD_INTHOUSANDTHSOFINCHES;

  Flags |= PSD_ENABLEPAGEPAINTHOOK;
  Flags |= PSD_ENABLEPAGESETUPHOOK;

  return Flags;
}
//---------------------------------------------------------------------------
//This function converts the options from the format returned in the
//TPageSetupDlg structure from PageSetupDlg() to user format returned by
//the Options property
void __fastcall TIPageSetupDialog::SetOptionFlags(int Flags)
{
  FOptions.Clear();
  FUnits = psuInches;
  if (Flags & PSD_DEFAULTMINMARGINS) FOptions << ::psoDefaultMinMargins;
  if (Flags & PSD_DISABLEMARGINS) FOptions << ::psoDisableMargins;
  if (Flags & PSD_DISABLEORIENTATION) FOptions << ::psoDisableOrientation;
  if (Flags & PSD_DISABLEPAGEPAINTING) FOptions << ::psoDisablePagePainting;
  if (Flags & PSD_DISABLEPAPER) FOptions << ::psoDisablePaper;
  if (Flags & PSD_DISABLEPRINTER) FOptions << ::psoDisablePrinter;
  if (Flags & PSD_MARGINS) FOptions << ::psoMargins;
  if (Flags & PSD_MINMARGINS) FOptions << ::psoMinMargins;
  if (Flags & PSD_NOWARNING) FOptions << ::psoNoWarning;
  if (Flags & PSD_SHOWHELP) FOptions << ::psoShowHelp;
  if (Flags & PSD_INHUNDREDTHSOFMILLIMETERS) FUnits = psuMillimeters;
  if (Flags & PSD_INTHOUSANDTHSOFINCHES) FUnits = psuInches;
}
//---------------------------------------------------------------------------
//Center the window with the given handle on the screen
void CenterWindow(HWND Wnd)
{
  TRect Rect;
  Forms::TMonitor *Monitor;
  GetWindowRect(Wnd, &Rect);
  if(Application->MainForm != NULL)
	Monitor = Application->MainForm->Monitor;
  else
    Monitor = Screen->Monitors[0];
  SetWindowPos(Wnd, 0,
    Monitor->Left + ((Monitor->Width - Rect.Right + Rect.Left) / 2),
    Monitor->Top + ((Monitor->Height - Rect.Bottom + Rect.Top) / 3),
    0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
}
//---------------------------------------------------------------------------
//Hook procedure: called by the dialog when it is created and destroyed.
//Also called when the sample page should be drawn. An event is called
//witch enables the user to either draw the sample page or to tell
//Windows to do it
UINT APIENTRY TIPageSetupDialog::DialogHook(
    HWND hdlg,	   // handle to the dialog box window
    UINT uiMsg,	   // message identifier
    WPARAM wParam, // message parameter
	LPARAM lParam) // message parameter
{
  //Static variable containing pixel coordinates for the sample paper
  static TRect PaperSize;
  TCanvas *Canvas;
  Dialog->FHandle = hdlg; //Save window handle to dialog for use by events
  switch(uiMsg)
  {
	case WM_PSD_PAGESETUPDLG: return false;
	case WM_PSD_FULLPAGERECT:
		PaperSize=*(TRect*) lParam; //Save paper size in static variable
		return false;
	case WM_PSD_MINMARGINRECT: return false;
	case WM_PSD_MARGINRECT:
		if(!Dialog->FOnPaintMargins)
          return false;
        Canvas=new TCanvas;
        Canvas->Handle=(HDC)wParam; //Set context handle and call event
        Dialog->FOnPaintMargins(Dialog,Canvas,*(TRect*)(LPARAM)(LPRECT)lParam,PaperSize);
        delete Canvas;
        return true;
    case WM_PSD_GREEKTEXTRECT:
        if(!Dialog->FOnPaintText)
          return false;
        Canvas=new TCanvas;
        Canvas->Handle=(HDC)wParam; //Set context handle and call event
        Dialog->FOnPaintText(Dialog,Canvas,*(TRect*)(LPARAM)(LPRECT)lParam,PaperSize);
        delete Canvas;
        return true;
    case WM_PSD_ENVSTAMPRECT:
        if(!Dialog->FOnPaintStamp)
          return false;
        Canvas->Handle=(HDC)wParam; //Set context handle and call event
        Dialog->FOnPaintStamp(Dialog,Canvas,*(TRect*)(LPARAM)(LPRECT)lParam,PaperSize);
        return true;
    case WM_PSD_YAFULLPAGERECT:
        if(!Dialog->FOnPaintAdress)
          return false;
        Canvas->Handle=(HDC)wParam; //Set context handle and call event
		Dialog->FOnPaintAdress(Dialog,Canvas,*(TRect*)(LPARAM)(LPRECT)lParam,PaperSize);
        return true;
    case WM_INITDIALOG:
      CenterWindow(hdlg);           //Center window on screen
      Dialog->DoShow();             //Call event
      return true;
    case WM_NCDESTROY:
      Dialog->DoClose();            //Call event
      return false;
    default: return false;
  }
}
//---------------------------------------------------------------------------
//This function returns the dots per inch (dpi) printed on the paper.
//It is returned as dpi vertical and horizontal
TPoint TIPageSetupDialog::GetResolution()
{
  TPoint Resolution;
  Resolution.x = GetDeviceCaps(Printer()->Handle, LOGPIXELSX);
  Resolution.y = GetDeviceCaps(Printer()->Handle, LOGPIXELSY);
  return Resolution;
}
//---------------------------------------------------------------------------
//Returns the minimum margins defined by the printer in 100th of millimetres
//or 1000th of inches
TRect TIPageSetupDialog::GetPrinterMargins()
{
  TRect Offset;
  TPrinter *Prn=Printer();
  Offset.Left=GetDeviceCaps(Prn->Handle,HORZSIZE)*GetDeviceCaps(Prn->Handle,PHYSICALOFFSETX)*100/GetDeviceCaps(Prn->Handle,PHYSICALWIDTH);
  Offset.Top=GetDeviceCaps(Prn->Handle,VERTSIZE)*GetDeviceCaps(Prn->Handle,PHYSICALOFFSETY)*100/GetDeviceCaps(Prn->Handle,PHYSICALHEIGHT);
  Offset.Right=GetDeviceCaps(Prn->Handle,PHYSICALWIDTH)*INCH_TO_MM*100/Resolution.x-GetDeviceCaps(Prn->Handle,HORZSIZE)*100-Offset.Left;
  Offset.Bottom=GetDeviceCaps(Prn->Handle,PHYSICALHEIGHT)*INCH_TO_MM*100/Resolution.y-GetDeviceCaps(Prn->Handle,VERTSIZE)*100-Offset.Top;
  if(Units==psuInches)
  {
    Offset.Left*=10*MM_TO_INCH;
    Offset.Top*=10*MM_TO_INCH;
    Offset.Right*=10*MM_TO_INCH;
    Offset.Bottom*=10*MM_TO_INCH;
  }
  return Offset;
}
//---------------------------------------------------------------------------
//Returns coordinates of the drawing area from the margins in the dialog
//Microsoft knowlege base: Q122037
TRect TIPageSetupDialog::GetDrawingArea()
{
  //Calculate margins in inches;
  double LeftMargin   = Margins->Left   * (Units==psuInches ? 1000 : MM_TO_INCH/100);
  double TopMargin    = Margins->Top    * (Units==psuInches ? 1000 : MM_TO_INCH/100);
  double RightMargin  = Margins->Right  * (Units==psuInches ? 1000 : MM_TO_INCH/100);
  double BottomMargin = Margins->Bottom * (Units==psuInches ? 1000 : MM_TO_INCH/100);

  // Init our pt struct in case escape not supported
  TPoint pt(0,0);
  HDC hPrnDC=Printer()->Handle;

  // Locate the upper left corner of the printable area
  pt.x = GetDeviceCaps(hPrnDC, PHYSICALOFFSETX);
  pt.y = GetDeviceCaps(hPrnDC, PHYSICALOFFSETY);

  //Get x-resolution and y-resolution
  int LogPixelsX = GetDeviceCaps(hPrnDC, LOGPIXELSX);
  int LogPixelsY = GetDeviceCaps(hPrnDC, LOGPIXELSY);

  // Figure out how much you need to offset output to produce the left
  // and top margins for the output in the printer. Note the
  // use of the "max" macro. It is possible that you are asking for
  // margins that are not possible on this printer. For example, the HP
  // LaserJet has a 0.25" unprintable area so we cannot get margins of
  // 0.1".

  int xOffset = std::max(int(LogPixelsX * LeftMargin - pt.x), 0);
  int yOffset = std::max(int(LogPixelsY * TopMargin - pt.y), 0);

  // Get the size of the printable area
  TPoint PT;
  PT.x = GetDeviceCaps(hPrnDC, PHYSICALWIDTH);
  PT.y = GetDeviceCaps(hPrnDC, PHYSICALHEIGHT);

  //Get right and bottom margins
  int xOffsetOfRightMargin = PT.x - LogPixelsX * RightMargin - pt.x;
  int yOffsetOfBottomMargin = PT.y - LogPixelsY * BottomMargin - pt.y;

  //Return drawing area
  //(0,0) is the top left corner of the printable area
  return TRect(xOffset,yOffset,xOffsetOfRightMargin,yOffsetOfBottomMargin);
}
//---------------------------------------------------------------------------
EPageSetup::EPageSetup(String Msg, int Error)
  : Exception(Msg), FErrorCode(Error)
{
}
//---------------------------------------------------------------------------
void __fastcall TPageSetupRect::Change(int Index, long Value)
{
  if(Value < 0)
    Value = 0;
  switch(Index)
  {
    case 0:
      Rect.Left = Value;
      break;
    case 1:
      Rect.Top = Value;
      break;
    case 2:
      Rect.Right = Value;
      break;
    case 3:
      Rect.Bottom = Value;
      break;
  }
}
//---------------------------------------------------------------------------

