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
#include "TParFuncFrame.h"
//---------------------------------------------------------------------------
#pragma link "MyEdit"
#pragma link "UpDownEx"
#pragma link "TEvalFrame"
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TParFuncFrame::TParFuncFrame(TComponent* Owner)
  : TEvalFrame(Owner)
{
}
//---------------------------------------------------------------------------
void TParFuncFrame::Eval(const TGraphElem *Elem)
{
  if(const TParFunc *Func = dynamic_cast<const TParFunc*>(Elem))
  {
    Clear();

    ErrorPrefix = "t: ";
    long double t = Form1->Data.Calc(ToWString(Edit1->Text));
    long double x = 0;
    long double y = 0;

    try
    {
      Func32::TComplex xTemp = Func->GetFunc().CalcX(Func32::TComplex(t));
      x = real(xTemp);
      if(!imag(xTemp))
        Edit2->Text = RoundToStr(x);
    }
    catch(Func32::ECalcError&)
    {
    }

    try
    {
      Func32::TComplex yTemp = Func->GetFunc().CalcY(Func32::TComplex(t));
      y = real(yTemp);
      if(!imag(yTemp))
        Edit3->Text = RoundToStr(y);
    }
    catch(Func32::ECalcError&)
    {
    }

    if(!Edit2->Text.IsEmpty() && !Edit3->Text.IsEmpty())
      Form1->SetCrossPos(x, y);

    Func32::TParamFunc Dif = Func->GetFunc().MakeDif();
    long double xDif = 0;
    long double yDif = 0;

    try
    {
      xDif = Dif.CalcX(t);
      if(!Edit2->Text.IsEmpty())
        Edit4->Text = RoundToStr(xDif);
    }
    catch(Func32::ECalcError&)
    {
    }

    try
    {
      yDif = Dif.CalcY(t);
      if(!Edit3->Text.IsEmpty())
        Edit5->Text = RoundToStr(yDif);
    }
    catch(Func32::ECalcError&)
    {
    }

    if(!Edit4->Text.IsEmpty() && !Edit5->Text.IsEmpty())
      if(xDif)
        Edit6->Text = RoundToStr(yDif/xDif);
  }
}
//---------------------------------------------------------------------------
void TParFuncFrame::Clear()
{
  Edit2->Text = "";
  Edit3->Text = "";
  Edit4->Text = "";
  Edit5->Text = "";
  Edit6->Text = "";
  Form1->CancelStatusError();
}
//---------------------------------------------------------------------------
void TParFuncFrame::SetPoint(const TGraphElem *Elem, int X, int Y)
{
  if(const TParFunc *Func = dynamic_cast<const TParFunc*>(Elem))
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
void __fastcall TParFuncFrame::ComboBox1Change(TObject *Sender)
{
  Edit1->ReadOnly = ComboBox1->ItemIndex;
  UpDownEx1->Enabled = ComboBox1->ItemIndex == 0;
  Edit1->Color = ComboBox1->ItemIndex ? clBtnFace : clWindow;
}
//---------------------------------------------------------------------------

