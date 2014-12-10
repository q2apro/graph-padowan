/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2007 Ivan Johansen
 *
 * Graph is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
#ifndef TStdFuncFrameH
#define TStdFuncFrameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include "MyEdit.h"
#include "UpDownEx.h"
#include "TEvalFrame.h"
//---------------------------------------------------------------------------
class TStdFuncFrame : public TEvalFrame
{
__published:	// IDE-managed Components
  TLabel *Label2;
  TLabel *Label3;
  TLabel *Label4;
  TLabel *Label1;
  TMyEdit *Edit1;
  TComboBox *ComboBox1;
  TLabel *Label5;
  TEdit *Edit2;
  TEdit *Edit3;
  TEdit *Edit4;
  TUpDownEx *UpDown1;
  void __fastcall ComboBox1Change(TObject *Sender);
private:	// User declarations
  std::string ErrorPrefix;
  void Clear();

public:		// User declarations
  __fastcall TStdFuncFrame(TComponent* Owner);
  void Eval(const TGraphElem *Elem);
  void SetPoint(const TGraphElem *Elem, int X, int Y);
  std::string GetErrorPrefix() {return ErrorPrefix;}
  TFrame* GetFrame() {return this;} //Workaround for nasty compiler bug
};
//---------------------------------------------------------------------------
#endif
