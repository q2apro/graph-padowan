/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2007 Ivan Johansen
 *
 * Graph is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
#ifndef PyGraphH
#define PyGraphH
#include "boost/variant/variant.hpp"
#include "PyVclConvert.h"
//---------------------------------------------------------------------------
struct _object;
typedef _object PyObject;
namespace Python
{
  enum TPluginEvent
  {
    peNew,
		peLoad,
    peSelect,
    peClose,
    peEdit,
    peAnimate,
    peDelete,
    peAxesChanged,
    peZoom,
    peOptionsChanged,
    peCustomFunctionsChanged,
    peNewElem,
    peChanged,
    peMoved
  };

  extern PyObject *PyEFuncError;
  extern PyObject *PyEGraphError;
	typedef boost::variant<int, double, std::wstring, PyObject*> TPyVariant;

	void InitPlugins();
	bool ExecutePythonCommand(const String &Command);
	void ShowPythonConsole(bool Visible);
	bool ExecutePluginEvent(TPluginEvent PluginEvent, PyObject *Param=NULL);
	bool ExecutePluginEvent(TPluginEvent PluginEvent, const TGraphElemPtr &Elem);
	bool ExecutePluginEvent(TPluginEvent PluginEvent, TPyVariant V1);
	bool ExecutePluginEvent(TPluginEvent PluginEvent, TPyVariant V1, TPyVariant V2);
	bool ExecutePluginEvent(TPluginEvent PluginEvent, TPyVariant V1, TPyVariant V2, TPyVariant V3);

	PyObject* ToPyObject(const TVariant &Variant);
	template<typename T1, typename T2>
	PyObject* CreateTuple(const T1 &V1, const T2 &V2)
	{
		return Py_BuildValue("NN", ToPyObject(V1), ToPyObject(V2));
	}

	template<typename T1, typename T2, typename T3>
	PyObject* CreateTuple(const T1 &V1, const T2 &V2, const T3 &V3)
	{
		return Py_BuildValue("NNN", ToPyObject(V1), ToPyObject(V2), ToPyObject(V3));
	}

	template<typename T1, typename T2, typename T3, typename T4>
	PyObject* CreateTuple(const T1 &V1, const T2 &V2, const T3 &V3, const T4 &V4)
	{
		return Py_BuildValue("NNNN", ToPyObject(V1), ToPyObject(V2), ToPyObject(V3), ToPyObject(V4));
	}

	template<typename T1, typename T2>
	bool FromTuple(PyObject *O, T1 &V1, T2 &V2)
	{
		PyObject *O1=NULL, *O2=NULL;
		if(!PyArg_ParseTuple(O, "OO", &O1, &O2)) return false;
		return FromPyObject(O1, V1) && FromPyObject(O2, V2);
	}

	template<typename T1, typename T2, typename T3>
	bool FromTuple(PyObject *O, T1 &V1, T2 &V2, T3 &V3)
	{
		PyObject *O1=NULL, *O2=NULL, *O3=NULL;
		if(!PyArg_ParseTuple(O, "OOO", &O1, &O2, &O3)) return false;
		return FromPyObject(O1, V1) && FromPyObject(O2, V2) && FromPyObject(O3, V3);
	}

	template<typename T1, typename T2, typename T3, typename T4>
	bool FromTuple(PyObject *O, T1 &V1, T2 &V2, T3 &V3, T4 &V4)
	{
		PyObject *O1=NULL, *O2=NULL, *O3=NULL, *O4=NULL;
		if(!PyArg_ParseTuple(O, "OOOO", &O1, &O2, &O3, &O4)) return false;
		return FromPyObject(O1, V1) && FromPyObject(O2, V2) && FromPyObject(O3, V3) && FromPyObject(O4, V4);
	}

	void ConvertException();
	void UnloadPlugin();
}
#endif
