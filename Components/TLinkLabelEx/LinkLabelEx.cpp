//===========================================================================
// Copyright © 2009 Ivan Johansen
// LinkLabelEx.cpp
//===========================================================================
#include <vcl.h>
#pragma hdrstop
#include "LinkLabelEx.h"
#include <Themes.hpp>
#include "ICompCommon.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
static TRegisterClass Dummy(__classid(TLinkLabelEx));
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
static inline void ValidCtrCheck(TLinkLabelEx *)
{
  new TLinkLabelEx(NULL);
}
//---------------------------------------------------------------------------
namespace Linklabelex
{
  void __fastcall PACKAGE Register()
  {
     TComponentClass classes[1] = {__classid(TLinkLabelEx)};
     RegisterComponents(L"IComp", classes, 0);
  }
}
//---------------------------------------------------------------------------
__fastcall TLinkLabelEx::TLinkLabelEx(TComponent* Owner)
  : TWinControl(Owner), FAutoSize(true), FWordWrap(false), FOnLinkClick(NULL)
{
  ParentDoubleBuffered = false;
  Width = 53;
  Height = 17;
}
//---------------------------------------------------------------------------
bool IsLinkLabelSupported()
{
  static bool ManifestResourceTested = false;
  static bool ManifestResourcePresent;
  if(!ManifestResourceTested)
  {
    ManifestResourcePresent = FindResource(MainInstance, CREATEPROCESS_MANIFEST_RESOURCE_ID, RT_MANIFEST) != 0;
    ManifestResourceTested = True;
  }
  return ManifestResourcePresent && ThemeServices()->ThemesAvailable;
}
//---------------------------------------------------------------------------
void TLinkLabelEx::AdjustBounds()
{
  if(!ComponentState.Contains(csLoading) && AutoSize)
  {
    if(CheckWin32Version(6) && IsLinkLabelSupported())
    {
      if(HandleAllocated())
      {
        TSize TextSize;
        SendMessage(Handle, LM_GETIDEALSIZE, WordWrap ? Width : MaxInt, reinterpret_cast<LONG>(&TextSize));
        SetBounds(Left, Top, WordWrap ? Width : TextSize.cx, WordWrap ? TextSize.cy : Height);
      }
    }
    else
    {
      HDC DC = GetDC(0);
      try
      {
        HFONT SaveFont = reinterpret_cast<HFONT>(SelectObject(DC, Font->Handle));
        try
        {
          String Parsed = ParseLinks();
          if(FWordWrap)
          {
            TRect Rect(0, 0, Width, Height);
            DrawText(DC, Parsed.c_str(), Parsed.Length(), &Rect, DT_WORDBREAK | DT_NOCLIP | DT_CALCRECT | DT_NOPREFIX);
            Height = Rect.Height();
          }
          else
          {
            TSize TextSize;
            GetTextExtentPoint32(DC, Parsed.c_str(), Parsed.Length(), &TextSize);
            SetBounds(Left, Top, TextSize.cx, TextSize.cy);
          }
        }
        __finally
        {
          SelectObject(DC, SaveFont);
        }
      }
      __finally
      {
        ReleaseDC(0, DC);
      }
    }
  }
}
//---------------------------------------------------------------------------
void TLinkLabelEx::DoOnLinkClick(const String &Link, TSysLinkType LinkType)
{
  if(FOnLinkClick)
    FOnLinkClick(this, Link, LinkType);
}
//---------------------------------------------------------------------------
String TLinkLabelEx::ParseLinks()
{
  enum TParseSearchMode {psmLinkStart, psmLinkEnd};

  if(Caption.IsEmpty())
    return "";

  String Result;
  Result.SetLength(Caption.Length());
  int Pos = 1;
  TParseSearchMode SearchMode = psmLinkStart;
  int Len = 0;
  int Last = Caption.Length();

  while(Pos <= Last)
  {
    if(Caption[Pos] == '<') //do not localize
    {
      int LookAhead = Pos;
      if(Pos == Last)
      {
        Len++;
        Result[Len] = Caption[Pos];
        break;
      }

      LookAhead++;
      if(SearchMode == psmLinkStart)
      {
        if(Caption[LookAhead] == 'a' || Caption[LookAhead] == 'A') //do not localize
        {
          while(Caption[LookAhead] != '>' && LookAhead <= Last) //do not localize
            LookAhead++;

          if(LookAhead > Last)
          {
            for(int I = 0; I < LookAhead - Pos; I++)
              Result[Len + 1 + I] = Caption[Pos + I];
            Len += LookAhead - Pos;
            break;
          }
          else //LookAhead > Last
          {
            Pos = LookAhead + 1;
            SearchMode = psmLinkEnd;
          }
        }
        else //CharInSet(LookAhead^, ['a', 'A'])
        {
          Len++;
          Result[Len] = Caption[Pos];
          Pos++;
        }
      }
      else //SearchMode = psmLinkStart
      {
        //A bit of an ugly way to do this, but its fast
        if(Caption[LookAhead] == '/' && //do not localize
          (Caption[LookAhead + 1] == 'a' || Caption[LookAhead + 1] == 'A') && //do not localize
           Caption[LookAhead + 2] == '>') //do not localize
        {
          Pos += 4;
          SearchMode = psmLinkStart;
        }
        else
        {
          Len++;
          Result[Len] = Caption[Pos];
          Pos++;
        }
      }
    }
    else
    {
      Len++;
      Result[Len] = Caption[Pos];
      Pos++;
    }
  }

  Result.SetLength(Len);
  return Result;
}
//---------------------------------------------------------------------------
void TLinkLabelEx::CMFontChanged(TMessage &Message)
{
  TWinControl::Dispatch(&Message);
  AdjustBounds();
}
//---------------------------------------------------------------------------
void TLinkLabelEx::CMTextChanged(TMessage &Message)
{
  TWinControl::Dispatch(&Message);
  AdjustBounds();
  Invalidate();
  if(!CheckWin32Version(5, 1) || !IsLinkLabelSupported())
  {
    String Parsed = ParseLinks();
    if(Parsed != Caption)
      Caption = Parsed;
  }
}
//---------------------------------------------------------------------------
void TLinkLabelEx::CMNotify(TWMNotify &Message)
{
  switch(Message.NMHdr->code)
  {
    case NM_CLICK:
    case NM_RETURN:
    {
      TLItem &Item = reinterpret_cast<TNMLink*>(Message.NMHdr)->item;
      if(Item.szID[0] == 0)
        DoOnLinkClick(String(Item.szUrl), sltURL);
      else
        DoOnLinkClick(String(Item.szID), sltID);
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TLinkLabelEx::CreateParams(TCreateParams &Params)
{
  TWinControl::CreateParams(Params);
  if(CheckWin32Version(5, 1) && IsLinkLabelSupported())
    CreateSubClass(Params, WC_LINK);
  else
    CreateSubClass(Params, L"STATIC");
  Params.WindowClass.style &= ~CS_VREDRAW;
}
//---------------------------------------------------------------------------
void __fastcall TLinkLabelEx::CreateWnd()
{
  TWinControl::CreateWnd();
  AdjustBounds();
}
//---------------------------------------------------------------------------
void __fastcall TLinkLabelEx::SetAutoSize(bool Value)
{
  if(Value != FAutoSize)
  {
    FAutoSize = Value;
    AdjustBounds();
  }
}
//---------------------------------------------------------------------------
void __fastcall TLinkLabelEx::SetWordWrap(bool Value)
{
  if(Value != FWordWrap)
  {
    FWordWrap = Value;
    AdjustBounds();
  }
}
//---------------------------------------------------------------------------

