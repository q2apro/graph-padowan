/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2007 Ivan Johansen
 *
 * Graph is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
#include "Config.h"
#pragma hdrstop
#include "Tokenizer.h"
//---------------------------------------------------------------------------
void TTokenizer::Extract(TTokenString &S)
{
  TTokenString::const_iterator Start = Iter;
  TTokenString::const_iterator End = Str.end();

  if(Iter == End)
  {
    FFailed = true;
    return;
  }

  if(FQuote && *Iter == FQuote)
  {
    ++Start;
    while(++Iter != End && *Iter != FQuote);
    if(Iter == End)
    {
      FFailed = true;
      return;
    }
    S = TTokenString(Start, Iter++);
    NextDelimiter = FDelimiter;
    if(Iter != End && *(++Iter) != FQuote)
      FFailed = true;
    return;
  }

  while(++Iter != End)
  {
    if(*Iter == NextDelimiter)
    {
      S = TTokenString(Start, Iter++);
      NextDelimiter = FDelimiter;
      return;
    }
  }
  S = TTokenString(Start, End);
  NextDelimiter = FDelimiter;
}
//---------------------------------------------------------------------------
//Ignore the next Count tokens
void TTokenizer::Ignore(unsigned Count)
{
  TTokenString Temp;
  while(Count--)
    Extract(Temp);
}
//---------------------------------------------------------------------------

