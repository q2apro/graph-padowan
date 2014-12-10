/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2007 Ivan Johansen
 *
 * Graph is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
#include "Graph.h"
#pragma hdrstop
#define PYTHON_WRAP(type,name) type& name = GetPythonAddress<type>(#name);
#include "PythonBind.h"
#include "python.hpp"
#include "ConfigRegistry.h"
#include <Rtti.hpp>
#include "PyVclObject.h"
#pragma link "python32.lib"
//---------------------------------------------------------------------------
namespace Python
{
HINSTANCE PythonInstance = NULL;
//---------------------------------------------------------------------------
template<typename T>
T& GetPythonAddress(const char *Name)
{
  static T Dummy;
  if(IsPythonInstalled())
    return *reinterpret_cast<T*>(GetProcAddress(PythonInstance, Name));
  return Dummy;
}
//---------------------------------------------------------------------------
bool IsPythonInstalled()
{
	static int Result = -1;
  if(Result == -1)
  {
    PythonInstance = LoadLibrary(L"Python32.dll");
    Result = PythonInstance != NULL;
  }
  return Result;
}
//---------------------------------------------------------------------------
PyObject* PyReturnNone()
{
  Py_INCREF(Py_None);
	return Py_None;
}
//---------------------------------------------------------------------------
TLockGIL::TLockGIL()
{
  _control87(PYTHON_FPU_CONTROL, FPU_MASK);
  State = PyGILState_Ensure();
}
//---------------------------------------------------------------------------
TLockGIL::~TLockGIL()
{
  PyGILState_Release(static_cast<PyGILState_STATE>(State));
  _clear87();
  _control87(DEFAULT_FPU_CONTROL, FPU_MASK);
}
//---------------------------------------------------------------------------
TUnlockGIL::TUnlockGIL()
{
  State = PyEval_SaveThread();
  _clear87();
  _control87(DEFAULT_FPU_CONTROL, FPU_MASK);
}
//---------------------------------------------------------------------------
TUnlockGIL::~TUnlockGIL()
{
  _control87(PYTHON_FPU_CONTROL, FPU_MASK);
  PyEval_RestoreThread(State);
}
//---------------------------------------------------------------------------
PyObject* SetErrorString(PyObject *Type, const String &Str)
{
	PyErr_SetString(Type, AnsiString(Str).c_str());
	return NULL;
}
//---------------------------------------------------------------------------
} //namespace Python
//Helper functions needed for use of boost::intrusive_ptr<PyObject>
void boost::intrusive_ptr_add_ref(PyObject *O)
{
	if(O)
	{
		Python::TLockGIL Dummy;
		Py_INCREF(O);
	}
}
//---------------------------------------------------------------------------
void boost::intrusive_ptr_release(PyObject *O)
{
	if(O)
	{
		Python::TLockGIL Dummy;
		Py_DECREF(O);
	}
}
//---------------------------------------------------------------------------








