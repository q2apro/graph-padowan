/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2007 Ivan Johansen
 *
 * Graph is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
#ifndef Unit5H
#define Unit5H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "LineSelect.h"
#include "MyEdit.h"
#include <ComCtrls.hpp>
#include <ImgList.hpp>
#include "ExtColorBox.h"
#include "GridPanelEx.h"
#include "ExtComboBox.h"
//---------------------------------------------------------------------------
class TForm5 : public TForm
{
__published:	// IDE-managed Components
  TButton *Button1;
  TButton *Button2;
  TComboBox *ComboBox1;
  TGroupBox *GroupBox1;
  TMyEdit *Edit1;
  TLabel *Label2;
  TLabel *Label3;
  TGroupBox *GroupBox2;
  TGroupBox *GroupBox4;
  TLabel *Label4;
  TLabel *Label5;
  TMyEdit *Edit3;
  TMyEdit *Edit4;
  TButton *Button3;
  TLabel *Label1;
  TMyEdit *Edit2;
  TLabel *Label6;
  TMyEdit *Edit5;
  TGroupBox *GroupBox3;
  TLabel *Label10;
  TMyEdit *Edit7;
  TGroupBox *GroupBox5;
  TExtComboBox *ComboBox2;
  TExtComboBox *ComboBox3;
  TLabel *Label11;
  TLabel *Label12;
	TGridPanelEx *GridPanelEx1;
	TLabel *Label7;
	TLineSelect *LineSelect1;
	TLabel *Label13;
	TComboBox *ComboBox4;
	TLabel *Label9;
	TExtColorBox *ExtColorBox1;
	TLabel *Label8;
	TPanel *Panel1;
	TEdit *Edit6;
	TUpDown *UpDown1;
	void __fastcall Button1Click(TObject *Sender);
  void __fastcall ComboBox1Change(TObject *Sender);
  void __fastcall Button3Click(TObject *Sender);
  void __fastcall ComboBox1KeyPress(TObject *Sender, char &Key);
  void __fastcall ComboBoxDrawItem(TWinControl *Control, int Index, TRect &Rect, TOwnerDrawState State);
  void __fastcall ComboBox4Change(TObject *Sender);
private:	// User declarations
  struct TTempData
  {
    String Text1, Text2, From, To, Steps;
  };

  TTempData TempData[3];
  int OldItemIndex;
  boost::shared_ptr<TBaseFuncType> F;
  TData &Data;

  void Translate();

public:		// User declarations
	__fastcall TForm5(TComponent* Owner, TData &AData);
  int EditFunc(boost::shared_ptr<TBaseFuncType> Func);
};
//---------------------------------------------------------------------------
#endif
