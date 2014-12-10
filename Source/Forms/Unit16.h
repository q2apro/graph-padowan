/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2007 Ivan Johansen
 *
 * Graph is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
#ifndef Unit16H
#define Unit16H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "ShadeSelect.h"
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
#include "MyRadioButton.h"
#include <ComCtrls.hpp>
#include "ExtColorBox.h"
#include "MyEdit.h"
//---------------------------------------------------------------------------
class TForm16 : public TForm
{
__published:	// IDE-managed Components
  TPageControl *PageControl1;
  TTabSheet *TabSheet1;
  TTabSheet *TabSheet2;
  TTabSheet *TabSheet3;
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
  TPanel *Panel6;
  TImage *Image6;
  TMyRadioButton *RadioButton3;
  TMyRadioButton *RadioButton2;
  TMyRadioButton *RadioButton1;
  TMyRadioButton *RadioButton6;
  TMyRadioButton *RadioButton4;
  TMyRadioButton *RadioButton5;
  TButton *Button1;
  TButton *Button2;
  TButton *Button3;
  TListBox *ListBox1;
  TGroupBox *GroupBox2;
  TMyEdit *Edit3;
  TLabel *Label8;
  TLabel *Label9;
  TMyEdit *Edit4;
  TCheckBox *CheckBox4;
  TCheckBox *CheckBox3;
  TLabel *Label11;
  TGroupBox *GroupBox1;
  TLabel *Label7;
  TLabel *Label6;
  TMyEdit *Edit1;
  TMyEdit *Edit2;
  TCheckBox *CheckBox2;
  TCheckBox *CheckBox1;
  TGroupBox *GroupBox3;
  TLabel *Label5;
  TLabel *Label4;
  TShadeSelect *ShadeSelect1;
  TLabel *Label12;
  TEdit *Edit5;
  TCheckBox *CheckBox5;
  TExtColorBox *ExtColorBox1;
  void __fastcall ImageClick(TObject *Sender);
  void __fastcall Button1Click(TObject *Sender);
  void __fastcall ExtColorBox1Change(TObject *Sender);
  void __fastcall RadioButton6CheckedChange(TObject *Sender);
  void __fastcall FormShow(TObject *Sender);
  void __fastcall Button3Click(TObject *Sender);

private:	// User declarations
  boost::shared_ptr<TBaseFuncType> Func;
  std::vector<boost::shared_ptr<TBaseFuncType> > FuncList;
  boost::shared_ptr<TShading> OldShade;
  TData &Data;

  void ShowFuncList();
  void EnableGroupBox2(bool Enable);

public:		// User declarations
  __fastcall TForm16(TComponent* Owner, TData &AData);
  int EditShade(const boost::shared_ptr<TShading> &AShade);
  TModalResult InsertShade(const boost::shared_ptr<TBaseFuncType> &AFunc);
};
//---------------------------------------------------------------------------
#endif

