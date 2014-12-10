/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2007 Ivan Johansen
 *
 * Graph is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
#ifndef PyVclMethodH
#define PyVclMethodH
//---------------------------------------------------------------------------
namespace Python
{
	extern PyTypeObject VclMethodType;
  class TVclObject;
	PyObject* VclMethod_Create(TVclObject *Object, TObject *Instance, const DynamicArray<TRttiMethod*> &Methods);
	TValue CallMethod(TRttiType *Type, TObject *Instance, DynamicArray<TRttiMethod*> &Methods, PyObject *Args);
}
//---------------------------------------------------------------------------
#endif
