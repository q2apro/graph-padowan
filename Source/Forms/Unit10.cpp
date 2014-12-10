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
#include "Unit10.h"
//---------------------------------------------------------------------------
#pragma link "LinkLabelEx"
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TForm10::TForm10(TComponent* Owner)
  : TForm(Owner)
{
  TranslateProperties(this);
  SetAccelerators(this);
  ScaleForm(this);

  //Take care of range problems
  if(Property.NextTip < RES_FIRST_TIP || Property.NextTip > RES_LAST_TIP)
    Property.NextTip = RES_FIRST_TIP;

  CheckBox1->Checked = Property.ShowTipsAtStartup;
  LinkLabel1->Caption = LoadRes(Property.NextTip);

  Width = Width - Button3->Width + Canvas->TextWidth(Button3->Caption) + 20;
  FlipForm(this);
}
//---------------------------------------------------------------------------
void __fastcall TForm10::Button2Click(TObject *Sender)
{
  if(++Property.NextTip > RES_LAST_TIP)
    Property.NextTip = RES_FIRST_TIP;
  LinkLabel1->Caption = LoadRes(Property.NextTip);
}
//---------------------------------------------------------------------------
void __fastcall TForm10::Button3Click(TObject *Sender)
{
  if(--Property.NextTip < RES_FIRST_TIP)
    Property.NextTip = RES_LAST_TIP;
  LinkLabel1->Caption = LoadRes(Property.NextTip);
}
//---------------------------------------------------------------------------
void __fastcall TForm10::FormClose(TObject *Sender, TCloseAction &Action)
{
  Property.ShowTipsAtStartup = CheckBox1->Checked;
  if(++Property.NextTip > RES_LAST_TIP)
    Property.NextTip = RES_FIRST_TIP;
}
//---------------------------------------------------------------------------
void __fastcall TForm10::LinkLabel1LinkClick(TObject *Sender, const UnicodeString Link,
          TSysLinkType LinkType)
{
  if(LinkType == sltURL)
    ShellExecute(Handle, NULL, Link.c_str(), NULL, NULL, SW_SHOWMAXIMIZED);
}
//---------------------------------------------------------------------------

