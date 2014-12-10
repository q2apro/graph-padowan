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
#include "Unit13.h"
#include "Unit8.h"
//---------------------------------------------------------------------------
#pragma link "LineSelect"
#pragma link "MyRadioButton"
#pragma link "MyEdit"
#pragma link "ExtColorBox"
#pragma link "CheckBoxEx"
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TForm13::TForm13(TComponent* Owner, TData &AData)
        : TForm(Owner), Data(AData)
{
  int OldLabelWidth = TMaxWidth(Label5)(Label6)(Label7);
  TranslateProperties(this);
  TranslateStrings(ExtColorBox1->Items);
  ScaleForm(this); //Moving this may cause problem for the Arabic translation
  int LabelWidth = TMaxWidth(Label5)(Label6)(Label7);
  SetAccelerators(this);
  if(LabelWidth != OldLabelWidth)
    Width = Width + LabelWidth - OldLabelWidth;
  ResizeControl(Edit5, CheckBox1->Left + CheckBox1->Width);
  PageControl1->ActivePage = TabSheet1;
  FlipForm(this);
}
//---------------------------------------------------------------------------
void __fastcall TForm13::ImageClick(TObject *Sender)
{
  if(Sender == Image1)
  {
    if(RadioButton1->CanFocus())
      RadioButton1->SetFocus();
  }
  else if(Sender == Image2)
  {
    if(RadioButton2->CanFocus())
      RadioButton2->SetFocus();
  }
  else if(Sender == Image3)
  {
    if(RadioButton3->CanFocus())
      RadioButton3->SetFocus();
  }
  else if(Sender == Image4)
  {
    if(RadioButton4->CanFocus())
      RadioButton4->SetFocus();
  }
  else if(Sender == Image5)
	{
    if(RadioButton5->CanFocus())
      RadioButton5->SetFocus();
  }
  else if(Sender == Image6)
  {
    if(RadioButton6->CanFocus())
      RadioButton6->SetFocus();
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm13::EditKeyPress(TObject *Sender, char &Key)
{
  if(!isdigit(Key) && Key != '\b')
    Key = 0;
}
//---------------------------------------------------------------------------
void __fastcall TForm13::Button1Click(TObject *Sender)
{
  if(ToIntDef(Edit1->Text, -1) < 0)
  {
    Edit1->SetFocus();
    MessageBox(LoadRes(RES_LIMIT_ZERO, StaticText1->Caption), LoadRes(RES_ERROR_IN_VALUE));
    return;
  }

  if(ToIntDef(Edit2->Text, -1) < 1)
  {
    Edit2->SetFocus();
    MessageBox(LoadRes(RES_INT_GREATER_ZERO, Label5->Caption), LoadRes(RES_ERROR_IN_VALUE));
    return;
  }

  try
  {
		boost::shared_ptr<TBaseFuncType> BaseFunc;

    const std::vector<Func32::TDblPoint> &Points = Series->GetPointList();
    std::vector<double> Weights;
    if(Series->GetyErrorBarType() == ebtCustom)
    {
      Weights.reserve(Points.size());
      unsigned Count = Series->PointCount();
      for(unsigned I = 0; I < Count; I++)
      {
        double yError = Series->GetYError(I);
        if(yError == 0)
          throw Exception(LoadRes(RES_NOT_EQUAL_ZERO, LoadRes(RES_UNCERTAINTY, "Y")));
        Weights.push_back(1/(yError*yError));
      }
		}

    if(PageControl1->TabIndex == 1)
    {
      std::vector<std::wstring> Arguments = Func32::FindUnknowns(ToWString(Edit3->Text));
      std::vector<long double> Values(Arguments.size(), 1);

      if(ListBox1->ItemIndex != -1)
      {
        std::wstring ModelName = ToWString(ListBox1->Items->Strings[ListBox1->ItemIndex]);
        TUserModel &Model = Data.UserModels[ModelName];
        for(unsigned I = 0; I < Model.Defaults.size(); I++)
          Values[I] = ToDouble(Model.Defaults[I].second);
      }

      Arguments.insert(Arguments.begin(), L"x");
      Func32::TCustomFunc TempFunc(ToWString(Edit3->Text), Arguments, Data.CustomFunctions.SymbolList, Data.Axes.Trigonometry);
      Regression(Points, TempFunc, Values, Weights, 300);

      boost::shared_ptr<TStdFunc> Func(new TStdFunc(TempFunc.ConvToFunc(Values, 0)));
			Func->SetLegendText(L"f(x)=" + Func->GetFunc().MakeText(L"x", Property.RoundTo) + L"; R²=" + RoundToString(Correlation(Points, Func->GetFunc()), Data));
      Func->From.Value = -INF;
      Func->To.Value = +INF;
      Func->SetSteps(TTextValue(0, L""));
      BaseFunc = Func;
    }
    else if(RadioButton6->Checked) //Moving average
    {
      if(Edit4->Text.ToInt() == 0)
      {
        MessageBox(LoadRes(RES_INT_GREATER_ZERO, StaticText2->Caption), LoadRes(RES_ERROR_IN_VALUE));
        return;
      }

      unsigned N = Edit4->Text.ToInt();

      //Workaround for compiler bug in bcc 5.6.4. The following two lines may not be put together.
      boost::shared_ptr<TParFunc> Func;
      Func.reset(new TParFunc(Func32::MovingAverage(Points, N)));

      Func->From.Value = 0;
      Func->From.Text = L"0";
      Func->To.Value = Points.size() - N;
      Func->To.Text = ToWString(Func->To.Value);
      Func->SetSteps(TTextValue(1000));
      Func->SetLegendText(LoadString(RES_MOVING_AVERAGE));
      BaseFunc = Func;
    }
    else
    {
      Func32::TTrendType Type;
      if(RadioButton1->Checked)
        Type = Func32::ttLinear;
      else if(RadioButton2->Checked)
        Type = Func32::ttLogarithmic;
      else if(RadioButton3->Checked)
        Type = Func32::ttPolynomial;
      else if(RadioButton4->Checked)
        Type = Func32::ttPower;
      else if(RadioButton5->Checked)
        Type = Func32::ttExponential;
      else
        throw Exception("No radio button selected!");

      unsigned N = Edit1->Text.ToInt();
      //WARNING: Do not initialize Func with pointer. It will crash if Trendline() fails because of bug in Bcc 5.6.4
      boost::shared_ptr<TStdFunc> Func;
      if(CheckBox1->Checked)
        Func.reset(new TStdFunc(TrendLine(Type, Points, Weights, N, MakeFloat(Edit5))));
      else
        Func.reset(new TStdFunc(TrendLine(Type, Points, Weights, N)));
			Func->SetLegendText(L"f(x)=" + Func->GetFunc().MakeText(L"x", Property.RoundTo) + L"; R²=" + RoundToString(Correlation(Points, Func->GetFunc()), Data));
      Func->From.Value = -INF;
      Func->To.Value = +INF;
      Func->SetSteps(TTextValue(0, L""));
      BaseFunc = Func;
    }

    BaseFunc->Color = ExtColorBox1->Selected;
    BaseFunc->Size = Edit2->Text.ToInt();
    BaseFunc->Style = LineSelect1->LineStyle;
    Data.Insert(BaseFunc);
    UndoList.Push(TUndoAdd(BaseFunc));
  }
  catch(Func32::EFuncError &Error)
  {
    if(PageControl1->TabIndex == 1)
      ShowErrorMsg(Error, Edit3);
    else
      ShowErrorMsg(Error);
    return;
  }

  Property.DefaultTrendline.Set(LineSelect1->LineStyle, ExtColorBox1->Selected, Edit2->Text.ToInt());
  ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TForm13::Button3Click(TObject *Sender)
{
  Application->HelpContext(HelpContext);
}
//---------------------------------------------------------------------------
void TForm13::ShowUserModels(const std::wstring &Selected)
{
  ListBox1->Clear();
  for(TUserModels::const_iterator Iter = Data.UserModels.begin(); Iter != Data.UserModels.end(); ++Iter)
    ListBox1->AddItem(Iter->first.c_str(), NULL);
  ListBox1->ItemIndex = ListBox1->Items->IndexOf(Selected.c_str());
}
//---------------------------------------------------------------------------
void __fastcall TForm13::ListBox1Click(TObject *Sender)
{
  if(ListBox1->ItemIndex != -1)
  {
    Edit3->Text = Data.UserModels[ToWString(ListBox1->Items->Strings[ListBox1->ItemIndex])].Model.c_str();
    Button5->Enabled = true;
    Button6->Enabled = true;
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm13::Button4Click(TObject *Sender)
{
  try
  {
    std::wstring ModelName;
    if(CreateForm<TForm8>(Data)->AddModel(::ToWString(Edit3->Text), ModelName))
    {
      ShowUserModels(ModelName);
      Button5->Enabled = true;
      Button6->Enabled = true;
    }
  }
  catch(Func32::EFuncError &Error)
  {
    Edit3->SetFocus();
    ShowErrorMsg(Error, Edit3);
    return;
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm13::Button6Click(TObject *Sender)
{
  if(ListBox1->ItemIndex == -1)
    return;

  std::wstring ModelName = ToWString(ListBox1->Items->Strings[ListBox1->ItemIndex]);
  if(CreateForm<TForm8>(Data)->EditModel(ModelName))
    ShowUserModels(ModelName);
}
//---------------------------------------------------------------------------
void __fastcall TForm13::Button5Click(TObject *Sender)
{
  if(ListBox1->ItemIndex == -1)
    return;

  if(MessageBox(LoadRes(RES_DELETE_MODEL, ListBox1->Items->Strings[ListBox1->ItemIndex]), LoadRes(RES_CAPTION_DELETE), MB_ICONQUESTION | MB_YESNO) == IDYES)
  {
    Data.UserModels.erase(ToWString(ListBox1->Items->Strings[ListBox1->ItemIndex]));
    ListBox1->Items->Delete(ListBox1->ItemIndex);
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm13::Edit3Change(TObject *Sender)
{
  if(Edit3->Focused())
  {
    ListBox1->ItemIndex = -1;
    Button5->Enabled = false;
    Button6->Enabled = false;
  }
}
//---------------------------------------------------------------------------
int TForm13::InsertTrendline(const boost::shared_ptr<TPointSeries> &ASeries)
{
  Series = ASeries;

  bool NegXFound = false, NegYFound = false, ZeroXFound = false, ZeroYFound = false;

  unsigned Count = Series->PointCount();
  const TPointSeries::TPointList &Points = Series->GetPointList();
  for(unsigned I = 0; I < Count; I++)
  {
    const Func32::TDblPoint &P = Points[I];
    if(P.x < 0)
      NegXFound = true;
    else if(P.x == 0)
      ZeroXFound = true;

    if(P.y < 0)
      NegYFound = true;
    else if(P.y == 0)
      ZeroYFound = true;
  }

  if(NegXFound || ZeroXFound)
    RadioButton2->Enabled = false;

  if(NegYFound || ZeroYFound)
    RadioButton5->Enabled = false;

  if(NegXFound || NegYFound)
    RadioButton4->Enabled = false;

  LineSelect1->LineStyle = static_cast<TPenStyle>(Property.DefaultTrendline.Style);
  ExtColorBox1->Selected = Property.DefaultTrendline.Color;
  UpDown1->Position = Property.DefaultTrendline.Size;

  UpDown2->Max = Count - 1;
  UpDown3->Max = Count - 1;

  ShowUserModels(L"");
  return ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TForm13::RadioButtonClick(TObject *Sender)
{
  Edit1->Enabled = RadioButton3->Checked;
  UpDown2->Enabled = RadioButton3->Checked;

  Edit4->Enabled = RadioButton6->Checked;
  UpDown3->Enabled = RadioButton6->Checked;

  if((RadioButton1->Checked || RadioButton3->Checked || RadioButton5->Checked) && PageControl1->ActivePage == TabSheet1)
  {
    CheckBox1->Enabled = true;
    Edit5->Enabled = true;
    Edit5->Color = clWindow;
  }
  else
  {
    CheckBox1->Enabled = false;
    Edit5->Enabled = false;
    Edit5->Color = clBtnFace;
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm13::Edit5Change(TObject *Sender)
{
  CheckBox1->Checked = true;
}
//---------------------------------------------------------------------------
void __fastcall TForm13::CheckBox1Click(TObject *Sender)
{
  //This check is needed because this function may be called as part of the form initialization
  if(Series)
  {
    if(CheckBox1->Checked)
      UpDown2->Max = Series->PointCount();
    else
      UpDown2->Max = Series->PointCount() - 1;
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm13::Popup_ImportClick(TObject *Sender)
{
  if(OpenDialog1->Execute())
  {
    std::auto_ptr<TStrings> Strings(new TStringList);
    Strings->LoadFromFile(OpenDialog1->FileName);
    Data.ImportUserModels(Strings->Text.c_str());
    ShowUserModels(L"");
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm13::Popup_ExportClick(TObject *Sender)
{
  if(SaveDialog1->Execute())
  {
    std::auto_ptr<TStrings> Strings(new TStringList);
    Strings->Text = Data.ExportUserModels().c_str();
    Strings->SaveToFile(SaveDialog1->FileName);
  }
}
//---------------------------------------------------------------------------



