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
#include "Unit11.h"
//---------------------------------------------------------------------------
#pragma link "ShadeSelect"
#pragma link "MyEdit"
#pragma link "ExtColorBox"
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TForm11::TForm11(TComponent* Owner, TData &AData)
  : TForm(Owner), Data(AData)
{
  Translate();
  ScaleForm(this);

  ShadeSelect1->ShadeStyle = static_cast<TBrushStyle>(Property.DefaultRelation.Style);
  ExtColorBox1->Selected = Property.DefaultRelation.Color;
  UpDown1->Position = Property.DefaultRelation.Size;
  FlipForm(this);
}
//---------------------------------------------------------------------------
void TForm11::Translate()
{
  TranslateProperties(this);
  TranslateStrings(ExtColorBox1->Items);
  SetAccelerators(this);

  ResizeControl(Edit1, Label1, Label2);
  ResizeControl(Edit2, Label1, Label2);
  ResizeControl(Edit3, Label10);
}
//---------------------------------------------------------------------------
void __fastcall TForm11::Button3Click(TObject *Sender)
{
  Application->HelpContext(HelpContext);
}
//---------------------------------------------------------------------------
int TForm11::EditRelation(const boost::shared_ptr<TRelation> &ARelation)
{
  Relation = ARelation;
  Caption = LoadRes(551);

  Edit1->Text = Relation->GetText().c_str();
  Edit2->Text = Relation->GetConstraints().c_str();
  Edit3->Text = ToUString(Relation->GetLegendText());
  UpDown1->Position = Relation->GetSize();
  ExtColorBox1->Selected = Relation->GetColor();
  ShadeSelect1->ShadeStyle = Relation->GetBrushStyle();

  return ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TForm11::Button1Click(TObject *Sender)
{
  //Bug in CB2009
  //Instantiation of NewRelation must be outside of the try block.
  //Else trying to create an invalid equation followed by a correct equation will
  //cause an Access Violation
  boost::shared_ptr<TRelation> NewRelation;
  try
  {
    NewRelation.reset(new TRelation(
      ToWString(Edit1->Text),
      L"",
      Data.CustomFunctions.SymbolList,
      Data.Axes.Trigonometry
    ));
    try
    {
      NewRelation->SetConstraints(ToWString(Edit2->Text), Data.CustomFunctions.SymbolList);
    }
    catch(Func32::EParseError &E)
    {
      if(E.ErrorCode != Func32::ecEmptyString)
      {
        ShowErrorMsg(E, Edit2);
        return;
      }
    }

    NewRelation->SetLegendText(ToWString(Edit3->Text));
    NewRelation->SetSize(Edit4->Text.ToInt());
    if(NewRelation->GetRelationType() == rtEquation && NewRelation->GetSize() == 0)
      NewRelation->SetSize(1);
    NewRelation->SetColor(ExtColorBox1->Selected);
    NewRelation->SetBrushStyle(ShadeSelect1->ShadeStyle);

    Data.AbortUpdate();
    if(Relation)
    {
      NewRelation->SetVisible(Relation->GetVisible());
      NewRelation->SetShowInLegend(Relation->GetShowInLegend());
      UndoList.Push(TUndoChange(Relation, NewRelation));
      Data.Replace(Relation, NewRelation);
    }
    else
    {
      UndoList.Push(TUndoAdd(NewRelation));
      Data.Insert(NewRelation);
    }
    Data.SetModified();

    Property.DefaultRelation.Set(ShadeSelect1->ShadeStyle, ExtColorBox1->Selected, Edit4->Text.ToInt());
    ModalResult = mrOk;
  }
  catch(Func32::EFuncError &E)
  {
    ShowErrorMsg(E, Edit1);
  }
  catch(EGraphError &E)
  {
    ShowErrorMsg(E, Edit1);
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm11::Edit4KeyPress(TObject *Sender, char &Key)
{
  if(!std::isdigit(Key) && Key != '\b')
    Key = 0;
}
//---------------------------------------------------------------------------

