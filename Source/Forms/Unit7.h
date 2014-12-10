/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2007 Ivan Johansen
 *
 * Graph is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
#ifndef Unit7H
#define Unit7H
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
//---------------------------------------------------------------------------
class TForm7 : public TForm
{
__published:	// IDE-managed Components
  TButton *Button1;
  TButton *Button2;
  TGroupBox *GroupBox2;
  TLabel *Label4;
  TLabel *Label5;
  TMyEdit *Edit1;
  TMyEdit *Edit2;
  TGroupBox *GroupBox4;
  TLabel *Label7;
  TLabel *Label8;
  TLabel *Label9;
  TEdit *Edit4;
  TUpDown *UpDown1;
  TLineSelect *LineSelect1;
  TGroupBox *GroupBox3;
  TLabel *Label10;
  TMyEdit *Edit3;
  TButton *Button3;
  TExtColorBox *ExtColorBox1;
  void __fastcall Button1Click(TObject *Sender);
  void __fastcall Button3Click(TObject *Sender);

private:	// User declarations
  boost::shared_ptr<TBaseFuncType> Func;
  TData &Data;

public:		// User declarations
  __fastcall TForm7(TComponent* Owner, TData &AData);
  int InsertDif(const boost::shared_ptr<TBaseFuncType> &F);
};
//---------------------------------------------------------------------------
#endif

