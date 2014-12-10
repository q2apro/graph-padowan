/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2006 Ivan Johansen
 *
 * Graph is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
#include "Graph.h"
#pragma hdrstop
#include "Unit21.h"
//---------------------------------------------------------------------------
#pragma link "MyEdit"
#pragma resource "*.dfm"

const TLabelPlacement IndexToPlacement[] = {lpUserTopLeft, lpUserTopRight, lpUserBottomLeft, lpUserBottomRight};
//---------------------------------------------------------------------------
__fastcall TForm21::TForm21(TComponent* Owner, TData &AData, const TDraw &ADraw, const boost::shared_ptr<TTextLabel> &ALabel)
  : TForm(Owner), Label(ALabel), Data(AData), Draw(ADraw), OldPlacement(ALabel->GetPlacement())
{
  ScaleForm(this);
  TranslateProperties(this);
  SetAccelerators(this);
  FlipForm(this);

  const int PlacementToIndex[] = {0, -1, -1, -1, -1, 1, 2, 3};
  ComboBox1->ItemIndex = PlacementToIndex[Label->GetPlacement()];
  Edit1->Text = ToUString(Label->GetXPos().Text);
  Edit2->Text = ToUString(Label->GetYPos().Text);
}
//---------------------------------------------------------------------------
void __fastcall TForm21::TntButton1Click(TObject *Sender)
{
  TTextValue xPos;
  TTextValue yPos;
  xPos.Value = MakeFloat(Edit1);
  yPos.Value = MakeFloat(Edit2);
  xPos.Text = ToWString(Edit1->Text);
  yPos.Text = ToWString(Edit2->Text);

  int Index = ComboBox1->ItemIndex;
  if(Index == -1)
    Index = 0;

  boost::shared_ptr<TTextLabel> NewLabel(new TTextLabel(
    Label->GetText(),
    IndexToPlacement[Index],
    xPos,
    yPos,
    Label->GetBackgroundColor(),
    Label->GetRotation(),
    Label->GetOleLink()
  ));
  UndoList.Push(TUndoChange(Label, NewLabel));
  Data.Replace(Label, NewLabel);
  NewLabel->Update();
  ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TForm21::ComboBox1Change(TObject *Sender)
{
  TLabelPlacement NewPlacement = IndexToPlacement[ComboBox1->ItemIndex];
  double xPos = Data.Calc(ToWString(Edit1->Text));
  double yPos = Data.Calc(ToWString(Edit2->Text));

  double Width = Draw.xCoord(Label->GetRect().Right) - Draw.xCoord(Label->GetRect().Left);
  double Height = Draw.yCoord(Label->GetRect().Top) - Draw.yCoord(Label->GetRect().Bottom);
  switch(OldPlacement)
  {
    case lpUserTopRight:    xPos -= Width; break;
    case lpUserBottomLeft:  yPos += Height; break;
    case lpUserBottomRight: xPos -= Width; yPos += Height; break;
  }

  switch(NewPlacement)
  {
    case lpUserTopRight:    xPos += Width; break;
    case lpUserBottomLeft:  yPos -= Height; break;
    case lpUserBottomRight: xPos += Width; yPos -= Height; break;
  }

  Edit1->Text = xPos;
  Edit2->Text = yPos;
  OldPlacement = NewPlacement;
}
//---------------------------------------------------------------------------

