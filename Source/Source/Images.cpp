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
#include "Images.h"
#include <PngImage.hpp>
#include "pdflib.hpp"
#include "VersionInfo.h"
#include "Unit1.h"
#include "EmfParser.h"
#include "SvgWriter.h"
#include <Jpeg.hpp>
//---------------------------------------------------------------------------
void SaveAsPdf(const std::string &FileName, Graphics::TBitmap *Bitmap, const std::string &Title, const std::string &Subject, Printers::TPrinterOrientation Orientation)
{
  std::auto_ptr<TMemoryStream> Stream(new TMemoryStream);
  Bitmap->PixelFormat = pf8bit;
  std::auto_ptr<TPngImage> PngImage(new TPngImage);
  PngImage->Assign(Bitmap);
  PngImage->SaveToStream(Stream.get());
  double Width = Orientation == poPortrait ? a4_width : a4_height;
  double Height = Orientation == poPortrait ? a4_height : a4_width;

  try
  {
    PDFlib p;
    if (p.begin_document(FileName, "") == -1)
      throw ESaveError(LoadRes(RES_FILE_ACCESS, ToUString(FileName)));

    std::wstring Creator = NAME " " + TVersionInfo().StringValue(L"ProductVersion");
    p.set_info("Creator", ::ToString(Creator));
    p.set_info("Author", ::ToString(Creator));
    p.set_info("Title", Title);
    p.set_info("Subject", Subject);
    p.begin_page_ext(Width, Height, "");
    int image;
    p.create_pvf("/pvf/image/Temp.png" , Stream->Memory, Stream->Size, "");
    if((image = p.load_image("auto", "/pvf/image/Temp.png", "")) == -1)
      throw ESaveError(L"Error: Couldn't read logical image file when creating PDF file.");

    p.fit_image(image, Width*0.05, Height*0.05, "boxsize {" + ToString(Width*0.9) + " " + ToString(Height*0.9) + "} position 50 fitmethod meet");
    p.close_image(image);
    p.end_page_ext("");
    p.end_document("");
  }
  catch(PDFlib::Exception &ex)
  {
    throw ESaveError(L"PDFlib exception: [" + String(ex.get_errnum()) + L"] " +
      ToUString(ex.get_apiname()) + L": " + ToUString(ex.get_errmsg()));
  }
}
//---------------------------------------------------------------------------
void SaveAsImage(const String &FileName, const TImageOptions &ImageOptions)
{
  int ImageFileType;
  String FileExt = ExtractFileExt(FileName);
  if(FileExt.CompareIC(".emf") == 0)
    ImageFileType = ifMetafile;
  else if(FileExt.CompareIC(".bmp") == 0)
    ImageFileType = ifBitmap;
  else if(FileExt.CompareIC(".png") == 0)
    ImageFileType = ifPng;
  else if(FileExt.CompareIC(".jpeg") == 0 || FileExt.CompareIC(".jpg") == 0)
    ImageFileType = ifJpeg;
  else if(FileExt.CompareIC(".pdf") == 0)
    ImageFileType = ifPdf;
  else if(FileExt.CompareIC(".svg") == 0)
    ImageFileType = ifSvg;
  else
    throw ESaveError("Unknown file type");

  return SaveAsImage(FileName, ImageFileType, ImageOptions);
}
//---------------------------------------------------------------------------
void SaveAsImage(const String &FileName, int ImageFileType, const TImageOptions &ImageOptions)
{
  try
  {
    bool SameSize = !ImageOptions.UseCustomSize;
    int Width = SameSize ? Form1->Image1->Width : ImageOptions.CustomWidth;
    int Height = SameSize ? Form1->Image1->Height : ImageOptions.CustomHeight;

    //Show save icon in status bar
    Form1->SetStatusIcon(iiSave);
//    TCallOnRelease Dummy(&Form1->SetStatusIcon, -1); // Causes internal compiler error
    Form1->Draw.Wait();

    if(ImageFileType == ifMetafile || ImageFileType == ifSvg)
    {
      std::auto_ptr<TMetafile> Metafile(new TMetafile);
      Metafile->Width = Width-1;
      Metafile->Height = Height-1;

      std::auto_ptr<TMetafileCanvas> Meta(new TMetafileCanvas(Metafile.get(), 0));
      TData MetaData(Form1->Data);
      TDraw FileDraw(Meta.get(), SameSize ? &Form1->Data : &MetaData, false, "Metafile DrawThread");
      FileDraw.SetArea(TRect(0, 0, Width, Height));  //Set drawing area

      FileDraw.DrawAll();
      if(SameSize)
        FileDraw.Wait(); //Wait if using Data object
      else
        while(FileDraw.Updating())
        { //Process messages while waiting for draw thread to finish
          Sleep(100);
          Application->ProcessMessages();
        }
      Meta.reset();
      if(ImageFileType == ifSvg)
      {
        TEmfParser EmfParser;
        std::ofstream File(FileName.c_str());
        TSvgWriter SvgWriter(File);
        EmfParser.Parse(reinterpret_cast<HENHMETAFILE>(Metafile->Handle), SvgWriter);
      }
      else
        Metafile->SaveToFile(FileName);
    }
    else
    {
      std::auto_ptr<Graphics::TBitmap> Bitmap(new Graphics::TBitmap);
      Bitmap->Width = Width;
      Bitmap->Height = Height;
      TRect Rect(0, 0, Width, Height);
      if(SameSize)
        Bitmap->Canvas->CopyRect(Rect, Form1->Image1->Picture->Bitmap->Canvas, Rect);
      else
      {
        //Make sure background is drawn
        Bitmap->Canvas->Brush->Style = bsSolid;
        Bitmap->Canvas->Brush->Color = Form1->Data.Axes.BackgroundColor;
        Bitmap->Canvas->FillRect(Rect);

        TData FileData(Form1->Data);
        TDraw FileDraw(Bitmap->Canvas, &FileData, false, "Save as image DrawThread");
        FileDraw.SetArea(Rect);  //Set drawing area
        FileDraw.DrawAll();
        while(FileDraw.Updating())
        {
          Sleep(100);
          Application->ProcessMessages();
        }
      }

      switch(ImageFileType)
      {
        case ifBitmap:
          SaveCompressedBitmap(Bitmap.get(), Rect, FileName);
          break;

        case ifPng:
        {
          Bitmap->PixelFormat = pf8bit; //Change bitmap to 8 bit to keep file size down
          std::auto_ptr<TPngImage> PngImage(new TPngImage);
          PngImage->Assign(Bitmap.get());
          PngImage->SaveToFile(FileName);
          break;
        }
        case ifJpeg:
        {
          std::auto_ptr<TJPEGImage> Image(new TJPEGImage);
          Image->Assign(Bitmap.get());
          Image->CompressionQuality = ImageOptions.Jpeg.Quality;
          Image->ProgressiveEncoding = ImageOptions.Jpeg.ProgressiveEncoding;
          Image->Compress();
          Image->SaveToFile(FileName);
          break;
        }

        case ifPdf:
          SaveAsPdf(::ToString(FileName), Bitmap.get(), ::ToString(Form1->Data.GetFileName()), ::ToString(Form1->Data.Axes.Title), ImageOptions.Pdf.Orientation);
      }
    }
    Form1->SetStatusIcon(-1); //Replaces TCallOnRelease
  }
  catch(EOutOfResources &E)
  {
    Form1->SetStatusIcon(-1);
    throw ESaveError(LoadStr(RES_OUT_OF_RESOURCES));
  }
}
//---------------------------------------------------------------------------
bool SaveCompressedBitmap(Graphics::TBitmap *Bitmap, const TRect &Rect, const String &FileName)
{
  BITMAPFILEHEADER FileHeader;
  BITMAPINFOHEADER BitmapHeader;
  std::vector<RGBQUAD> Colors;
  std::vector<char> Data; //Area with compressed bitmap data
  CompressBitmap(Bitmap, Rect, Colors, Data);
  FillBitmapInfoHeader(BitmapHeader, Bitmap, Rect, Colors.size(), Data.size());

  FileHeader.bfType = 0x4D42; //Initialize file header; must be 'BM'; Remember little endian
  FileHeader.bfSize = sizeof(FileHeader)+sizeof(BitmapHeader)+Colors.size()*sizeof(RGBQUAD)+Data.size(); //File size
  FileHeader.bfReserved1 = 0;
  FileHeader.bfReserved2 = 0;
  FileHeader.bfOffBits = sizeof(FileHeader)+sizeof(BitmapHeader)+Colors.size()*sizeof(RGBQUAD);//Offset to compressed data

  //Create new text file; Erase if exists
  std::ofstream out(FileName.c_str(),std::ios::out|std::ios::binary);
  //If open not succesfull
  if(!out)
    return false;
  out.write((char*)&FileHeader, sizeof(FileHeader)); //Save Save file header
  out.write((char*)&BitmapHeader, sizeof(BitmapHeader)); //Save bitmap header
  out.write((char*)&Colors[0], Colors.size()*sizeof(TColor)); //Save palette
  out.write((char*)&Data[0], Data.size()); //Save compressed data
  out.close(); //Close file
  return true; //Return sucess
}
//---------------------------------------------------------------------------
inline bool operator==(RGBQUAD Color1, RGBQUAD Color2)
{
  return *reinterpret_cast<unsigned*>(&Color1) == *reinterpret_cast<unsigned*>(&Color2);
}
//---------------------------------------------------------------------------
inline bool operator!=(RGBQUAD Color1, RGBQUAD Color2)
{
  return *reinterpret_cast<unsigned*>(&Color1) != *reinterpret_cast<unsigned*>(&Color2);
}
//---------------------------------------------------------------------------

