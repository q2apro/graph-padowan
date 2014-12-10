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
#include "Unit16.h"
#include <float.h>
//---------------------------------------------------------------------------
#pragma link "ShadeSelect"
#pragma link "MyRadioButton"
#pragma link "ExtColorBox"
#pragma link "MyEdit"
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TForm16::TForm16(TComponent* Owner, TData &AData)
  : TForm(Owner), Data(AData)
{
  TranslateProperties(this);
  TranslateStrings(ExtColorBox1->Items);
  SetAccelerators(this);
  ResizeControl(Edit5, Label12);
  ScaleForm(this);
  FlipForm(this);

  ShadeSelect1->ShadeStyle = static_cast<TBrushStyle>(Property.DefaultShade.Style);
  ExtColorBox1->Selected = Property.DefaultShade.Color;
  EnableGroupBox2(false);

  int I = 1;
  String CmpStr;
  bool Found = true;
  while(Found)
  {
    CmpStr = LoadRes(RES_SHADE) + L" " + I++;
    Found = false;
    for(unsigned N = 0; N < Data.ElemCount(); N++)
      for(unsigned J = 0; J < Data.GetElem(N)->ChildCount(); J++)
        if(boost::shared_ptr<TShading> Shade = boost::dynamic_pointer_cast<TShading>(Data.GetElem(N)->GetChild(J)))
          if(CmpStr == ToUString(Shade->GetLegendText()))
            Found = true;
  }
  Edit5->Text = CmpStr;
}
//---------------------------------------------------------------------------
void TForm16::ShowFuncList()
{
  for(unsigned I = 0; I < Data.ElemCount(); I++)
    if(boost::shared_ptr<TBaseFuncType> F = boost::dynamic_pointer_cast<TBaseFuncType>(Data.GetElem(I)))
      if(F != Func)
      {
        FuncList.push_back(F);
        ListBox1->Items->Add(ToUString(F->MakeLegendText()));
      }

  if(ListBox1->Items->Count)
    if(!!OldShade && OldShade->ShadeStyle == ssBetween)
      ListBox1->ItemIndex = IndexOf(FuncList, OldShade->Func2);
    else
      ListBox1->ItemIndex = -1;
}
//---------------------------------------------------------------------------
void __fastcall TForm16::ImageClick(TObject *Sender)
{
  if(Sender == Image1)
    RadioButton1->SetFocus();
  else if(Sender == Image2)
    RadioButton2->SetFocus();
  else if(Sender == Image3)
    RadioButton3->SetFocus();
  else if(Sender == Image4)
    RadioButton4->SetFocus();
  else if(Sender == Image5)
  {
    if(RadioButton5->CanFocus())
      RadioButton5->SetFocus();
  }
  else if(Sender == Image6)
    RadioButton6->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TForm16::Button1Click(TObject *Sender)
{
  boost::shared_ptr<TShading> Shade(new TShading);
  if(RadioButton1->Checked)
    Shade->ShadeStyle = ssXAxis;
  else if(RadioButton2->Checked)
    Shade->ShadeStyle = ssBelow;
  else if(RadioButton3->Checked)
    Shade->ShadeStyle = ssAbove;
  else if(RadioButton4->Checked)
    Shade->ShadeStyle = ssYAxis;
  else if(RadioButton5->Checked)
    Shade->ShadeStyle = ssInside;
  else if(RadioButton6->Checked)
    Shade->ShadeStyle = ssBetween;
  else
  {
    MessageBox(L"No shading style selected!", L"Selection error");
    return;
  }

  Shade->BrushStyle = ShadeSelect1->ShadeStyle;
  Shade->Color = ExtColorBox1->Selected;

  Shade->SetLegendText(ToWString(Edit5->Text));
  Shade->sMin.Text = ToWString(Edit1->Text);
  Shade->sMax.Text = ToWString(Edit2->Text);
  if(Edit1->Text.IsEmpty())
    Shade->sMin.Value = -INF;
  else
    Shade->sMin.Value = MakeFloat(Edit1);

  if(Edit2->Text.IsEmpty())
    Shade->sMax.Value = +INF;
  else
    Shade->sMax.Value = MakeFloat(Edit2);

  Shade->ExtendMinToIntercept = CheckBox1->Checked;
  Shade->ExtendMaxToIntercept = CheckBox2->Checked;

  //Initialize values for 2. function to values of 1. function as default
  Shade->sMin2 = Shade->sMin;
  Shade->sMax2 = Shade->sMax;
  Shade->ExtendMin2ToIntercept = Shade->ExtendMinToIntercept;
  Shade->ExtendMax2ToIntercept = Shade->ExtendMaxToIntercept;
  Shade->MarkBorder = CheckBox5->Checked;

  if(Shade->ShadeStyle == ssBetween)
  {
    if(ListBox1->ItemIndex == -1)
    {
      PageControl1->ActivePageIndex = 2;
      ListBox1->SetFocus();
      MessageBox(LoadRes(539), LoadRes(RES_SELECTION_ERROR));
      return;
    }
    Shade->Func2 = FuncList[ListBox1->ItemIndex];
    if(!Edit3->Text.IsEmpty() || !Edit4->Text.IsEmpty())
    {
      Shade->sMin2.Text = ToWString(Edit3->Text);
      Shade->sMax2.Text = ToWString(Edit4->Text);
      if(Edit3->Text.IsEmpty())
        Shade->sMin2.Value = -INF;
      else
        Shade->sMin2.Value = MakeFloat(Edit3);

      if(Edit4->Text.IsEmpty())
        Shade->sMax2.Value = +INF;
      else
        Shade->sMax2.Value = MakeFloat(Edit4);

      Shade->ExtendMin2ToIntercept = CheckBox3->Checked;
      Shade->ExtendMax2ToIntercept = CheckBox4->Checked;
    }
  }
  else
    Shade->Func2.reset();

  if(OldShade)
  {
    Shade->SetVisible(OldShade->GetVisible());
    Shade->SetShowInLegend(OldShade->GetShowInLegend());
    int Index = OldShade->GetParent()->GetChildIndex(OldShade);
    UndoList.Push(TUndoChange(OldShade, Shade));
    OldShade->GetParent()->ReplaceChild(Index, Shade);
  }
  else
  {
    UndoList.Push(TUndoAdd(Shade));
    Func->InsertChild(Shade);
  }

  Property.DefaultShade.Set(ShadeSelect1->ShadeStyle, ExtColorBox1->Selected, 0);
  ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TForm16::ExtColorBox1Change(TObject *Sender)
{
  ShadeSelect1->Color = ExtColorBox1->Selected;
}
//---------------------------------------------------------------------------
void __fastcall TForm16::RadioButton6CheckedChange(TObject *Sender)
{
  ListBox1->Enabled = RadioButton6->Checked;
  EnableGroupBox2(RadioButton6->Checked);
}
//---------------------------------------------------------------------------
TModalResult TForm16::InsertShade(const boost::shared_ptr<TBaseFuncType> &AFunc)
{
  Func = AFunc;
  return ShowModal();
}
//---------------------------------------------------------------------------
int TForm16::EditShade(const boost::shared_ptr<TShading> &AShade)
{
  OldShade = AShade;
  if(OldShade)
  {
    Caption = LoadRes(538);
    Edit1->Text = OldShade->sMin.Text.c_str();
    Edit2->Text = OldShade->sMax.Text.c_str();
    Func = boost::dynamic_pointer_cast<TBaseFuncType>(OldShade->GetParent());
    ShadeSelect1->ShadeStyle = OldShade->BrushStyle;
    ExtColorBox1->Selected = OldShade->Color;
    CheckBox1->Checked = OldShade->ExtendMinToIntercept;
    CheckBox2->Checked = OldShade->ExtendMaxToIntercept;
    CheckBox5->Checked = OldShade->MarkBorder;
    Edit5->Text = ToUString(OldShade->GetLegendText());

    GroupBox2->Enabled = false;
    switch(OldShade->ShadeStyle)
    {
      case ssXAxis:   RadioButton1->Checked = true; break;
      case ssBelow:   RadioButton2->Checked = true; break;
      case ssAbove:   RadioButton3->Checked = true; break;
      case ssYAxis:   RadioButton4->Checked = true; break;
      case ssInside:  RadioButton5->Checked = true; break;
      case ssBetween:
        RadioButton6->Checked = true;
        ListBox1->Enabled = true;
        EnableGroupBox2(true);
        Edit3->Text = OldShade->sMin2.Text.c_str();
        Edit4->Text = OldShade->sMax2.Text.c_str();
        CheckBox3->Checked = OldShade->ExtendMin2ToIntercept;
        CheckBox4->Checked = OldShade->ExtendMax2ToIntercept;
    }
  }
  return ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TForm16::FormShow(TObject *Sender)
{
  PageControl1->ActivePageIndex = 0;
  ShowFuncList();
}
//---------------------------------------------------------------------------
void __fastcall TForm16::Button3Click(TObject *Sender)
{
  Application->HelpContext(HelpContext);
}
//---------------------------------------------------------------------------
void TForm16::EnableGroupBox2(bool Enable)
{
  Label8->Enabled = Enable;
  Label9->Enabled = Enable;
  CheckBox3->Enabled = Enable;
  CheckBox4->Enabled = Enable;
  Edit3->Enabled = Enable;
  Edit4->Enabled = Enable;
  GroupBox2->Enabled = Enable;
}
//---------------------------------------------------------------------------

