/* Copyright 2007 Ivan Johansen
 *
 * Func32 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
#include "Func32.h"
#include "Func32Impl.h"
#pragma hdrstop
namespace Func32
{
//---------------------------------------------------------------------------
const TBaseCustomFuncPtr& TSymbolList::Add(const std::wstring &Key)
{
  std::wstring Name = ToLower(Key);
  if(!IsValidName(Name))
    throw EFuncError(ecIllegalName, Name);
  return List[Name];
}
//---------------------------------------------------------------------------
TCustomFuncPtr TSymbolList::Add(const std::wstring &Key, const std::wstring &Str, const std::vector<std::wstring> &Args)
{
  std::wstring Name = ToLower(Key);
  if(!IsValidName(Name))
    throw EFuncError(ecIllegalName, Name);
  TCustomFuncPtr Func(new TCustomFunc(Str, Args, *this));
  List[Name] = Func;
  return Func;
}
//---------------------------------------------------------------------------
void TSymbolList::Add(const std::wstring &Key, const TBaseCustomFuncPtr &CustomFunc)
{
  std::wstring Name = ToLower(Key);
  if(!IsValidName(Name))
    throw EFuncError(ecIllegalName, Key);
  List[Name] = CustomFunc;
}
//---------------------------------------------------------------------------
TCustomFuncPtr TSymbolList::Add(const std::wstring &Key, const TComplex &Value)
{
  std::wstring Name = ToLower(Key);
  if(!IsValidName(Name))
    throw EFuncError(ecIllegalName, Name);
  TCustomFuncPtr Func(new TCustomFunc(Value));
  List[Name] = Func;
  return Func;
}
//---------------------------------------------------------------------------
TBaseCustomFuncPtr TSymbolList::Get(const std::wstring &Key) const
{
  TConstIterator Iter = List.find(ToLower(Key));
  if(Iter == List.end())
    return TCustomFuncPtr();
  return Iter->second;
}
//---------------------------------------------------------------------------
bool TSymbolList::Exists(const std::wstring &Key) const
{
  return List.count(ToLower(Key));
}
//---------------------------------------------------------------------------
void TSymbolList::Erase(const std::wstring &Key)
{
  List.erase(ToLower(Key));
}
//---------------------------------------------------------------------------
void TSymbolList::Clear()
{
  List.clear();
}
//---------------------------------------------------------------------------
TSymbolList& TSymbolList::operator-=(const TSymbolList &SymbolList)
{
  for(TConstIterator Iter = SymbolList.List.begin(); Iter != SymbolList.List.end(); ++Iter)
    List.erase(Iter->first);
  return *this;
}
//---------------------------------------------------------------------------
} //namespace Func32


