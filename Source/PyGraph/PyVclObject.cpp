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
#include <structmember.h>
#define private public
#include <Rtti.hpp>
#undef private
#include "PyVclObject.h"
#include "PyVclMethod.h"
#include "PyVcl.h"
#include "PyVclConvert.h"
#include "PyVclArrayProperty.h"
#include "PyVclRef.h"

namespace Python
{
//---------------------------------------------------------------------------
/** This object is created when needed and owned by a TComponent object which should
 *  be monitored for destruction. Delphi objects not derived from TComponent cannot
 *  be monitored. Only one TObjectDeleteHandler is needed for every object monitored,
 *  and it can always be found by its name "ObjectDeleteHandler".
 *
 *  VclObject objects will register themself to be notified when the referred object
 *  is destroyed. Events that call Python functions are also tracked for cleanup when
 *  the monitored object is destroyed.
 */
class TObjectDeleteHandler : public TComponent
{
	std::deque<TVclObject*> Objects;
  std::deque<TMethodImplementation*> MethodImplementations;
public:
	__fastcall TObjectDeleteHandler(TComponent *AOwner) : TComponent(AOwner)
	{
		Name = L"ObjectDeleteHandler";
	}

	__fastcall ~TObjectDeleteHandler()
	{
		for(std::deque<TVclObject*>::iterator Iter = Objects.begin(); Iter != Objects.end(); ++Iter)
		{
			(*Iter)->Instance = NULL;
			(*Iter)->Owned = false;
    }

		for(std::deque<TMethodImplementation*>::iterator Iter = MethodImplementations.begin(); Iter != MethodImplementations.end(); ++Iter)
		{
      Py_DECREF(static_cast<PyObject*>((*Iter)->FUserData));
		  delete (*Iter)->FInvokeInfo;
   	  delete *Iter;
    }
	}

