/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2007 Ivan Johansen
 *
 * Graph is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
#ifndef Unit12H
#define Unit12H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "LineSelect.h"
#include <ComCtrls.hpp>
#include "ExtColorBox.h"
#include "MyEdit.h"
#include "ExtComboBox.h"
//---------------------------------------------------------------------------
class TForm12 : public TForm
{
__published:	// IDE-managed Components
    TButton *Button1;
    TButton *Button2;
    TMyEdit *Edit1;
    TLabel *Label1;
    TRadioGroup *RadioGroup1;
    TGroupBox *GroupBox1;
    TLabel *Label10;
    TMyEdit *Edit2;
    TGroupBox *GroupBox2;
    TLabel *Label7;
    TLabel *Label8;
    TLabel *Label9;
    TEdit *Edit3;
    TUpDown *UpDown1;
    TLineSelect *LineSelect1;
    TButton *Button3;
    TGroupBox *GroupBox3;
    TLabel *Label4;
    TLabel *Label5;
    TMyEdit *Edit4;
    TMyEdit *Edit5;
  TGroupBox *GroupBox5;
  TLabel *Label11;
  TLabel *Label12;
  TExtComboBox *ComboBox1;
  TExtComboBox *ComboBox2;
  TExtColorBox *ExtColorBox1;

    void __fastcall Button1Click(TObject *Sender);
    void __fastcall Edit3KeyPress(TObject *Sender, char &Key);
    void __fastcall Button3Click(TObject *Sender);
  void __fastcall ComboBoxDrawItem(TWinControl *Control, int Index,
          TRect &Rect, TOwnerDrawState State);

private:	// User declarations
    TData &Data;
    TGraphElemPtr Parent;
    boost::shared_ptr<TTan> OldTan;

public:		// User declarations
    __fastcall TForm12(TComponent* Owner, TData &AData);
    int EditTan(const boost::shared_ptr<TTan> &Tan);
    int InsertTan(const TGraphElemPtr &AParent);
};
//---------------------------------------------------------------------------
#endif
