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
#include "Unit1.h"
#include "GuiUtil.h"
#include "PointSelect.h"
#include <ValueEdit.hpp>
#include <set>
#include <algorithm>
#pragma warn -8072 //Disable warning: Suspicous pointer arithmetic
#include <boost/format.hpp>
#include <MultiMon.hpp>
//---------------------------------------------------------------------------
void DrawLine(TCanvas *Canvas, int X1, int Y1, int X2, int Y2)
{
  Canvas->MoveTo(X1, Y1);
  Canvas->LineTo(X2, Y2);
}
//---------------------------------------------------------------------------
void DrawComboBoxEndPoint(TCanvas *Canvas, int Index, const TRect &Rect)
{
  //Clear drawing area
  Canvas->Brush->Color = clWhite;
  Canvas->FillRect(Rect);
  Canvas->Pen->Color = clRed;
  Canvas->Pen->Width = 2;

  TPoint Pos(Rect.Left/2 + Rect.Right/2, Rect.Top/2 + Rect.Bottom/2);
  switch(Index)
  {
    case 1:
      TPointSelect::DrawPoint(Canvas, Pos, 0, clRed, clWhite, 5);
      break;

		case 2:
			TPointSelect::DrawPoint(Canvas, Pos, 0, clRed, clRed, 5);
			break;

		case 3:
			TPointSelect::DrawPoint(Canvas, Pos, 7, clRed, clRed, 5);
			break;

		case 4:
      Canvas->Arc(Pos.x - 11, Pos.y - 6, Pos.x + 5, Pos.y + 7, Pos.x - 3, Pos.y + 7, Pos.x - 3, Pos.y - 7);
      break;

    case 5:
      Canvas->Arc(Pos.x + 11, Pos.y - 6, Pos.x - 5, Pos.y + 7, Pos.x + 3, Pos.y - 7, Pos.x + 3, Pos.y + 7);
      break;

    case 6:
      DrawLine(Canvas, Pos.x + 2, Pos.y - 5, Pos.x - 4, Pos.y - 5);
      DrawLine(Canvas, Pos.x + 2, Pos.y - 6, Pos.x + 2, Pos.y + 6);
      DrawLine(Canvas, Pos.x + 2, Pos.y + 6, Pos.x - 4, Pos.y + 6);
      break;

    case 7:
      DrawLine(Canvas, Pos.x - 4, Pos.y - 5, Pos.x + 2, Pos.y - 5);
      DrawLine(Canvas, Pos.x - 3, Pos.y - 6, Pos.x - 3, Pos.y + 6);
      DrawLine(Canvas, Pos.x - 3, Pos.y + 6, Pos.x + 2, Pos.y + 6);
      break;
  }

  //This is necesarry to use white as brush before the VCL can draw the focus
  //rectangle correct; I don't know why.
  TRect Rect1(0,0,0,0);
  Canvas->Brush->Color = clWhite;
  Canvas->FillRect(Rect1);
}
//---------------------------------------------------------------------------
void ScaleComponent(TComponent *Component, unsigned Scale)
{
  if(TComboBox *ComboBox = dynamic_cast<TComboBox*>(Component))
    ComboBox->ItemHeight = (ComboBox->ItemHeight * Scale) / 100;
  else if(TColorBox *ColorBox = dynamic_cast<TColorBox*>(Component))
    ColorBox->ItemHeight = (ColorBox->ItemHeight * Scale) / 100;
  else if(TValueListEditor *Editor = dynamic_cast<TValueListEditor*>(Component))
    Editor->DefaultRowHeight = (Editor->DefaultRowHeight * Scale) / 100;
  for(int I = 0; I < Component->ComponentCount; I++)
    ScaleComponent(Component->Components[I], Scale);
}
//---------------------------------------------------------------------------
void FlipAnchors(TControl *Control)
{
  TAnchors Anchors;
  if(Control->Anchors.Contains(akTop)) Anchors << akTop;
  if(Control->Anchors.Contains(akBottom)) Anchors << akBottom;
  if(Control->Anchors.Contains(akLeft)) Anchors << akRight;
  if(Control->Anchors.Contains(akRight)) Anchors << akLeft;
  Control->Anchors = Anchors;

  Control->Margins->SetBounds(
    Control->Margins->Right,
    Control->Margins->Top,
    Control->Margins->Left,
    Control->Margins->Bottom);

  if(TWinControl *WinControl = dynamic_cast<TWinControl*>(Control))
    for(int I = 0; I < WinControl->ControlCount; I++)
      FlipAnchors(WinControl->Controls[I]);
}
//---------------------------------------------------------------------------
void FlipMargins(TControl *Control)
{
  Control->Margins->SetBounds(
    Control->Margins->Right,
    Control->Margins->Top,
    Control->Margins->Left,
    Control->Margins->Bottom);

  if(TWinControl *WinControl = dynamic_cast<TWinControl*>(Control))
    for(int I = 0; I < WinControl->ControlCount; I++)
      FlipMargins(WinControl->Controls[I]);
}
//---------------------------------------------------------------------------
void ScaleForm(TForm *Form)
{
  //Change font for form. All components should have ParentFont=true
  Form->Font->Name = "MS Shell Dlg";

  //Set FontScale to 100 to disable scaling
  int FontScale = Property.FontScale;
  if(FontScale != 100)
  {
    Form->ScaleBy(FontScale, 100);
    ScaleComponent(Form, FontScale);
  }
}
//---------------------------------------------------------------------------
void FlipForm(TForm *Form)
{
  if(SysLocale.MiddleEast)
  {
    Form->FlipChildren(true);
    FlipAnchors(Form);
    FlipMargins(Form);
    Form->ParentBiDiMode = true;
  }
}
//---------------------------------------------------------------------------
void ResizeControl(TControl *Control, int Left)
{
  int Width = Control->Width - (Left - Control->Left);
  Control->Left = Left;
  Control->Width = Width;
}
//---------------------------------------------------------------------------
void ResizeControl(TControl *Control, TControl *Control1, TControl *Control2, TControl *Control3)
{
  int MaxRight = Control1->Left + Control1->Width;
  if(Control2) MaxRight = std::max(MaxRight, Control2->Left + Control2->Width);
  if(Control3) MaxRight = std::max(MaxRight, Control3->Left + Control3->Width);
  ResizeControl(Control, MaxRight + 5);
}
//---------------------------------------------------------------------------
void MoveControl(TControl *Control, TCustomLabel *Label)
{
  Control->Left = Label->Left + Label->Width + 5;
}
//---------------------------------------------------------------------------
void MoveLabel(TControl *Control, TCustomLabel *Label)
{
  Label->Left = Control->Left - Label->Width - 5;
}
//---------------------------------------------------------------------------
void AddAccelerator(TControl *Control, std::set<wchar_t> &Accelerators)
{
  //Don't add accelerator to OK buttons, Cancel buttons or Help buttons
  if(TButton *Button = dynamic_cast<TButton*>(Control))
    if(Button->Default || Button->Cancel || Button->Caption == _("Help"))
   return;

  //Only add accelerators to labels if FocusCotrol is set
  //Dont add to TEdit
  if(TLabel *Label = dynamic_cast<TLabel*>(Control))
    if(Label->FocusControl == NULL)
      return;
  if(dynamic_cast<TCustomEdit*>(Control) != NULL || dynamic_cast<TCustomComboBox*>(Control) != NULL)
    return;

  String Caption = GetControlText(Control);
  String LowerCaption = LowerCase(Caption);

  for(int I = 1; I <= LowerCaption.Length(); I++)
  {
    wchar_t Ch = LowerCaption[I];
    if(Accelerators.count(Ch))
    {
      Accelerators.erase(Ch);
      Caption.Insert(L'&', I);
      SetControlText(Control, Caption);
      break;
    }
  }
}
//---------------------------------------------------------------------------
void SetAccelerators(TWinControl *WinControl, std::set<wchar_t> &Accelerators)
{
  std::vector<TWinControl*> PostProcess;

  //Now loop through controls with children
  for(int I = 0; I < WinControl->ControlCount; I++)
  {
    TControl *Control = WinControl->Controls[I];
    TWinControl *WinControl2 = dynamic_cast<TWinControl*>(Control);

    if(WinControl2 && WinControl2->ControlCount)
    {
      if(dynamic_cast<TPageControl*>(WinControl2) ||
         dynamic_cast<TFrame*>(WinControl2))
        PostProcess.push_back(WinControl2);
      else
        SetAccelerators(WinControl2, Accelerators);
    }
    else
      AddAccelerator(Control, Accelerators);
  }

  for(unsigned I = 0; I < PostProcess.size(); I++)
    for(int J = 0; J < PostProcess[I]->ControlCount; J++)
      if(TWinControl *WinControl2 = dynamic_cast<TWinControl*>(PostProcess[I]->Controls[J]))
      {
        //Don't update Accelerators from page controls
        std::set<wchar_t> Temp(Accelerators);
        SetAccelerators(WinControl2, Temp);
      }
}
//---------------------------------------------------------------------------
void SetAccelerators(TWinControl *Control)
{
  const wchar_t Accelerators[] = L"abcdefghijklmnopqrstuvwxyz0123456789";
  std::set<wchar_t> Temp(Accelerators, Accelerators + sizeof(Accelerators) - 1);
  SetAccelerators(Control, Temp);
}
//---------------------------------------------------------------------------
String GetControlText(TControl *Control)
{
  std::vector<wchar_t> Str(Control->GetTextLen() + 1);
  if(Str.empty())
    return String();
  Control->GetTextBuf(&Str[0], Str.size());
  return &Str[0];
}
//---------------------------------------------------------------------------
void SetControlText(TControl *Control, const String &Str)
{
  Control->SetTextBuf(Str.c_str());
}
//---------------------------------------------------------------------------
void TranslateStrings(TStrings *Strings)
{
  for(int I = 0; I < Strings->Count; I++)
    Strings->Strings[I] = gettext(Strings->Strings[I]);
}
//---------------------------------------------------------------------------
String LoadRes(short Ident, String Par1, String Par2, String Par3)
{
  return ToUString(LoadRes(Ident, ToWString(Par1), ToWString(Par2), ToWString(Par3)));
}
//---------------------------------------------------------------------------
std::wstring LoadRes(short Ident, std::wstring Par1, std::wstring Par2, const std::wstring &Par3)
{
  std::wstring ResStr = LoadString(Ident);
  if(ResStr.empty())
  {
    MessageBox(L"Resource " + ToWString(Ident) + L" not found!", L"Error", MB_ICONSTOP);
    return L"";
  }
	if(Par1 != L"&") //An & might be found in an expression
		Par1.erase(std::remove(Par1.begin(), Par1.end(), L'&'));
	Par2.erase(std::remove(Par2.begin(), Par2.end(), L'&'));

	if(!Par1.empty() && Par1[Par1.size()-1] == L':')
    Par1.erase(Par1.end()-1);

  boost::wformat fmter(ResStr);
  fmter.exceptions(boost::io::all_error_bits ^ boost::io::too_many_args_bit);
  return (fmter % Par1 % Par2 % Par3).str();
}
//---------------------------------------------------------------------------
String FormatStr(const String &Format, const String &Str)
{
  boost::wformat fmter(ToWString(Format));
  fmter.exceptions(boost::io::all_error_bits ^ boost::io::too_many_args_bit);
  return ToUString((fmter % Str.c_str()).str());
}
//---------------------------------------------------------------------------
std::wstring LoadString(unsigned Ident)
{
  return LoadStr(Ident).c_str();
}
//---------------------------------------------------------------------------
int MessageBox(const wchar_t *Text, const wchar_t *Caption, int Flags)
{
  return Application->MessageBox(Text, Caption, Flags);
}
//---------------------------------------------------------------------------
int MessageBox(const String &Text, const String &Caption, int Flags)
{
  return Application->MessageBox(Text.c_str(), Caption.c_str(), Flags);
}
//---------------------------------------------------------------------------
int MessageBox(const std::wstring &Text, const std::wstring &Caption, int Flags)
{
  return MessageBox(Text.c_str(), Caption.c_str(), Flags);
}
//---------------------------------------------------------------------------
class THelpButtonHelper
{
  TWndMethod OldWindowProc;
  int ErrorCode;
public:
	THelpButtonHelper(int AErrorCode);
	~THelpButtonHelper()
	{
		if(Screen->ActiveForm)
			Screen->ActiveForm->WindowProc = OldWindowProc;
	}
	void __fastcall WindowProc(TMessage &Message)
	{
		if(Message.Msg == WM_HELP && ErrorCode)
			ShowHelp(String().sprintf(L"Errors.html#Error%02d", ErrorCode));
		OldWindowProc(Message);
	}
};
//---------------------------------------------------------------------------
//Constructor may not be inlined. It will hit a compiler bug in C++ Builder XE
THelpButtonHelper::THelpButtonHelper(int AErrorCode) : OldWindowProc(NULL), ErrorCode(AErrorCode)
{
	if(Screen->ActiveForm)
	{
		OldWindowProc = Screen->ActiveForm->WindowProc;
		Screen->ActiveForm->WindowProc = &WindowProc;
	}
}
//---------------------------------------------------------------------------
//Shows error message corresponding to ErrorCode in Func
//If Edit parameter is suported the Edit box gets focus and
//the cursor position is set to where the error ocoured
void ShowErrorMsg(const Func32::EFuncError &Error, TCustomEdit *Edit)
{
  String Str = LoadRes(RES_ERROR_NUMBER, Error.ErrorCode);
  THelpButtonHelper HelpButtonHelper(Error.ErrorCode);
  MessageBox(GetErrorMsg(Error), Str, MB_ICONWARNING | MB_HELP);

  if(Edit)
  {
    SetGlobalFocus(Edit);
    if(const Func32::EParseError *ParseError = dynamic_cast<const Func32::EParseError*>(&Error))
      Edit->SelStart = ParseError->ErrorPos;
  }
}
//---------------------------------------------------------------------------
void ShowErrorMsg(const ECustomFunctionError &Error, TCustomEdit *Edit)
{
  MessageBox(LoadRes(Error.ErrorCode + 200, ToUString(Error.Text)), LoadRes(RES_ERROR), MB_ICONWARNING);
  if(Edit)
  {
    SetGlobalFocus(Edit);
    Edit->SelStart = Error.ErrorPos;
  }
}
//---------------------------------------------------------------------------
void ShowErrorMsg(const EGraphError &Error, TCustomEdit *Edit)
{
  MessageBox(Error.Message, LoadString(RES_ERROR), MB_ICONWARNING);
  if(Edit)
    SetGlobalFocus(Edit);
}
//---------------------------------------------------------------------------
void ShowErrorMsg(const std::exception &Error, TCustomEdit *Edit)
{
  if(const EGraphError *E = dynamic_cast<const EGraphError*>(&Error))
    ShowErrorMsg(*E);
  else if(const Func32::EFuncError *E = dynamic_cast<const Func32::EFuncError*>(&Error))
    ShowErrorMsg(*E);
  else if(const ECustomFunctionError *E = dynamic_cast<const ECustomFunctionError*>(&Error))
    ShowErrorMsg(*E);
  else
    MessageBox(Error.what(), LoadRes(RES_ERROR), MB_ICONWARNING);
}
//---------------------------------------------------------------------------
void ShowStatusError(const String &Str)
{
  Form1->ShowStatusError(Str);
}
//---------------------------------------------------------------------------
class TInputQueryForm : public TForm
{
  TEdit *Edit;
  TLabel *Label;
  TButton *Button1;
  TButton *Button2;
  int &Value;

public:
  __fastcall TInputQueryForm(const String &ACaption, const String &APrompt, int &AValue)
    : TForm(NULL, 0), Value(AValue)
  {
    Caption = ACaption;
    BorderStyle = bsDialog;
    Position = poMainFormCenter;
    ClientHeight = 70;

    Label = new TLabel(this);
    Label->Caption = APrompt;
    Label->Parent = this;
    Label->Left = 8;
    Label->Top = 8;

    Edit = new TEdit(this);
    Edit->Text = Value;
    Edit->Parent = this;
    Edit->Left = Label->Left + Label->Width + 5;
    Edit->Top = 8;
    Edit->Width = std::max(180 - Edit->Left - 10, 50);
    Width = Edit->Left + Edit->Width + 10;

    Button1 = new TButton(this);
    Button1->Parent = this;
    Button1->Caption = _("OK");
    Button1->Left = Width / 2 - Button1->Width - 5;
    Button1->Top = 38;
    Button1->Default = true;
    Button1->OnClick = InputQueryClick;

    Button2 = new TButton(this);
    Button2->Parent = this;
    Button2->Caption = _("Cancel");
    Button2->Left = 50;
    Button2->Left = Width / 2 + 5;
    Button2->Top = 38;
    Button2->Cancel = true;
    Button2->ModalResult = mrCancel;
  }

