/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2007 Ivan Johansen
 *
 * Graph is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
#ifndef EncodeH
#define EncodeH
#include <string>
#include <vector>
//---------------------------------------------------------------------------
class EInvalidBase64Char : public std::exception
{
public:
  const char* what() const throw() {return "Invalid Base 64 character";}
};

std::string EncodeEscapeSequence(const std::string &Str);
std::string DecodeEscapeSequence(const std::string &Str);
std::string Base64Encode(const void *Buffer, unsigned Size);
unsigned Base64Decode(const std::string &Str, void *Buffer);
#endif
