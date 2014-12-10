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
#include "Unit9.h"
#include <complex>
#include <float.h>
//---------------------------------------------------------------------------
#pragma link "TStdFuncFrame"
#pragma link "TTanFrame"
#pragma link "TParFuncFrame"
#pragma link "TPolFuncFrame"
#pragma link "TAreaFrame"
#pragma link "TAreaFrame"
#pragma link "TParFuncFrame"
#pragma link "TPolFuncFrame"
#pragma link "TStdFuncFrame"
#pragma link "TTanFrame"
#pragma link "TEvalFrame"
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TForm9::TForm9(TComponent* Owner)
    : TForm(Owner), VisibleFrame(NULL), SizeDif(Height - ClientHeight), EvalType(etEval)
{
  TP_GlobalIgnoreClassProperty(__classid(TForm), "PopupParent");
  BiDiMode = bdLeftToRight;
  StdFuncStrings->Assign(StdFuncFrame1->ComboBox1->Items);
  ParFuncStrings->Assign(ParFuncFrame1->ComboBox1->Items);
  PolFuncStrings->Assign(PolFuncFrame1->ComboBox1->Items);
  TranslateComponent(this);
  Translate();
  ScaleForm(this);
  FlipForm(this);
}
//---------------------------------------------------------------------------
void TForm9::Translate()
{
  RetranslateComponent(this);
  StdFuncFrame1->ComboBox1->Items->Assign(StdFuncStrings);
  TranslateStrings(StdFuncFrame1->ComboBox1->Items);
  ParFuncFrame1->ComboBox1->Items->Assign(ParFuncStrings);
  TranslateStrings(ParFuncFrame1->ComboBox1->Items);
  PolFuncFrame1->ComboBox1->Items->Assign(PolFuncStrings);
  TranslateStrings(PolFuncFrame1->ComboBox1->Items);
  AreaFrame1->Label3->Caption = LoadRes(EvalType == etArea ? RES_AREA : RES_LENGTH) + L':';
  SetAccelerators(this);
  ResizeControl(StdFuncFrame1->ComboBox1, StdFuncFrame1->Label5);
  ResizeControl(ParFuncFrame1->ComboBox1, ParFuncFrame1->Label7);
  ResizeControl(PolFuncFrame1->ComboBox1, PolFuncFrame1->Label7);

  StdFuncFrame1->ComboBox1->ItemIndex = 0;
  ParFuncFrame1->ComboBox1->ItemIndex = 0;
  PolFuncFrame1->ComboBox1->ItemIndex = 0;
}
//---------------------------------------------------------------------------
//User clicked on a (X,Y) pixel coordinate
void TForm9::StartValueChanged(int X, int Y)
{
  try
  {
    if(!Elem || !Visible)
      return;

    if(VisibleFrame)
      VisibleFrame->SetPoint(Elem.get(), X, Y);
  }
  catch(Func32::EFuncError &Error)
  {
    if(Error.ErrorCode != Func32::ecEmptyString)
      Form1->ShowStatusError(GetErrorMsg(Error));
  }
}
//---------------------------------------------------------------------------
void TForm9::EndValueChanged(int X, int Y)
{
  if(VisibleFrame)
    VisibleFrame->SetEndPoint(Elem.get(), X, Y);
}
//---------------------------------------------------------------------------
void TForm9::FuncChanged(const boost::shared_ptr<TGraphElem> &AElem)
{
  Elem = AElem;
  if(!Elem)
  {
    ShowFrame(NULL);
    return;
  }
  switch(EvalType)
  {
    case etEval:
      if(dynamic_cast<TTan*>(Elem.get()) || dynamic_cast<TPointSeries*>(Elem.get()))
        ShowFrame(TanFrame1);
      else if(dynamic_cast<TStdFunc*>(Elem.get()))
        ShowFrame(StdFuncFrame1);
      else if(dynamic_cast<TParFunc*>(Elem.get()))
        ShowFrame(ParFuncFrame1);
      else if(dynamic_cast<TPolFunc*>(Elem.get()))
        ShowFrame(PolFuncFrame1);
      else
        ShowFrame(NULL);

      Edit1Change(NULL);
      break;

    case etArea:
    case etArc:
      if(dynamic_cast<TBaseFuncType*>(Elem.get()) ||
         dynamic_cast<TPointSeries*>(Elem.get()))
      {
        ShowFrame(AreaFrame1); //Only show area frame for functions and tangents
        Edit1Change(NULL);
      }
      else
        ShowFrame(NULL);
      break;

    default:
      ShowFrame(NULL);
      break;
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm9::FormHide(TObject *Sender)
{
  Form1->Cross->Hide();
  Form1->EvalAction->Checked = false;
  Form1->PathAction->Checked = false;
  Form1->AreaAction->Checked = false;
  Form1->AnimateAction->Checked = false;
  Form1->Panel1->Height = 0;

  Form1->IPolygon1->Hide();
}
//---------------------------------------------------------------------------
void TForm9::Clear()
{
  Hide();
  Elem.reset();
}
//---------------------------------------------------------------------------
void __fastcall TForm9::Edit1Change(TObject *Sender)
{
  try
  {
    Form1->Cross->Hide();
    Form1->IPolygon1->Hide();
    if(Sender != NULL)
      Form1->CancelStatusError();

    if(Visible && VisibleFrame)
      switch(EvalType)
      {
        case etEval:
          VisibleFrame->Eval(Elem.get());
          break;

        case etArea:
          AreaFrame1->EvalArea(Elem.get());
          break;

        case etArc:
          AreaFrame1->EvalArc(Elem.get());
          break;
      }
  }
  catch(Func32::EFuncError &Error)
  {
    if(Error.ErrorCode != Func32::ecEmptyString)
      Form1->ShowStatusError(ToUString(VisibleFrame->GetErrorPrefix()) + GetErrorMsg(Error) + " (" + LoadRes(RES_ERROR_NUMBER, Error.ErrorCode) + ")");
  }
  catch(EOverflow&)
  {
    Form1->ShowStatusError(LoadRes(RES_Overflow));
  }
  catch(EAbort&)
  { //Ignore
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm9::FormShortCut(TWMKey &Msg, bool &Handled)
{
  if(GetKeyState(VK_CONTROL))
    switch(Msg.CharCode)
    {
      case 'X':
        ActiveControl->Perform(WM_CUT, 0, 0);
        Handled = true;
        break;
      case 'C':
        ActiveControl->Perform(WM_COPY, 0, 0);
        Handled = true;
        break;
      case 'V':
        ActiveControl->Perform(WM_PASTE, 0, 0);
        Handled = true;
        break;
      case 'A':
        ActiveControl->Perform(EM_SETSEL, 0, -1);
        Handled = true;
        break;
      case 'Z':
        ActiveControl->Perform(WM_UNDO, 0, 0);
        Handled = true;
        break;
    }
}
//---------------------------------------------------------------------------
void __fastcall TForm9::FormStartDock(TObject *Sender,
      TDragDockObject *&DragObject)
{
  //Graphics error when hints are shown when draging
  Form1->StatusBar1->AutoHint = false;

  //This is necessary to take care of a resize problem when first undocked.
  //The problem was introduced in Graph 2.7 with no obvius cause.
  //It may be Patch 4 for BCB6 that introduced it.
  if(VisibleFrame)
    GetFrame()->Width = ClientWidth;
}
//---------------------------------------------------------------------------
void __fastcall TForm9::FormEndDock(TObject *Sender, TObject *Target,
      int X, int Y)
{
  Form1->StatusBar1->AutoHint = true;
}
//---------------------------------------------------------------------------
void TForm9::ShowFrame(TEvalFrame *Frame)
{
  if(Frame != VisibleFrame)
  {
    if(VisibleFrame)
      GetFrame()->Hide();

    VisibleFrame = Frame;

    if(VisibleFrame)
    {
      GetFrame()->Show();
      ClientHeight = GetFrame()->Height;
    }
    if(Form1->Panel1->Height)
      Form1->Panel1->Height = VisibleFrame ? GetFrame()->Height+10 : 110;
    UndockHeight = ClientHeight + SizeDif;
  }

  Edit1Change(NULL);
}
//---------------------------------------------------------------------------
void TForm9::SetEvalType(TEvalType AEvalType)
{
  if(AEvalType != etArc && AEvalType != etArea)
    Form1->IPolygon1->Hide();

  EvalType = AEvalType;
  Visible = true;
  if(Form1->Panel4->VisibleDockClientCount)
    Form1->Panel1->Height = VisibleFrame ? GetFrame()->Height+10 : 100;
  switch(EvalType)
  {
    case etEval:
      Caption = LoadRes(527);
      break;

    case etArea:
      AreaFrame1->Label3->Caption = LoadRes(RES_AREA) + L':';
			Caption = LoadRes(522);
      AreaFrame1->Edit3->Hint = LoadRes(546);
      //Update grid panel. Setting AutoSize=true might have worked if it was accesible
      AreaFrame1->GridPanel1->Align = alNone;
      AreaFrame1->GridPanel1->Align = alClient;
      break;

    case etArc:
      AreaFrame1->Label3->Caption = LoadRes(RES_LENGTH) + L':';
      Caption = LoadRes(523);
      AreaFrame1->Edit3->Hint = LoadRes(547);
      //Update grid panel. Setting AutoSize=true might have worked if it was accesible
      AreaFrame1->GridPanel1->Align = alNone;
      AreaFrame1->GridPanel1->Align = alClient;
      break;
  }
  FuncChanged(Elem);
}
//---------------------------------------------------------------------------
void __fastcall TForm9::FormResize(TObject *Sender)
{
  if(VisibleFrame)
    GetFrame()->Width = ClientWidth;
}
//---------------------------------------------------------------------------
void __fastcall TForm9::UpDownExUpDown(TObject *Sender, TUDBtnType Button)
{
  try
  {
    if(TUpDownEx *UpDown = dynamic_cast<TUpDownEx*>(Sender))
      if(TCustomEdit *Edit = dynamic_cast<TCustomEdit*>(UpDown->Associate))
      {
        Func32::TComplex x = Form1->Data.CalcComplex(ToWString(Edit->Text));
        Edit->Text = ComplexToString(x + (Button == btPrev ? -0.1L : 0.1L));
      }
  }
  catch(Func32::EFuncError &E)
  { //Ignore errors
  }
}
//---------------------------------------------------------------------------

