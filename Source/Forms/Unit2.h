/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2007 Ivan Johansen
 *
 * Graph is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
#ifndef Unit2H
#define Unit2H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
#include <ComCtrls.hpp>
#include "SymbolDialog.h"
#include <pngimage.hpp>
#include "LinkLabelEx.h"
#include <GIFImg.hpp>
//---------------------------------------------------------------------------
class TForm2 : public TForm
{
__published:	// IDE-managed Components
  TButton *Button1;
	TPanel *Panel1;
  TLabel *ProductName;
  TLabel *Version;
  TLabel *Copyright;
  TLinkLabelEx *Comments;
  TLabel *Label3;
  TImage *ProgramIcon;
  TLinkLabelEx *LinkLabel1;
  TLinkLabelEx *LinkLabel2;
  TLinkLabelEx *LinkLabel3;
  TImage *Image1;
  void __fastcall ProgramIconDblClick(TObject *Sender);
  void __fastcall LinkLabel1LinkClick(TObject *Sender, const UnicodeString Link,
          TSysLinkType LinkType);
  void __fastcall LinkLabel2LinkClick(TObject *Sender, const UnicodeString Link,
          TSysLinkType LinkType);
  void __fastcall FormShow(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TForm2(TComponent* Owner);
};
//---------------------------------------------------------------------------
#endif
