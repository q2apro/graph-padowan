/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2007 Ivan Johansen
 *
 * Graph is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
#ifndef ImagesH
#define ImagesH
#include "Unit18.h"
//---------------------------------------------------------------------------
class ESaveError : public Exception
{
public:
  ESaveError(const String &Str) : Exception(Str) {}
};

void SaveAsImage(const String &FileName, const TImageOptions &ImageOptions);
void SaveAsImage(const String &FileName, int ImageFileType, const TImageOptions &ImageOptions);

int CountColors(Graphics::TBitmap *Bitmap);
void GetColors(Graphics::TBitmap *Bitmap, const TRect &Rect, std::vector<RGBQUAD> &Colors);
void CompressBitmap(Graphics::TBitmap *Bitmap, const TRect &Rect, std::vector<RGBQUAD> &Colors, std::vector<char> &Data);
bool SaveCompressedBitmap(Graphics::TBitmap *Bitmap, const TRect &Rect, const String &FileName);
void FillBitmapInfoHeader(BITMAPINFOHEADER &BitmapHeader, Graphics::TBitmap *Bitmap, const TRect &Rect, unsigned Colors, unsigned DataSize);
RGBQUAD ColorToRGBQUAD(TColor Color);
#endif
