//===========================================================================
// Copyright © 2003 Ivan Johansen
// IPolygon.h
//===========================================================================
#ifndef IPolygonH
#define IPolygonH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <Controls.hpp>
#include <vector>
#include "VclObject.h"
//---------------------------------------------------------------------------
enum TPolygonFillMode {pfmAlternate, pfmWinding};
enum TPolygonType {ptPolygon, ptPolyline};

class TIPolygon : public TGraphicControl
{
private:
  std::vector<TPoint> PointList;
  TVclObject<TStringList> FPoints;
  TPolygonFillMode FPolyFillMode;
  TPolygonType FPolygonType;

  void __fastcall Change(TObject *Sender) {Invalidate();}
  void __fastcall PointsChange(TObject *Sender);
  void __fastcall Paint();
  TBrush* __fastcall GetBrush() {return Canvas->Brush;}
  void __fastcall SetBrush(TBrush *ABrush) {Canvas->Brush = ABrush; Invalidate();}
  TPen* __fastcall GetPen();
  void __fastcall SetPen(TPen *APen) {Canvas->Pen = APen; Invalidate();}
  void __fastcall SetPoints(TStrings *APoints) {FPoints->Assign(APoints);}
  TStrings* __fastcall GetPoints();
  void __fastcall SetPolyFillMode(TPolygonFillMode APolyFillMode);
  void __fastcall SetPolygonType(TPolygonType APolygonType);

public:
  __fastcall TIPolygon(TComponent* Owner);
  const std::vector<TPoint>& GetPointList() {return PointList;}
  void Clear();
  void AddPoint(const TPoint &Point);
  void AddPoints(const TPoint *APoints, unsigned Size);

__published:
  __property TBrush* Brush = {read=GetBrush, write=SetBrush};
  __property TPen *Pen = {read=GetPen, write=SetPen};
  __property TStrings *Points = {read=GetPoints, write=SetPoints};
  __property TPolygonFillMode PolyFillMode = {read=FPolyFillMode, write=SetPolyFillMode};
  __property TPolygonType PolygonType = {read=FPolygonType, write=SetPolygonType};
  __property Visible;
  __property Align;
  __property OnClick;
  __property OnDblClick;
  __property OnMouseDown;
  __property OnMouseMove;
  __property OnMouseUp;
};
//---------------------------------------------------------------------------
#endif