	void Register(TVclObject *VclObject) {Objects.push_back(VclObject);}
  void Register(TMethodImplementation *Impl) {MethodImplementations.push_back(Impl);}
	void Unregister(TVclObject *VclObject)
	{
		std::deque<TVclObject*>::iterator Iter = std::find(Objects.begin(), Objects.end(), VclObject);
		if(Iter != Objects.end())
		  Objects.erase(Iter);
	}
	void Unregister(TMethodImplementation *Impl)
	{
		std::deque<TMethodImplementation*>::iterator Iter = std::find(MethodImplementations.begin(), MethodImplementations.end(), Impl);
		if(Iter != MethodImplementations.end())
    {
      Py_DECREF(static_cast<PyObject*>(Impl->FUserData));
		  delete Impl->FInvokeInfo;
   	  delete Impl;
		  MethodImplementations.erase(Iter);
    }
	}
};
//---------------------------------------------------------------------------
/** Find and optionally create a TObjectDeleteHandler associated to a TComponent
 *  reffered by a VclObject.
 */
TObjectDeleteHandler* FindDeleteHandler(TVclObject *VclObject, bool Create)
{
	if(TComponent *Component = dynamic_cast<TComponent*>(VclObject->Instance))
	{
		TObjectDeleteHandler *DeleteHandler =
			dynamic_cast<TObjectDeleteHandler*>(Component->FindComponent("ObjectDeleteHandler"));
		if(DeleteHandler == NULL && Create)
			DeleteHandler = new TObjectDeleteHandler(Component);
		return DeleteHandler;
	}
  return NULL;
}
//---------------------------------------------------------------------------
/** Create a delete handler if it doesn't already exists and register the TVclObject
 *  object, which the delete handler is attached to.
 */
void CreateDeleteHandler(TVclObject *VclObject)
{
  if(TObjectDeleteHandler *DeleteHandler = FindDeleteHandler(VclObject, true))
		DeleteHandler->Register(VclObject);
}
//---------------------------------------------------------------------------
/** Class with function used to assign all events that are python objects.
 *  There is only one global instance, in fact the instance is not used but is
 *  necesarry because a closure is needed.
 */
struct TPythonCallback : public TCppInterfacedObject<TMethodImplementationCallback>
{
	void __fastcall Invoke(void * UserData, System::DynamicArray<TValue> Args, TValue &Result);
};
TPythonCallback *PythonCallback = new TPythonCallback;
//---------------------------------------------------------------------------
/** Function called for all events assigned to Python functions.
 *  The function will convert the arguments passed with the event to Python objects and
 *  call the Python callback function.
 */
void __fastcall TPythonCallback::Invoke(void * UserData, System::DynamicArray<TValue> Args, TValue &Result)
{
	TLockGIL Dummy;
	TMethodImplementation *Impl = static_cast<TMethodImplementation*>(Args[0].AsObject());
	TMethodImplementation::TInvokeInfo *InvokeInfo = Impl->FInvokeInfo;
	DynamicArray<TMethodImplementation::TParamLoc> Params = InvokeInfo->GetParamLocs();
	PyObject *Object = static_cast<PyObject*>(UserData);
	int Count = Args.get_length() - 1;
	PyObject *PyArgs = Count != 0 ? PyTuple_New(Count) : NULL;
	for(int I = 0; I < Count; I++)
		PyTuple_SET_ITEM(PyArgs, I, Params[I+1].FByRefParam ? VclRef_Create(&Args[I+1]) : ToPyObject(Args[I+1]));
	PyObject *PyResult = PyObject_CallObject(Object, PyArgs);
	Py_XDECREF(PyArgs);
	if(PyResult != NULL && PyResult != Py_None)
		Result = ToValue(PyResult, NULL); //Bug: Type of result missing
	Py_XDECREF(PyResult);
	if(PyResult == NULL)
	  PyErr_Print();
}
//---------------------------------------------------------------------------
/** Create TInvokeInfo object with parameter information for a Delphi event.
 *  This is used to decode the parameters inside the generic event handler.
 */
TMethodImplementation::TInvokeInfo* CreateInvokeInfo(TTypeInfo *TypeInfo)
{
	TTypeData *TypeData = reinterpret_cast<TTypeData*>(&TypeInfo->Name[TypeInfo->Name[0]+1]);
	int Index = 0;
	std::vector<Typinfo::TParamFlags> ParamFlags;
	for(int I = 0; I < TypeData->ParamCount; I++)
	{
		ParamFlags.push_back(reinterpret_cast<Typinfo::TParamFlags&>(TypeData->ParamList[Index++]));
		Index += TypeData->ParamList[Index] + 1;
		Index += TypeData->ParamList[Index] + 1;
	}
	if(TypeData->MethodKind == mkFunction)
	{
		Index += TypeData->ParamList[Index] + 1;
		Index += 4;
	}
	Typinfo::TCallConv CallConv = static_cast<Typinfo::TCallConv>(TypeData->ParamList[Index]);
	Index++;
	TMethodImplementation::TInvokeInfo *InvokeInfo = new TMethodImplementation::TInvokeInfo(CallConv, true);
	InvokeInfo->AddParameter(__delphirtti(TObject), false);
	for(int I = 0; I < TypeData->ParamCount; I++)
	{
		TTypeInfo *ParamType = **(TTypeInfo***)&TypeData->ParamList[Index];
		InvokeInfo->AddParameter(ParamType, ParamFlags[I].Contains(Typinfo::pfVar));
		Index += 4;
	}
	InvokeInfo->Seal();
	return InvokeInfo;
}
//---------------------------------------------------------------------------
/** This creates a weak reference to a callable object. If Func is a bound method,
 *  this will return a bound method weakproxy Func.__self__ to Func.__func__.
 *  Otherwise a normal strong reference is stored to Func to make sure the object is
 *  not destroyed if there are no references, e.g. lambda function.
 *  \param Func: Callable object used as event.
 *  \return New reference to event object. NULL if a Python exception has been set.
 */
PyObject* CreateWeakEvent(PyObject* Func)
{
  if(!PyCallable_Check(Func))
    return SetErrorString(PyExc_TypeError, "Event must be callable.");
  if(!PyMethod_Check(Func))
  { //Not a  bound method
    Py_INCREF(Func);
    return Func;
  }

  PyObject *obj = PyObject_GetAttrString(Func, "__self__");
  if(obj == NULL)
    return NULL;

  PyObject *fn = PyObject_GetAttrString(Func, "__func__");
  if(fn == NULL) //Apparently not a bound method
  {
    Py_DECREF(obj);
    return NULL;
  }
  PyObject *proxy = PyWeakref_NewProxy(obj, NULL);
  Py_DECREF(obj);
  if(proxy == NULL)
  {
    Py_DECREF(fn);
    return NULL;
  }
  PyObject *BoundMethod = PyMethod_New(fn, proxy);
  Py_DECREF(fn);
  Py_DECREF(proxy);
  return BoundMethod;
}
//---------------------------------------------------------------------------
/** Returns a Python string that represent the VclObject.
 */
static PyObject *VclObject_Repr(TVclObject* self)
{
	try
	{
		String Str;
		TComponent *Component = dynamic_cast<TComponent*>(self->Instance);
		if(self->Instance == NULL)
			Str = "<Reference to released object>";
		else if(Component != NULL)
			Str = "<object '" + Component->Name + "' of type '" + Component->ClassName() + "'>";
		else
			Str = "<object of type '" + self->Instance->ClassName() + "'>";
		return PyUnicode_FromUnicode(Str.c_str(), Str.Length());
	}
	catch(...)
	{
		return PyVclHandleException();
	}
}
//---------------------------------------------------------------------------
/** Returns a list of all properties, methods and public fields of the object.
 */
static PyObject* VclObject_Dir(TVclObject *self, PyObject *arg)
{
	PyObject *List = PyList_New(0);
	if(self->Instance != NULL)
	{
		TRttiType *Type = Context.GetType(self->Instance->ClassType());
		DynamicArray<TRttiProperty*> Properties = Type->GetProperties();
		DynamicArray<TRttiMethod*> Methods = Type->GetMethods();
		DynamicArray<TRttiField*> Fields = Type->GetFields();
		int PropertyCount = Properties.Length;
		int MethodCount = Methods.Length;
		int FieldCount = Fields.Length;
		PyObject *Value;
		for(int I = 0; I < PropertyCount; I++)
		{
			Value = ToPyObject(Properties[I]->Name);
			if(!PySequence_Contains(List, Value))
				PyList_Append(List, Value);
			Py_DECREF(Value);
		}
		for(int I = 0; I < MethodCount; I++)
		{
			Value = ToPyObject(Methods[I]->Name);
			if(!PySequence_Contains(List, Value))
				PyList_Append(List, Value);
			Py_DECREF(Value);
		}
		for(int I = 0; I < FieldCount; I++)
			if(Fields[I]->Visibility == mvPublic)
			{
				Value = ToPyObject(Fields[I]->Name);
				PyList_Append(List, Value);
				Py_DECREF(Value);
			}
	}
	return List;
}
//---------------------------------------------------------------------------
/** Destructor for the VclObject. If the instance is owned, first remove all objects
 *  that have the instance as parent, and then free the owned instance.
 *  If not owned, unregister from the delete handler.
 */
static void VclObject_Dealloc(TVclObject* self)
{
	if(self->Owned)
	{
		if(TWinControl *Control = dynamic_cast<TWinControl*>(self->Instance))
			while(Control->ControlCount)
				Control->Controls[Control->ControlCount-1]->Parent = NULL;
		delete self->Instance;
	}
	else
    if(TObjectDeleteHandler *DeleteHandler = FindDeleteHandler(self, false))
	  	DeleteHandler->Unregister(self);
	Py_TYPE(self)->tp_free(reinterpret_cast<PyObject*>(self));
}
//---------------------------------------------------------------------------
/** Retrieve a method, property or field. If a property or field is retrieved,
 *  we try to convert it to a built-in type, else a new VclObject is created to
 *  refference it without owning it.
 *  If a method is retrieved, a TVclMethod object is returned, which may be used
 *  to call the method.
 */
PyObject* VclObject_GetAttro(TVclObject *self, PyObject *attr_name)
{
	try
	{
		if(self->Instance == NULL)
			throw EPyVclError("Referenced object has been deleted.");

		TObject *Object = self->Instance;
		String Name = PyUnicode_AsUnicode(attr_name);
		TRttiType *Type = Context.GetType(Object->ClassType());
		if(Type == NULL)
			throw EPyVclError("Type not found.");
		TRttiProperty *Property = Type->GetProperty(Name);
		if(Property == NULL)
		{
			DynamicArray<TRttiMethod*> Methods = Type->GetMethods(Name);
			if(Methods.Length != 0 && !Methods[0]->IsDestructor)
				return VclMethod_Create(self, self->Instance, Methods);

			TRttiField *Field = Type->GetField(Name);
			if(Field == NULL || Field->Visibility != mvPublic)
			{
				PyObject *ArrayProperty = VclArrayProperty_Create(Object, Name);
				if(ArrayProperty != NULL)
					return ArrayProperty;
				PyObject *Result = PyObject_GenericGetAttr(reinterpret_cast<PyObject*>(self), attr_name);
				if(Result == NULL)
					SetErrorString(PyExc_AttributeError, "'" + Type->Name + "' object has no attribute '" + Name + "'");
				return Result;
			}
			TValue Result = Field->GetValue(Object);
			return ToPyObject(Result);
		}
		TValue Result = Property->GetValue(Object);
		return ToPyObject(Result);
	}
	catch(...)
	{
		return PyVclHandleException();
	}
}
//---------------------------------------------------------------------------
/** Set the value of a public property or field in a VclObject. If an event is
 *  set, we use a generic event, which will convert the parameters and call the
 *  Python event. If the Python event is a bound method, we store a weak reference
 *  to the instance of the bound method to avoid circular references.
 */
int VclObject_SetAttro(TVclObject *self, PyObject *attr_name, PyObject *v)
{
	try
	{
		if(self->Instance == NULL)
			throw EPyVclError("Referenced object has been deleted.");

		String Name = PyUnicode_AsUnicode(attr_name);
		TRttiType *Type = Context.GetType(self->Instance->ClassType());
		TRttiProperty *Property = Type->GetProperty(Name);
		if(Property == NULL)
		{
			int Result = PyObject_GenericSetAttr(reinterpret_cast<PyObject*>(self), attr_name, v);
			if(Result == -1)
				SetErrorString(PyExc_AttributeError, "'" + Type->Name + "' object has no attribute '" + Name + "'");
			return Result;
		}

		TTypeInfo *TypeInfo = Property->PropertyType->Handle;
		TValue Value;
		if(TypeInfo->Kind == tkMethod)
		{
      //Free exisitng event if it is a PyObject
      Value = Property->GetValue(self->Instance);
      TMethod Method;
			Value.ExtractRawDataNoCopy(&Method);
			TObject *Object = static_cast<TObject*>(Method.Data);
			if(TMethodImplementation *Impl = dynamic_cast<TMethodImplementation*>(Object))
			{
				if(TObjectDeleteHandler *DeleteHandler = FindDeleteHandler(self, false))
					DeleteHandler->Unregister(Impl);
			}

			if(v == Py_None)
				Value = TValue::From<TObject*>(NULL);
			else
			{
				TMethodImplementation::TInvokeInfo *InvokeInfo = CreateInvokeInfo(TypeInfo);
        PyObject *Func = CreateWeakEvent(v);
        if(Func == NULL)
          return -1;
				TMethodImplementation *Implementation = new TMethodImplementation(Func, InvokeInfo, PythonCallback);
				TMethod Method = {Implementation->CodeAddress, Implementation}; //Pass InvokeInfo in this, which can be used as another data pointer
				TValue::Make(&Method, TypeInfo, Value);
				if(TObjectDeleteHandler *DeleteHandler = FindDeleteHandler(self, true))
					DeleteHandler->Register(Implementation);
			}
		}
		else
			Value = ToValue(v, TypeInfo);
		Property->SetValue(self->Instance, Value);
		return 0;
	}
	catch(...)
	{
		PyVclHandleException();
		return -1;
	}
}
//---------------------------------------------------------------------------
static PyMemberDef VclObject_Members[] =
{
	{"_owned", T_BOOL, offsetof(TVclObject, Owned), 0, "Indicates if the VCL object is freed when the proxy is destroyed"},
	{NULL, 0, 0, 0, NULL}
};
//---------------------------------------------------------------------------
static PyMethodDef VclObject_Methods[] =
{
	{"__dir__", (PyCFunction)VclObject_Dir, METH_NOARGS, ""},
	{NULL, NULL, 0, NULL}
};
//---------------------------------------------------------------------------
/** Proxy object for a VCL object. If _owned is set to True, the VCL object is
 *  owned by the proxy and destroyed when the proxy is destroyed.
 *  If possible the VCL object will own a delete handler, which will clear the
 *  reference in the proxy if the VCL object is destroyed before the proxy.
 */
PyTypeObject VclObjectType =
{
	PyObject_HEAD_INIT(NULL)
	"vcl.VclObject",        	 /* tp_name */
	sizeof(TVclObject),        /* tp_basicsize */
	0,                         /* tp_itemsize */
	(destructor)VclObject_Dealloc, /* tp_dealloc */
	0,                         /* tp_print */
	0,                         /* tp_getattr */
	0,                         /* tp_setattr */
	0,                         /* tp_compare */
	(reprfunc)VclObject_Repr,  /* tp_repr */
	0,                         /* tp_as_number */
	0,                         /* tp_as_sequence */
	0,                         /* tp_as_mapping */
	0,                         /* tp_hash */
	0, 												 /* tp_call */
	0,                         /* tp_str */
	(getattrofunc)VclObject_GetAttro, /* tp_getattro */
	(setattrofunc)VclObject_SetAttro, /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT, 			 /* tp_flags */
	"VCL object",       			 /* tp_doc */
	0,		                     /* tp_traverse */
	0,		                     /* tp_clear */
	0,		                     /* tp_richcompare */
	0,		                     /* tp_weaklistoffset */
	0,		                     /* tp_iter */
	0,		                     /* tp_iternext */
	VclObject_Methods,         /* tp_methods */
	VclObject_Members,         /* tp_members */
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
/** Create new VclObject.
 *  \param Instance: VCL instance, which the VclObject will be a proxy for.
 *  \param Owned: If True, Instance will be owned by the proxy and destroyed
 *                when the proxy is destroyed.
 *  \return New reference to VclObject.
 */
PyObject* VclObject_Create(TObject *Instance, bool Owned)
{
	TVclObject *VclObject = PyObject_New(TVclObject, &VclObjectType);
	VclObject->Instance = Instance;
	VclObject->Owned = Owned;
  CreateDeleteHandler(VclObject);
	return reinterpret_cast<PyObject*>(VclObject);
}
//---------------------------------------------------------------------------
/** Return true if O is a VclObject.
 */
bool VclObject_Check(PyObject *O)
{
	return O->ob_type->tp_repr == VclObjectType.tp_repr;
}
//---------------------------------------------------------------------------
/** Return the VCL object, which the given VclObject is a proxy for.
 *  \param O: Pointer to a VclObject instance.
 *  \return VCL object refered to by O. This may be NULL if the object has been destroyed
 *  \throw EPyVclError if O is not a VclObject.
 */
TObject* VclObject_AsObject(PyObject *O)
{
	if(O->ob_type->tp_repr != VclObjectType.tp_repr)
		throw EPyVclError("Object is not a VclObject type");
	return reinterpret_cast<TVclObject*>(O)->Instance;
}
//---------------------------------------------------------------------------
} //namespace Python
