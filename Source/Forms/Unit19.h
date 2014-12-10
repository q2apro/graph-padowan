/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2007 Ivan Johansen
 *
 * Graph is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
#ifndef Unit19H
#define Unit19H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <MPlayer.hpp>
#include "ProgressForm.h"
#include "MyEdit.h"
#include <map>
//---------------------------------------------------------------------------
class TForm19 : public TForm
{
__published:	// IDE-managed Components
  TLabel *Label1;
  TComboBox *ComboBox1;
  TButton *Button1;
  TButton *Button2;
  TButton *Button4;
  TProgressForm *ProgressForm1;
  TButton *Button3;
  TGroupBox *TntGroupBox1;
  TGroupBox *TntGroupBox2;
  TLabel *Label2;
  TMyEdit *Edit1;
  TLabel *Label3;
  TMyEdit *Edit2;
  TLabel *Label4;
  TMyEdit *Edit3;
  TLabel *Label5;
  TMyEdit *Edit4;
  TLabel *Label6;
  TMyEdit *Edit5;
  TLabel *Label7;
  TMyEdit *Edit6;
  void __fastcall Button1Click(TObject *Sender);
  void __fastcall Button3Click(TObject *Sender);
  void __fastcall TntEditKeyPress(TObject *Sender, char &Key);
  void __fastcall ComboBox1Change(TObject *Sender);
private:	// User declarations
  TData Data;
  Graph::TAnimationInfo &AnimationInfo;

  void GetSymbolDependent(const std::wstring &SymbolName, std::vector<TGraphElemPtr> &List, const TGraphElemPtr &Elem) const;

public:		// User declarations
  __fastcall TForm19(TComponent* Owner, TData &AData, int AWidth, int AHeight);
};
//---------------------------------------------------------------------------
#endif
