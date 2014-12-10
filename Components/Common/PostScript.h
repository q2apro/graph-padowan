/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2007 Ivan Johansen
 *
 * Graph is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
#ifndef PostScriptH
#define PostScriptH
#include <iosfwd>
#include <fstream>
//---------------------------------------------------------------------------
class TPostScript
{
  std::ostream &Stream;
  std::ofstream FileStream;
  double xOrigin, yOrigin;
  double xRes, yRes;

  void WriteInitData();

public:
  TPostScript(std::ostream &AStream);
  TPostScript(const char *FileName);
  void SetOrigin(double x, double y);
  void SetSize(double x, double y);
  void MoveTo(double x, double y);
  void LineTo(double x, double y);
  void TextOut(const char *Str);
};
#endif
