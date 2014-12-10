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
#include "PyVclConvert.h"
#include "PyVclObject.h"
#include "PyVcl.h"

namespace Python
{
//---------------------------------------------------------------------------
struct TVclArrayProperty
{
	PyObject_HEAD
	TObject *Instance;
	int PropertyId;
};
//---------------------------------------------------------------------------
/** Retrieve the number of elements of the array property.
 */
Py_ssize_t VclArrayProperty_Length(TVclArrayProperty *self)
{
	switch(self->PropertyId)
	{
		case 0:
			return static_cast<TCollection*>(self->Instance)->Count;
		case 1:
			return static_cast<TStrings*>(self->Instance)->Count;
		case 2:
			return static_cast<TScreen*>(self->Instance)->FormCount;
		case 3:
			return static_cast<TComponent*>(self->Instance)->ComponentCount;
		case 4:
			return static_cast<TWinControl*>(self->Instance)->ControlCount;
		case 5:
			return static_cast<TMenuItem*>(self->Instance)->Count;
	}
	return 0;
}
//---------------------------------------------------------------------------
/** Get an element of the array property.
 *  \param self: The array property wrapper, indicating instance and property.
 *  \param i: The index for the item to get.
 *  \return New reference to the retrieved object.
 */
PyObject* VclArrayProperty_Item(TVclArrayProperty *self, Py_ssize_t i)
{
	if(i >= VclArrayProperty_Length(self))
		return SetErrorString(PyExc_IndexError, "List index out of range");

	try
	{
		switch(self->PropertyId)
		{
			case 0:
				return ToPyObject(static_cast<TCollection*>(self->Instance)->Items[i]);
			case 1:
				return ToPyObject(static_cast<TStrings*>(self->Instance)->Strings[i]);
			case 2:
				return ToPyObject(static_cast<TScreen*>(self->Instance)->Forms[i]);
			case 3:
				return ToPyObject(static_cast<TComponent*>(self->Instance)->Components[i]);
			case 4:
				return ToPyObject(static_cast<TWinControl*>(self->Instance)->Controls[i]);
			case 5:
				return ToPyObject(static_cast<TMenuItem*>(self->Instance)->Items[i]);
		}
	}
	catch(...)
	{
		return PyVclHandleException();
	}
	return NULL;
}
//---------------------------------------------------------------------------
/** Return a string representation of the content of the array property.
 */
static PyObject *VclArrayProperty_Repr(TVclArrayProperty* self)
{
  unsigned Count = VclArrayProperty_Length(self);
	PyObject *List = PyList_New(Count);
	for(unsigned I = 0; I < Count; I++)
		PyList_SET_ITEM(List, I, VclArrayProperty_Item(self, I));
	PyObject *Result = PyObject_Repr(List);
	Py_DECREF(List);
	return Result;
}
//---------------------------------------------------------------------------
/** Set an element of the array property.
 *  \param self: The array property wrapper, indicating instance and property.
 *  \param i: The index for the item to set.
 *  \param v: The value to set. This is checked for corret type.
 */
int VclArrayProperty_SetItem(TVclArrayProperty *self, Py_ssize_t i, PyObject *v)
{
	try
	{
		switch(self->PropertyId)
		{
			case 0:
			{
				TObject *Object = VclObject_AsObject(v);
				if(TCollectionItem *Item = dynamic_cast<TCollectionItem*>(Object))
					static_cast<TCollection*>(self->Instance)->Items[i] = Item;
				else
					throw EPyVclError("Only TCollectionItem can be assigned to a TCollection");
				break;
			}
			case 1:
				if(!PyUnicode_Check(v))
					throw EPyVclError("Only strings can be assigned to a TStrings");
				static_cast<TStrings*>(self->Instance)->Strings[i] = PyUnicode_AsUnicode(v);
				break;

			default:
				SetErrorString(PyVclException, "Property is read only");
				return -1;
		}
		return  0;
	}
	catch(...)
	{
		PyVclHandleException();
		return -1;
	}
}
//---------------------------------------------------------------------------
PySequenceMethods VclArrayPropertySequence =
{
	(lenfunc)VclArrayProperty_Length, /* sq_length */
	0, /* sq_concat */
	0, /* sq_repeat */
	(ssizeargfunc)VclArrayProperty_Item, /* sq_item */
	0, /* was_sq_slice */
	(ssizeobjargproc)VclArrayProperty_SetItem, /* sq_ass_item */
	0, /* was_sq_ass_slice */
	0, /* sq_contains */
	0, /* sq_inplace_concat */
	0, /* sq_inplace_repeat */
};

/** VclArrayProperty is a wrapper around an array property in a Delphi object.
 *  This is a wrokaround as the Delphi RTTI does not support access to read/write array
 *  properties in a generic way.
 */
PyTypeObject VclArrayPropertyType =
{
	PyObject_HEAD_INIT(NULL)
	"vcl.VclArrayProperty",  	 /* tp_name */
	sizeof(TVclArrayProperty), /* tp_basicsize */
	0,                         /* tp_itemsize */
	0, 												 /* tp_dealloc */
	0,                         /* tp_print */
	0,                         /* tp_getattr */
	0,                         /* tp_setattr */
	0,                         /* tp_compare */
	(reprfunc)VclArrayProperty_Repr,  /*tp_repr */
	0, 												 /* tp_as_number */
	&VclArrayPropertySequence, /* tp_as_sequence */
	0,                         /* tp_as_mapping */
	0,                         /* tp_hash */
	0, 												 /* tp_call */
	0,                         /* tp_str */
	0, 												 /* tp_getattro */
	0, 												 /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT, 			 /* tp_flags */
	"VCL array property",			 /* tp_doc */
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
/** Create a new array property wrapper object.
 *  \param Instance: This is the VCL object instance that has the array property.
 *  \param Name: This is the name of the array property.
 *  \return New reference to an array property wrapper.
 */
PyObject* VclArrayProperty_Create(TObject *Instance, const String &Name)
{
	int PropertyId = -1;
	if(Name == "Items" && dynamic_cast<TCollection*>(Instance))
		PropertyId = 0;
	else if(Name == "Strings" && dynamic_cast<TStrings*>(Instance))
		PropertyId = 1;
	else if(Name == "Forms" && dynamic_cast<TScreen*>(Instance))
		PropertyId = 2;
	else if(Name == "Components" && dynamic_cast<TComponent*>(Instance))
		PropertyId = 3;
	else if(Name == "Controls" && dynamic_cast<TWinControl*>(Instance))
		PropertyId =4;
	else if(Name == "Items" && dynamic_cast<TMenuItem*>(Instance))
	  PropertyId = 5;
	else
		return NULL;
	TVclArrayProperty *VclProperty = PyObject_New(TVclArrayProperty, &VclArrayPropertyType);
	VclProperty->Instance = Instance;
	VclProperty->PropertyId = PropertyId;
	return reinterpret_cast<PyObject*>(VclProperty);
}
//---------------------------------------------------------------------------
} //namespace Python