  void __fastcall InputQueryClick(TObject *Sender)
  {
    Value = Edit->Text.ToInt(); //Verify that a valid integer is entered
    ModalResult = mrOk;
  }
};
//---------------------------------------------------------------------------
bool InputQuery(const String &Caption, const String &Prompt, int &Value)
{
  std::auto_ptr<TForm> Form(new TInputQueryForm(Caption, Prompt, Value));
  return Form->ShowModal() == mrOk;
}
//---------------------------------------------------------------------------
bool GetKeyState(Classes__1 Key)
{
  switch(Key)
  {
    case ssShift:
      return GetKeyState(VK_SHIFT) & 0x8000;
  }
  return false;
}
//---------------------------------------------------------------------------
String GetKeyName(UINT Key)
{
  UINT ScanCode = MapVirtualKey(Key, 0) << 16;
  if(ScanCode != 0)
  {
    if(Key > VK_ESCAPE && Key < 'A' /*VK_A*/) //Not sure about this range
      ScanCode |= 0x01000000;
    wchar_t KeyName[20];
    GetKeyNameText(ScanCode, KeyName, sizeof(KeyName) / sizeof(KeyName[0]));
    CharLower(&KeyName[1]);
    return String(KeyName);
  }
  return String();
}
//---------------------------------------------------------------------------
namespace Menus
{
//Replaces the function ShortCutToText() in Menus.pas
//This function translates the key names instead of using hardcoded names
  String __fastcall ShortCutToText(TShortCut ShortCut)
  {
    String Str;
    if(ShortCut & scShift)
      Str += LoadStr(RES_KEY_SHIFT) + "+";
    if(ShortCut & scCtrl)
      Str += LoadStr(RES_KEY_CTRL) + "+";
    if(ShortCut & scAlt)
      Str += LoadStr(RES_KEY_ALT) + "+";

    switch(ShortCut & 0xFF)
    {
      case VK_DELETE: Str += LoadStr(RES_KEY_DEL); break;
      case VK_INSERT: Str += LoadStr(RES_KEY_INS); break;
      case VK_HOME:   Str += LoadStr(RES_KEY_HOME); break;
      case VK_PRIOR:  Str += LoadStr(RES_KEY_PGUP); break;
      case VK_NEXT:   Str += LoadStr(RES_KEY_PGDN); break;
      case VK_END:    Str += LoadStr(RES_KEY_END); break;
      case VK_ESCAPE: Str += LoadStr(RES_KEY_ESC); break;
      case VK_RETURN: Str += LoadStr(RES_KEY_ENTER); break;
      case VK_SPACE:  Str += LoadStr(RES_KEY_SPACE); break;
      case VK_BACK:   Str += LoadStr(RES_KEY_BKSP); break;
      case VK_TAB:    Str += LoadStr(RES_KEY_TAB); break;
      case VK_LEFT:   Str += LoadStr(RES_KEY_LEFT); break;
      case VK_RIGHT:  Str += LoadStr(RES_KEY_RIGHT); break;
      case VK_UP:     Str += LoadStr(RES_KEY_UP); break;
      case VK_DOWN:   Str += LoadStr(RES_KEY_DOWN); break;
      default:
        Str += GetKeyName(ShortCut & 0xFF);
    }
    return Str;
  }
}
//---------------------------------------------------------------------------
namespace Windows
{
  //Missing declaration in Windows.hpp
  const unsigned HH_DISPLAY_TOPIC      = 0;
  extern PACKAGE HWND __fastcall HtmlHelp(HWND hWndCaller, System::WideChar * pszFile, unsigned uCommand, unsigned dwData);
}
void ShowHelp(const String &File, const String &HelpFile)
{
  //Workaround for bug in THtmlHelpViewer, which only support the .htm extension
  String Str = (HelpFile.IsEmpty() ? Application->HelpFile : HelpFile) + "::/" + File;
  Windows::HtmlHelp(NULL, Str.c_str(), Windows::HH_DISPLAY_TOPIC, 0);
}
//---------------------------------------------------------------------------

