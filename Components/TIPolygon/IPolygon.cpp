//===========================================================================
// Copyright © 2003 Ivan Johansen
// IPolygon.cpp
//===========================================================================
#include "Config.h"
#include <vcl.h>
#pragma hdrstop
#include "IPolygon.h"
#include "ICompCommon.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
static TRegisterClass Dummy(__classid(TIPolygon));
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//
static inline void ValidCtrCheck(TIPolygon *)
{
  new TIPolygon(NULL);
}
//---------------------------------------------------------------------------
namespace Ipolygon
{
  void __fastcall PACKAGE Register()
  {
    TComponentClass classes[1] = {__classid(TIPolygon)};
    RegisterComponents("IComp", classes, 0);
  }
}
//---------------------------------------------------------------------------
__fastcall TIPolygon::TIPolygon(TComponent* Owner)
 : TGraphicControl(Owner), FPolyFillMode(pfmAlternate)
{
  Canvas->Pen->OnChange = Change;
  Canvas->Brush->OnChange = Change;
  FPoints->OnChange = PointsChange;
}
//---------------------------------------------------------------------------
void __fastcall TIPolygon::Paint()
{
  if(!PointList.empty())
  {
    switch(FPolygonType)
    {
      case ptPolygon:
        ::SetPolyFillMode(Canvas->Handle, FPolyFillMode == pfmAlternate ? ALTERNATE : WINDING);
        Canvas->Polygon(&*PointList.begin(), PointList.size() - 1);
        break;

      case ptPolyline:
        Canvas->Polyline(&*PointList.begin(), PointList.size() - 1);
        break;
    }
  }
}
//---------------------------------------------------------------------------
TPen* __fastcall TIPolygon::GetPen()
{
  return Canvas->Pen;
}
//---------------------------------------------------------------------------
TStrings* __fastcall TIPolygon::GetPoints()
{
  FPoints->OnChange = NULL;
  FPoints->Clear();
  char OldDecimalSeparator = FormatSettings.DecimalSeparator;
  for(unsigned I = 0; I < PointList.size(); I++)
    FPoints->Add(String(PointList[I].x) + ',' + PointList[I].y);
  FormatSettings.DecimalSeparator = OldDecimalSeparator;
  FPoints->OnChange = PointsChange;
  return FPoints;
}
//---------------------------------------------------------------------------
void __fastcall TIPolygon::PointsChange(TObject *Sender)
{
  char OldDecimalSeparator = FormatSettings.DecimalSeparator;
  std::vector<TPoint> Temp;

  for(int I = 0; I < FPoints->Count; I++)
  {
    String Str = FPoints->Strings[I].Trim();
    if(Str.IsEmpty())
      continue;
    int Pos = Str.Pos(',');
    if(Pos == 0)
      throw Exception("No comma found in string \"" + Str + "\"");
    int x = Str.SubString(1, Pos - 1).ToInt();
    int y = Str.SubString(Pos + 1, Str.Length() - Pos).ToInt();
    Temp.push_back(TPoint(x, y));
  }

  PointList.swap(Temp);
  FormatSettings.DecimalSeparator = OldDecimalSeparator;
  Invalidate();
}
//---------------------------------------------------------------------------
void TIPolygon::AddPoint(const TPoint &Point)
{
  PointList.push_back(Point);
  Invalidate();
}
//---------------------------------------------------------------------------
void TIPolygon::AddPoints(const TPoint *APoints, unsigned Size)
{
  PointList.insert(PointList.end(), APoints, APoints + Size);
  Invalidate();
}
//---------------------------------------------------------------------------
void TIPolygon::Clear()
{
  PointList.clear();
  Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TIPolygon::SetPolyFillMode(TPolygonFillMode APolyFillMode)
{
  FPolyFillMode = APolyFillMode;
  Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TIPolygon::SetPolygonType(TPolygonType APolygonType)
{
  FPolygonType = APolygonType;
  Invalidate();
}
//---------------------------------------------------------------------------

