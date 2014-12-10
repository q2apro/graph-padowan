/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2007 Ivan Johansen
 *
 * Graph is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
#ifndef PythonBindH
#define PythonBindH
#include <boost/intrusive_ptr.hpp>
//---------------------------------------------------------------------------
struct _object;
struct _typeobject;
struct _ts;
typedef _object PyObject;

#define PYTHON_FPU_CONTROL MCW_EM | IC_PROJECTIVE | RC_NEAR | PC_53
#define DEFAULT_FPU_CONTROL EM_INVALID | EM_DENORMAL | EM_UNDERFLOW | EM_INEXACT | IC_AFFINE | RC_NEAR | PC_64
#define FPU_MASK MCW_EM | MCW_IC | MCW_RC | MCW_PC

namespace Python
{
template<typename T> T& GetPythonAddress(const char *Name);

#ifndef PYTHON_WRAP
#define PYTHON_WRAP(type,name) extern type& name;
#endif
PYTHON_WRAP(_typeobject, PyBool_Type)
PYTHON_WRAP(_typeobject, PyTuple_Type)
PYTHON_WRAP(_typeobject, PyLong_Type)
PYTHON_WRAP(_typeobject, PyString_Type)
PYTHON_WRAP(_typeobject, PyUnicode_Type)
PYTHON_WRAP(_typeobject, PyBaseObject_Type)
PYTHON_WRAP(_typeobject, PyCFunction_Type)
PYTHON_WRAP(_typeobject, PyFloat_Type)
PYTHON_WRAP(_typeobject, PyType_Type)
PYTHON_WRAP(_typeobject, PySlice_Type)
PYTHON_WRAP(_typeobject, PyComplex_Type)
PYTHON_WRAP(_typeobject, PyFunction_Type)
PYTHON_WRAP(_typeobject, PyMethod_Type)
PYTHON_WRAP(PyObject*, PyExc_TypeError)
PYTHON_WRAP(PyObject*, PyExc_KeyError)
PYTHON_WRAP(PyObject*, PyExc_AttributeError)
PYTHON_WRAP(PyObject*, PyExc_IOError)
PYTHON_WRAP(PyObject*, PyExc_IndexError)
PYTHON_WRAP(PyObject*, PyExc_MemoryError)
PYTHON_WRAP(PyObject*, PyExc_NameError)
PYTHON_WRAP(PyObject*, PyExc_OverflowError)
PYTHON_WRAP(PyObject*, PyExc_RuntimeError)
PYTHON_WRAP(PyObject*, PyExc_SyntaxError)
PYTHON_WRAP(PyObject*, PyExc_SystemError)
PYTHON_WRAP(PyObject*, PyExc_ValueError)
PYTHON_WRAP(PyObject*, PyExc_ZeroDivisionError)
PYTHON_WRAP(PyObject*, PyExc_NotImplementedError)
PYTHON_WRAP(PyObject*, PyExc_StopIteration)
PYTHON_WRAP(PyObject, _Py_NoneStruct)
PYTHON_WRAP(PyObject, _Py_TrueStruct)
PYTHON_WRAP(PyObject, _Py_FalseStruct)
PYTHON_WRAP(PyObject, _Py_NotImplementedStruct)

bool IsPythonInstalled();

class TLockGIL
{
	/*PyGILState_STATE*/int State;
public:
	TLockGIL();
	~TLockGIL();
};

class TUnlockGIL
{
	_ts *State;
public:
	TUnlockGIL();
	~TUnlockGIL();
};

	typedef boost::intrusive_ptr<PyObject> TPyObjectPtr;

	PyObject* SetErrorString(PyObject *Type, const String &Str);
}

namespace boost
{
	void intrusive_ptr_add_ref(PyObject *O);
	void intrusive_ptr_release(PyObject *O);
}

#ifdef WRAP_PYOBJECTS
#define PyBool_Type Python::PyBool_Type
#define PyTuple_Type Python::PyTuple_Type
#define PyLong_Type Python::PyLong_Type
#define PyString_Type Python::PyString_Type
#define PyUnicode_Type Python::PyUnicode_Type
#define PyBaseObject_Type Python::PyBaseObject_Type
#define PyCFunction_Type Python::PyCFunction_Type
#define PyFloat_Type Python::PyFloat_Type
#define PyType_Type Python::PyType_Type
#define PySlice_Type Python::PySlice_Type
#define PyComplex_Type Python::PyComplex_Type
#define PyMethod_Type Python::PyMethod_Type
#define PyExc_TypeError Python::PyExc_TypeError
#define PyExc_KeyError Python::PyExc_KeyError
#define PyExc_AttributeError Python::PyExc_AttributeError
#define PyExc_IOError Python::PyExc_IOError
#define PyExc_IndexError Python::PyExc_IndexError
#define PyExc_MemoryError Python::PyExc_MemoryError
#define PyExc_NameError Python::PyExc_NameError
#define PyExc_OverflowError Python::PyExc_OverflowError
#define PyExc_RuntimeError Python::PyExc_RuntimeError
#define PyExc_SyntaxError Python::PyExc_SyntaxError
#define PyExc_SystemError Python::PyExc_SystemError
#define PyExc_ValueError Python::PyExc_ValueError
#define PyExc_ZeroDivisionError Python::PyExc_ZeroDivisionError
#define PyExc_NotImplementedError Python::PyExc_NotImplementedError
#define PyExc_StopIteration Python::PyExc_StopIteration
#define _Py_NoneStruct Python::_Py_NoneStruct
#define _Py_TrueStruct &Python::_Py_TrueStruct
#define _Py_FalseStruct &Python::_Py_FalseStruct
#define _Py_NotImplementedStruct Python::_Py_NotImplementedStruct
#endif

#endif
