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
#include "Unit3.h"
//---------------------------------------------------------------------------
#pragma link "ExtColorBox"
#pragma link "MyEdit"
#pragma link "CheckBoxEx"
#pragma link "GridPanelEx"
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TForm3::TForm3(TComponent* Owner, TData &AData)
	: TForm(Owner), Data(AData)
{
  Translate();
  ScaleForm(this);

  const TAxes &Axes = Data.Axes;
  Edit1->Text = RoundToStr(Axes.xAxis.Min, 8);
  Edit2->Text = RoundToStr(Axes.xAxis.Max, 8);
  Edit3->Text = RoundToStr(Axes.xAxis.TickUnit, 8);
  Edit4->Text = RoundToStr(Axes.xAxis.GridUnit, 8);
  Edit5->Text = Axes.xAxis.Label.c_str();
  Edit6->Text = RoundToStr(Axes.xAxis.AxisCross, 8);
  CheckBox1->Checked = Axes.xAxis.LogScl;
  CheckBox2->Checked = Axes.xAxis.ShowNumbers;
  CheckBox3->Checked = Axes.xAxis.ShowLabel;
  CheckBox4->Checked = Axes.xAxis.AutoTick;
  CheckBox5->Checked = Axes.xAxis.AutoGrid;
  CheckBox6->Checked = Axes.xAxis.ShowTicks;
  CheckBox7->Checked = Axes.xAxis.ShowGrid;
	CheckBox17->Checked = Axes.xAxis.MultipleOfPi;

  Edit7->Text = RoundToStr(Axes.yAxis.Min, 8);
  Edit8->Text = RoundToStr(Axes.yAxis.Max, 8);
  Edit9->Text = RoundToStr(Axes.yAxis.TickUnit, 8);
  Edit10->Text = RoundToStr(Axes.yAxis.GridUnit, 8);
  Edit11->Text = Axes.yAxis.Label.c_str();
  Edit12->Text = RoundToStr(Axes.yAxis.AxisCross, 8);
  CheckBox8->Checked = Axes.yAxis.LogScl;
  CheckBox9->Checked = Axes.yAxis.ShowNumbers;
  CheckBox10->Checked = Axes.yAxis.ShowLabel;
  CheckBox11->Checked = Axes.yAxis.AutoTick;
  CheckBox12->Checked = Axes.yAxis.AutoGrid;
  CheckBox13->Checked = Axes.yAxis.ShowTicks;
  CheckBox14->Checked = Axes.yAxis.ShowGrid;
	CheckBox18->Checked = Axes.yAxis.MultipleOfPi;

  Edit13->Text = Axes.Title.c_str();
  TitleFont->Assign(Axes.TitleFont);
  CheckBox15->Checked = Axes.ShowLegend;
  RadioGroup1->ItemIndex = Axes.AxesStyle;
  RadioGroup2->ItemIndex = Axes.Trigonometry == Func32::Degree;
  RadioGroup3->ItemIndex = Axes.LegendPlacement - 1;
  RadioGroup3->Enabled = Axes.ShowLegend;
  CheckBox19->Checked = Axes.CalcComplex;

  ExtColorBox1->Selected = Axes.BackgroundColor;
  ExtColorBox2->Selected = Axes.AxesColor;
  ExtColorBox3->Selected = Axes.GridColor;
  LabelFont = Axes.LabelFont;
  NumberFont = Axes.NumberFont;
  LegendFont = Axes.LegendFont;

  Panel1->Align = alClient;
  Panel2->Align = alClient;
  FlipForm(this);
  UpdateEnabledControls();
}
//---------------------------------------------------------------------------
void TForm3::Translate()
{
  TranslateProperties(this);
  TranslateStrings(ExtColorBox1->Items);
  TranslateStrings(ExtColorBox2->Items);
  TranslateStrings(ExtColorBox3->Items);
  SetAccelerators(this);
  ResizeControl(Edit13, Label16);
}
//---------------------------------------------------------------------------
void __fastcall TForm3::Button1Click(TObject *Sender)
{
  TAxis xAxis;
  TAxis yAxis;

  xAxis.Min = MakeFloat(Edit1);
  xAxis.Max = MakeFloat(Edit2);
  yAxis.Min = MakeFloat(Edit7);
  yAxis.Max = MakeFloat(Edit8);
  xAxis.TickUnit = MakeFloat(Edit3);
  yAxis.TickUnit = MakeFloat(Edit9);
  xAxis.GridUnit = MakeFloat(Edit4);
  yAxis.GridUnit = MakeFloat(Edit10);
  xAxis.AxisCross = MakeFloat(Edit6);
  yAxis.AxisCross = MakeFloat(Edit12);
  RangeCheck(xAxis.Min < xAxis.Max, Edit2, LoadRes(500));
  RangeCheck(yAxis.Min < yAxis.Max, Edit8, LoadRes(501));

  double xUnitLimit = CheckBox1->Checked ? 1 : 0; //Limit is 1 for log scale, else it is 0
  double yUnitLimit = CheckBox8->Checked ? 1 : 0; //Limit is 1 for log scale, else it is 0

  RangeCheck(xAxis.TickUnit > xUnitLimit, Edit3, LoadRes(RES_GREATER, Label3->Caption, xUnitLimit));
  RangeCheck(xAxis.GridUnit > xUnitLimit, Edit4, LoadRes(RES_GREATER, Label14->Caption, xUnitLimit));
  RangeCheck(yAxis.TickUnit > yUnitLimit, Edit9, LoadRes(RES_GREATER, Label7->Caption, yUnitLimit));
  RangeCheck(yAxis.GridUnit > yUnitLimit, Edit10, LoadRes(RES_GREATER, Label9->Caption, yUnitLimit));

  //If log scale for x-axis
  if(CheckBox1->Checked)
  {
    RangeCheck(xAxis.Min>0, Edit1, LoadRes(506));

    if(RadioGroup1->ItemIndex != 1)
      yAxis.AxisCross = 1;
    else
      RangeCheck(yAxis.AxisCross>0, Edit12, LoadRes(507));
  }

  //If log scale for y-axis
  if(CheckBox8->Checked)
  {
    RangeCheck(yAxis.Min>0, Edit7, LoadRes(508));

    if(RadioGroup1->ItemIndex != 1)
      xAxis.AxisCross = 1;
    else
      RangeCheck(xAxis.AxisCross>0, Edit6, LoadRes(509));
  }

  //Check for out of range
  if(xAxis.Max-xAxis.Min < 1E-10 || yAxis.Max-yAxis.Min < 1E-10)
  {
    MessageBox(LoadRes(RES_MAX_ZOOM), LoadRes(RES_ERROR_IN_VALUE));
    return;
  }

  //Check if we have zoomed so much out that log10(xMin/xMax) can no longer be evaluated
  if(xAxis.Max>1E15 || yAxis.Max>1E15 || xAxis.Min < -1E15 || yAxis.Min < -1E15)
  {
    MessageBox(LoadRes(RES_MIN_ZOOM), LoadRes(RES_ERROR_IN_VALUE));
    return;
  }

  Data.AbortUpdate();
  Data.ClearCache();
  UndoList.Push(TUndoAxes(Data));

  xAxis.LogScl = CheckBox1->Checked;
  xAxis.ShowNumbers = CheckBox2->Checked;
  xAxis.ShowLabel = CheckBox3->Checked;
  xAxis.Label = ToWString(Edit5->Text);
  xAxis.AutoTick = CheckBox4->Checked;
  xAxis.AutoGrid = CheckBox5->Checked;
  xAxis.ShowTicks = CheckBox6->Checked;
  xAxis.ShowGrid = CheckBox7->Checked;
	xAxis.MultipleOfPi = CheckBox17->Checked;

  yAxis.LogScl = CheckBox8->Checked;
  yAxis.ShowNumbers = CheckBox9->Checked;
  yAxis.ShowLabel = CheckBox10->Checked;
  yAxis.Label = ToWString(Edit11->Text);
  yAxis.AutoTick = CheckBox11->Checked;
  yAxis.AutoGrid = CheckBox12->Checked;
  yAxis.ShowTicks = CheckBox13->Checked;
  yAxis.ShowGrid = CheckBox14->Checked;
	yAxis.MultipleOfPi = CheckBox18->Checked;

  TAxes &Axes = Data.Axes;
  Axes.xAxis = xAxis;
  Axes.yAxis = yAxis;

  Axes.Title = ToWString(Edit13->Text);
  Axes.TitleFont->Assign(TitleFont);
  Axes.ShowLegend = CheckBox15->Checked;
  Axes.AxesStyle = static_cast<TAxesStyle>(RadioGroup1->ItemIndex);
  Func32::TTrigonometry OldTrig = Axes.Trigonometry;
  Axes.Trigonometry = RadioGroup2->ItemIndex == 0 ? Func32::Radian : Func32::Degree;
  Axes.LegendPlacement = static_cast<TLegendPlacement>(RadioGroup3->ItemIndex + 1);
  Axes.CalcComplex = CheckBox19->Checked;
  //Ensure that the legend position is valid.
  if(Axes.xAxis.LogScl && Axes.LegendPos.x <= 0)
    Axes.LegendPos.x = Axes.xAxis.Min;
  if(Axes.yAxis.LogScl && Axes.LegendPos.y <= 0)
    Axes.LegendPos.y = Axes.yAxis.Min;

  Axes.BackgroundColor = ExtColorBox1->Selected;
  Axes.AxesColor = ExtColorBox2->Selected;
  Axes.GridColor = ExtColorBox3->Selected;
  Axes.LabelFont = LabelFont;
  Axes.NumberFont = NumberFont;
  Axes.LegendFont = LegendFont;

  //Handle Zoom Square
//  Axes.HandleZoomSquare(Form1->Draw.GetScaledYAxis());

  //Set trigonometry property for all functions if the setting changed
  if(OldTrig != Axes.Trigonometry)
    Data.Update();

  if(CheckBox16->Checked)
    Data.SaveDefault();

  ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void TForm3::RangeCheck(bool Condition, TWinControl *WinControl, const String &Str)
{
	if(!Condition)
	{
		for(TWinControl *Parent = WinControl->Parent; Parent != NULL; Parent = Parent->Parent)
			if(TTabSheet *TabSheet = dynamic_cast<TTabSheet*>(Parent))
			{
				PageControl1->ActivePage = TabSheet;
				WinControl->SetFocus();
			}
		MessageBox(Str, LoadRes(RES_ERROR_IN_VALUE));
    throw EAbort("");
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm3::Button3Click(TObject *Sender)
{
  Application->HelpContext(HelpContext);
}
//---------------------------------------------------------------------------
void __fastcall TForm3::BitBtn1Click(TObject *Sender)
{
  FontDialog1->Font->Assign(TitleFont);
  if(FontDialog1->Execute())
    TitleFont->Assign(FontDialog1->Font);
}
//---------------------------------------------------------------------------
void __fastcall TForm3::BitBtn2Click(TObject *Sender)
{
  FontDialog1->Font->Assign(LabelFont);
  if(FontDialog1->Execute())
    LabelFont->Assign(FontDialog1->Font);
}
//---------------------------------------------------------------------------
void __fastcall TForm3::BitBtn3Click(TObject *Sender)
{
  FontDialog1->Font->Assign(NumberFont);
  if(FontDialog1->Execute())
    NumberFont->Assign(FontDialog1->Font);
}
//---------------------------------------------------------------------------
void TForm3::UpdateEnabledControls()
{
  Edit3->Enabled = !CheckBox4->Checked;
  Edit4->Enabled = !CheckBox5->Checked;
  Edit5->Enabled = CheckBox3->Checked;

  Edit9->Enabled = !CheckBox11->Checked;
  Edit10->Enabled = !CheckBox12->Checked;
  Edit11->Enabled = CheckBox10->Checked;

  Edit6->Enabled = RadioGroup1->ItemIndex == 1;
  Edit12->Enabled = RadioGroup1->ItemIndex == 1;

  CheckBox17->Enabled = CheckBox2->Checked;
  CheckBox18->Enabled = CheckBox9->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TForm3::CheckBoxClick(TObject *Sender)
{
  UpdateEnabledControls();
}
//---------------------------------------------------------------------------
void __fastcall TForm3::BitBtn4Click(TObject *Sender)
{
  FontDialog1->Font->Assign(LegendFont);
  if(FontDialog1->Execute())
    LegendFont->Assign(FontDialog1->Font);
}
//---------------------------------------------------------------------------
void __fastcall TForm3::FormShow(TObject *Sender)
{
  PageControl1->ActivePageIndex = 0;
  Edit1->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TForm3::RadioGroup1Click(TObject *Sender)
{
  UpdateEnabledControls();
}
//---------------------------------------------------------------------------
void __fastcall TForm3::CheckBox1Click(TObject *Sender)
{
  if(!Visible)
    return;   //Don't adjust while initializing form

  if(!Edit1->Modified)
    Edit1->Text = CheckBox1->Checked ? 0.1 : -10;
  if(!Edit2->Modified)
    Edit2->Text = CheckBox1->Checked ? 1000 : 10;
  if(!Edit3->Modified)
    Edit3->Text = CheckBox1->Checked ? 10 : 1;
  if(!Edit4->Modified)
    Edit4->Text = CheckBox1->Checked ? 10 : 1;
  if(!Edit12->Modified)
    Edit12->Text = CheckBox1->Checked ? 1 : 0;
}
//---------------------------------------------------------------------------
void __fastcall TForm3::CheckBox8Click(TObject *Sender)
{
  if(!Visible)
    return;   //Don't adjust while initializing form

  if(!Edit7->Modified)
    Edit7->Text = CheckBox8->Checked ? 0.1 : -10;
  if(!Edit8->Modified)
    Edit8->Text = CheckBox8->Checked ? 1000 : 10;
  if(!Edit9->Modified)
    Edit9->Text = CheckBox8->Checked ? 10 : 1;
  if(!Edit10->Modified)
    Edit10->Text = CheckBox8->Checked ? 10 : 1;
  if(!Edit6->Modified)
    Edit6->Text = CheckBox8->Checked ? 1 : 0;
}
//---------------------------------------------------------------------------
void __fastcall TForm3::CheckBox15Click(TObject *Sender)
{
  RadioGroup3->Enabled = CheckBox15->Checked;
}
//---------------------------------------------------------------------------


