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
#include "Unit17.h"
#include "PyGraph.h"
//---------------------------------------------------------------------------
#pragma link "Grid"
#pragma link "SaveDialogEx"
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TForm17::TForm17(TComponent* Owner, TData &AData)
  : TForm(Owner), Data(AData)
{
  TranslateProperties(this);
  SetAccelerators(this);
  ScaleForm(this);
  FlipForm(this);
  Grid1->ColWidths[1] = Grid1->Width - Grid1->ColWidths[0] - GetSystemMetrics(SM_CXHTHUMB) - 6;

  unsigned I = 1;
  Grid1->RowCount = Data.CustomFunctions.End() - Data.CustomFunctions.Begin() + 2;
  for(TCustomFunctions::TConstIterator Iter = Data.CustomFunctions.Begin(); Iter != Data.CustomFunctions.End(); ++Iter, I++)
  {
    Grid1->Cells[0][I] = Iter->GetName().c_str();
    Grid1->Cells[1][I] = Iter->Text.c_str();
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm17::Button4Click(TObject *Sender)
{
  Application->HelpContext(HelpContext);
}
//---------------------------------------------------------------------------
void __fastcall TForm17::Button1Click(TObject *Sender)
{
  Button3->Click();
  ModalResult = mrOk;
}
//---------------------------------------------------------------------------
int TForm17::FindGridRow(int Index)
{
	int Index2 = -1;
	for(int I = 1; I < Grid1->RowCount; I++)
		if(!Grid1->Cells[0][I].IsEmpty() || !Grid1->Cells[1][I].IsEmpty())
			if(++Index2 == Index)
				return I;
	return Grid1->RowCount-1;
}
//---------------------------------------------------------------------------
void __fastcall TForm17::Button3Click(TObject *Sender)
{
	int Index = 1;
	try
	{
    Data.AbortUpdate();

		TCustomFunctions CustomFunctions;
		for(Index = 1; Index < Grid1->RowCount; Index++)
			if(!Grid1->Cells[0][Index].IsEmpty() || !Grid1->Cells[1][Index].IsEmpty())
				CustomFunctions.Add(ToWString(Grid1->Cells[0][Index]), ToWString(Grid1->Cells[1][Index]));

    CustomFunctions.UpdateAll();
    UndoList.Push(TUndoCustomFunctions(Data));
    Data.CustomFunctions.Swap(CustomFunctions);
    Data.Update();
  }
  catch(Func32::EFuncError &E)
	{
		if(Index < Grid1->RowCount)
		{
			Grid1->Row = Index;
			Grid1->Col = 1;
			Grid1->SetFocus();
		}
		ShowErrorMsg(E);
		throw EAbort("");
	}
	catch(ECustomFunctionError &E)
	{
		Grid1->SetFocus();
		if(E.ErrorCode == cfeParseError)
		{
			Grid1->Row = FindGridRow(E.ErrorPos);
      Grid1->Col = 1;
      Grid1->SetCursorPos(E.ParseError.ErrorPos);
      ShowErrorMsg(E.ParseError);
    }
    else
    {
      Grid1->Row = Index;
      Grid1->Col = 0;
      Grid1->SetCursorPos(E.ErrorPos);
      ShowErrorMsg(E);
    }
    throw EAbort("");
  }

  Data.SetModified();
  Data.ClearCache();
  Python::ExecutePluginEvent(Python::peCustomFunctionsChanged);
  Form1->Redraw();
}
//---------------------------------------------------------------------------
void __fastcall TForm17::Popup_InsertClick(TObject *Sender)
{
  Grid1->InsertRows(Grid1->Row, 1);
}
//---------------------------------------------------------------------------
void __fastcall TForm17::Popup_RemoveClick(TObject *Sender)
{
  Grid1->RemoveRows(Grid1->Selection.Top, Grid1->Selection.Bottom - Grid1->Selection.Top + 1);
}
//---------------------------------------------------------------------------
void __fastcall TForm17::Grid1EditorKeyPress(TInplaceEdit *InplaceEdit, char &Key)
{
  if(Key == '\n')
    Button1->Click();
}
//---------------------------------------------------------------------------
void __fastcall TForm17::Popup_CutClick(TObject *Sender)
{
  Grid1->CutToClipboard(0);
}
//---------------------------------------------------------------------------
void __fastcall TForm17::Popup_CopyClick(TObject *Sender)
{
  Grid1->CopyToClipboard(0);
}
//---------------------------------------------------------------------------
void __fastcall TForm17::Popup_PasteClick(TObject *Sender)
{
  Grid1->PasteFromClipboard(0);
}
//---------------------------------------------------------------------------
void __fastcall TForm17::Popup_DeleteClick(TObject *Sender)
{
  Grid1->ClearSelection();
}
//---------------------------------------------------------------------------
void __fastcall TForm17::Popup_SelectClick(TObject *Sender)
{
  Grid1->SelectAll();
}
//---------------------------------------------------------------------------
void __fastcall TForm17::Popup_ImportClick(TObject *Sender)
{
	OpenDialog1->Filter = LoadRes(RES_EXPORT_DATA_FILTER);
	if(OpenDialog1->Execute())
		Grid1->ImportFromFile(OpenDialog1->FileName, 0);
}
//---------------------------------------------------------------------------
void __fastcall TForm17::Popup_ExportClick(TObject *Sender)
{
	SaveDialog1->Filter = LoadRes(RES_EXPORT_DATA_FILTER);
	if(SaveDialog1->Execute())
		if(!Grid1->ExportToFile(SaveDialog1->FileName, SaveDialog1->FilterIndex == 1 ? ';' : '\t', Property.DecimalSeparator))
			MessageBox(LoadRes(RES_FILE_ACCESS, SaveDialog1->FileName), LoadStr(RES_WRITE_FAILED), MB_ICONSTOP);
}
//---------------------------------------------------------------------------

