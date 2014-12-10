/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2007 Ivan Johansen
 *
 * Graph is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
#ifndef Unit6H
#define Unit6H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <ToolWin.hpp>
#include <ImgList.hpp>
#include "IFontBox.h"
#include <Graphics.hpp>
#include "IRichEdit.h"
#include <Menus.hpp>
#include "ExtColorBox.h"
#include "SymbolDialog.h"
#include <ActnPopup.hpp>
#include <PlatformDefaultStyleActnCtrls.hpp>
//---------------------------------------------------------------------------
class TForm6 : public TForm
{
__published:	// IDE-managed Components
  TButton *Button1;
  TButton *Button2;
  TLabel *Label1;
  TLabel *Label2;
  TLabel *Label3;
  TComboBox *ComboBox1;
  TGroupBox *GroupBox1;
  TToolBar *ToolBar1;
  TToolButton *ToolButton1;
  TToolButton *ToolButton2;
  TToolButton *ToolButton3;
  TImageList *ImageList1;
  TToolButton *ToolButton4;
  TToolButton *ToolButton5;
  TToolButton *ToolButton6;
  TToolButton *ToolButton7;
  TToolButton *ToolButton8;
  TIFontBox *IFontBox1;
  TButton *Button3;
  TIRichEdit *IRichEdit1;
  TToolButton *ToolButton9;
  TToolButton *ToolButton10;
  TPopupActionBar *PopupMenu1;
  TMenuItem *Popup_Cut;
  TMenuItem *Popup_Copy;
  TMenuItem *Popup_Paste;
  TMenuItem *N1;
  TMenuItem *Popup_Undo;
  TMenuItem *Popup_Redo;
  TMenuItem *Popup_PasteSpecial;
  TMenuItem *Popup_InsertObject;
  TMenuItem *Popup_EditObject;
  TMenuItem *N2;
  TLabel *Label4;
  TExtColorBox *ColorBox1;
  TExtColorBox *ColorBox2;
  TToolButton *ToolButton11;
  TToolButton *ToolButton12;
  TToolButton *ToolButton13;
  TSymbolDialog *SymbolDialog1;
  void __fastcall ParagraphChange(TObject *Sender);
  void __fastcall IRichEdit1SelectionChange(TObject *Sender);
  void __fastcall IRichEdit1KeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
  void __fastcall ComboBoxKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
  void __fastcall ComboBoxKey1Press(TObject *Sender, char &Key);
  void __fastcall IFontBox1KeyPress(TObject *Sender, char &Key);
  void __fastcall FormShow(TObject *Sender);
  void __fastcall RichEdit1KeyPress(TObject *Sender, char &Key);
  void __fastcall ToolButton9Click(TObject *Sender);
  void __fastcall ToolButton10Click(TObject *Sender);
  void __fastcall ToolButton1Click(TObject *Sender);
  void __fastcall ToolButton2Click(TObject *Sender);
  void __fastcall ToolButton3Click(TObject *Sender);
  void __fastcall ToolButton4Click(TObject *Sender);
  void __fastcall IFontBox1Exit(TObject *Sender);
  void __fastcall ColorBox1Change(TObject *Sender);
  void __fastcall ComboBox1Exit(TObject *Sender);
  void __fastcall Popup_CutClick(TObject *Sender);
  void __fastcall Popup_CopyClick(TObject *Sender);
  void __fastcall Popup_PasteClick(TObject *Sender);
  void __fastcall Popup_UndoClick(TObject *Sender);
  void __fastcall Popup_RedoClick(TObject *Sender);
  void __fastcall PopupMenu1Popup(TObject *Sender);
  void __fastcall IRichEdit1MouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
  void __fastcall Popup_PasteSpecialClick(TObject *Sender);
  void __fastcall Popup_InsertObjectClick(TObject *Sender);
  void __fastcall Popup_EditObjectClick(TObject *Sender);
  void __fastcall IRichEdit1KeyPress(TObject *Sender, char &Key);
  void __fastcall Button3Click(TObject *Sender);
  void __fastcall IRichEdit1OleError(TIRichEdit *Sender, int Oper,
          int ErrorCode);
  void __fastcall ColorBox2Change(TObject *Sender);
  void __fastcall ToolButton11Click(TObject *Sender);
  void __fastcall ToolButton13Click(TObject *Sender);
  void __fastcall SymbolDialog1InsertWideChar(TSymbolDialog *Sender,
          wchar_t Symbol, const String &FontName);
  void __fastcall SymbolDialog1InsertAnsiChar(TSymbolDialog *Sender,
          char Symbol, const String &FontName);
  void __fastcall SymbolDialog1Show(TObject *Sender);
  bool __fastcall IRichEdit1ActivateObject(TIRichEdit *Sender);
  void __fastcall FormCanResize(TObject *Sender, int &NewWidth, int &NewHeight, bool &Resize);

private:	// User declarations
  TWndMethod OrgComboBox1WindowProc;
  TWndMethod OrgIFontBox1WindowProc;
  TWndMethod OrgColorBox1WindowProc;

  void __fastcall ComboBox1Proc(TMessage &msg);
  void __fastcall IFontBox1Proc(TMessage &msg);
  void __fastcall ColorBox1Proc(TMessage &msg);
  void UpdateFont();

public:		// User declarations
  __fastcall TForm6(TComponent* Owner, TVclObject<TFont> DefaultFont, const String &HostApp, const String &HostDoc);
  AnsiString __fastcall GetText();
  bool GetOleLink();
  void __fastcall SetText(const AnsiString &Str);
  bool IsEmpty();
  void SetBackgroundColor(TColor Color);
  TColor GetBackgroundColor() const;
  TVclObject<TFont> GetFont() const;
};
//---------------------------------------------------------------------------
#endif
