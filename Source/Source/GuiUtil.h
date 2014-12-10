/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2007 Ivan Johansen
 *
 * Graph is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
#ifndef GuiUtilH
#define GuiUtilH
//---------------------------------------------------------------------------
namespace Tntextctrls
{
  class TTntRadioGroup;
}

namespace Tntclasses
{
  class TTntStrings;
}

void DrawComboBoxEndPoint(TCanvas *Canvas, int Index, const TRect &Rect);

void ResizeControl(TControl *Control, int Left);
void ResizeControl(TControl *Control, TControl *Control1, TControl *Control2 = NULL, TControl *Control3 = NULL);
void MoveLabel(TControl *Control, TCustomLabel *Label);
void MoveControl(TControl *Control, TCustomLabel *Label);

void SetAccelerators(TWinControl *Control);

String GetControlText(TControl *Control);
void SetControlText(TControl *Control, const String &Str);

void ScaleComponent(TComponent *Component, unsigned Scale);
void ScaleForm(TForm *Form);
void FlipForm(TForm *Form);

void TranslateStrings(TStrings *Strings);

String LoadRes(short Ident, String Par1 = String(), String Par2 = String(), String Par3 = String());
std::wstring LoadRes(short Ident, std::wstring Par1, std::wstring Par2 = std::wstring(), const std::wstring &Par3 = std::wstring());
std::wstring LoadString(unsigned Ident);
String FormatStr(const String &Format, const String &Str);

int MessageBox(const wchar_t *Text, const wchar_t *Caption, int Flags = MB_OK | MB_ICONWARNING);
int MessageBox(const std::wstring &Text, const std::wstring &Caption, int Flags = MB_OK | MB_ICONWARNING);
int MessageBox(const String &Text, const String &Caption, int Flags = MB_OK | MB_ICONWARNING);

void ShowErrorMsg(const Func32::EFuncError &Error, TCustomEdit *Edit=NULL);//Shows error message corresponding to ErrorCode in Error
void ShowErrorMsg(const class ECustomFunctionError &Error, TCustomEdit *Edit=NULL);
void ShowErrorMsg(const class EGraphError &Error, TCustomEdit *Edit=NULL);
void ShowErrorMsg(const std::exception &Error, TCustomEdit *Edit=NULL);
void ShowStatusError(const String &Str);

bool InputQuery(const String &Caption, const String &Prompt, int &Value);
bool GetKeyState(Classes__1 Key);
void ShowHelp(const String &File, const String &HelpFile=String());

#endif
