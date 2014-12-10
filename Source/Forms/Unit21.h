/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2006 Ivan Johansen
 *
 * Graph is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
#ifndef Unit21H
#define Unit21H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include "MyEdit.h"
//---------------------------------------------------------------------------
class TForm21 : public TForm
{
__published:	// IDE-managed Components
  TButton *TntButton1;
  TButton *TntButton2;
  TLabel *Label1;
  TComboBox *ComboBox1;
  TLabel *Label2;
  TMyEdit *Edit1;
  TMyEdit *Edit2;
  TLabel *Label3;
  void __fastcall TntButton1Click(TObject *Sender);
  void __fastcall ComboBox1Change(TObject *Sender);
private:	// User declarations
  TLabelPlacement OldPlacement;
  boost::shared_ptr<TTextLabel> Label;
  TData &Data;
  const TDraw &Draw;

public:		// User declarations
  __fastcall TForm21(TComponent* Owner, TData &AData, const TDraw &ADraw, const boost::shared_ptr<TTextLabel> &ALabel);
};
//---------------------------------------------------------------------------
#endif

