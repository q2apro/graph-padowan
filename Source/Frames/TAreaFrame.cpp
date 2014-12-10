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
#include <algorithm>
#include "float.h"
#include "Unit1.h"
#include "TAreaFrame.h"
//---------------------------------------------------------------------------
#pragma link "MyEdit"
#pragma link "UpDownEx"
#pragma link "TEvalFrame"
#pragma link "GridPanelEx"
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TAreaFrame::TAreaFrame(TComponent* Owner)
        : TEvalFrame(Owner)
{
}
//---------------------------------------------------------------------------
void TAreaFrame::EvalArea(const TGraphElem *GraphElem)
{
  Edit3->Text = "";
  Form1->IPolygon1->Clear();

  long double From = Form1->Data.Calc(ToWString(Edit1->Text));
  long double To = Form1->Data.Calc(ToWString(Edit2->Text));

  if(const TBaseFuncType *Func = dynamic_cast<const TBaseFuncType*>(GraphElem))
    EvalArea(Func, From, To);
  else if(const TPointSeries *PointSeries = dynamic_cast<const TPointSeries*>(GraphElem))
    EvalArea(PointSeries, From, To);

  Form1->IPolygon1->PolygonType = ptPolygon;
	Form1->IPolygon1->Visible = true;
}
//---------------------------------------------------------------------------
void TAreaFrame::EvalArea(const TBaseFuncType *Func, long double From, long double To)
{
  Edit3->Text = RoundToStr(Func->CalcArea(From, To));

  if(!Func->GetVisible() || Func->sList.empty())
    return;

  if(From > To)
    std::swap(From, To);

  unsigned N1 = std::lower_bound(Func->sList.begin(), Func->sList.end(), From, TCompCoordSet()) - Func->sList.begin();
  unsigned N2 = std::lower_bound(Func->sList.begin() + N1, Func->sList.end(), To, TCompCoordSet()) - Func->sList.begin();

  Func32::TCoord<long double> Min, Max;
  try
  {
    if(_finitel(From))
      Min = Func->Eval(From);
    else
      Min = Func->sList[N1];
  }
  catch(Func32::ECalcError &E)
  {
    Min = Func->sList[N1];
  }

	try
  {
    if(_finitel(To))
      Max = Func->Eval(To);
    else
      Max = Func->sList[N2-1];
  }
  catch(Func32::ECalcError &E)
  {
    Max = Func->sList[N2-1];
  }

  if(N1 != N2)
  {
    Form1->IPolygon1->AddPoint(Form1->Draw.xyPoint(Min.x, Min.y));
    Form1->IPolygon1->AddPoints(&Func->Points[N1], N2 - N1);
    Form1->IPolygon1->AddPoint(Form1->Draw.xyPoint(Max.x, Max.y));
  }

  if(const TPolFunc *PolFunc = dynamic_cast<const TPolFunc*>(Func))
  {
    if(N1 != N2)
      Form1->IPolygon1->AddPoint(TPoint(Form1->Draw.xyPoint(0, 0)));
  }
  else
  {
    Form1->IPolygon1->AddPoint(Form1->Draw.xyPoint(Max.x, 0));
    Form1->IPolygon1->AddPoint(Form1->Draw.xyPoint(Min.x, 0));
  }

  Form1->IPolygon1->Pen->Width = Func->Size;
}
//---------------------------------------------------------------------------
void TAreaFrame::EvalArea(const TPointSeries *PointSeries, long double From, long double To)
{
  TPointSeries::TPointList::const_iterator FromIter = PointSeries->FindPoint(From);
  TPointSeries::TPointList::const_iterator ToIter = PointSeries->FindPoint(To);
  Func32::TDblPoint FromCoord = FindCoord(FromIter, From);
  Func32::TDblPoint ToCoord = FindCoord(ToIter, To);

  Form1->IPolygon1->AddPoint(Form1->Draw.xyPoint(Func32::TDblPoint(FromCoord.x, 0)));
  Form1->IPolygon1->AddPoint(Form1->Draw.xyPoint(FromCoord));
  if(ToIter > FromIter)
    ++FromIter, ++ToIter;

  for(TPointSeries::TPointList::const_iterator Iter = FromIter; Iter != ToIter; ToIter > FromIter ? ++Iter : --Iter)
    Form1->IPolygon1->AddPoint(Form1->Draw.xyPoint(*Iter));
  Form1->IPolygon1->AddPoint(Form1->Draw.xyPoint(ToCoord));
  Form1->IPolygon1->AddPoint(Form1->Draw.xyPoint(Func32::TDblPoint(ToCoord.x, 0)));

  //Area between x-axis and line from P1 to P2: A=(x2-x1)*(y2+y1)/2
  long double Area = 0;
  if(FromIter == ToIter)
    Area = (ToCoord.x - FromCoord.x) * (ToCoord.y + FromCoord.y) / 2;
  else
  {
    Area = (FromIter->x - FromCoord.x) * (FromIter->y + FromCoord.y) / 2;
    if(ToIter > FromIter)
		{
      --ToIter;
      for(TPointSeries::TPointList::const_iterator Iter = FromIter; Iter != ToIter; ++Iter)
        Area += ((Iter+1)->x - Iter->x) * ((Iter+1)->y + Iter->y) / 2;
    }
    else
    {
      ++ToIter;
      for(TPointSeries::TPointList::const_iterator Iter = FromIter; Iter != ToIter; --Iter)
        Area += ((Iter-1)->x - Iter->x) * ((Iter-1)->y + Iter->y) / 2;
    }
    Area += (ToCoord.x - ToIter->x) * (ToCoord.y + ToIter->y) / 2;
  }
  Edit3->Text = ToUString(Area);
}
//---------------------------------------------------------------------------
void TAreaFrame::EvalArc(const TGraphElem *GraphElem)
{
  Edit3->Text = "";
  Form1->IPolygon1->Clear();

  double Min = Form1->Data.Calc(ToWString(Edit1->Text));
  double Max = Form1->Data.Calc(ToWString(Edit2->Text));

  if(Max < Min)
    std::swap(Min, Max);

  if(const TBaseFuncType *Func = dynamic_cast<const TBaseFuncType*>(GraphElem))
  {
		Edit3->Text = RoundToStr(Func->GetFunc().CalcArc(Min, Max, 1E-3));

    if(!GraphElem->GetVisible())
      return;

    unsigned N1 = std::lower_bound(Func->sList.begin(), Func->sList.end(), Min, TCompCoordSet()) - Func->sList.begin();
    unsigned N2 = std::upper_bound(Func->sList.begin() + N1, Func->sList.end(), Max, TCompCoordSet()) - Func->sList.begin();
    if(N1 != N2)
      Form1->IPolygon1->AddPoints(&Func->Points[N1], N2 - N1);
    Form1->IPolygon1->Pen->Width = Func->Size;
  }
  else if(const TTan *Tan = dynamic_cast<const TTan*>(GraphElem))
  {
    try
    {
      double dx = Max - Min;
      double yMin = Tan->GetFunc().CalcY(Min);
      double yMax = Tan->GetFunc().CalcY(Max);
      double dy = yMax - yMin;

      //Length = sqrt(dx^2+dy^2) = sqrt(dx^2+^(a*dx)^2)
      Edit3->Text = RoundToStr(std::sqrt(dx*dx + dy*dy));

      if(!GraphElem->GetVisible())
        return
      Form1->IPolygon1->AddPoint(TPoint(Form1->Draw.xPoint(Min), Form1->Draw.yPoint(yMin)));
      Form1->IPolygon1->AddPoint(TPoint(Form1->Draw.xPoint(Max), Form1->Draw.yPoint(yMax)));
      Form1->IPolygon1->Pen->Width = Tan->Size;
    }
		catch(...)
    {
    }
  }

  Form1->IPolygon1->PolygonType = ptPolyline;
  Form1->IPolygon1->Visible = true;
}
//---------------------------------------------------------------------------
void TAreaFrame::SetPoint(const TGraphElem *Elem, int X, int Y)
{
  if(const TBaseFuncType *Func = dynamic_cast<const TBaseFuncType*>(Elem))
  {
    double t = FindNearestPoint(Func, X, Y); //Returns NAN if no point found
    if(_isnan(t))
    {
      Form1->Cross->Hide();
      Edit1->Text = "";
      Edit2->Text = "";
    }
    else
    {
      Edit1->Text = RoundToStr(t);
      Edit2->Text = Edit1->Text;
    }
  }
  else if(const TPointSeries *PointSeries = dynamic_cast<const TPointSeries*>(Elem))
    Edit1->Text = RoundToStr(Form1->Draw.xCoord(X));
}
//---------------------------------------------------------------------------
void TAreaFrame::SetEndPoint(const TGraphElem *Elem, int X, int Y)
{
  if(const TBaseFuncType *Func = dynamic_cast<const TBaseFuncType*>(Elem))
  {
    double t = FindNearestPoint(Func, X, Y);
    Edit2->Text = _isnan(t) ? String("") : RoundToStr(t);
  }
  else if(const TPointSeries *PointSeries = dynamic_cast<const TPointSeries*>(Elem))
    Edit2->Text = RoundToStr(Form1->Draw.xCoord(X));
}
//---------------------------------------------------------------------------

