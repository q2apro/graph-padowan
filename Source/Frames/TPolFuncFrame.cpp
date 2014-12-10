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
#include <float.h>
#include "Unit1.h"
#include "TPolFuncFrame.h"
//---------------------------------------------------------------------------
#pragma link "MyEdit"
#pragma link "UpDownEx"
#pragma link "TEvalFrame"
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TPolFuncFrame::TPolFuncFrame(TComponent* Owner)
  : TEvalFrame(Owner)
{
}
//---------------------------------------------------------------------------
void TPolFuncFrame::Eval(const TGraphElem *Elem)
{
  if(const TPolFunc *Func = dynamic_cast<const TPolFunc*>(Elem))
  {
    Clear();

    ErrorPrefix = "t: ";
    long double t = Form1->Data.Calc(ToWString(Edit1->Text));
    ErrorPrefix = "r(t): ";
    long double r = Func->GetFunc().CalcR(t);
    ErrorPrefix = "x(t): ";
    long double x = Func->GetFunc().CalcX(t);
    ErrorPrefix = "y(t): ";
    long double y = Func->GetFunc().CalcY(t);

    Edit2->Text = RoundToStr(r);
    Edit3->Text = RoundToStr(x);
    Edit4->Text = RoundToStr(y);

    Form1->SetCrossPos(x, y);

    ErrorPrefix = "dr/dt: ";
    Edit5->Text = RoundToStr(Func->GetFunc().MakeDif().CalcR(t));
    ErrorPrefix = "dy/dx: ";
    long double dydx = Func->GetFunc().CalcSlope(t);
    if(_finite(dydx))
      Edit6->Text = RoundToStr(dydx);
  }
}
//---------------------------------------------------------------------------
void TPolFuncFrame::SetPoint(const TGraphElem *Elem, int X, int Y)
{
  if(const TPolFunc *Func = dynamic_cast<const TPolFunc*>(Elem))
  {
    TTraceType TraceType;
    switch(ComboBox1->ItemIndex)
    {
      case 0: TraceType = ttTrace;        break;
      case 1: TraceType = ttIntersection; break;
      case 2: TraceType = ttXAxis;        break;
      case 3: TraceType = ttYAxis;        break;
      case 4: TraceType = ttExtremeX;     break;
      case 5: TraceType = ttExtremeY;     break;
    }

    double t = TraceFunction(Func, TraceType, X, Y, Form1->Data, Form1->Draw);
    if(_isnan(t))
      Edit1->Text = "";
    else
      Edit1->Text = RoundToStr(t, ComboBox1->ItemIndex == 0 ? Property.RoundTo : std::max(8, Property.RoundTo));
  }
}
//---------------------------------------------------------------------------
void TPolFuncFrame::Clear()
{
  Edit2->Text = "";
  Edit3->Text = "";
  Edit4->Text = "";
  Edit5->Text = "";
  Edit6->Text = "";
  Form1->CancelStatusError();
}
//---------------------------------------------------------------------------
void __fastcall TPolFuncFrame::ComboBox1Change(TObject *Sender)
{
  Edit1->ReadOnly = ComboBox1->ItemIndex;
  UpDownEx1->Enabled = ComboBox1->ItemIndex == 0;
  Edit1->Color = ComboBox1->ItemIndex ? clBtnFace : clWindow;
}
//---------------------------------------------------------------------------

