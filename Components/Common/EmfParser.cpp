/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2008 Ivan Johansen
 *
 * Graph is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
#include "Config.h"
#pragma hdrstop
#include "EmfParser.h"
#include <cassert>
#include <vector>
#include <iostream>
//---------------------------------------------------------------------------
TEmfParser::TEmfParser() : PolyFillMode(pfmNonZero)
{
}
//---------------------------------------------------------------------------
#pragma argsused
int CALLBACK TEmfParser::EnhMetaFileProc(
    HDC hDC,	// handle to device context
    HANDLETABLE FAR *lpHTable,	// pointer to metafile handle table
    const ENHMETARECORD FAR *lpEMFR,	// pointer to metafile record
    int nObj,	// count of objects
    LPARAM lpData 	// pointer to optional data
   )
{
  assert(lpData);
  TEmfParser *Object = reinterpret_cast<TEmfParser*>(lpData);
  Object->HandleRecord(lpEMFR);
  return true;
}
//---------------------------------------------------------------------------
void TEmfParser::HandleRecord(const ENHMETARECORD *lpEMFR)
{
  switch(lpEMFR->iType)
  {
    case EMR_HEADER: //Ignore
      break;

    case EMR_MOVETOEX:
    {
      EMRMOVETOEX* MoveTo = (EMRMOVETOEX*)lpEMFR;
      Pos = MoveTo->ptl;
      break;
    }
    case EMR_LINETO:
    {
      EMRLINETO* LineTo = (EMRLINETO*)lpEMFR;
      POINT P = FindEndPoint(Pos.x, Pos.y, LineTo->ptl.x, LineTo->ptl.y);
      Writer->Line(Pos.x, Pos.y, P.x, P.y);
      break;
    }

    case EMR_CREATEPEN:
    {
      EMRCREATEPEN *LogPen = (EMRCREATEPEN*)lpEMFR;
      TPenInfo Pen(LogPen->lopn.lopnStyle, LogPen->lopn.lopnWidth.x, SwapColor(LogPen->lopn.lopnColor));
      PenList[LogPen->ihPen] = Pen;
      Writer->SetPen(Pen);
      break;
    }

    case EMR_EXTCREATEPEN:
    {
      EMREXTCREATEPEN *ExtPen = (EMREXTCREATEPEN*)lpEMFR;
      TPenInfo Pen(ExtPen->elp.elpPenStyle, ExtPen->elp.elpWidth, SwapColor(ExtPen->elp.elpColor));
      PenList[ExtPen->ihPen] = Pen;
      Writer->SetPen(Pen);
      break;
    }

    case EMR_CREATEBRUSHINDIRECT:
    {
      EMRCREATEBRUSHINDIRECT  *LogBrush = (EMRCREATEBRUSHINDIRECT*)lpEMFR;
      TBrushInfo Brush(LogBrush->lb.lbStyle, SwapColor(LogBrush->lb.lbColor), LogBrush->lb.lbHatch);
      BrushList[LogBrush->ihBrush] = Brush;
      Writer->SetBrush(Brush);
      break;
    }

    case EMR_POLYGON16:
    {
      EMRPOLYGON16 *Polygon = (EMRPOLYGON16*)lpEMFR;
      Writer->Polygon(Polygon->apts, Polygon->cpts, PolyFillMode);
      break;
    }

    case EMR_POLYLINE16:
    {
      EMRPOLYGON16 *Polyline = (EMRPOLYLINE16*)lpEMFR;
      Writer->Polyline(Polyline->apts, Polyline->cpts);
      break;
    }

    case EMR_EXTCREATEFONTINDIRECTW:
    {
      EMREXTCREATEFONTINDIRECTW *ExtFont = (EMREXTCREATEFONTINDIRECTW*)lpEMFR;
      std::wstring Temp = ExtFont->elfw.elfLogFont.lfFaceName;
      TFontMember FontData;
      FontData.Name = std::string(Temp.begin(), Temp.end());
      FontData.Size = -ExtFont->elfw.elfLogFont.lfHeight;
      FontData.Weight = ExtFont->elfw.elfLogFont.lfWeight;
      FontData.Italic = ExtFont->elfw.elfLogFont.lfItalic;
      FontData.Underline = ExtFont->elfw.elfLogFont.lfUnderline;
      FontData.StrikeOut = ExtFont->elfw.elfLogFont.lfStrikeOut;
      FontList[ExtFont->ihFont] = FontData;
      break;
    }

    case EMR_SETTEXTCOLOR:
      Font.Color = SwapColor(reinterpret_cast<const EMRSETTEXTCOLOR*>(lpEMFR)->crColor);
      break;

    case EMR_SETBKCOLOR:
      Font.BkColor = SwapColor(reinterpret_cast<const EMRSETBKCOLOR*>(lpEMFR)->crColor);
      break;

    case EMR_SETBKMODE:
      Font.TransparentBk = reinterpret_cast<const EMRSETBKMODE*>(lpEMFR)->iMode == TRANSPARENT;
      break;

    case EMR_EXTTEXTOUTW:
    {
      EMREXTTEXTOUTW *Text = (EMREXTTEXTOUTW*)lpEMFR;
      int X = Text->rclBounds.left;
      int Y = Text->rclBounds.top + Font.Font.Size - ((std::max(Font.Font.Size, 11U)-11) / 3);
      wchar_t *Str = (wchar_t*)(((char*)Text)+Text->emrtext.offString);
      int Size = Text->emrtext.nChars;
      Writer->Text(X, Y, std::wstring(Str, Size), Font, Text->rclBounds);
      break;
    }

    case EMR_RECTANGLE:
      Writer->Rectangle(reinterpret_cast<const EMRRECTANGLE*>(lpEMFR)->rclBox);
      break;

    case EMR_ELLIPSE:
    {
      Writer->Ellipse(reinterpret_cast<const EMRELLIPSE*>(lpEMFR)->rclBox);
      break;
    }

    case EMR_SELECTOBJECT:
    {
      unsigned Index = reinterpret_cast<const EMRSELECTOBJECT*>(lpEMFR)->ihObject;
      if(Index & 0x80000000)
        //Object is a stock object. See MSN article Q142319
        switch(Index & 0x7FFFFFFF)
        {
          case BLACK_BRUSH:  Writer->SetBrush(TBrushInfo(BS_SOLID, 0x00FFFFFF, 0)); break;
          case DKGRAY_BRUSH: Writer->SetBrush(TBrushInfo(BS_SOLID, 0x00808080, 0)); break;
          case GRAY_BRUSH:   Writer->SetBrush(TBrushInfo(BS_SOLID, 0x00808080, 0)); break;
          case LTGRAY_BRUSH: Writer->SetBrush(TBrushInfo(BS_SOLID, 0x00C0C0C0, 0)); break;
          case NULL_BRUSH:   Writer->SetBrush(TBrushInfo(BS_NULL, 0, 0));           break;
          case WHITE_BRUSH:  Writer->SetBrush(TBrushInfo(BS_SOLID, 0x00000000, 0)); break;
          case BLACK_PEN:    Writer->SetPen(TPenInfo(PS_SOLID, 1, 0x00FFFFFF));     break;
          case NULL_PEN:     Writer->SetPen(TPenInfo(PS_NULL, 0, 0));               break;
          case WHITE_PEN:    Writer->SetPen(TPenInfo(PS_SOLID, 1, 0x00000000));     break;
        }
      else if(PenList.count(Index))
        Writer->SetPen(PenList[Index]);
      else if(BrushList.count(Index))
        Writer->SetBrush(BrushList[Index]);
      else if(FontList.count(Index))
        Font.Font = FontList[Index];
      break;
    }

    case EMR_DELETEOBJECT:
    {
      unsigned Index = reinterpret_cast<const EMRDELETEOBJECT*>(lpEMFR)->ihObject;
      PenList.erase(Index);
      BrushList.erase(Index);
      FontList.erase(Index);
      break;
    }

    case EMR_EXCLUDECLIPRECT:
      Writer->ExcludeClipRect(reinterpret_cast<const EMREXCLUDECLIPRECT*>(lpEMFR)->rclClip);
      break;

    case EMR_FILLRGN:
    {
      const EMRFILLRGN *Region = reinterpret_cast<const EMRFILLRGN*>(lpEMFR);
      const RGNDATA *RegionData = reinterpret_cast<const RGNDATA*>(Region->RgnData);
      const RECT *Data = reinterpret_cast<const RECT*>(RegionData->Buffer);
      if(BrushList.count(Region->ihBrush))
      {
        Writer->SetBrush(BrushList[Region->ihBrush]);
        Writer->PaintRegion(Data, RegionData->rdh.nCount);
      }
      break;
    }

    case EMR_FRAMERGN:
    {
      const EMRFRAMERGN *Region = reinterpret_cast<const EMRFRAMERGN*>(lpEMFR);
      const RGNDATA *RegionData = reinterpret_cast<const RGNDATA*>(Region->RgnData);
      const RECT *Data = reinterpret_cast<const RECT*>(RegionData->Buffer);
      if(BrushList.count(Region->ihBrush))
        Writer->FrameRegion(Data, RegionData->rdh.nCount, BrushList[Region->ihBrush], Region->szlStroke.cx, Region->szlStroke.cy);
      break;
    }

    case EMR_PAINTRGN:
    {
      const EMRPAINTRGN *Region = reinterpret_cast<const EMRPAINTRGN*>(lpEMFR);
      const RGNDATA *RegionData = reinterpret_cast<const RGNDATA*>(Region->RgnData);
      const RECT *Data = reinterpret_cast<const RECT*>(RegionData->Buffer);
      Writer->PaintRegion(Data, RegionData->rdh.nCount);
      break;
    }

    case EMR_GDICOMMENT: //Application specific data
      break;
/*
    case EMR_SETMAPMODE:
    {
      const EMRSETMAPMODE *MapMode = reinterpret_cast<const EMRSETMAPMODE*>(lpEMFR);
//      assert(MapMode->iMode == MM_TEXT);
      break;
    }
*/
    case EMR_SETVIEWPORTEXTEX:
    {
      const EMRSETVIEWPORTEXTEX *ViewPort = reinterpret_cast<const EMRSETVIEWPORTEXTEX*>(lpEMFR);
      ViewportSize = ViewPort->szlExtent;
      break;
    }
    case EMR_SETWINDOWEXTEX:
    {
      const EMRSETWINDOWEXTEX *Window = reinterpret_cast<const EMRSETWINDOWEXTEX*>(lpEMFR);
      WindowSize = Window->szlExtent;
      Writer->SetWindowMapping(WindowSize, ViewportSize, WindowOrg);
      break;
    }
/*
    case EMR_SETTEXTALIGN:
    {
      const EMRSETTEXTALIGN *TextAlign = reinterpret_cast<const EMRSETTEXTALIGN*>(lpEMFR);
      break;
    }
*/
    case EMR_SETWINDOWORGEX:
    {
      const EMRSETWINDOWORGEX *EmrWindowOrg = reinterpret_cast<const EMRSETWINDOWORGEX*>(lpEMFR);
      WindowOrg = EmrWindowOrg->ptlOrigin;
      break;
    }

    case EMR_SETPOLYFILLMODE:
      PolyFillMode = reinterpret_cast<const EMRSETPOLYFILLMODE*>(lpEMFR)->iMode == ALTERNATE ? pfmEvenOdd : pfmNonZero;
      break;

    case EMR_ARC:
    {
      const EMRARC *Arc = reinterpret_cast<const EMRARC*>(lpEMFR);
      Writer->Arc(Arc->rclBox, Arc->ptlStart, Arc->ptlEnd);
      break;
    }

    case EMR_STRETCHDIBITS:
    {
      const EMRSTRETCHDIBITS *Stretch = reinterpret_cast<const EMRSTRETCHDIBITS*>(lpEMFR);
      Writer->StretchBitmap(&Stretch->rclBounds, Stretch->cxSrc, Stretch->cySrc,
        reinterpret_cast<const BYTE*>(Stretch) + Stretch->offBmiSrc,
        Stretch->emr.nSize - Stretch->offBmiSrc, Stretch->cbBmiSrc);
      break;
    }

    case EMR_SELECTPALETTE:
    case EMR_EXTSELECTCLIPRGN:
    case EMR_EOF:
    case EMR_SETROP2:
    case EMR_SETVIEWPORTORGEX:
    case EMR_INTERSECTCLIPRECT:
    case EMR_SAVEDC:
    case EMR_SETLAYOUT:
    case EMR_SETMETARGN:
    case EMR_SETSTRETCHBLTMODE:
    case EMR_SETTEXTALIGN:
    case EMR_SETBRUSHORGEX:
    case EMR_SETMITERLIMIT:
    case EMR_SETWORLDTRANSFORM:
      break;

    default:
      std::clog << "Unknown EMF command: " << lpEMFR->iType << std::endl;
      break;
  }
}
//---------------------------------------------------------------------------
void TEmfParser::Parse(HENHMETAFILE Handle, TGraphicWriter &AWriter)
{
  ENHMETAHEADER Header;
  GetEnhMetaFileHeader(Handle, sizeof(Header), &Header);

  Writer = &AWriter;
  RECTL Rect;
  xScale = (double)Header.szlDevice.cx / Header.szlMillimeters.cx;
  yScale = (double)Header.szlDevice.cy / Header.szlMillimeters.cy;
  Rect.left = Header.rclFrame.left * xScale / 100 + 0.5;
  Rect.top = Header.rclFrame.top * yScale / 100 + 0.5;
  Rect.right = Header.rclFrame.right * xScale / 100 + 0.5;
  Rect.bottom = Header.rclFrame.bottom * yScale / 100 + 0.5;
  Writer->BeginFile(Rect, Header.rclFrame.right - Header.rclFrame.left, Header.rclFrame.bottom - Header.rclFrame.top);

  EnumEnhMetaFile(NULL, Handle, &EnhMetaFileProc, this, NULL);
  Writer->EndOfFile();
}
//---------------------------------------------------------------------------
void TEmfParser::Parse(const char *FileName, TGraphicWriter &AWriter)
{
  HENHMETAFILE Handle = GetEnhMetaFileA(FileName);
  if(Handle == NULL)
    return;
  Parse(Handle, AWriter);
  DeleteEnhMetaFile(Handle);
}
//---------------------------------------------------------------------------
unsigned TEmfParser::SwapColor(unsigned Color)
{
  return ((Color & 0xFF) << 16) | (Color & 0xFF00) | ((Color & 0xFF0000) >> 16); // Swap Red and Blue
}
//---------------------------------------------------------------------------
void CALLBACK TEmfParser::LineDDAProc(int X, int Y, LPARAM lpData)
{
  TEmfParser *EmfParser = reinterpret_cast<TEmfParser*>(lpData);
  EmfParser->TempX = X;
  EmfParser->TempY = Y;
}
//---------------------------------------------------------------------------
POINT TEmfParser::FindEndPoint(int X1, int Y1, int X2, int Y2)
{
  LineDDA(X1, Y1, X2, Y2, LineDDAProc, reinterpret_cast<long>(this));
  POINT P = {TempX, TempY};
  return P;
}
//---------------------------------------------------------------------------

