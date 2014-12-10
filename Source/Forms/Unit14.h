/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2007 Ivan Johansen
 *
 * Graph is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
#ifndef Unit14H
#define Unit14H
#include "ExtColorBox.h"
#include "Grid.h"
#include "LineSelect.h"
#include "MyEdit.h"
#include "PointSelect.h"
#include "SaveDialogEx.h"
#include <ActnPopup.hpp>
#include <Buttons.hpp>
#include <Classes.hpp>
#include <ComCtrls.hpp>
#include <Controls.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include <Grids.hpp>
#include <Menus.hpp>
#include <PlatformDefaultStyleActnCtrls.hpp>
#include <StdCtrls.hpp>
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
#include <ComCtrls.hpp>
#include <Menus.hpp>
#include <ExtCtrls.hpp>
#include "Grid.h"
#include "PointSelect.h"
#include "LineSelect.h"
#include <Dialogs.hpp>
#include <Buttons.hpp>
#include "ExtColorBox.h"
#include "MyEdit.h"
#include "SaveDialogEx.h"
#include <ActnPopup.hpp>
#include <PlatformDefaultStyleActnCtrls.hpp>
#include "ExtComboBox.h"
//---------------------------------------------------------------------------
class TForm14 : public TForm
{
__published:	// IDE-managed Components
  TButton *Button1;
  TGrid *Grid;
  TPopupActionBar *PopupMenu1;
  TMenuItem *Popup_Cut;
  TMenuItem *Popup_Copy;
  TMenuItem *Popup_Paste;
  TMenuItem *Popup_Delete;
  TMenuItem *N1;
  TMenuItem *Popup_Select;
  TButton *Button2;
  TMyEdit *Edit1;
  TLabel *Label3;
  TGroupBox *GroupBox4;
  TPaintBox *PaintBox1;
  TButton *Button3;
  TMenuItem *Popup_Insert;
  TMenuItem *N2;
  TMenuItem *Popup_Remove;
  TMenuItem *Popup_Import;
  TOpenDialog *OpenDialog1;
  TFontDialog *FontDialog1;
  TMenuItem *Popup_Export;
  TSaveDialogEx *SaveDialog1;
  TPageControl *PageControl1;
  TTabSheet *TabSheet1;
  TTabSheet *TabSheet2;
  TGroupBox *GroupBox1;
  TLabel *Label1;
  TLabel *Label2;
  TLabel *Label4;
  TEdit *Edit2;
  TUpDown *UpDown1;
  TPointSelect *PointSelect1;
  TGroupBox *GroupBox2;
  TLabel *Label5;
  TLabel *Label6;
  TLabel *Label7;
  TLineSelect *LineSelect1;
  TEdit *Edit3;
  TUpDown *UpDown2;
  TGroupBox *GroupBox3;
  TLabel *Label8;
  TCheckBox *CheckBox2;
  TBitBtn *BitBtn1;
  TComboBox *ComboBox1;
  TGroupBox *GroupBox5;
  TCheckBox *CheckBox3;
  TRadioButton *RadioButton1;
  TMyEdit *Edit4;
  TRadioButton *RadioButton2;
  TRadioButton *RadioButton3;
  TMyEdit *Edit5;
  TLabel *Label9;
  TGroupBox *GroupBox6;
  TLabel *Label10;
  TCheckBox *CheckBox4;
  TRadioButton *RadioButton4;
  TMyEdit *Edit6;
  TRadioButton *RadioButton5;
  TRadioButton *RadioButton6;
  TMyEdit *Edit7;
  TExtComboBox *ComboBox2;
  TLabel *Label11;
  TExtColorBox *ExtColorBox1;
  TExtColorBox *ExtColorBox2;
  TRadioGroup *RadioGroup1;
  void __fastcall FormShow(TObject *Sender);
  void __fastcall Popup_CutClick(TObject *Sender);
  void __fastcall Popup_CopyClick(TObject *Sender);
  void __fastcall Popup_PasteClick(TObject *Sender);
  void __fastcall Popup_DeleteClick(TObject *Sender);
  void __fastcall Popup_SelectClick(TObject *Sender);
  void __fastcall PopupMenu1Popup(TObject *Sender);
  void __fastcall Button1Click(TObject *Sender);
  void __fastcall GridEditorKeyPress(TInplaceEdit *InplaceEdit,
    char &Key);
  void __fastcall PaintBox1Paint(TObject *Sender);
  void __fastcall Change(TObject *Sender);
  void __fastcall Button3Click(TObject *Sender);
  void __fastcall Popup_InsertClick(TObject *Sender);
  void __fastcall Popup_RemoveClick(TObject *Sender);
  void __fastcall Popup_ImportClick(TObject *Sender);
  void __fastcall BitBtn1Click(TObject *Sender);
  void __fastcall Popup_ExportClick(TObject *Sender);
  void __fastcall EditKeyPress(TObject *Sender, char &Key);
  void __fastcall RadioButtonClick(TObject *Sender);
  void __fastcall EditChange(TObject *Sender);
  void __fastcall GridGetText(TObject *Sender, long ACol, long ARow,
          String &Value);
  void __fastcall GridSetText(TObject *Sender, long ACol, long ARow,
          const String &Value);
  void __fastcall RadioGroup1Click(TObject *Sender);

private:	// User declarations
  boost::shared_ptr<TPointSeries> Series;
  std::vector<TPointSeriesPoint> DataPoints;
  TData &Data;
  bool FontChanged;

  static HPEN SetPen(TColor Color, TPenStyle Style, int Width);
  void UpdateErrorBars();
  std::wstring& GetText(int ACol, int ARow);
  void Translate();

public:		// User declarations
  __fastcall TForm14(TComponent* Owner, TData &AData);
  int EditPointSeries(const boost::shared_ptr<TPointSeries> &P);
};
//---------------------------------------------------------------------------
#endif
