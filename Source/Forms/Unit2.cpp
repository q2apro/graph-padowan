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
#include "Unit2.h"
#include "VersionInfo.h"
//---------------------------------------------------------------------------
#pragma link "SymbolDialog"
#pragma link "LinkLabelEx"
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TForm2::TForm2(TComponent* Owner)
	: TForm(Owner)
{
  String TranslatorString = LinkLabel3->Caption;
  TranslateProperties(this);
  ScaleForm(this);
  FlipForm(this);

  TVersionInfo Info;
  //Make string with file version information
  if(Info.InfoAvailable())
  {
    TVersion FileVersion = Info.FileVersion();
    Label3->Caption = LoadRes(RES_BUILD, FileVersion.Build);
    FileVersion.Build = 0;
    Version->Caption = ToUString(LoadRes(RES_VERSION, FileVersion.Text()));
    if(Info.FileFlags() & ffDebug)
      Version->Caption = Version->Caption + " Beta";
    Copyright->Caption = Info.StringValue(L"LegalCopyright").c_str();
  }

  const wchar_t *SupportStr = L"<a href=\"" SUPPORT_PAGE L"\">" SUPPORT_PAGE L"</a>";
  LinkLabel2->Caption = FormatStr(LinkLabel2->Caption, SupportStr);
  const wchar_t *LinkStr = L"<a href=\"" HOMEPAGE L"\">" HOMEPAGE L"</a>";
  LinkLabel1->Caption = FormatStr(LinkLabel1->Caption, LinkStr);
  if(LinkLabel3->Caption != TranslatorString)
    LinkLabel3->Visible = true;
  Comments->Caption = FormatStr(Comments->Caption, "http://www.gnu.org/licenses/old-licenses/gpl-2.0.html");
}
//---------------------------------------------------------------------------
void __fastcall TForm2::ProgramIconDblClick(TObject *Sender)
{
  Image1->Visible = !Image1->Visible;
  ProgramIcon->Visible = !ProgramIcon->Visible;
  if(TGIFImage *Gif = dynamic_cast<TGIFImage*>(Image1->Picture->Graphic))
  {
    Gif->AnimateLoop = glContinously;
    Gif->Animate = true;
    Gif->AnimationSpeed = 400;
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm2::LinkLabel1LinkClick(TObject *Sender, const UnicodeString Link,
          TSysLinkType LinkType)
{
  ShellExecute(Handle, NULL, Link.c_str(), NULL, NULL, SW_SHOWMAXIMIZED);
}
//---------------------------------------------------------------------------
void __fastcall TForm2::LinkLabel2LinkClick(TObject *Sender, const UnicodeString Link,
          TSysLinkType LinkType)
{
  ShellExecute(Handle, NULL, Link.c_str(), NULL, NULL, SW_SHOWDEFAULT);
}
//---------------------------------------------------------------------------
void __fastcall TForm2::FormShow(TObject *Sender)
{
  //Must be done in FormShow() under Windows Vista because a handle is needed to auto size a TLinkLabelEx
  if(LinkLabel3->Visible)
    ClientHeight = ClientHeight + LinkLabel3->Height + 10;
}
//---------------------------------------------------------------------------

