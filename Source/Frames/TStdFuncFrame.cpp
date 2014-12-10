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
#include "TStdFuncFrame.h"
#include <float.h>
//---------------------------------------------------------------------------
#pragma link "MyEdit"
#pragma link "UpDownEx"
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TStdFuncFrame::TStdFuncFrame(TComponent* Owner)
  : TEvalFrame(Owner)
{
}
//---------------------------------------------------------------------------
void TStdFuncFrame::Clear()
{
  Edit2->Text = "";
  Edit3->Font->Color = clWindowText;
  Edit3->Text = "";
  Edit4->Font->Color = clWindowText;
  Edit4->Text = "";
  Form1->CancelStatusError();
}
//---------------------------------------------------------------------------
void TStdFuncFrame::Eval(const TGraphElem *Elem)
{
  if(const TStdFunc *Func = dynamic_cast<const TStdFunc*>(Elem))
  {
    Clear();
    ErrorPrefix = "x: ";
    bool UseReal = Property.ComplexFormat == cfReal;
    std::wstring xStr = ToWString(Edit1->Text);
    Func32::TComplex x = UseReal ? Func32::TComplex(Form1->Data.Calc(xStr)) : Form1->Data.CalcComplex(xStr);

    ErrorPrefix = "f(x): ";
    Func32::TComplex y;
    y = UseReal ? Func32::TComplex(Func->GetFunc().CalcY(real(x))): Func->GetFunc().CalcY(x);

    if(!x.imag() && std::abs(y.imag()) < MIN_ZERO)
      Form1->SetCrossPos(x.real(), y.real());

    ErrorPrefix = "f'(x): ";
    Edit2->Text = ComplexToString(y);
    Func32::TFunc Dif1 = Func->GetFunc().MakeDif();
    Func32::TComplex yDif1 = UseReal ? Func32::TComplex(Dif1.CalcY(real(x))) : Dif1.CalcY(x);
    Edit3->Text = ComplexToString(yDif1);

    ErrorPrefix = "f''(x): ";
    Func32::TFunc Dif2 = Dif1.MakeDif();
    Func32::TComplex yDif2 = UseReal ? Func32::TComplex(Dif2.CalcY(real(x))) : Dif2.CalcY(x);
    Edit4->Text = ComplexToString(yDif2);
  }
}
//---------------------------------------------------------------------------
void TStdFuncFrame::SetPoint(const TGraphElem *Elem, int X, int Y)
{
  if(const TStdFunc *Func = dynamic_cast<const TStdFunc*>(Elem))
  {
    Edit1->Text = "";
    TTraceType TraceType;
    switch(ComboBox1->ItemIndex)
    {
      case 0: TraceType = ttTrace;        break;
      case 1: TraceType = ttIntersection; break;
      case 2: TraceType = ttXAxis;        break;
      case 3: TraceType = ttExtremeY;     break;
    }

    double t = TraceFunction(Func, TraceType, X, Y, Form1->Data, Form1->Draw);
    if(_isnan(t))
      Edit1->Text = "";
    else
      Edit1->Text = RoundToStr(t, ComboBox1->ItemIndex == 0 ? Property.RoundTo : std::max(8, Property.RoundTo));
  }
}
//---------------------------------------------------------------------------
void __fastcall TStdFuncFrame::ComboBox1Change(TObject *Sender)
{
  Edit1->ReadOnly = ComboBox1->ItemIndex;
  UpDown1->Enabled = ComboBox1->ItemIndex == 0;
  Edit1->Color = ComboBox1->ItemIndex ? clBtnFace : clWindow;
}
//---------------------------------------------------------------------------

