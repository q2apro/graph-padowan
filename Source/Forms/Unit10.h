/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2007 Ivan Johansen
 *
 * Graph is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
#ifndef Unit10H
#define Unit10H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
#include "LinkLabelEx.h"
//---------------------------------------------------------------------------
class TForm10 : public TForm
{
__published:	// IDE-managed Components
  TButton *Button1;
  TButton *Button2;
  TButton *Button3;
  TCheckBox *CheckBox1;
  TPanel *Panel1;
  TLabel *Label1;
  TImage *Image1;
  TLinkLabelEx *LinkLabel1;
  void __fastcall Button2Click(TObject *Sender);
  void __fastcall Button3Click(TObject *Sender);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall LinkLabel1LinkClick(TObject *Sender, const UnicodeString Link,
          TSysLinkType LinkType);

private:	// User declarations

public:		// User declarations
  __fastcall TForm10(TComponent* Owner);
};
//---------------------------------------------------------------------------
#endif

