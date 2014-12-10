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
#include "Context.h"
#include "ConfigRegistry.h"
//---------------------------------------------------------------------------
struct TPointHandler
{
  std::vector<TPoint> PointList;
  void Append(const TPoint *Points, unsigned Size) {PointList.insert(PointList.end(), Points, Points + Size);}
};
static void Clip(TPoint &P1, const TPoint &P2, TOutCode OutCode, const TRect &Rect);
static TPoint Crop(TOutCode OutCode, const TRect &Rect);
static TOutCode CompOutCode(const TPoint &P, const TRect &Rect);
static void ClipToRect(TClipCallback ClipCallback, const TPoint *Points, unsigned Size, const TRect &Rect, bool DoCrop);
//---------------------------------------------------------------------------
void TContext::DrawPolyline(const std::vector<TPoint> &Points)
{
  if(!Points.empty())
    DrawPolyline(&Points.front(), Points.size());
}
//---------------------------------------------------------------------------
void TContext::DrawPolyline(const TPoint *Points, unsigned Size)
{
  unsigned WinLimit = PenWidth > 1 ? 4096 : 16384; //An undocumented limit in Windows
  while(Size > WinLimit)
  {
    Canvas->Polyline(Points, WinLimit - 1);
    Points += WinLimit;
    Size -= WinLimit;
  }
  Canvas->Polyline(Points, Size - 1);
}
//---------------------------------------------------------------------------
//Return value indicating position of P related to Rect
inline TOutCode CompOutCode(const TPoint &P, const TRect &Rect)
{
  int OutCode = ocInside;
  if(P.y < Rect.Top) OutCode = ocTop;
  else if(P.y > Rect.Bottom) OutCode = ocBottom;
  if(P.x < Rect.Left) OutCode |= ocLeft;
  else if(P.x > Rect.Right) OutCode |= ocRight;
  return static_cast<TOutCode>(OutCode);
}
//---------------------------------------------------------------------------
//Move the point P1 inside Rect so the line P1-P2 is the same
//Only clips at one side. Recalculate OutCode an call again to make sure P1 is completely inside
inline void Clip(TPoint &P1, const TPoint &P2, TOutCode OutCode, const TRect &Rect)
{
  //No clipping needed if it is the same point; Prevent division by zero
  if(P1 == P2)
    return;

  //Notice: The += operations may overflow if both the left and right sides are above MAXINT
  if(OutCode & ocTop)
  {
    BOOST_ASSERT(P2.y != P1.y);
    P1.x += (static_cast<long long>(P2.x - P1.x)*(Rect.Top - P1.y))/(P2.y - P1.y);
    P1.y = Rect.Top;
  }
  else if(OutCode & ocBottom)
  {
    BOOST_ASSERT(P2.y != P1.y);
    P1.x += (static_cast<long long>(P2.x - P1.x)*(Rect.Bottom - P1.y))/(P2.y - P1.y);
    P1.y = Rect.Bottom;
  }
  else if(OutCode & ocRight)
  {
    BOOST_ASSERT(P2.x != P1.x);
    P1.y += (static_cast<long long>(P2.y - P1.y)*(Rect.Right - P1.x))/(P2.x - P1.x);
    P1.x = Rect.Right;
  }
  else if(OutCode & ocLeft)
  {
    BOOST_ASSERT(P2.x != P1.x);
    P1.y += (static_cast<long long>(P2.y - P1.y)*(Rect.Left - P1.x))/(P2.x - P1.x);
    P1.x = Rect.Left;
  }
}
//---------------------------------------------------------------------------
//Draw polyline and clip to Rect.
//NOTICE: data pointed to by Points may be changed temorarely but is always restored before exiting the function.
void TContext::DrawPolyline(const TPoint *Points, unsigned Size, const TRect &Rect)
{
  TClipCallback Callback = &this->DrawPolyline;
  ClipToRect(Callback, Points, Size, Rect, false);
}
//---------------------------------------------------------------------------
//Splits Points into segments that fits into Rect, and call ClipCallback for each segment
//Clipping is implemented using the a modified
//Cohen-Sutherland algorithm. More information at:
//http://shamimkhaliq.50megs.com/Java/lineclipper.htm
//http://www.cc.gatech.edu/grads/h/Hao-wei.Hsieh/Haowei.Hsieh/mm.html
//NOTICE: data pointed to by Points may be changed temorarely but is always restored before exiting the function.
//NOTICE: Only 31 bit signed integer may be used as points. The calculation may overflow if the points exceeds MAXINT/2
void ClipToRect(TClipCallback ClipCallback, const TPoint *Points, unsigned Size, const TRect &Rect, bool DoCrop)
{
  if(Size < 2)
    return;

  TPoint *Begin = const_cast<TPoint*>(Points);
  TPoint *End = Begin + Size;

  if(DoCrop)
  {
    //Clip line between first and last point
    TOutCode OutCode1 = CompOutCode(*(End-1), Rect);
    TOutCode OutCode2 = CompOutCode(*Begin, Rect);
    if(!(OutCode1 & OutCode2))
    {
      TPoint P[2] = {*(End-1), *Begin};
      Clip(P[0], P[1], OutCode1, Rect);
      Clip(P[0], P[1], CompOutCode(P[0], Rect), Rect);
      Clip(P[1], P[0], OutCode2, Rect);
      Clip(P[1], P[0], CompOutCode(P[1], Rect), Rect);
      ClipCallback(P, 2);
    }
  }

  for(TPoint *P2 = Begin+1; P2 != End; ++P2)
  {
    TOutCode OutCode1 = CompOutCode(*(P2-1), Rect); //Position of P1
    TOutCode OutCode2 = CompOutCode(*P2, Rect); //Position of P2

    //Search for line that goes inside Rect (Loop while line is only outside)
    while((OutCode1 & OutCode2))
    {
      //Check if line is crossing Rect
      if(DoCrop && OutCode1 != OutCode2)
      {
        TPoint Temp[2] = {Crop(OutCode1, Rect), Crop(OutCode2, Rect)};
        ClipCallback(Temp, 2);
      }

      if(++P2 == End)
        return;
      OutCode1 = OutCode2;
      OutCode2 = CompOutCode(*P2, Rect);
    }

    //Store old P1 value and move P1 inside Rect
    TPoint *P1 = P2-1; //Start of polyline segment
    TPoint OldP1 = *P1;
    Clip(*P1, *P2, OutCode1, Rect);
    Clip(*P1, *P2, CompOutCode(*P1, Rect), Rect);

    //Search for point outside so clipping is needed
    while(OutCode2 == ocInside && ++P2 != End)
      OutCode2 = CompOutCode(*P2, Rect);

    if(P2 == End)
    {
      //Draw last segment and return
      ClipCallback(P1, P2 - P1);
      return;
    }

    //Store old P2 and clip twice so both x- and y-coordinate gets inside
    TPoint OldP2 = *P2;
    Clip(*P2, *(P2-1), OutCode2, Rect);
    Clip(*P2, *(P2-1), CompOutCode(*P2, Rect), Rect);

    //Draw the line segment and restore start and end points
    ClipCallback(P1, P2 - P1 + 1);
    *P1 = OldP1;
    *P2 = OldP2;
  }
}
//---------------------------------------------------------------------------
//Move a point with OutCode, which is outside, inside Rect
TPoint Crop(TOutCode OutCode, const TRect &Rect)
{
  return Point(
    OutCode & ocLeft ? Rect.Left : Rect.Right,
    OutCode & ocTop  ? Rect.Top  : Rect.Bottom
  );
}
//---------------------------------------------------------------------------
void TContext::DrawLine(int X1, int Y1, int X2, int Y2)
{
  Canvas->MoveTo(X1, Y1);
  Canvas->LineTo(X2, Y2);
}
//---------------------------------------------------------------------------
void TContext::DrawLine(const TPoint &P1, const TPoint &P2)
{
  DrawLine(P1.x, P1.y, P2.x, P2.y);
}
//---------------------------------------------------------------------------
void TContext::DestroyClipRect()
{
  SelectClipRgn(Canvas->Handle, NULL);
}
//---------------------------------------------------------------------------
void TContext::SetFont(TFont *Font)
{
  Canvas->Font->Assign(Font);
}
//---------------------------------------------------------------------------
void TContext::SetFontColor(TColor Color)
{
  Canvas->Font->Color = Color;
}
//---------------------------------------------------------------------------
void TContext::SetBrush(TBrushStyle Style, TColor Color)
{
  Canvas->Brush->Color = Color;
  Canvas->Brush->Style = Style;
  SetBkColor(Canvas->Handle, clWhite);  //Workaround for bug in TCanvas.CreateBrush()
}
//---------------------------------------------------------------------------
TSize TContext::GetTextExtent(const std::string &Str)
{
  return Canvas->TextExtent(Str.c_str());
}
//---------------------------------------------------------------------------
TSize TContext::GetTextExtent(const std::wstring &Str)
{
  TSize Result;
  Win32Check(GetTextExtentPoint32W(Canvas->Handle, Str.c_str(), Str.size(), &Result));
  return Result;
}
//---------------------------------------------------------------------------
int TContext::GetTextHeight(const std::string &Str)
{
  return Canvas->TextHeight(Str.c_str());
}
//---------------------------------------------------------------------------
int TContext::GetTextHeight(const std::wstring &Str)
{
  return GetTextExtent(Str).cy;
}
//---------------------------------------------------------------------------
int TContext::GetTextWidth(const std::string &Str)
{
  return Canvas->TextWidth(Str.c_str());
}
//---------------------------------------------------------------------------
int TContext::GetTextWidth(const std::wstring &Str)
{
  return GetTextExtent(Str).cx;
}
//---------------------------------------------------------------------------
void TContext::DrawText(const std::string &Str, int X, int Y)
{
  Canvas->TextOut(X, Y, Str.c_str());
}
//---------------------------------------------------------------------------
void TContext::DrawText(const std::wstring &Str, int X, int Y)
{
  //It looks like ExtTextOutW() is using 16 bit integers
  if(X > -MAXSHORT && X < MAXSHORT && Y > -MAXSHORT && Y < MAXSHORT)
    ExtTextOutW(Canvas->Handle, X, Y, 0, NULL, Str.c_str(), Str.size(), NULL);
  Changed();
}
//---------------------------------------------------------------------------
void TContext::DrawText(const std::wstring &Str, int X, int Y, const TRect &Rect)
{
  ExtTextOutW(Canvas->Handle, X, Y, ETO_CLIPPED, &Rect, Str.c_str(), Str.size(), NULL);
  Changed();
}
//---------------------------------------------------------------------------
void TContext::SetClipRegion(const TRegion &Region)
{
  SelectClipRgn(Canvas->Handle, Region.Handle);
}
//---------------------------------------------------------------------------
void TContext::IntersectClipRect(const TRect &Rect)
{
  ::IntersectClipRect(Canvas->Handle, Rect.Left, Rect.Top, Rect.Right, Rect.Bottom);
}
//---------------------------------------------------------------------------
void TContext::ExcludeClipRect(const TRect &Rect)
{
  ::ExcludeClipRect(Canvas->Handle, Rect.Left, Rect.Top, Rect.Right, Rect.Bottom);
}
//---------------------------------------------------------------------------
void TContext::SetDeviceOrigin(int X, int Y)
{
  SetViewportOrgEx(Canvas->Handle, X, Y, NULL);
}
//---------------------------------------------------------------------------
void TContext::SetPen(TPenStyle Style, TColor Color, int Width, TEndCap EndCap, TPenStyleJoin Join)
{
  PenStyle = Style;
  PenColor = Color;
  PenWidth = Width;

  if(Width > 1)
  {
    LOGBRUSH LogBrush = {BS_SOLID, Color};
    Canvas->Pen->Handle = ExtCreatePen(PS_GEOMETRIC | Style | Join | EndCap, Width, &LogBrush, 0, NULL);
  }
  else
    Canvas->Pen->Handle = CreatePen(Style, Style == psSolid ? Width : 1, Color);
}
//---------------------------------------------------------------------------
static bool UseThinGridLines = GetRegValue(REGISTRY_KEY, L"UseThinGridLines", HKEY_CURRENT_USER, false);
void TContext::SetGridPen(TColor Color, unsigned Width)
{
  //PS_ALTERNATE is not supported on Win9x and apparently most printers.
  //It may also cause problems if PS_ALTERNATE is used in a metafile and printed afterwards,
  if(Canvas == Printer()->Canvas)
  {
    SetPen(psDot, Color, UseThinGridLines ? 1 : Width);
    return;
  }
  if(Width > 1 || dynamic_cast<TMetafileCanvas*>(Canvas))
  {
    SetPen(psDot, Color, Width);
    return;
  }

  LOGBRUSH LogBrush = {BS_SOLID, Color};
  Canvas->Pen->Handle = ExtCreatePen(PS_COSMETIC | PS_ALTERNATE, 1, &LogBrush, 0, NULL);
  PenStyle = static_cast<TPenStyle>(PS_ALTERNATE);
  PenColor = Color;
  PenWidth = 1;
}
//---------------------------------------------------------------------------
void TContext::DrawPolygon(const std::vector<TPoint> &Points)
{
  if(!Points.empty())
    DrawPolygon(&Points.front(), Points.size());
}
//---------------------------------------------------------------------------
void TContext::DrawPolygon(const TPoint *Points, unsigned Size)
{
  SetPolyFillMode(Canvas->Handle, WINDING);
  Canvas->Polygon(Points, Size-1);
}
//---------------------------------------------------------------------------
void TContext::DrawPolygon(const TPoint *Points, unsigned Size, const TRect &Rect)
{
  TPointHandler PointHandler;
  ClipToRect(&PointHandler.Append, Points, Size, Rect, true);
  DrawPolygon(PointHandler.PointList);
}
//---------------------------------------------------------------------------
void TContext::DrawPolygon(const std::vector<TPoint> &Points, const TRect &Rect)
{
  if(!Points.empty())
    DrawPolygon(&Points.front(), Points.size(), Rect);
}
//---------------------------------------------------------------------------
void TContext::FillRect(const TRect &Rect)
{
  Canvas->FillRect(Rect);
}
//---------------------------------------------------------------------------
void TContext::DrawRectangle(const TRect &Rect)
{
  Canvas->Rectangle(Rect);
}
//---------------------------------------------------------------------------
void TContext::SetFontName(const std::string &Name)
{
  Canvas->Font->Name = Name.c_str();
}
//---------------------------------------------------------------------------
void TContext::DrawPolydots(const TPoint *Points, unsigned Size, TColor Color)
{
  while(Size--)
  {
    Canvas->Pixels[Points->x][Points->y] = Color;
    Points++;
  }
}
//---------------------------------------------------------------------------
void TContext::DrawEllipse(int X1, int Y1, int X2, int Y2)
{
  //Ellipse() seems to use 16 bit integers
  if(X1 > -MAXSHORT && Y1 > -MAXSHORT && X2 < MAXSHORT && Y2 < MAXSHORT)
    Canvas->Ellipse(X1, Y1, X2, Y2);
}
//---------------------------------------------------------------------------
void TContext::DrawArc(int X1, int Y1, int X2, int Y2, int X3, int Y3, int X4, int Y4)
{
  Canvas->Arc(X1, Y1, X2, Y2, X3, Y3, X4, Y4);
}
//---------------------------------------------------------------------------
void TContext::SetFont(const std::string &Name, unsigned Size, TColor Color, TFontStyles Style)
{
  Canvas->Font->Name = Name.c_str();
  Canvas->Font->Size = Size;
  Canvas->Font->Color = Color;
  Canvas->Font->Style = Style;
}
//---------------------------------------------------------------------------
void TContext::SetPixelColor(int X, int Y, TColor Color)
{
  Canvas->Pixels[X][Y] = Color;
}
//---------------------------------------------------------------------------
void TContext::Lock()
{
  Canvas->Lock();
}
//---------------------------------------------------------------------------
void TContext::Unlock()
{
  Changed();
  Canvas->Unlock();
}
//---------------------------------------------------------------------------
void TContext::Changed()
{
  if(Canvas->OnChange)
    Canvas->OnChange(Canvas);
}
//---------------------------------------------------------------------------
void TContext::DrawRegion(const TRegion &Region)
{
  PaintRgn(Canvas->Handle, Region.Handle);
  Changed();
}
//---------------------------------------------------------------------------
void TContext::DrawFrameRegion(const TRegion &Region, unsigned Width)
{
  //Danger, danger Will Robinson: The FrameRgn() function can be extremely slow
  //for complex regions.
  FrameRgn(Canvas->Handle, Region.Handle, Canvas->Brush->Handle, Width, Width);
  Changed();
}
//---------------------------------------------------------------------------
TRegion::TRegion(const std::vector<TPoint> &Points, int Mode)
 : Handle(NULL)
{
  TPointHandler PointHandler;
  //Clip to prevent an OS error
  TRect Rect(-MAXSHORT/2, -MAXSHORT/2, MAXSHORT/2, MAXSHORT/2);
  ClipToRect(&PointHandler.Append, &Points.at(0), Points.size(), Rect, true);
  Handle = CreatePolygonRgn(&PointHandler.PointList[0], PointHandler.PointList.size(), Mode);
  if(Handle == 0)
    RaiseLastOSError();
}
//---------------------------------------------------------------------------
TRegion::TRegion(const TPoint *Points, unsigned Count, int Mode)
 : Handle(NULL)
{
  Handle = CreatePolygonRgn(Points, Count, Mode);
  if(Handle == 0)
    RaiseLastOSError();
}
//---------------------------------------------------------------------------
TRegion::TRegion(const std::vector<TPoint> &Points, const std::vector<int> &Counts, int Mode)
 : Handle(NULL)
{
  Handle = CreatePolyPolygonRgn(&Points.at(0), &Counts.at(0), Counts.size(), Mode);
  if(Handle == 0)
    RaiseLastOSError();
}
//---------------------------------------------------------------------------
TRegion::TRegion(const TRect &Rect)
{
  Handle = CreateRectRgnIndirect(&Rect);
  if(Handle == 0)
    RaiseLastOSError();
}
//---------------------------------------------------------------------------
TRegion::TRegion(const std::vector<TRect> &Data)
{
  SetRegion(Data);
}
//---------------------------------------------------------------------------
void TRegion::SetRegion(const std::vector<TRect> &Data)
{
  if(Handle != 0)
    DeleteObject(Handle);
  unsigned Count = Data.size();
  std::vector<char> Buffer(Count * sizeof(TRect) + sizeof(RGNDATAHEADER));
  RGNDATAHEADER *Header = reinterpret_cast<RGNDATAHEADER*>(&Buffer[0]);
  Header->dwSize = sizeof(RGNDATAHEADER);
  Header->iType = RDH_RECTANGLES;
  Header->nCount = Count;
  Header->nRgnSize = 0;
  Header->rcBound = TRect(0, 0, 0, 0); //????
  if(!Data.empty())
    memcpy(&Buffer[sizeof(RGNDATAHEADER)], &Data[0], Count * sizeof(TRect));
  Handle = ExtCreateRegion(NULL, Buffer.size(), reinterpret_cast<RGNDATA*>(&Buffer[0]));
  if(Handle == 0)
    RaiseLastOSError();
}
//---------------------------------------------------------------------------
void TRegion::Offset(int X, int Y)
{
  OffsetRgn(Handle, X, Y);
}
//---------------------------------------------------------------------------
bool TRegion::PointInRegion(const TPoint &Point) const
{
  return PtInRegion(Handle, Point.x, Point.y);
}
//---------------------------------------------------------------------------
bool TRegion::RectInRegion(const TRect &Rect) const
{
  return ::RectInRegion(Handle, &Rect);
}
//---------------------------------------------------------------------------
TRect TRegion::GetBoundingRect() const
{
  TRect Result;
  GetRgnBox(Handle, &Result);
  return Result;
}
//---------------------------------------------------------------------------
TRegion& TRegion::operator=(const TRegion& Region)
{
  CombineRgn(Handle, Region.Handle, NULL, RGN_COPY);
  return *this;
}
//---------------------------------------------------------------------------
TRegion& TRegion::operator-=(const TRegion& Region)
{
  CombineRgn(Handle, Handle, Region.Handle, RGN_DIFF);
  return *this;
}
//---------------------------------------------------------------------------
TRegion& TRegion::operator&=(const TRegion& Region)
{
  CombineRgn(Handle, Handle, Region.Handle, RGN_AND);
  return *this;
}
//---------------------------------------------------------------------------
TRegion& TRegion::operator|=(const TRegion& Region)
{
  CombineRgn(Handle, Handle, Region.Handle, RGN_OR);
  return *this;
}
//---------------------------------------------------------------------------
TRegion& TRegion::operator^=(const TRegion& Region)
{
  CombineRgn(Handle, Handle, Region.Handle, RGN_XOR);
  return *this;
}
//---------------------------------------------------------------------------
void TRegion::GetData(std::vector<TRect> &Data)
{
  unsigned Count = GetRegionData(Handle, 0, NULL);
  std::vector<unsigned char> RgnTempData(Count);
  RGNDATA *RgnData = reinterpret_cast<RGNDATA*>(&RgnTempData.at(0));
  if(GetRegionData(Handle, Count, RgnData) == 0)
    RaiseLastOSError();

  Data.assign(reinterpret_cast<TRect*>(RgnData->Buffer), reinterpret_cast<TRect*>(RgnData->Buffer) + RgnData->rdh.nCount);
}
//---------------------------------------------------------------------------
void TContext::ClipLine(TPoint &P1, TPoint &P2, const TRect &Rect)
{
  Clip(P1, P2, CompOutCode(P1, Rect), Rect);
  Clip(P1, P2, CompOutCode(P1, Rect), Rect);

  Clip(P2, P1, CompOutCode(P2, Rect), Rect);
  Clip(P2, P1, CompOutCode(P2, Rect), Rect);
}
//---------------------------------------------------------------------------
TPoint TContext::ClipLine(const TPoint &P1, const TPoint &P2, const TRect &Rect)
{
  TPoint P = P1;
  Clip(P, P2, CompOutCode(P1, Rect), Rect);
  Clip(P, P2, CompOutCode(P1, Rect), Rect);
  return P;
}
//---------------------------------------------------------------------------
void TRegion::Widen(int X1, int Y1, int X2, int Y2)
{
  std::vector<TRect> Data;
  GetData(Data);
  std::vector<TRect>::iterator End = Data.end();
  for(std::vector<TRect>::iterator Iter = Data.begin(); Iter != End; ++Iter)
  {
    Iter->Left += X1;
    Iter->Top += Y1;
    Iter->Right += X2;
    Iter->Bottom += Y2;
  }
  SetRegion(Data);
}
//---------------------------------------------------------------------------



