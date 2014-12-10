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
#include "Python.hpp"
#include <Rtti.hpp>
#include "PyVclMethod.h"
#include "PyVcl.h"
#include "PyVclConvert.h"
#include "PyVclObject.h"
namespace Python
{
//---------------------------------------------------------------------------
struct TVclMethod
{
	PyObject_HEAD
  TObject *Instance;
	TVclObject *Object; //NULL if we are not bound to a Python object
	DynamicArray<TRttiMethod*> Methods;
};
//---------------------------------------------------------------------------
/** Return a string reprecentation of the VCL method.
 *  \return New reference
 */
static PyObject *VclMethod_Repr(TVclMethod* self)
{
	String Str = L'<';
	int VirtualIndex = MaxInt;
	for(int I = 0; I < self->Methods.get_length(); I++)
	{
		TRttiMethod *Method = self->Methods[I];
		if(Method->VirtualIndex != VirtualIndex)
			Str += Method->ToString() + ", ";
		VirtualIndex = Method->VirtualIndex;
	}
	Str.Delete(Str.Length()-1, 2); //Remove last ", "
	TComponent *Component = dynamic_cast<TComponent*>(self->Instance);
	if(Component != NULL)
		Str += " of <object '" + Component->Name + "' of type '" + Component->ClassName() + "'>>";
	else
		Str += " of <object of type '" + self->Instance->ClassName() + "'>>";
	return PyUnicode_FromUnicode(Str.c_str(), Str.Length());
}
//---------------------------------------------------------------------------
/** Call a method of a VCL object or a constructor of a VCL type.
 *  The methods are considered in order and the first that matches the arguments
 *  will be used.
 *  \param Type: The type to construct if the method is a constructor; NULL otherwise.
 *  \param Instance: The VCL object of the method; NULL if it is a constructor
 *  \param Methods: The methods to consider
 *  \param Args: Arguments that will be passed to the method.
 *  \return The result of the called method or the newly constructed object if it
 *  is a constructor.
 */
TValue CallMethod(TRttiType *Type, TObject *Instance, DynamicArray<TRttiMethod*> &Methods, PyObject *Args)
{
	unsigned MethodCount = Methods.get_length();
	TRttiType *Parent = Methods[0]->Parent;
	for(unsigned I = 0; I < MethodCount; I++)
	{
		TRttiMethod *Method = Methods[I];
		if(Instance == NULL) //If looking for constructor
			if(Method->Parent != Parent)
			  break; //Only look for constructors defined in the last class with constructors

		DynamicArray<TRttiParameter*> ParameterTypes = Method->GetParameters();
		std::vector<TValue> Parameters;
		int ParamCount = PyTuple_Size(Args);
		if(ParamCount != ParameterTypes.Length)
			if(MethodCount == 1)
				throw EPyVclError(Method->Name + "() takes exactly " + ParameterTypes.get_length() + " arguments (" + ParamCount + " given)");
			else
				continue;
		try
		{
			TupleToValues(Args, Parameters, ParameterTypes);
		}
		catch(Exception &E)
		{
			if(MethodCount == 1)
				throw;
			continue;
		}

		if(Method->IsConstructor)
		{
			if(Type == NULL)
				throw EPyVclError("Cannot call constructor");
			return Method->Invoke(Type->AsInstance->MetaclassType, Parameters.size() == 0 ? NULL : &Parameters[0], Parameters.size()-1);
		}
		else if(Method->IsClassMethod || Method->IsStatic)
			return Method->Invoke(Instance->ClassType(), Parameters.size() == 0 ? NULL : &Parameters[0], Parameters.size()-1);
		else
			return Method->Invoke(Instance, Parameters.size() == 0 ? NULL : &Parameters[0], Parameters.size()-1);
	}

	throw EPyVclError("No suitable overload found for the function " + Methods[0]->Name);
}
//---------------------------------------------------------------------------
/** Call the first suitable method refferenced by the VclMethod object.
 *  Keyword arguments are not allowed as they cannot be mapped to arguemnts to a
 *  VCL method.
 */
static PyObject *VclMethod_Call(TVclMethod* self, PyObject *args, PyObject *keywds)
{
	try
	{
		if(keywds != NULL)
			return SetErrorString(PyExc_TypeError, self->Methods[0]->Name + "() does not accept keyword arguments");
		return ToPyObject(CallMethod(NULL, self->Instance, self->Methods, args));
		return ToPyObject(CallMethod(NULL, self->Object->Instance, self->Methods, args));
	}
	catch(...)
	{
		return PyVclHandleException();
	}
}
//---------------------------------------------------------------------------
/** Destructor for VclMethod.
 */
static void VclMethod_Dealloc(TVclMethod* self)
{
	self->Methods.~DynamicArray();
  Py_XDECREF(reinterpret_cast<PyObject*>(self->Object));
	Py_TYPE(self)->tp_free(reinterpret_cast<PyObject*>(self));
}
//---------------------------------------------------------------------------
/** VclMethod is a proxy for one or more methods of a VCL object.
 */
PyTypeObject VclMethodType =
{
	PyObject_HEAD_INIT(NULL)
	"vcl.VclMethod",        	 /* tp_name */
	sizeof(TVclMethod),        /* tp_basicsize */
	0,                         /* tp_itemsize */
	(destructor)VclMethod_Dealloc, /* tp_dealloc */
	0,                         /* tp_print */
	0,                         /* tp_getattr */
	0,                         /* tp_setattr */
	0,                         /* tp_compare */
	(reprfunc)VclMethod_Repr,  /* tp_repr */
	0,                         /* tp_as_number */
	0,                         /* tp_as_sequence */
	0,                         /* tp_as_mapping */
	0,                         /* tp_hash */
	(ternaryfunc)VclMethod_Call, /* tp_call */
	0,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT, 			 /* tp_flags */
	"VCL method object",       /* tp_doc */
	0,		                     /* tp_traverse */
	0,		                     /* tp_clear */
	0,		                     /* tp_richcompare */
	0,		                     /* tp_weaklistoffset */
	0,		                     /* tp_iter */
	0,		                     /* tp_iternext */
	0, 								         /* tp_methods */
	0,                         /* tp_members */
	0,       									 /* tp_getset */
	0,                         /* tp_base */
	0,                         /* tp_dict */
	0,                         /* tp_descr_get */
	0,                         /* tp_descr_set */
	0,                         /* tp_dictoffset */
	0,                         /* tp_init */
	0,                         /* tp_alloc */
	0,						             /* tp_new */
};
//---------------------------------------------------------------------------
/** Create new VclMethod object, which is a proxy object to one or more methods of
 *  a VCL object.
 *  \param Instance: The VCL object of the methods.
 *  \param Methods: The methods we are creating a proxy for.
 *  \return New reference to a VclMethod
 */
PyObject* VclMethod_Create(TVclObject *Object, TObject *Instance, const DynamicArray<TRttiMethod*> &Methods)
{
	TVclMethod *VclMethod = PyObject_New(TVclMethod, &VclMethodType);
	new(&VclMethod->Methods) DynamicArray<TRttiMethod*>(Methods);
  Py_XINCREF(reinterpret_cast<PyObject*>(Object));
	VclMethod->Object = Object;
  VclMethod->Instance = Instance;
	return reinterpret_cast<PyObject*>(VclMethod);
}
//---------------------------------------------------------------------------
} //namespace Python