//==  CountColors  =====================================================
// Count number of unique R-G-B triples in a pf24bit Bitmap.
//
// Use 2D array of TBits objects -- when (R,G) combination occurs
// for the first time, create 256-bit array of bits in blue dimension.
// So, overall this is a fairly sparse matrix for most pictures.
// Tested with pictures created with a known number of colors, including
// a specially constructed image with 1024*1024 = 1,048,576 colors.
struct pRGBTripleArray
{
  unsigned char rgbtRed;
  unsigned char rgbtGreen;
  unsigned char rgbtBlue;
};

int CountColors(Graphics::TBitmap *Bitmap)
{
  int i;
  int j;
  int k;
  pRGBTripleArray *rowIn;
  TBits* Flags[256][256];

  // Be sure bitmap is 24-bits/pixel
  if(Bitmap->PixelFormat != pf24bit)
    return -1;

  // Clear 2D array of TBits objects
  for(j = 0; j <= 255;j++)
    for(i = 0;i <= 255;i++)
      Flags[i][j] = NULL;

  // Step through each scanline of image
  for(j = 0;j < Bitmap->Height;j++)
  {
    rowIn = (pRGBTripleArray*)Bitmap->ScanLine[j];
    for(i = 0;i < Bitmap->Width;i++)
    {
      if(!Flags[rowIn[i].rgbtRed][rowIn[i].rgbtGreen])
      {
          // Create 3D column when needed
          Flags[rowIn[i].rgbtRed][rowIn[i].rgbtGreen] = new TBits;
          Flags[rowIn[i].rgbtRed][rowIn[i].rgbtGreen]->Size = 256;
      }

        // Mark this R-G-B triple
        Flags[rowIn[i].rgbtRed][rowIn[i].rgbtGreen]->Bits[rowIn[i].rgbtBlue] = true;
    }
  }

  int RESULT = 0;
  // Count and Free TBits objects
  for(j = 0;j <= 255;j++)
    for(i = 0;i<= 255;i++)
      if(Flags[i,j])
        if(Flags[i][j])
        {
          for(k = 0;k <= 255;k++)
            if(Flags[i][j]->Bits[k])
              ++RESULT;
          delete Flags[i,j];
        }
  return RESULT;
}
//---------------------------------------------------------------------------
void GetColors(Graphics::TBitmap *Bitmap, const TRect &Rect, std::vector<RGBQUAD> &Colors)
{
  Colors.clear();
  Bitmap->HandleType = bmDIB;
  Bitmap->PixelFormat = pf32bit;
  RGBQUAD LastColor = {0xFF, 0xFF, 0xFF, 0xFF}; //Used as cache

  for(int Row = Rect.Top; Row < Rect.Bottom; Row++)
  {
    RGBQUAD *ScanLine = static_cast<RGBQUAD*>(Bitmap->ScanLine[Row]);
    for(int Col = Rect.Left; Col < Rect.Right; Col++)
      if(!(ScanLine[Col] == LastColor) && std::find(Colors.begin(), Colors.end(), ScanLine[Col]) == Colors.end())
      {
        LastColor = ScanLine[Col]; //Store in cache
        Colors.push_back(ScanLine[Col]);
      }
  }
}
//---------------------------------------------------------------------------
void FillBitmapInfoHeader(BITMAPINFOHEADER &BitmapHeader, Graphics::TBitmap *Bitmap, const TRect &Rect, unsigned Colors, unsigned DataSize)
{
  BitmapHeader.biSize = sizeof(BitmapHeader);  //Size of header
  BitmapHeader.biWidth = Rect.Width();         //Width of bitmap
  BitmapHeader.biHeight = Rect.Height();       //Height of bitmap
  BitmapHeader.biPlanes = 1;                   //Planes; Must be 1
  BitmapHeader.biBitCount = 8;                 //Color depth; Bits per pixel
  BitmapHeader.biCompression = BI_RLE8;        //Compression format
  BitmapHeader.biSizeImage = DataSize;       //Size of bitmap in bytes
  //Calculate the x- and y-resolution in pixels per meter
  BitmapHeader.biXPelsPerMeter = GetDeviceCaps(Bitmap->Canvas->Handle,HORZRES)*1000/GetDeviceCaps(Bitmap->Canvas->Handle,HORZSIZE);
  BitmapHeader.biYPelsPerMeter = GetDeviceCaps(Bitmap->Canvas->Handle,VERTRES)*1000/GetDeviceCaps(Bitmap->Canvas->Handle,VERTSIZE);
  BitmapHeader.biClrUsed = Colors;      //Used indexes in the color table
  BitmapHeader.biClrImportant = Colors; //Required indexes
}
//---------------------------------------------------------------------------
unsigned FindNearestColor(RGBQUAD Color, const std::vector<RGBQUAD> &Colors)
{
  unsigned Size = Colors.size();
  unsigned BestDist = -1;
  unsigned BestIndex = 0;
  for(unsigned I = 0; I < Size; I++)
  {
    RGBQUAD C = Colors[I];
    unsigned Dist = std::abs(Color.rgbRed - C.rgbRed) +
                    std::abs(Color.rgbGreen - C.rgbGreen) +
                    std::abs(Color.rgbBlue - C.rgbBlue);
    if(Dist < BestDist)
    {
      BestIndex = I;
      BestDist = Dist;
    }
  }
  return BestIndex;
}
//---------------------------------------------------------------------------
unsigned AddToPalette(RGBQUAD Color, std::vector<RGBQUAD> &Colors)
{
  std::vector<RGBQUAD>::iterator Iter = std::find(Colors.begin(), Colors.end(), Color);
  if(Iter == Colors.end())
  {
    if(Colors.size() < 256)
    {
      Colors.push_back(Color);
      return Colors.size() - 1;
    }
    return FindNearestColor(Color, Colors); //The palette cannot have more than 256 colors
  }
  return Iter - Colors.begin();
}
//---------------------------------------------------------------------------
//Colors must be sorted
//Notice that the Colors are not real TColor values, as DIB use inverted colors.
void CompressBitmap(Graphics::TBitmap *Bitmap, const TRect &Rect, std::vector<RGBQUAD> &Colors, std::vector<char> &Data)
{
  Bitmap->HandleType = bmDIB;
  Bitmap->PixelFormat = pf32bit;

  Data.clear();

  //Loop through scanlines from bottom to top
  for(int y = Rect.Bottom - 1; y >= Rect.Top; y--)
  {
    const RGBQUAD *ScanLine = static_cast<RGBQUAD*>(Bitmap->ScanLine[y]); //Get pointer to scanline
    unsigned Count = 1;  //Number of equal pixels following each other
    RGBQUAD Color = ScanLine[Rect.Left]; //Get color of first pixel in scanline
    unsigned ColorIndex = AddToPalette(Color, Colors);

    //Loop through pixels in scanline
    for(int x = Rect.Left + 1; x < Rect.Right; x++)
      //Check if we are in a block of equal colors
      if(ScanLine[x] == Color && Count < 255)
        Count++;
      else
      {
        //No more equal colors
        Data.push_back(Count);      //Add length of equal pixels
        Data.push_back(ColorIndex); //Add the color
        Count = 1;                  //Set count to 1; The one we are looking at now
        Color = ScanLine[x];        //Save the color
        ColorIndex = AddToPalette(Color, Colors);
      }
    Data.push_back(Count);          //Add length of last block of equal colors
    Data.push_back(ColorIndex);     //Add the color
    Data.push_back(0);              //Add escape sequense telling:
    Data.push_back(0);              //End of scanline
  }
  Data.push_back(0);                //Add escape sequense telling:
  Data.push_back(1);                //End of bitmap
}
//---------------------------------------------------------------------------
RGBQUAD ColorToRGBQUAD(TColor Color)
{
  RGBQUAD Result = {(Color >> 16) & 0xFF, (Color >> 8) & 0xFF, Color & 0xFF};
  return Result;
}
//---------------------------------------------------------------------------



