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
#include "PostScript.h"
#include <ostream>
#include <cassert>

const double X_PAS = 28.3464566929134;
//---------------------------------------------------------------------------
TPostScript::TPostScript(std::ostream &AStream)
  : Stream(AStream)
{
  WriteInitData();
}
//---------------------------------------------------------------------------
TPostScript::TPostScript(const char *FileName)
  : Stream(FileStream), FileStream(FileName)
{
  assert(FileStream);
  WriteInitData();
}
//---------------------------------------------------------------------------
void TPostScript::WriteInitData()
{
  Stream << "%!PS-Adobe-2.0 EPSF-1.2\n";
  Stream << "%PLOT_PS V6.4\n";
  Stream << "%%Creator: Graph\n";
  Stream << "%%Title: Test\n";
  Stream << "%%Pages: (atend)\n";
  Stream << "%%BoundingBox: (atend)\n";
  Stream << "%%EndComments\n";
	Stream << "save\n";
	Stream << "/M { moveto } bind def\n";
	Stream << "/rM { rmoveto } bind def\n";
	Stream << "/L { lineto } bind def\n";
	Stream << "/rL { rlineto } bind def\n";
	Stream << "/S { show } bind def\n";
	Stream << "/GSG { gsave stroke grestore } bind def\n";
}
//---------------------------------------------------------------------------
void TPostScript::SetOrigin(double x, double y)
{
	xOrigin = x * X_PAS;
	yOrigin = y * X_PAS;
}
//---------------------------------------------------------------------------
void TPostScript::SetSize(double x, double y)
{
	xRes = x * X_PAS;
	yRes = y * X_PAS;
}
//---------------------------------------------------------------------------
void TPostScript::MoveTo(double x, double y)
{

}
//---------------------------------------------------------------------------
void TPostScript::LineTo(double x, double y)
{

}
//---------------------------------------------------------------------------
void TPostScript::TextOut(const char *Str)
{

}
//---------------------------------------------------------------------------

