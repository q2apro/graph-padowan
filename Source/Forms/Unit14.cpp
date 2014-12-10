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
#include <Clipbrd.hpp>
#include "Unit14.h"
#include "ConfigRegistry.h"
#include "ICompCommon.h"
#pragma warn -8072 //Disable warning: Suspicous pointer arithmetic
#include <boost/format.hpp>
//---------------------------------------------------------------------------
#pragma link "Grid"
#pragma link "PointSelect"
#pragma link "LineSelect"
#pragma link "ExtColorBox"
#pragma link "MyEdit"
#pragma link "SaveDialogEx"
#pragma link "ExtColorBox"
#pragma link "Grid"
#pragma link "LineSelect"
#pragma link "MyEdit"
#pragma link "PointSelect"
#pragma link "SaveDialogEx"
#pragma link "ExtComboBox"
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
const int ToGuiAlgorithm[4] = {0, 2, 3, 1};
const TInterpolationAlgorithm FromGuiAlgorithm[4] = {iaLinear, iaCubicSpline2, iaCubicSpline, iaHalfCosine};
//---------------------------------------------------------------------------
__fastcall TForm14::TForm14(TComponent* Owner, TData &AData)
    : TForm(Owner), Data(AData), FontChanged(false)
{
  Translate();
  ScaleForm(this);

  FontDialog1->Font->Assign(Property.DefaultPointLabelFont);
  Edit1->Text = Data.CreatePointSeriesDescription().c_str();

  PointSelect1->ItemIndex = Property.DefaultPoint.Style;
  ExtColorBox1->Selected = Property.DefaultPoint.Color;
  UpDown1->Position = Property.DefaultPoint.Size;

  LineSelect1->LineStyle = static_cast<TPenStyle>(Property.DefaultPointLine.Style);
  ExtColorBox2->Selected = Property.DefaultPointLine.Color;
  UpDown2->Position = Property.DefaultPointLine.Size;

  ComboBox1->ItemIndex = 1;
  UpdateErrorBars();
  PageControl1->ActivePage = TabSheet1;
  ComboBox2->ItemIndex = ToGuiAlgorithm[GetRegValue(REGISTRY_KEY "\\Property", L"Interpolation", HKEY_CURRENT_USER, iaLinear)];
  FlipForm(this);
}
//---------------------------------------------------------------------------
void TForm14::Translate()
{
  TranslateProperties(this);
  TranslateStrings(ExtColorBox1->Items);
  TranslateStrings(ExtColorBox2->Items);
  SetAccelerators(this);

  ResizeControl(Edit1, Label3);
  ResizeControl(ComboBox1, Label8);
  int LabelWidth = TMaxWidth(Label1)(Label2)(Label4)(Label5)(Label6)(Label7)(Label11);
  if(LabelWidth > 61)
    Width = Width + LabelWidth - 61;

  ComboBox2->ItemIndex = 0;
}
//---------------------------------------------------------------------------
void __fastcall TForm14::FormShow(TObject *Sender)
{
  //Grid needs do receive focus from something else
  Button1->SetFocus();
  Grid->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TForm14::Popup_CutClick(TObject *Sender)
{
  Grid->CutToClipboard(Property.DecimalSeparator);
}
//---------------------------------------------------------------------------
void __fastcall TForm14::Popup_CopyClick(TObject *Sender)
{
  Grid->CopyToClipboard(Property.DecimalSeparator);
}
//---------------------------------------------------------------------------
void __fastcall TForm14::Popup_PasteClick(TObject *Sender)
{
  Grid->PasteFromClipboard(Property.DecimalSeparator);
}
//---------------------------------------------------------------------------
void __fastcall TForm14::Popup_DeleteClick(TObject *Sender)
{
  Grid->ClearSelection();
}
//---------------------------------------------------------------------------
void __fastcall TForm14::Popup_SelectClick(TObject *Sender)
{
  Grid->SelectAll();
}
//---------------------------------------------------------------------------
void __fastcall TForm14::PopupMenu1Popup(TObject *Sender)
{
  Popup_Paste->Enabled = Clipboard()->HasFormat(CF_TEXT);
  Popup_Copy->Enabled = Grid->CanCopy();
  Popup_Cut->Enabled = Grid->CanCopy();
}
//---------------------------------------------------------------------------
void __fastcall TForm14::GridEditorKeyPress(TInplaceEdit *InplaceEdit,
      char &Key)
{
  if(Key == '\n')
    Button1->Click();
}
//---------------------------------------------------------------------------
void __fastcall TForm14::Button1Click(TObject *Sender)
{
  TErrorBarType xErrorBarType, yErrorBarType;
  double xErrorValue = 0, yErrorValue = 0;
  if(!CheckBox3->Checked)
    xErrorBarType = ebtNone;
  else if(RadioButton1->Checked)
  {
    xErrorValue = MakeFloat(Edit4, LoadRes(RES_POSITIVE, RadioButton1->Caption), 0);
    xErrorBarType = ebtFixed;
  }
  else if(RadioButton2->Checked)
  {
    xErrorValue = MakeFloat(Edit5, LoadRes(RES_POSITIVE, RadioButton2->Caption), 0);
    xErrorBarType = ebtRelative;
  }
  else
    xErrorBarType = ebtCustom;

  if(!CheckBox4->Checked)
    yErrorBarType = ebtNone;
  else if(RadioButton4->Checked)
  {
    yErrorValue = MakeFloat(Edit6, LoadRes(RES_POSITIVE, RadioButton4->Caption), 0);
    yErrorBarType = ebtFixed;
  }
  else if(RadioButton5->Checked)
  {
    yErrorValue = MakeFloat(Edit7, LoadRes(RES_POSITIVE, RadioButton5->Caption), 0);
    yErrorBarType = ebtRelative;
  }
  else
    yErrorBarType = ebtCustom;

  boost::shared_ptr<TPointSeries> PointSeries(new TPointSeries(
    clBlack,
    ExtColorBox1->Selected,
    ExtColorBox2->Selected,
    Edit2->Text.ToInt(),
    Edit3->Text.ToInt(),
    PointSelect1->ItemIndex,
    LineSelect1->LineStyle,
    FromGuiAlgorithm[ComboBox2->ItemIndex],
    CheckBox2->Checked,
    FontDialog1->Font,
    static_cast<Graph::TLabelPosition>(ComboBox1->ItemIndex),
    RadioGroup1->ItemIndex ? ptPolar : ptCartesian,
    xErrorBarType,
    xErrorValue,
    yErrorBarType,
    yErrorValue
  ));
  PointSeries->SetLegendText(ToWString(Edit1->Text));

  for(int Row = 1; Row < Grid->RowCount; Row++)
  {
    DataPoints[Row-1].First = Trim(DataPoints[Row-1].First);
    DataPoints[Row-1].Second = Trim(DataPoints[Row-1].Second);
    if(DataPoints[Row-1].First.empty() && DataPoints[Row-1].Second.empty())
      continue;

    if(DataPoints[Row-1].First.empty() || DataPoints[Row-1].Second.empty())
    {
      Grid->Col = DataPoints[Row-1].Second.empty();
      Grid->Row = Row;
      Grid->SetFocus();
      MessageBox(LoadRes(534), LoadRes(533));
      return;
    }

    //Just for validation
    CellToDouble(Grid, 0, Row);
    CellToDouble(Grid, 1, Row);

    PointSeries->InsertPoint(DataPoints[Row-1], -1, false);
  }

  if(PointSeries->PointCount() == 0)
  {
    MessageBox(LoadRes(536), LoadRes(533), MB_ICONWARNING);
    return;
  }

  if(Series)
  {
    PointSeries->SetVisible(Series->GetVisible());
    PointSeries->SetShowInLegend(Series->GetShowInLegend());
    int Index = Data.GetIndex(Series);
    UndoList.Push(TUndoChange(Series, PointSeries));
    Data.Replace(Series, PointSeries);
  }
  else
  {
    UndoList.Push(TUndoAdd(PointSeries));
    Data.Insert(PointSeries);
  }

  PointSeries->Update();
  Property.DefaultPoint.Set(PointSelect1->ItemIndex, ExtColorBox1->Selected, Edit2->Text.ToInt());
  Property.DefaultPointLine.Set(LineSelect1->LineStyle, ExtColorBox2->Selected, Edit3->Text.ToInt());
  if(FontChanged)
    Property.DefaultPointLabelFont->Assign(FontDialog1->Font);
  SetRegValue(REGISTRY_KEY "\\Property", L"Interpolation", HKEY_CURRENT_USER, FromGuiAlgorithm[ComboBox2->ItemIndex]);

  ModalResult = mrOk;
}
//---------------------------------------------------------------------------
int TForm14::EditPointSeries(const boost::shared_ptr<TPointSeries> &P)
{
  Series = P;
  if(P)
  {
    Caption = LoadRes(537);
    Edit1->Text = ToUString(P->GetLegendText());
    ExtColorBox1->Selected = P->GetFillColor();
    UpDown1->Position = P->GetSize();
    PointSelect1->ItemIndex = P->GetStyle();
    ExtColorBox2->Selected = P->GetLineColor();
    UpDown2->Position = P->GetLineSize();
    LineSelect1->LineStyle = P->GetLineStyle();
		ComboBox2->ItemIndex = ToGuiAlgorithm[P->GetInterpolation()];
    Grid->RowCount = P->PointCount() + 2;
    CheckBox2->Checked = P->GetShowLabels();
    ComboBox1->Enabled = CheckBox2->Checked;
    FontDialog1->Font->Assign(P->GetFont());
    ComboBox1->ItemIndex = P->GetLabelPosition();
    RadioGroup1->ItemIndex = P->GetPointType();

    DataPoints = P->GetPointData(); //Create a working copy of all data

    CheckBox3->Checked = P->GetxErrorBarType() != ebtNone;
    switch(P->GetxErrorBarType())
    {
      case ebtFixed:
        RadioButton1->Checked = true;
        Edit4->Text = P->GetxErrorValue();
        break;

      case ebtRelative:
        RadioButton2->Checked = true;
        Edit5->Text = P->GetxErrorValue();
        break;

      case ebtCustom:
        RadioButton3->Checked = true;
        break;
    }

    CheckBox4->Checked = P->GetyErrorBarType() != ebtNone;
    switch(P->GetyErrorBarType())
    {
      case ebtFixed:
        RadioButton4->Checked = true;
        Edit6->Text = P->GetyErrorValue();
        break;

      case ebtRelative:
        RadioButton5->Checked = true;
        Edit7->Text = P->GetyErrorValue();
        break;

      case ebtCustom:
        RadioButton6->Checked = true;
        break;
    }
  }
  return ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TForm14::PaintBox1Paint(TObject *Sender)
{
  try
  {
    int X = PaintBox1->Width / 2;
    int Y = PaintBox1->Height / 2;
    int PointSize = Edit2->Text.ToInt();

    PaintBox1->Canvas->Pen->Width = 1;
    PaintBox1->Canvas->Pen->Color = clBlack;
    if(CheckBox3->Checked && PointSize)
    {
      PaintBox1->Canvas->MoveTo(X - 10, Y);
      PaintBox1->Canvas->LineTo(X + 10, Y);
      PaintBox1->Canvas->MoveTo(X - 10, Y - 4);
      PaintBox1->Canvas->LineTo(X - 10, Y + 5);
      PaintBox1->Canvas->MoveTo(X + 10, Y - 4);
      PaintBox1->Canvas->LineTo(X + 10, Y + 5);
    }

    if(CheckBox4->Checked && PointSize)
    {
      PaintBox1->Canvas->MoveTo(X, Y - 10);
      PaintBox1->Canvas->LineTo(X, Y + 10);
      PaintBox1->Canvas->MoveTo(X - 4, Y - 10);
      PaintBox1->Canvas->LineTo(X + 5, Y - 10);
      PaintBox1->Canvas->MoveTo(X - 4, Y + 10);
      PaintBox1->Canvas->LineTo(X + 5, Y + 10);
    }

    PaintBox1->Canvas->Pen->Handle = SetPen(ExtColorBox2->Selected, LineSelect1->LineStyle, Edit3->Text.ToInt());
    PaintBox1->Canvas->MoveTo(0, Y);
    PaintBox1->Canvas->LineTo(PaintBox1->Width, Y);

    TPointSelect::DrawPoint(PaintBox1->Canvas, TPoint(X, Y), PointSelect1->ItemIndex, clBlack, ExtColorBox1->Selected, PointSize);

    if(CheckBox2->Checked)
    {
      using boost::wformat;
	  std::wstring Str;
      if(RadioGroup1->ItemIndex == 0)
		Str = str(wformat(GuiFormatSettings.CartesianPointFormat) % L"2.37" % L"9.53");
	  else if(Data.Axes.Trigonometry == Func32::Radian)
		Str = str(wformat(GuiFormatSettings.RadianPointFormat) % L"1.18" % L"12.5");
	  else
		Str = str(wformat(GuiFormatSettings.DegreePointFormat) % L"87.3" % L"12.5");
      PaintBox1->Canvas->Font->Assign(FontDialog1->Font);
      TDraw::DrawPointLabel(PaintBox1->Canvas, TPoint(X, Y), PointSize, Str, static_cast<Graph::TLabelPosition>(ComboBox1->ItemIndex));
    }
  }
  catch(...)
  {
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm14::Change(TObject *Sender)
{
  PaintBox1->Invalidate();
  PointSelect1->FillColor = ExtColorBox1->Selected;
  ComboBox1->Enabled = CheckBox2->Checked;
}
//---------------------------------------------------------------------------
HPEN TForm14::SetPen(TColor Color, TPenStyle Style, int Width)
{
  if(Win32Platform == VER_PLATFORM_WIN32_NT && Width > 1)
  {
    unsigned long DashStyle[] = {16, 8};
    LOGBRUSH LogBrush;
    LogBrush.lbStyle = BS_SOLID;
    LogBrush.lbColor = Color;
    if(Style == PS_DASH)
      return ExtCreatePen(PS_GEOMETRIC | PS_USERSTYLE, Width, &LogBrush, sizeof(DashStyle)/sizeof(*DashStyle), DashStyle);
    return ExtCreatePen(PS_GEOMETRIC | Style, Width, &LogBrush, 0, NULL);
  }
  return CreatePen(Style, Width, Color);
}
//---------------------------------------------------------------------------
void __fastcall TForm14::Button3Click(TObject *Sender)
{
  Application->HelpContext(HelpContext);
}
//---------------------------------------------------------------------------
void __fastcall TForm14::Popup_InsertClick(TObject *Sender)
{
  Grid->InsertRows(Grid->Row, 1);
}
//---------------------------------------------------------------------------
void __fastcall TForm14::Popup_RemoveClick(TObject *Sender)
{
  Grid->RemoveRows(Grid->Selection.Top, Grid->Selection.Bottom - Grid->Selection.Top + 1);
}
//---------------------------------------------------------------------------
void __fastcall TForm14::Popup_ImportClick(TObject *Sender)
{
	const char Separators[] = {';',',','\t',' ',0};
  OpenDialog1->Filter = LoadRes(RES_DATA_FILTER);
  if(OpenDialog1->Execute())
  {
    Grid->ImportFromFile(OpenDialog1->FileName, Property.DecimalSeparator, Separators[OpenDialog1->FilterIndex-1]);
    int Row = Grid->Selection.Top;
    if(Grid->Cells[0][Row] == L"X" && Grid->Cells[1][Row] == L"Y")
      Grid->RemoveRows(Row, 1);
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm14::BitBtn1Click(TObject *Sender)
{
  if(FontDialog1->Execute())
  {
    PaintBox1->Invalidate();
    FontChanged = true;
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm14::Popup_ExportClick(TObject *Sender)
{
  SaveDialog1->Filter = LoadRes(RES_EXPORT_DATA_FILTER);
  if(SaveDialog1->Execute())
    if(!Grid->ExportToFile(SaveDialog1->FileName, SaveDialog1->FilterIndex == 1 ? ';' : '\t', Property.DecimalSeparator))
      MessageBox(LoadRes(RES_FILE_ACCESS, SaveDialog1->FileName), LoadRes(RES_WRITE_FAILED), MB_ICONSTOP);
}
//---------------------------------------------------------------------------
void __fastcall TForm14::EditKeyPress(TObject *Sender, char &Key)
{
  if(!isdigit(Key) && Key!='\b')
    Key = 0;
}
//---------------------------------------------------------------------------
void __fastcall TForm14::RadioButtonClick(TObject *Sender)
{
  UpdateErrorBars();
  PaintBox1->Invalidate();
}
//---------------------------------------------------------------------------
void TForm14::UpdateErrorBars()
{
  unsigned Cols = 2;
  Cols += CheckBox3->Checked && RadioButton3->Checked;
  Cols += CheckBox4->Checked && RadioButton6->Checked;
  Width = Width + (Cols - Grid->ColCount) * Grid->DefaultColWidth;
  Grid->ColCount = Cols;

  RadioButton1->Enabled = CheckBox3->Checked;
  RadioButton2->Enabled = CheckBox3->Checked;
  RadioButton3->Enabled = CheckBox3->Checked;
  Edit4->Enabled = CheckBox3->Checked;
  Edit5->Enabled = CheckBox3->Checked;

  RadioButton4->Enabled = CheckBox4->Checked;
  RadioButton5->Enabled = CheckBox4->Checked;
  RadioButton6->Enabled = CheckBox4->Checked;
  Edit6->Enabled = CheckBox4->Checked;
  Edit7->Enabled = CheckBox4->Checked;
}
//---------------------------------------------------------------------------
std::wstring& TForm14::GetText(int ACol, int ARow)
{
  DataPoints.resize(std::max(Grid->RowCount - 1, ARow));
  switch(ACol)
  {
    case 0: return DataPoints[ARow-1].First;
    case 1: return DataPoints[ARow-1].Second;
    case 2: return CheckBox3->Enabled && RadioButton3->Checked ? DataPoints[ARow-1].xError.Text : DataPoints[ARow-1].yError.Text;
    case 3: return DataPoints[ARow-1].yError.Text;
  }
  throw Exception("Invalid Coloumn");
}
//---------------------------------------------------------------------------
void __fastcall TForm14::EditChange(TObject *Sender)
{
  if(Sender == Edit4)
    RadioButton1->Checked = true;
  else if(Sender == Edit5)
    RadioButton2->Checked = true;
  else if(Sender == Edit6)
    RadioButton4->Checked = true;
  else if(Sender == Edit7)
    RadioButton5->Checked = true;
}
//---------------------------------------------------------------------------
void __fastcall TForm14::GridGetText(TObject *Sender, long ACol, long ARow,
      String &Value)
{
  if(ARow == 0)
    switch(ACol)
    {
      case 0: Value = RadioGroup1->ItemIndex ? L"\x3B8" : L"X"; break;
      case 1: Value = RadioGroup1->ItemIndex ? L"r" : L"Y"; break;
      case 2: Value = LoadRes(RES_UNCERTAINTY, (CheckBox3->Checked && RadioButton3->Checked) ? "X" : "Y"); break;
      case 3: Value = LoadRes(RES_UNCERTAINTY, "Y"); break;
    }
  else
    Value = GetText(ACol, ARow).c_str();
}
//---------------------------------------------------------------------------
void __fastcall TForm14::GridSetText(TObject *Sender, long ACol, long ARow,
      const String &Value)
{
  if(ARow > 0)
    GetText(ACol, ARow) = ToWString(Value);
}
//---------------------------------------------------------------------------
void __fastcall TForm14::RadioGroup1Click(TObject *Sender)
{
  Grid->Invalidate();
  PaintBox1->Invalidate();
}
//---------------------------------------------------------------------------

