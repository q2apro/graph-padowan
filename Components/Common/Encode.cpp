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
#include "Encode.h"
#include <algorithm>
//---------------------------------------------------------------------------
const char *Base64Table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

std::string EncodeEscapeSequence(const std::string &Str)
{
  std::string Dest;
  Dest.reserve(Str.size());
  for(unsigned I = 0; I < Str.size(); I++)
    switch(Str[I])
    {
      case '\\': Dest += "\\\\"; break;
      case '\r': break;
      case '\n': Dest += "\\n"; break;
      case '\t': Dest += "\\t"; break;
      default:
        Dest += Str[I];
    }
  return Dest;
}
//---------------------------------------------------------------------------
//This must be used to decode escape sequences to make sure a string like
//"\\\\n" is decode corectly to "\\n" and not "\n"
std::string DecodeEscapeSequence(const std::string &Str)
{
  std::string Dest;
  Dest.reserve(Str.size());
  for(unsigned I = 0; I < Str.size(); I++)
    if(Str[I] == '\\')
    {
      if(++I < Str.size())
        switch(Str[I])
        {
          case '\\': Dest += '\\';   break;
          case 'n':  Dest += "\r\n"; break;
          case 't':  Dest += '\t';   break;
        }
    }
    else
      Dest += Str[I];
  return Dest;
}
//---------------------------------------------------------------------------
//Encode binary data as base 64
std::string Base64Encode(const void *Buffer, unsigned Size)
{
  std::string Str;
  Str.reserve((Size * 4 + 2) / 3);
  const unsigned char *Data = static_cast<const unsigned char*>(Buffer);

  while(Size >= 3)
  {
    //11111122 22223333 33444444
    Str += Base64Table[Data[0] >> 2];
    Str += Base64Table[((Data[0] << 4) | (Data[1] >> 4)) & 0x3F];
    Str += Base64Table[((Data[1] << 2) | (Data[2] >> 6)) & 0x3F];
    Str += Base64Table[Data[2] & 0x3F];
    Size -=3;
    Data += 3;
  }

  if(Size == 1)
  {
    Str += Base64Table[Data[0] >> 2];
    Str += Base64Table[(Data[0] << 4) & 0x3F];
    Str += "==";
  }
  else if(Size == 2)
  {
    Str += Base64Table[Data[0] >> 2];
    Str += Base64Table[((Data[0] << 4) | (Data[1] >> 4)) & 0x3F];
    Str += Base64Table[(Data[1] << 2) & 0x3F];
    Str += '=';
  }

  return Str;
}
//---------------------------------------------------------------------------
unsigned char Base64Index(char Ch)
{
  if(Ch == '=')
    return 0;
  const char *Iter = std::find(Base64Table, Base64Table+64, Ch);
  if(Iter == Base64Table+64)
    throw EInvalidBase64Char();
  return Iter - Base64Table;
}
//---------------------------------------------------------------------------
//Decode a base 64 encoded string. The size of Buffer must be at least (Str.size()*3)/4.
//Returns the actual number of written bytes
unsigned Base64Decode(const std::string &Str, void *Buffer)
{
  unsigned char *Data = static_cast<unsigned char*>(Buffer);
  unsigned Size = Str.size();
  unsigned Index = 0;
  for(unsigned I = 0; I < Size; I += 4)
  {
    //11111122 22223333 33444444
    unsigned char Byte1 = Base64Index(Str[I]);
    unsigned char Byte2 = Base64Index(Str[I+1]);
    unsigned char Byte3 = Base64Index(Str[I+2]);
    unsigned char Byte4 = Base64Index(Str[I+3]);

    Data[Index++] = (Byte1 << 2) | (Byte2 >> 4);
    if(Str[I+1] != '=')
      Data[Index++] = (Byte2 << 4) | (Byte3 >> 2);
    if(Str[I+2] != '=')
      Data[Index++] = (Byte3 << 6) | Byte4;
  }
  return Index;
}
//---------------------------------------------------------------------------

