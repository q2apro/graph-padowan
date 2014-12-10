/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2007 Ivan Johansen
 *
 * Graph is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
#ifndef Unit4H
#define Unit4H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include "CheckBoxEx.h"
#include "GridPanelEx.h"
#include "RadioGroupEx.h"
//---------------------------------------------------------------------------
class TForm4 : public TForm
{
__published:	// IDE-managed Components
  TButton *Button1;
  TButton *Button2;
  TButton *Button3;
  TGridPanelEx *GridPanelEx1;
  TLabel *Label1;
  TPanel *Panel1;
  TEdit *Edit1;
  TUpDown *UpDown1;
  TRadioGroupEx *RadioGroup1;
  TLabel *Label2;
  TPanel *Panel2;
  TEdit *Edit2;
  TUpDown *UpDown2;
  TLabel *Label3;
  TPanel *Panel3;
  TEdit *Edit3;
  TUpDown *UpDown3;
  TLabel *Label5;
  TComboBox *ComboBox1;
  TLabel *Label4;
  TComboBox *ComboBox2;
  TCheckBoxEx *CheckBox2;
  TCheckBoxEx *CheckBox3;
  TCheckBoxEx *CheckBox4;
  TCheckBoxEx *CheckBox5;
  TEdit *Edit4;
	void __fastcall Button1Click(TObject *Sender);
  void __fastcall Button3Click(TObject *Sender);
  void __fastcall ComboBox1KeyPress(TObject *Sender, char &Key);
  void __fastcall CheckBox5Click(TObject *Sender);
private:	// User declarations
  int OldLanguageIndex; //Saved language index in ComboBox
  TData &Data;

public:		// User declarations
	__fastcall TForm4(TComponent* Owner, TData &AData);
};
//---------------------------------------------------------------------------
#endif
