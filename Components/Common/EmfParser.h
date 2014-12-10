/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2008 Ivan Johansen
 *
 * Graph is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
#ifndef EmfParserH
#define EmfParserH
#include <windows.h>
#include <string>
#include <map>
//---------------------------------------------------------------------------
struct TPenInfo
{
  unsigned Style;
  unsigned Width;
  unsigned Color;
  TPenInfo() : Style(0), Width(0), Color(0) {}
  TPenInfo(unsigned AStyle, unsigned AWidth, unsigned AColor)
    : Style(AStyle), Width(AWidth), Color(AColor) {}
};

struct TBrushInfo
{
  unsigned Style;
  unsigned Color;
  unsigned Hatch;
  TBrushInfo() : Style(0), Color(0), Hatch(0) {}
  TBrushInfo(unsigned AStyle, unsigned AColor, unsigned AHatch)
    : Style(AStyle), Color(AColor), Hatch(AHatch) {}
  bool operator==(const TBrushInfo &O) const {return Style == O.Style && Color == O.Color && Hatch == O.Hatch;}  
};

struct TFontMember
{
  std::string Name;
  unsigned Size;
  bool Italic;
  bool Underline;
  bool StrikeOut;
  unsigned Weight;
};

struct TFontInfo
{
  TFontMember Font;
  unsigned Color;
  unsigned BkColor;
  bool TransparentBk;
};

enum TPolyFillMode {pfmNonZero, pfmEvenOdd};

class TGraphicWriter
{
public:
  virtual ~TGraphicWriter() {}
  virtual void BeginFile(const RECTL &Rect, unsigned Width, unsigned Height)=0; //Width/Height in 0.01 millimeters
  virtual void EndOfFile()=0;
  virtual void Line(int X1, int Y1, int X2, int Y2)=0;
  virtual void Polygon(const POINTS *Points, int Count, TPolyFillMode PolyFillMode)=0;
  virtual void Polyline(const POINTS *Points, int Count)=0;
  virtual void Rectangle(const RECTL &Rect)=0;
  virtual void Ellipse(const RECTL &Rect)=0;
  virtual void Arc(const RECTL &Box, const POINTL &Start, const POINTL &End)=0;
  virtual void Text(int X, int Y, const std::wstring &Str, const TFontInfo &Font, const RECTL &Rect)=0;
  virtual void SetPen(const TPenInfo &APen)=0;
  virtual void SetBrush(const TBrushInfo &ABrush)=0;
  virtual void ExcludeClipRect(const RECTL &Rect)=0;
  virtual void SetWindowMapping(SIZEL WindowSize, SIZEL ViewportSize, POINTL WindowOrg)=0;
  virtual void PaintRegion(const RECT *Rect, unsigned Count)=0;
  virtual void FrameRegion(const RECT *Rect, unsigned Count, const TBrushInfo &Brush, unsigned xSize, unsigned ySize)=0;
  virtual void StretchBitmap(const RECTL *DestRect, unsigned SrcWidth, unsigned SrcHeight, const BYTE *BitmapData, unsigned Size, unsigned BitsOffset)=0;
};

class TEmfParser
{
  TGraphicWriter *Writer;
  POINTL Pos;
  TFontInfo Font;
  std::map<unsigned, TPenInfo> PenList;
  std::map<unsigned, TBrushInfo> BrushList;
  std::map<unsigned, TFontMember> FontList;
  SIZEL WindowSize, ViewportSize;
  POINTL WindowOrg;
  double xScale, yScale;
  int TempX, TempY;
  TPolyFillMode PolyFillMode;

  static int CALLBACK EnhMetaFileProc(HDC hDC, HANDLETABLE *lpHTable, const ENHMETARECORD *lpEMFR, int nObj, LPARAM lpData);
  void HandleRecord(const ENHMETARECORD *lpEMFR);
  static unsigned SwapColor(unsigned Color);
  static void CALLBACK LineDDAProc(int X, int Y, LPARAM lpData);
  POINT FindEndPoint(int X1, int Y1, int X2, int Y2);

public:
  TEmfParser();
  void Parse(HENHMETAFILE Handle, TGraphicWriter &AWriter);
  void Parse(const char *FileName, TGraphicWriter &AWriter);
};
#endif
