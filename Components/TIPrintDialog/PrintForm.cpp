//===========================================================================
// Copyright © 2004 Ivan Johansen
// PrintForm.cpp
//===========================================================================
#include <vcl.h>
#pragma hdrstop
#include "Config.h"
#include "PrintForm.h"
#include "IPrintDialog.h"
#include <vector>
#include <cmath>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

class TPrinterHandle
{
  HANDLE hPrinter;
public:
  TPrinterHandle(const String &PrinterName)
  {
    if(!OpenPrinter(PrinterName.c_str(), &hPrinter, NULL))
      RaiseLastOSError();
  }

  ~TPrinterHandle()
  {
    if(!ClosePrinter(hPrinter))
      RaiseLastOSError();
  }

  operator HANDLE() const {return hPrinter;}
};

//---------------------------------------------------------------------------
__fastcall TPrintFrm::TPrintFrm(TComponent* Owner, TIPrintDialog *APrintDialog)
  : TForm(Owner), PrintDialog(APrintDialog)
{
  //Prevent flicker when paper is redrawn
  ControlStyle = ControlStyle << csOpaque;
  DoubleBuffered = true;
}
//---------------------------------------------------------------------------
void __fastcall TPrintFrm::EditKeyPress(TObject *Sender, char &Key)
{
  if(TEdit *Edit = dynamic_cast<TEdit*>(Sender))
    if(!std::isdigit(Key) && Key != VK_BACK)
      Key = 0;
}
//---------------------------------------------------------------------------
void __fastcall TPrintFrm::CheckBox1Click(TObject *Sender)
{
  Edit1->Enabled = !CheckBox1->Checked;
  Edit2->Enabled = !CheckBox1->Checked;
  DoPaintSample();
}
//---------------------------------------------------------------------------
void TPrintFrm::ResizePaper()
{
  double FontScale = Width / 449.0; //Divide by the original width in case it was scaled
  TPoint PaperCenter(373 * FontScale, 209 * FontScale);

  if(SysLocale.MiddleEast)
    PaperCenter.x = ClientWidth - PaperCenter.x;

  //DevMode->dmPaperWidth doesn't always work. Don't know why
  //Instead we create a DC ask that for the size
  Printer()->PrinterIndex = Printer()->PrinterIndex; //We need to set PrinterIndex to recreate DC for printer
  HDC PrinterDC = Printer()->Handle;
  double PaperWidth = GetDeviceCaps(PrinterDC, HORZSIZE);
  double PaperHeight = GetDeviceCaps(PrinterDC, VERTSIZE);

  HDC ScreenDC = GetDC(NULL);
  //LOGPIXELSX doesn't work. Always returns 96 for screen
  double xScale = (double)GetDeviceCaps(ScreenDC, HORZRES) / GetDeviceCaps(ScreenDC, HORZSIZE);
  double yScale = (double)GetDeviceCaps(ScreenDC, VERTRES) / GetDeviceCaps(ScreenDC, VERTSIZE);
  ReleaseDC(NULL, ScreenDC);

  double xyScale = yScale / xScale;
  int MaxPixelHeight = 144 * FontScale;
  int PixelWidth = 104 * FontScale;
  int PixelHeight = PaperHeight / PaperWidth * PixelWidth * xyScale;

  if(PixelHeight > MaxPixelHeight)
  {
    PixelHeight = MaxPixelHeight;
    PixelWidth = PaperWidth / PaperHeight * MaxPixelHeight / xyScale;
  }

  Image1->Height = PixelHeight;
  Image1->Picture->Bitmap->Height = PixelHeight;
  Shape1->Height = PixelHeight + 2;
  Shape2->Height = PixelHeight + 2;

  Image1->Top = PaperCenter.y - PixelHeight / 2;
  Shape1->Top = PaperCenter.y - PixelHeight / 2 - 1;
  Shape2->Top = PaperCenter.y - PixelHeight / 2 + 7;

  Image1->Width = PixelWidth;
  Image1->Picture->Bitmap->Width = PixelWidth;
  Shape1->Width = PixelWidth + 2;
  Shape2->Width = PixelWidth + 2;

  Image1->Left = PaperCenter.x - PixelWidth / 2;
  Shape1->Left = PaperCenter.x - PixelWidth / 2 - 1;
  Shape2->Left = PaperCenter.x - PixelWidth / 2 + 7;

  DoPaintSample();
}
//---------------------------------------------------------------------------
void TPrintFrm::DoPaintSample()
{
  double Scale = Edit4->Text.ToIntDef(100) / 100.0;

  //LOGPIXELSX doesn't work. Always returns 96 for screen
  HDC ScreenDC = GetDC(NULL);
  double xScale = GetDeviceCaps(ScreenDC, HORZSIZE) / (double)GetDeviceCaps(ScreenDC, HORZRES);
  double yScale = GetDeviceCaps(ScreenDC, VERTSIZE) / (double)GetDeviceCaps(ScreenDC, VERTRES);
  ReleaseDC(NULL, ScreenDC);

  //DevMode->dmPaperWidth doesn't always work. Don't know why
  //Instead we create a DC ask that for the size
  HDC PrinterDC = Printer()->Handle;
  double PaperWidth = GetDeviceCaps(PrinterDC, HORZSIZE);
  double PaperHeight = GetDeviceCaps(PrinterDC, VERTSIZE);

  if(CheckBox1->Checked)
  {
    UpDown1->Position = std::floor((PaperWidth - PrintDialog->ImageWidth * xScale * Scale) / 2 + 0.5);
    UpDown2->Position = std::floor((PaperHeight - PrintDialog->ImageHeight * yScale * Scale) / 2 + 0.5);
    Edit1->Text = UpDown1->Position;
    Edit2->Text = UpDown2->Position;
  }

  int Left = Edit1->Text.ToIntDef(0);
  int Top = Edit2->Text.ToIntDef(0);

  TRect PaintRect;
  PaintRect.Left = Left * Image1->Width / PaperWidth;
  PaintRect.Top = Top * Image1->Height / PaperHeight;
  PaintRect.Right = PaintRect.Left + PrintDialog->ImageWidth * xScale * Scale * Image1->Width / PaperWidth;
  PaintRect.Bottom = PaintRect.Top + PrintDialog->ImageHeight * yScale * Scale * Image1->Height / PaperHeight;


  Image1->Canvas->Brush->Color = clWhite;
  Image1->Canvas->Brush->Style = bsSolid;
  Image1->Canvas->FillRect(Image1->ClientRect);

  Image1->Canvas->DrawFocusRect(TRect(PaintRect.Left, PaintRect.Top, PaintRect.Right+1, PaintRect.Bottom+1));

  HRGN MyRgn = CreateRectRgn(PaintRect.Left, PaintRect.Top, PaintRect.Right, PaintRect.Bottom);
  SelectClipRgn(Image1->Canvas->Handle, MyRgn);
  DeleteObject(MyRgn);

  if(PrintDialog->OnPaintSample)
    PrintDialog->OnPaintSample(PrintDialog, Image1->Canvas, PaintRect, Image1->ClientRect);
}
//---------------------------------------------------------------------------
void __fastcall TPrintFrm::RadioGroup1Click(TObject *Sender)
{
  Printer()->Orientation = RadioGroup1->ItemIndex ? poLandscape : poPortrait;
  ResizePaper();
}
//---------------------------------------------------------------------------
void __fastcall TPrintFrm::Edit1Change(TObject *Sender)
{
  DoPaintSample();
}
//---------------------------------------------------------------------------
void __fastcall TPrintFrm::FormShow(TObject *Sender)
{
  if(PrintDialog->OnShow)
    PrintDialog->OnShow(PrintDialog);

  int Right = Edit1->Left + Edit1->Width;
  Edit1->Left = Label3->Left + std::max(Label3->Width, Label4->Width) + 10;
  Edit2->Left = Edit1->Left;
  Edit1->Width = Right - Edit1->Left;
  Edit2->Width = Right - Edit2->Left;

  Right = Edit3->Left + Edit3->Width;
  Edit3->Left = Label5->Left + Label5->Width + 10;
  Edit3->Width = Right - Edit3->Left;

  if(SysLocale.MiddleEast)
  {
    FlipChildren(true);
    BiDiMode = bdRightToLeft;
  }

  //WARNING: These names will be "%PrinterName%" on Windows NT, but "%PrinterName% on %Port%" on Windows 9x
  ComboBox1->Items->Assign(Printer()->Printers);
  ComboBox1->ItemIndex = Printer()->PrinterIndex;

  //This is necesarry when Arabic language is used. I have no idea why.
  UpDown4->Associate = NULL;
  UpDown4->Associate = Edit4;

  PrinterChanged();
}
//---------------------------------------------------------------------------
void __fastcall TPrintFrm::FormClose(TObject *Sender, TCloseAction &Action)
{
  if(PrintDialog->OnClose)
    PrintDialog->OnClose(PrintDialog);
}
//---------------------------------------------------------------------------
void TPrintFrm::PrinterChanged()
{
  wchar_t Device[255], Driver[255], Port[255];
  THandle DeviceMode;
  Printer()->GetPrinter(Device, Driver, Port, DeviceMode);
  TPrinterHandle PrinterHandle(Device); //Don't trust ComboBox1->Text

  DWORD BufferSize;
  GetPrinter(PrinterHandle, 2, NULL, 0, &BufferSize);

  std::vector<BYTE> Buffer(BufferSize);
  if(!GetPrinter(PrinterHandle, 2, &Buffer[0], Buffer.size(), &BufferSize))
    RaiseLastOSError();

  PRINTER_INFO_2 *PrinterInfo2 = reinterpret_cast<PRINTER_INFO_2*>(&Buffer[0]);
  Label11->Caption = PrinterInfo2->pDriverName;
  Label12->Caption = (PrinterInfo2->pLocation && *PrinterInfo2->pLocation) ? PrinterInfo2->pLocation : PrinterInfo2->pPortName;
  Label13->Caption = PrinterInfo2->pComment;

  Printer()->Orientation = RadioGroup1->ItemIndex ? poLandscape : poPortrait;
  ResizePaper();
}
//---------------------------------------------------------------------------
void __fastcall TPrintFrm::ComboBox1Change(TObject *Sender)
{
  Printer()->PrinterIndex = ComboBox1->ItemIndex;
  PrinterChanged();
}
//---------------------------------------------------------------------------
void __fastcall TPrintFrm::Button3Click(TObject *Sender)
{
  wchar_t Device[255], Driver[255], Port[255];
  THandle DeviceMode;
  Printer()->GetPrinter(Device, Driver, Port, DeviceMode);
  DEVMODE *DevMode = static_cast<DEVMODE*>(GlobalLock(reinterpret_cast<HGLOBAL>(DeviceMode)));

  TPrinterHandle PrinterHandle(Device);
  DocumentProperties(Handle, PrinterHandle, Device, DevMode, DevMode, DM_IN_BUFFER | DM_IN_PROMPT | DM_OUT_BUFFER);
  GlobalUnlock(reinterpret_cast<HGLOBAL>(DeviceMode));
  Printer()->PrinterIndex = Printer()->PrinterIndex; //We need to set PrinterIndex to recreate DC for printer
  RadioGroup1->ItemIndex = Printer()->Orientation == poLandscape;
  ResizePaper();
}
//---------------------------------------------------------------------------

