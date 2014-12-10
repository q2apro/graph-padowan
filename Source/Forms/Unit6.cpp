/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2007 Ivan Johansen
 *
 * Graph is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
#include "Graph.h"
#pragma hdrstop
#include "Unit6.h"
#include "SymbolForm.h"
//---------------------------------------------------------------------------
#pragma link "IFontBox"
#pragma link "IRichEdit"
#pragma link "ExtColorBox"
#pragma link "SymbolDialog"
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TForm6::TForm6(TComponent* Owner, TVclObject<TFont> DefaultFont, const String &HostApp, const String &HostDoc)
  : TForm(Owner)
{
  ScaleForm(this);
  TranslateProperties(this);
  SetAccelerators(this);
  TranslateStrings(ColorBox1->Items);
  FlipForm(this);

  OrgComboBox1WindowProc = ComboBox1->WindowProc;
  ComboBox1->WindowProc = ComboBox1Proc;
  OrgIFontBox1WindowProc = IFontBox1->WindowProc;
  IFontBox1->WindowProc = IFontBox1Proc;
  OrgColorBox1WindowProc = ColorBox1->WindowProc;
  ColorBox1->WindowProc = ColorBox1Proc;

  IRichEdit1->SetHostNames(HostApp, HostDoc.IsEmpty() ? HostApp : HostDoc);
  IRichEdit1->SelectAll();
  IRichEdit1->SelAttributes->Assign(DefaultFont);
  IRichEdit1->SelLength = 0;
  IRichEdit1->ClearUndo();
}
//---------------------------------------------------------------------------
void __fastcall TForm6::ParagraphChange(TObject *Sender)
{
  if(ToolButton5->Down)
    IRichEdit1->Paragraph->Alignment = pfaLeft;
  else if(ToolButton6->Down)
    IRichEdit1->Paragraph->Alignment = pfaCenter;
  else if(ToolButton7->Down)
    IRichEdit1->Paragraph->Alignment = pfaRight;
}
//---------------------------------------------------------------------------
void __fastcall TForm6::IRichEdit1SelectionChange(TObject *Sender)
{
  if(!IFontBox1->Focused())
    UpdateFont();
}
//---------------------------------------------------------------------------
void __fastcall TForm6::IRichEdit1KeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
  if(Key == VK_ESCAPE)
     Button2->Click();

  if(Shift.Contains(ssCtrl))
  {
    switch(Key)
    {
      case 'B': ToolButton1->Down = !ToolButton1->Down; ToolButton1->Click(); break;
      case 'I': ToolButton2->Down = !ToolButton2->Down; ToolButton2->Click(); break;
      case 'U': ToolButton3->Down = !ToolButton3->Down; ToolButton3->Click(); break;
      case 'S': ToolButton4->Down = !ToolButton4->Down; ToolButton4->Click(); break;
      case 'L': ToolButton5->Down = true; ToolButton5->Click(); break;
      case 'E': ToolButton6->Down = true; ToolButton6->Click(); break;
      case 'R': ToolButton7->Down = true; ToolButton7->Click(); break;
      case 'G': ToolButton11->Click(); break;
      case VK_RETURN: if(Shift.Contains(ssCtrl)) {Key = 0; Button1->Click();} break;
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm6::ComboBoxKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
  if(Key == VK_RETURN)
  {
    if(Shift.Contains(ssCtrl))
      Button1->Click();
    Key = 0;
    IRichEdit1->SetFocus();
  }
  else if(Key == VK_ESCAPE)
  {
    if(TCustomCombo *ComboBox = dynamic_cast<TCustomCombo*>(Sender))
      if(ComboBox->DroppedDown)
        ComboBox->DroppedDown = false;
      else
        Button2->Click();
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm6::ComboBoxKey1Press(TObject *Sender, char &Key)
{
  if(!std::isdigit(Key) && Key != '\b')
    Key = 0;
}
//---------------------------------------------------------------------------
void __fastcall TForm6::ComboBox1Proc(TMessage &msg)
{
  OrgComboBox1WindowProc(msg);
  if(msg.Msg == WM_GETDLGCODE)
    msg.Result |= DLGC_WANTALLKEYS;
}
//-------------------------------------------------------
void __fastcall TForm6::IFontBox1Proc(TMessage &msg)
{
  OrgIFontBox1WindowProc(msg);
  if(msg.Msg == WM_GETDLGCODE)
    msg.Result |= DLGC_WANTALLKEYS;
}
//-------------------------------------------------------
void __fastcall TForm6::ColorBox1Proc(TMessage &msg)
{
  OrgColorBox1WindowProc(msg);
  if(msg.Msg == WM_GETDLGCODE)
    msg.Result |= DLGC_WANTALLKEYS;
}
//-------------------------------------------------------
void __fastcall TForm6::IFontBox1KeyPress(TObject *Sender, char &Key)
{
  if(Key == '\r' || Key == 27)
    Key = 0;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TForm6::GetText()
{
  return IRichEdit1->GetRichText();
}
//---------------------------------------------------------------------------
void __fastcall TForm6::SetText(const AnsiString &Str)
{
  IRichEdit1->SetRichText(Str);
}
//---------------------------------------------------------------------------
bool TForm6::IsEmpty()
{
  return IRichEdit1->IsEmpty();
}
//---------------------------------------------------------------------------
void __fastcall TForm6::FormShow(TObject *Sender)
{
  UpdateFont();
}
//---------------------------------------------------------------------------
void __fastcall TForm6::RichEdit1KeyPress(TObject *Sender, char &Key)
{
  //For some strange reason Ctrl-I also generates a TAB character
  if(Key == '\t')
    Key = 0;
}
//---------------------------------------------------------------------------
void __fastcall TForm6::ToolButton9Click(TObject *Sender)
{
  IRichEdit1->TextFormat.SetSuperscript(ToolButton9->Down);
  UpdateFont();
}
//---------------------------------------------------------------------------
void __fastcall TForm6::ToolButton10Click(TObject *Sender)
{
  IRichEdit1->TextFormat.SetSubscript(ToolButton10->Down);
  UpdateFont();
}
//---------------------------------------------------------------------------
void __fastcall TForm6::ToolButton1Click(TObject *Sender)
{
  IRichEdit1->TextFormat.SetBold(ToolButton1->Down);
  UpdateFont();
}
//---------------------------------------------------------------------------
void __fastcall TForm6::ToolButton2Click(TObject *Sender)
{
  IRichEdit1->TextFormat.SetItalic(ToolButton2->Down);
  UpdateFont();
}
//---------------------------------------------------------------------------
void __fastcall TForm6::ToolButton3Click(TObject *Sender)
{
  IRichEdit1->TextFormat.SetUnderline(ToolButton3->Down);
  UpdateFont();
}
//---------------------------------------------------------------------------
void __fastcall TForm6::ToolButton4Click(TObject *Sender)
{
  IRichEdit1->TextFormat.SetStrikeOut(ToolButton4->Down);
  UpdateFont();
}
//---------------------------------------------------------------------------
void __fastcall TForm6::IFontBox1Exit(TObject *Sender)
{
  IRichEdit1->TextFormat.SetName(IFontBox1->FontName);
}
//---------------------------------------------------------------------------
void __fastcall TForm6::ColorBox1Change(TObject *Sender)
{
  IRichEdit1->TextFormat.SetColor(ColorBox1->Selected);
  UpdateFont();
}
//---------------------------------------------------------------------------
void __fastcall TForm6::ComboBox1Exit(TObject *Sender)
{
  try
  {
    IRichEdit1->TextFormat.SetSize(ComboBox1->Text.ToInt());
    UpdateFont();
  }
  catch(EConvertError&)
  {
  }
}
//---------------------------------------------------------------------------
void TForm6::UpdateFont()
{
  IFontBox1->FontName = IRichEdit1->TextFormat.GetName();
  ComboBox1->Text = IRichEdit1->TextFormat.GetSize();
  ComboBox1->ItemIndex = ComboBox1->Items->IndexOf(ComboBox1->Text);
  ColorBox1->Selected = IRichEdit1->TextFormat.GetColor();
  ColorBox2->Selected = IRichEdit1->BackgroundColor;
  ToolButton1->Down = IRichEdit1->TextFormat.GetBold();
  ToolButton2->Down = IRichEdit1->TextFormat.GetItalic();
  ToolButton3->Down = IRichEdit1->TextFormat.GetUnderline();
  ToolButton4->Down = IRichEdit1->TextFormat.GetStrikeOut();

  switch(IRichEdit1->Paragraph->Alignment)
  {
    case pfaLeft:    ToolButton5->Down = true; ToolButton6->Down = false; ToolButton7->Down = false; break;
    case pfaCenter:  ToolButton6->Down = true; ToolButton5->Down = false; ToolButton7->Down = false; break;
    case pfaRight:   ToolButton7->Down = true; ToolButton5->Down = false; ToolButton6->Down = false; break;
  }

  ToolButton9->Down = IRichEdit1->TextFormat.GetSuperscript();
  ToolButton10->Down = IRichEdit1->TextFormat.GetSubscript();
}
//---------------------------------------------------------------------------
void __fastcall TForm6::Popup_CutClick(TObject *Sender)
{
  IRichEdit1->CutToClipboard();
}
//---------------------------------------------------------------------------
void __fastcall TForm6::Popup_CopyClick(TObject *Sender)
{
  IRichEdit1->CopyToClipboard();
}
//---------------------------------------------------------------------------
void __fastcall TForm6::Popup_PasteClick(TObject *Sender)
{
  IRichEdit1->PasteFromClipboard();
}
//---------------------------------------------------------------------------
void __fastcall TForm6::Popup_UndoClick(TObject *Sender)
{
  IRichEdit1->Undo();
}
//---------------------------------------------------------------------------
void __fastcall TForm6::Popup_RedoClick(TObject *Sender)
{
  IRichEdit1->Redo();
}
//---------------------------------------------------------------------------
void __fastcall TForm6::PopupMenu1Popup(TObject *Sender)
{
  Popup_Undo->Enabled = IRichEdit1->CanUndo;
  Popup_Redo->Enabled = IRichEdit1->CanRedo();
  Popup_Cut->Enabled = IRichEdit1->SelLength;
  Popup_Copy->Enabled = IRichEdit1->SelLength;
  Popup_Paste->Enabled = IRichEdit1->CanPaste();
  Popup_PasteSpecial->Enabled = IRichEdit1->CanPaste();
  Popup_EditObject->Enabled = IRichEdit1->ObjectSelected();
}
//---------------------------------------------------------------------------
void __fastcall TForm6::IRichEdit1MouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  if(Button == mbRight)
  {
    TPoint Pos = IRichEdit1->ClientToScreen(TPoint(X, Y));
    PopupMenu1->Popup(Pos.x, Pos.y);
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm6::Popup_PasteSpecialClick(TObject *Sender)
{
  IRichEdit1->PasteSpecial();
}
//---------------------------------------------------------------------------
void __fastcall TForm6::Popup_InsertObjectClick(TObject *Sender)
{
  IRichEdit1->InsertObject();
}
//---------------------------------------------------------------------------
void __fastcall TForm6::Popup_EditObjectClick(TObject *Sender)
{
  if(!IRichEdit1->OpenObject())
    MessageBox(LoadRes(502), LoadRes(RES_OLE_ERROR), MB_ICONSTOP);
}
//---------------------------------------------------------------------------
TVclObject<TFont> TForm6::GetFont() const
{
  TVclObject<TFont> Font;
  Font->Assign(IRichEdit1->SelAttributes);
  return Font;
}
//---------------------------------------------------------------------------
void __fastcall TForm6::IRichEdit1KeyPress(TObject *Sender, char &Key)
{
  //For some strange reason Ctrl-I generates a KeyPress with a TAB character
  //This can be ignored without problems. Tab can still be used to move focus,
  //ans Ctrl-Tab will insert a TAB character
  if(Key == '\t')
    Key = 0;
}
//---------------------------------------------------------------------------
void __fastcall TForm6::Button3Click(TObject *Sender)
{
  Application->HelpContext(HelpContext);
}
//---------------------------------------------------------------------------
void __fastcall TForm6::IRichEdit1OleError(TIRichEdit *Sender, int Oper,
      int ErrorCode)
{
  MessageBox(LoadRes(502), LoadRes(RES_OLE_ERROR), MB_ICONSTOP);
}
//---------------------------------------------------------------------------
void __fastcall TForm6::ColorBox2Change(TObject *Sender)
{
  IRichEdit1->BackgroundColor = ColorBox2->Selected; 
}
//---------------------------------------------------------------------------
void TForm6::SetBackgroundColor(TColor Color)
{
  IRichEdit1->BackgroundColor = Color == clNone ? clDefault : Color;
  ColorBox2->Selected = Color == clNone ? clDefault : Color;
}
//---------------------------------------------------------------------------
TColor TForm6::GetBackgroundColor() const
{
  return ColorBox2->Selected == clDefault ? clNone : ColorBox2->Selected;
}
//---------------------------------------------------------------------------
void __fastcall TForm6::ToolButton11Click(TObject *Sender)
{
  //Only set dialog font for Windows NT. Win 9x will use default font.
  if(Win32Platform == VER_PLATFORM_WIN32_NT)
    SymbolDialog1->FontName = IRichEdit1->TextFormat.GetName();
  SymbolDialog1->Execute();
}
//---------------------------------------------------------------------------
void __fastcall TForm6::ToolButton13Click(TObject *Sender)
{
  IRichEdit1->InsertObject();
}
//---------------------------------------------------------------------------
void __fastcall TForm6::SymbolDialog1InsertWideChar(TSymbolDialog *Sender,
      wchar_t Symbol, const String &FontName)
{
  String OldName = IRichEdit1->TextFormat.GetName();
  IRichEdit1->SetSelText(Symbol, FontName, ToIntDef(ComboBox1->Text, 12));
  IRichEdit1->TextFormat.SetName(OldName);
}
//---------------------------------------------------------------------------
void __fastcall TForm6::SymbolDialog1InsertAnsiChar(TSymbolDialog *Sender,
      char Symbol, const String &FontName)
{
  String OldName = IRichEdit1->TextFormat.GetName();
  IRichEdit1->SetSelText(Symbol, FontName, ToIntDef(ComboBox1->Text, 12));
  IRichEdit1->TextFormat.SetName(OldName);
}
//---------------------------------------------------------------------------
void __fastcall TForm6::SymbolDialog1Show(TObject *Sender)
{
  ScaleForm(SymbolDialog1->SymbolForm);
}
//---------------------------------------------------------------------------
bool __fastcall TForm6::IRichEdit1ActivateObject(TIRichEdit *Sender)
{
  //Implemented workaround to support Open Office objects.
  //Else they fail if the client tries to default instantiate them (e.g. by double
  //clicking on them). Instead we have to force them to run out of process.
  IRichEdit1->OpenObject();
  return false;
}
//---------------------------------------------------------------------------
void __fastcall TForm6::FormCanResize(TObject *Sender, int &NewWidth, int &NewHeight,
          bool &Resize)
{
  ToolBar1->Indent = ToolBar1->Indent + std::max(NewWidth, Constraints->MinWidth) - Width;
}
//---------------------------------------------------------------------------
bool TForm6::GetOleLink()
{
  return IRichEdit1->HasOleLink();
}
//---------------------------------------------------------------------------

