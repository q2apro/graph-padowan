/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2007 Ivan Johansen
 *
 * Graph is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
#ifndef PyVclFunctionH
#define PyVclFunctionH
//---------------------------------------------------------------------------
namespace Python
{
	extern PyTypeObject VclFunctionType;
	PyObject* VclFunction_Create(const String &Name);
	unsigned GetVclFunctionCount();
	const wchar_t* GetVclFunctionName(unsigned Index);
}
//---------------------------------------------------------------------------
#endif
