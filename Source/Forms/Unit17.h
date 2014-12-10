/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2007 Ivan Johansen
 *
 * Graph is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
#ifndef Unit17H
#define Unit17H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
#include <ValEdit.hpp>
#include "Grid.h"
#include <Menus.hpp>
#include <Dialogs.hpp>
#include <ActnPopup.hpp>
#include <PlatformDefaultStyleActnCtrls.hpp>
#include "SaveDialogEx.h"
//---------------------------------------------------------------------------
class TForm17 : public TForm
{
__published:	// IDE-managed Components
  TButton *Button1;
  TButton *Button2;
  TButton *Button4;
  TButton *Button3;
  TGrid *Grid1;
  TPopupActionBar *PopupMenu1;
  TMenuItem *Popup_Insert;
  TMenuItem *Popup_Remove;
  TMenuItem *Popup_Cut;
  TMenuItem *Popup_Copy;
  TMenuItem *Popup_Paste;
  TMenuItem *Popup_Delete;
  TMenuItem *N1;
  TMenuItem *N2;
  TMenuItem *Popup_Import;
  TMenuItem *Popup_Export;
  TMenuItem *Popup_Select;
  TOpenDialog *OpenDialog1;
  TSaveDialogEx *SaveDialog1;
  void __fastcall Button4Click(TObject *Sender);
  void __fastcall Button1Click(TObject *Sender);
  void __fastcall Button3Click(TObject *Sender);
  void __fastcall Popup_InsertClick(TObject *Sender);
  void __fastcall Popup_RemoveClick(TObject *Sender);
  void __fastcall Grid1EditorKeyPress(TInplaceEdit *InplaceEdit,
          char &Key);
  void __fastcall Popup_CutClick(TObject *Sender);
  void __fastcall Popup_CopyClick(TObject *Sender);
  void __fastcall Popup_PasteClick(TObject *Sender);
  void __fastcall Popup_DeleteClick(TObject *Sender);
  void __fastcall Popup_SelectClick(TObject *Sender);
  void __fastcall Popup_ImportClick(TObject *Sender);
  void __fastcall Popup_ExportClick(TObject *Sender);
private:	// User declarations
  TData &Data;

	int FindGridRow(int Index);
public:		// User declarations
  __fastcall TForm17(TComponent* Owner, TData &AData);
};
//---------------------------------------------------------------------------
#endif
