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
struct TVclType
{
	PyTypeObject Type;
	TRttiType *RttiType;
};
//---------------------------------------------------------------------------
PyObject *VclType_New(PyTypeObject *subtype, PyObject *args, PyObject *kwds)
{
	return subtype->tp_alloc(subtype, 0);
}
//---------------------------------------------------------------------------
/** Constructor for the proxy type. This will retrieve a list of Delphi constructors
 *  for the referenced type and call the first one that fits the arguments.
 *  \param self: Newly constructed proxy object. The created instance will be owned by this proxy.
 *  \param args: Tuple with arguments converted and passed to the actual constructor.
 *  \param kwds: Keyword arguments used to set properties after the object is created.
 */
int VclType_Init(TVclObject *self, PyObject *args, PyObject *kwds)
{
	try
	{
		TVclType *Type = reinterpret_cast<TVclType*>(self->ob_base.ob_type);
		DynamicArray<TRttiMethod*> Methods = Type->RttiType->GetMethods("Create");
		if(Methods.Length == 0)
		  throw EPyVclError("No constructor found");
		self->Instance = CallMethod(Type->RttiType, NULL, Methods, args).AsObject();
		self->Owned = true;
		CreateDeleteHandler(self);
		if(kwds)
		{
			PyObject *Key, *Value;
			Py_ssize_t Pos = 0;
			while(PyDict_Next(kwds, &Pos, &Key, &Value))
				if(self->ob_base.ob_type->tp_setattro(reinterpret_cast<PyObject*>(self), Key, Value) == -1)
					return -1;
    }
	}
	catch(...)
	{
		PyVclHandleException();
		return -1;
	}
	return 0;
}
//---------------------------------------------------------------------------
/** Create a new a new VclType object representing a Delphi class.
 *  This could for example be vcl.TForm
 *  \param Type: The Delphi class to preresent.
 *  \return New reference to a type object.
 */
PyObject* VclType_Create(TRttiType *Type)
{
	TVclType *VclType = new TVclType;
	memset(VclType, 0, sizeof(TVclType));

	VclType->Type.ob_base.ob_base.ob_refcnt = 1;
	char *Name = new char[Type->Name.Length()+1];
	strcpy(Name, AnsiString(Type->Name).c_str());
	VclType->Type.tp_name = Name;
	VclType->Type.tp_doc = "VCL type";
	VclType->Type.tp_new = VclType_New;
	VclType->Type.tp_init = reinterpret_cast<initproc>(VclType_Init);
	VclType->Type.tp_base = &VclObjectType;
	VclType->Type.tp_flags = Py_TPFLAGS_DEFAULT;
	VclType->RttiType = Type;
	if(PyType_Ready(&VclType->Type) < 0)
		return NULL;
	return reinterpret_cast<PyObject*>(VclType);
}
//---------------------------------------------------------------------------
} //namespace Python
