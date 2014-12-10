/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2007 Ivan Johansen
 *
 * Graph is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
#include "Graph.h"
#pragma hdrstop
#include "Unit18.h"
#include "ConfigRegistry.h"
//---------------------------------------------------------------------------
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TForm18::TForm18(TComponent* Owner)
  : TForm(Owner)
{
  TranslateProperties(this);
  SetAccelerators(this);
  ScaleForm(this);

  ResizeControl(Edit1, Label1, Label2);
  ResizeControl(Edit2, Label1, Label2);
  ComboBox1->Left = Label3->Left + Label3->Width + 5;
  PageControl1->ActivePage = TabSheet1;
  FlipForm(this);
}
//---------------------------------------------------------------------------
void __fastcall TForm18::EditKeyPress(TObject *Sender, char &Key)
{
  if((Key < '0' || Key > '9') && Key != VK_BACK)
    Key = 0;
}
//---------------------------------------------------------------------------
void TForm18::EditOptions(TImageOptions &Options, TImageFormat ImageFormat, int AWidth, int AHeight)
{
  ScreenWidth = AWidth;
  ScreenHeight = AHeight;
  CustomWidth = Options.CustomWidth;
  CustomHeight = Options.CustomHeight;

  RadioGroup3->ItemIndex = Options.UseCustomSize;
  Edit1->Text = Options.UseCustomSize ? CustomWidth : ScreenWidth;
  Edit2->Text = Options.UseCustomSize ? CustomHeight : ScreenHeight;
  Edit1->Enabled = Options.UseCustomSize;
  Edit2->Enabled = Options.UseCustomSize;
  Edit3->Text = Options.Jpeg.Quality;
  RadioGroup1->ItemIndex = Options.Jpeg.ProgressiveEncoding;
  RadioGroup2->ItemIndex = Options.Pdf.Orientation;
  ComboBox1->ItemIndex = Options.Pdf.PaperSize;

  switch(ImageFormat)
  {
    case ifJpeg:
      TabSheet2->TabVisible = true;
      TabSheet3->TabVisible = false;
      break;

    case ifPdf:
      TabSheet2->TabVisible = false;
      TabSheet3->TabVisible = true;
      break;

    case ifMetafile:
    case ifBitmap:
    case ifPng:
    case ifSvg:
    default:
      TabSheet2->TabVisible = false;
      TabSheet3->TabVisible = false;
      break;
  }

  if(!ShowModal())
    return;

  Options.CustomWidth = Edit1->Text.ToInt();
  Options.CustomHeight = Edit2->Text.ToInt();
  Options.UseCustomSize = RadioGroup3->ItemIndex;
  Options.Jpeg.Quality = TrackBar1->Position;
  Options.Jpeg.ProgressiveEncoding = RadioGroup1->ItemIndex;
  Options.Pdf.Orientation = RadioGroup2->ItemIndex ? poLandscape : poPortrait;
  Options.Pdf.PaperSize = static_cast<TPaperSize>(ComboBox1->ItemIndex);

  if(CheckBox1->Checked)
    Options.SaveSettings();
}
//---------------------------------------------------------------------------
void __fastcall TForm18::TrackBar1Change(TObject *Sender)
{
  Edit3->Text = TrackBar1->Position;
}
//---------------------------------------------------------------------------
void __fastcall TForm18::Edit3Change(TObject *Sender)
{
  try
  {
    TrackBar1->Position = Edit3->Text.ToInt();
  }
  catch(...)
  { //Ignore errors
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm18::RadioGroup3Click(TObject *Sender)
{
  Edit1->Enabled = RadioGroup3->ItemIndex == 1;
  Edit2->Enabled = RadioGroup3->ItemIndex == 1;
  if(RadioGroup3->ItemIndex == 1)
  {
    Edit1->Text = CustomWidth;
    Edit2->Text = CustomHeight;
  }
  else
  {
    CustomWidth = Edit1->Text.ToInt();
    CustomHeight = Edit2->Text.ToInt();
    Edit1->Text = ScreenWidth;
    Edit2->Text = ScreenHeight;
  }
}
//---------------------------------------------------------------------------
void TImageOptions::LoadSettings()
{
  TConfigRegistry Registry;
  if(Registry.OpenKey(REGISTRY_KEY "\\SaveAsImage"))
  {
    CustomWidth = Registry.Read("Width", 500);
    CustomHeight = Registry.Read("Height", 500);
    UseCustomSize = Registry.Read("UseCustomSize", false);
    Jpeg.Quality = Registry.Read("JpegQuality", 90);
    Jpeg.ProgressiveEncoding = Registry.Read("ProgressiveEncoding", false);
    Pdf.Orientation = Registry.Read("Orientation", poPortrait);
    Pdf.PaperSize = Registry.Read("PaperSize", psA4);
  }
}
//---------------------------------------------------------------------------
void TImageOptions::SaveSettings() const
{
  TConfigRegistry Registry;
  if(Registry.CreateKey(REGISTRY_KEY "\\SaveAsImage"))
  {
    Registry.Write("Width", CustomWidth);
    Registry.Write("Height", CustomHeight);
    Registry.Write("UseCustomSize", UseCustomSize);
    Registry.Write("JpegQuality", Jpeg.Quality);
    Registry.Write("ProgressiveEncoding", Jpeg.ProgressiveEncoding);
    Registry.Write("Orientation", Pdf.Orientation);
    Registry.Write("PaperSize", Pdf.PaperSize);
  }
}
//---------------------------------------------------------------------------



