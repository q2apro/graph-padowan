/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2007 Ivan Johansen
 *
 * Graph is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
#ifndef Unit13H
#define Unit13H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <Graphics.hpp>
#include "LineSelect.h"
#include "MyRadioButton.h"
#include "MyEdit.h"
#include <Grids.hpp>
#include <ValEdit.hpp>
#include "ExtColorBox.h"
#include <Dialogs.hpp>
#include <Menus.hpp>
#include "CheckBoxEx.h"
//---------------------------------------------------------------------------
class TForm13 : public TForm
{
__published:	// IDE-managed Components
  TButton *Button1;
  TButton *Button2;
  TButton *Button3;
  TGroupBox *GroupBox1;
  TLabel *Label5;
  TLabel *Label6;
  TLabel *Label7;
  TEdit *Edit2;
  TLineSelect *LineSelect1;
  TUpDown *UpDown1;
  TPageControl *PageControl1;
  TTabSheet *TabSheet1;
  TTabSheet *TabSheet2;
  TPanel *Panel1;
  TImage *Image1;
  TPanel *Panel2;
  TImage *Image2;
  TPanel *Panel3;
  TImage *Image3;
  TPanel *Panel4;
  TImage *Image4;
  TPanel *Panel5;
  TImage *Image5;
  TEdit *Edit1;
  TStaticText *StaticText1;
  TUpDown *UpDown2;
  TMyRadioButton *RadioButton3;
  TMyRadioButton *RadioButton2;
  TMyRadioButton *RadioButton1;
  TMyRadioButton *RadioButton4;
  TMyRadioButton *RadioButton5;
  TListBox *ListBox1;
  TButton *Button4;
  TButton *Button5;
  TLabel *Label1;
  TButton *Button6;
  TMyEdit *Edit3;
  TPanel *Panel6;
  TImage *Image6;
  TMyRadioButton *RadioButton6;
  TStaticText *StaticText2;
  TEdit *Edit4;
  TUpDown *UpDown3;
  TExtColorBox *ExtColorBox1;
  TMyEdit *Edit5;
  TCheckBoxEx *CheckBox1;
        TPopupMenu *TntPopupMenu1;
        TMenuItem *Popup_Export;
        TMenuItem *Popup_Import;
        TSaveDialog *SaveDialog1;
        TOpenDialog *OpenDialog1;
  void __fastcall ImageClick(TObject *Sender);
  void __fastcall EditKeyPress(TObject *Sender, char &Key);
  void __fastcall Button1Click(TObject *Sender);
  void __fastcall Button3Click(TObject *Sender);
  void __fastcall ListBox1Click(TObject *Sender);
  void __fastcall Button4Click(TObject *Sender);
  void __fastcall Button6Click(TObject *Sender);
  void __fastcall Button5Click(TObject *Sender);
  void __fastcall Edit3Change(TObject *Sender);
  void __fastcall RadioButtonClick(TObject *Sender);
  void __fastcall Edit5Change(TObject *Sender);
  void __fastcall CheckBox1Click(TObject *Sender);
        void __fastcall Popup_ImportClick(TObject *Sender);
        void __fastcall Popup_ExportClick(TObject *Sender);

private:	// User declarations
  boost::shared_ptr<TPointSeries> Series;
  TData &Data;

  void ShowUserModels(const std::wstring &Selected);

public:		// User declarations
  __fastcall TForm13(TComponent* Owner, TData &AData);
  int InsertTrendline(const boost::shared_ptr<TPointSeries> &ASeries);
};
//---------------------------------------------------------------------------
#endif

