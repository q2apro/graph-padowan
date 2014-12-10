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
#include "PyVclConvert.h"
#include "PyVcl.h"
#include "FindClass.hpp"

namespace Python
{
//---------------------------------------------------------------------------
const int MaxArgCount = 5;
struct TFunctionEntry
{
	const wchar_t *Name;
	void *Address;
	TTypeInfo *Result;
	TTypeInfo *Args[MaxArgCount];
};
//---------------------------------------------------------------------------
template <typename T, bool IsDelphiClass>
struct TDelphiRttiHelper
{
	static PTypeInfo GetTypeInfo()	{return __delphirtti (T);}
};

template <typename T>
struct TDelphiRttiHelper<T, true>
{
	static PTypeInfo GetTypeInfo() {return static_cast<PTypeInfo>(__classid(T)->ClassInfo());}
};

template <typename T> inline PTypeInfo Rtti()
{
	return TDelphiRttiHelper<T, __is_base_of(System::TObject, T)>::GetTypeInfo();
}
//---------------------------------------------------------------------------
/** List of global VCL function made available to PyVcl.
 *  \Warning: Don't use __delphirtti() on classes. It may make it impossible to access
 *  properties on that class through RTTI. See QC #90773
 */
const TFunctionEntry FunctionList[] =
{
	{L"ShortCutToText", ShortCutToText, __delphirtti(String), __delphirtti(TShortCut)},
	{L"TextToShortCut", TextToShortCut, __delphirtti(TShortCut), __delphirtti(String)},
	{L"ReadComponentResFile", ReadComponentResFile, Rtti<TComponent>(), __delphirtti(String), Rtti<TComponent>()},
	{L"ObjectTextToBinary", (void __fastcall (*)(TStream*, TStream*))ObjectTextToBinary, NULL, Rtti<TStream>(), Rtti<TStream>()},
  {L"ShowMessage", ShowMessage, NULL, __delphirtti(String)},
  {L"CreateMessageDialog", (TForm* __fastcall (*)(const UnicodeString, TMsgDlgType, TMsgDlgButtons, TMsgDlgBtn))&CreateMessageDialog, Rtti<Forms::TForm>(), __delphirtti(String), Rtti<TMsgDlgType>(), Rtti<TMsgDlgButtons>(), Rtti<TMsgDlgBtn>()},
  {L"MessageDlg", (int __fastcall (*)(const UnicodeString, TMsgDlgType, TMsgDlgButtons, int, TMsgDlgBtn))&MessageDlg, __delphirtti(int), __delphirtti(String), Rtti<TMsgDlgType>(), Rtti<TMsgDlgButtons>(), __delphirtti(int), Rtti<TMsgDlgBtn>()},
};
//---------------------------------------------------------------------------
struct TVclFunction
{
	PyObject_HEAD
	const TFunctionEntry *Function;
};
//---------------------------------------------------------------------------
/** Return string representation of the given VclFunction.
 */
static PyObject *VclFunction_Repr(TVclFunction* self)
{
	String Str = (self->Function->Result ? "<Function " : "<procedure ") + String(self->Function->Name) + L"(";
	for(int I = 0; I < MaxArgCount && self->Function->Args[I] != NULL; I++)
		Str += AnsiString(self->Function->Args[I]->Name) + ";";
	Str[Str.Length()] = ')';
	if(self->Function->Result)
		Str += ":" + AnsiString(self->Function->Result->Name);
	Str += ">";
	return PyUnicode_FromUnicode(Str.c_str(), Str.Length());
}
//---------------------------------------------------------------------------
/** Call a global VCL function through the VclFunction object.
 */
static PyObject *VclFunction_Call(TVclFunction* self, PyObject *args, PyObject *keywds)
{
	try
	{
		if(keywds != NULL)
			throw EPyVclError("Function does not accept keyword arguments");
		DynamicArray<TValue> Arguments;

		int Count;
		for(Count=0; Count < MaxArgCount && self->Function->Args[Count] != NULL; Count++);
		Arguments.Length = Count;
		if(Count != PyTuple_Size(args))
			throw EPyVclError("Function argument count mismatch!");

		for(int I = 0; I < Count; I++)
			 Arguments[I] = ToValue(PyTuple_GET_ITEM(args, I), self->Function->Args[I]);

		TValue Result = Invoke(self->Function->Address, Arguments, ccReg, self->Function->Result);
		return ToPyObject(Result);
	}
	catch(...)
	{
		return PyVclHandleException();
	}
}
//---------------------------------------------------------------------------
/** A VclFunction is a proxy of a global VCL function made availabe to PyVcl.
 */
PyTypeObject VclFunctionType =
{
	PyObject_HEAD_INIT(NULL)
	"vcl.VclFunction",       	 /* tp_name */
	sizeof(TVclFunction),      /* tp_basicsize */
	0,                         /* tp_itemsize */
	0, 												 /* tp_dealloc */
	0,                         /* tp_print */
	0,                         /* tp_getattr */
	0,                         /* tp_setattr */
	0,                         /* tp_compare */
	(reprfunc)VclFunction_Repr, /* tp_repr */
	0,                         /* tp_as_number */
	0,                         /* tp_as_sequence */
	0,                         /* tp_as_mapping */
	0,                         /* tp_hash */
	(ternaryfunc)VclFunction_Call, /* tp_call */
	0,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT, 			 /* tp_flags */
	"VCL global function object", /* tp_doc */
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
/** Create a new VclFunction object from a name.
 *  \param Name: The name of the function. Must be case sensitive.
 *  \return New reference to a VCLFunction object.
 */
PyObject* VclFunction_Create(const String &Name)
{
	TFunctionEntry *Function = NULL;
	for(unsigned I = 0; I < sizeof(FunctionList) / sizeof(FunctionList[0]); I++)
		if(Name == FunctionList[I].Name)
		{
			TVclFunction *VclFunction = PyObject_New(TVclFunction, &VclFunctionType);
			VclFunction->Function = &FunctionList[I];
			return reinterpret_cast<PyObject*>(VclFunction);
		}

	return NULL;
}
//---------------------------------------------------------------------------
/** Retrieve number of global VCL functions in the list. 
 */
unsigned GetVclFunctionCount()
{
	return sizeof(FunctionList)/sizeof(FunctionList[0]);
}
//---------------------------------------------------------------------------
/** Retrieve name of VCL global by index.
 */
const wchar_t* GetVclFunctionName(unsigned Index)
{
	if(Index < sizeof(FunctionList)/sizeof(FunctionList[0]))
		return FunctionList[Index].Name;
	return NULL;
}
//---------------------------------------------------------------------------
} //namespace Python
