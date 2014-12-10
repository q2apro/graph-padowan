//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "ICompCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
static TTranslateFunc TranslateFunc = NULL;

String TranslateCompText(const String &Str)
{
  if(TranslateFunc)
    return TranslateFunc(Str);
  return Str;
}
//---------------------------------------------------------------------------
void SetCompTranslateFunc(TTranslateFunc Func)
{
  TranslateFunc = Func;
}
//---------------------------------------------------------------------------
std::string Trim(const std::string &Str)
{
  size_t Pos1 = Str.find_first_not_of(" ");
  size_t Pos2 = Str.find_last_not_of(" ");
  if(Pos1 == std::string::npos)
    return "";
  if(!Str.empty() && (Pos1 != 0 || Pos2 != Str.size()-1))
    return Str.substr(Pos1, Pos2 - Pos1 + 1);
  return Str;
}
//--------------------------------------------------------------------------
std::wstring Trim(const std::wstring &Str)
{
  unsigned Pos1 = Str.find_first_not_of(L" ");
  unsigned Pos2 = Str.find_last_not_of(L" ");
  if(!Str.empty() && (Pos1 != 0 || Pos2 != Str.size()-1))
    return Str.substr(Pos1, Pos2 - Pos1 + 1);
  return Str;
}
//--------------------------------------------------------------------------

