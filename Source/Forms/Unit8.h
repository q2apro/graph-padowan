/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2007 Ivan Johansen
 *
 * Graph is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
#ifndef Unit8H
#define Unit8H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
#include <ValEdit.hpp>
//---------------------------------------------------------------------------
class TForm8 : public TForm
{
__published:	// IDE-managed Components
  TEdit *Edit1;
  TLabel *Label1;
  TValueListEditor *ValueListEditor1;
  TButton *Button1;
  TButton *Button2;
  void __fastcall Button1Click(TObject *Sender);
private:	// User declarations
  TData &Data;
  bool AllowReplace;

public:		// User declarations
  __fastcall TForm8(TComponent* Owner, TData &AData);
  bool EditModel(std::wstring &Name);
  bool AddModel(const std::wstring &model, std::wstring &Name);
};
//---------------------------------------------------------------------------
#endif
