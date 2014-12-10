/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2007 Ivan Johansen
 *
 * Graph is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
#ifndef Unit11H
#define Unit11H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "ShadeSelect.h"
#include "MyEdit.h"
#include <ComCtrls.hpp>
#include "ExtColorBox.h"
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TForm11 : public TForm
{
__published:	// IDE-managed Components
  TButton *Button1;
  TButton *Button2;
  TButton *Button3;
  TLabel *Label1;
  TGroupBox *GroupBox1;
  TLabel *Label10;
  TMyEdit *Edit3;
  TGroupBox *GroupBox2;
  TLabel *Label7;
  TLabel *Label9;
  TShadeSelect *ShadeSelect1;
  TMyEdit *Edit1;
  TLabel *Label2;
  TLabel *Label3;
  TMyEdit *Edit2;
  TExtColorBox *ExtColorBox1;
  TPanel *Panel1;
  TEdit *Edit4;
  TUpDown *UpDown1;
  void __fastcall Button3Click(TObject *Sender);
  void __fastcall Button1Click(TObject *Sender);
  void __fastcall Edit4KeyPress(TObject *Sender, char &Key);
private:	// User declarations
  boost::shared_ptr<TRelation> Relation;
  TData &Data;

  void Translate();

public:		// User declarations
  __fastcall TForm11(TComponent* Owner, TData &AData);
  int EditRelation(const boost::shared_ptr<TRelation> &ARelation);
};
//---------------------------------------------------------------------------
#endif

