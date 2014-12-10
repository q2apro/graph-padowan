//===========================================================================
// Copyright © 2003 Ivan Johansen
// MyEdit.cpp
//===========================================================================
#include <vcl.h>
#pragma hdrstop
#include "MyEdit.h"
#include "ICompCommon.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
static TRegisterClass Dummy(__classid(TMyEdit));

// Map latin characters to Greek characters the same way they are located on a
// Greek keyboard
wchar_t Greek[26][2] =
{
/* Aa */ L'Α', L'α',
/* Bb */ L'Β', L'β',
/* Cc */ L'Ψ', L'ψ',
/* Dd */ L'Δ', L'δ',
/* Ee */ L'Ε', L'ε',
/* Ff */ L'Φ', L'φ',
/* Gg */ L'Γ', L'γ',
/* Hh */ L'Η', L'η',
/* Ii */ L'Ι', L'ι',
/* Jj */ L'Ξ', L'ξ',
/* Kk */ L'Κ', L'κ',
/* Ll */ L'Λ', L'λ',
/* Mm */ L'Μ', L'μ',
/* Nn */ L'Ν', L'ν',
/* Oo */ L'Ο', L'ο',
/* Pp */ L'Π', L'π',
/* Qq */ L':', L';',
/* Rr */ L'Ρ', L'ρ',
/* Ss */ L'Σ', L'σ',
/* Tt */ L'Τ', L'τ',
/* Uu */ L'Θ', L'θ',
/* Vv */ L'Ω', L'ω',
/* Ww */ L'΅', L'ς',
/* Xx */ L'Χ', L'χ',
/* Yy */ L'Υ', L'υ',
/* Zz */ L'Ζ', L'ζ',
};

// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//
static inline void ValidCtrCheck(TMyEdit *)
{
        new TMyEdit(NULL);
}
//---------------------------------------------------------------------------
namespace Myedit
{
        void __fastcall PACKAGE Register()
        {
                 TComponentClass classes[1] = {__classid(TMyEdit)};
                 RegisterComponents("IComp", classes, 0);
        }
}
//---------------------------------------------------------------------------
__fastcall TMyEdit::TMyEdit(TComponent* Owner)
		: TEdit(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TMyEdit::KeyPress(Char &Key)
{
  if(Key == 0x07) //Ctrl+G
  {
    if(SelLength == 0)
    {
      if(SelStart > 0)
      {
        SelStart = SelStart - 1;
        SelLength = 1;
        SelText = ConvertToGreek(SelText[1]);
        SelStart = SelStart + 1;
        SelLength = 0;
      }
    }
    else
      SelText = ConvertToGreek(SelText);
    Key = 0;
  }
  else if(Key == L'\xB2') //Superscript 2
  {
    SendMessage(Handle, WM_CHAR, '^', 0);
    Key = '2';
  }
  else
    TEdit::KeyPress(Key);
}
//---------------------------------------------------------------------------
void __fastcall TMyEdit::WMDeadChar(TMessage &Message)
{
  //I am not sure why this works, but it does
  if(Message.WParam == '^')
    PostMessage(Handle, WM_KEYUP, VK_SPACE, 0);
}
//---------------------------------------------------------------------------
wchar_t TMyEdit::ConvertToGreek(wchar_t Ch)
{
  if(Ch >= L'a' && Ch <= L'z')
    return Greek[Ch - L'a'][1];
  if(Ch >= L'A' && Ch <= L'Z')
    return Greek[Ch - L'A'][0];
  return Ch;
}
//---------------------------------------------------------------------------
String TMyEdit::ConvertToGreek(String Str)
{
  for(int I = 1; I <= Str.Length(); I++)
    Str[I] = ConvertToGreek(Str[I]);
  return Str;
}
//---------------------------------------------------------------------------

