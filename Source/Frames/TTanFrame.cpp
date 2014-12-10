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
#include "Unit1.h"
#include "TTanFrame.h"
#include <float.h>
//---------------------------------------------------------------------------
#pragma link "MyEdit"
#pragma link "UpDownEx"
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TTanFrame::TTanFrame(TComponent* Owner)
  : TEvalFrame(Owner)
{
}
//---------------------------------------------------------------------------
void TTanFrame::EvalTan(const TTan *Tan)
{
  Edit2->Text = "";

  double x = Form1->Data.Calc(ToWString(Edit1->Text));
  double y = Tan->GetFunc().CalcY(x);
  Edit2->Text = RoundToStr(y);

  Form1->SetCrossPos(x, y);
}
//---------------------------------------------------------------------------
void TTanFrame::EvalSeries(const TPointSeries *Series)
{
  Edit2->Text = "";
  long double x = Form1->Data.Calc(ToWString(Edit1->Text));
  TPointSeries::TPointList::const_iterator Iter = Series->FindPoint(x);
  Func32::TDblPoint Coord = FindCoord(Iter, x);
  Edit2->Text = RoundToStr(Coord.y);

  Form1->SetCrossPos(Coord.x, Coord.y);
}
//---------------------------------------------------------------------------
void TTanFrame::SetPoint(const TTan *Tan, int X)
{
  Edit1->Text = RoundToStr(Range(Tan->From.Value, Form1->Draw.xCoord(X), Tan->To.Value));
}
//---------------------------------------------------------------------------
void TTanFrame::SetPoint(const TPointSeries *Series, int X)
{
  Edit1->Text = RoundToStr(Form1->Draw.xCoord(X));
}
//---------------------------------------------------------------------------
void TTanFrame::Eval(const TGraphElem *Elem)
{
  if(const TTan *Tan = dynamic_cast<const TTan*>(Elem))
    EvalTan(Tan);
  else if(const TPointSeries *Series = dynamic_cast<const TPointSeries*>(Elem))
    EvalSeries(Series);
}
//---------------------------------------------------------------------------
void TTanFrame::SetPoint(const TGraphElem *Elem, int X, int Y)
{
  if(const TTan *Tan = dynamic_cast<const TTan*>(Elem))
    SetPoint(Tan, X);
  else if(const TPointSeries *Series = dynamic_cast<const TPointSeries*>(Elem))
    SetPoint(Series, X);
}
//---------------------------------------------------------------------------


