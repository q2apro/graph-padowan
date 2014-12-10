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
#define private public
#include <Rtti.hpp>
#undef private
#include "PyVclObject.h"
#include "PyVclMethod.h"
//---------------------------------------------------------------------------
namespace Python
{
TRttiContext Context;
PyObject *PyVclException = NULL;
//---------------------------------------------------------------------------
/** Convert a PyObject to a TValue as used when calling functions and setting properties in a generic way through Delphi RTTI.
 *  \param O: PyObject to convert
 *  \param TypeInfo: The expected return type
 *  \return A TValue with a value of the type given by TypeInfo.
 *  \throw EPyVclError on conversion errors.
 */
TValue ToValue(PyObject *O, TTypeInfo *TypeInfo)
{
	TValue Result;
	switch(TypeInfo->Kind)
	{
		case tkClass:
			if(VclObject_Check(O))
				Result = TValue::From(VclObject_AsObject(O));
			else if(O == Py_None || PyLong_Check(O))
				Result = TValue::From((TObject*)(O == Py_None ? NULL : PyLong_AsLong(O)));
			else
				throw EPyVclError("Cannot convert Python object of type '" + String(O->ob_type->tp_name) + "' to '" + AnsiString(TypeInfo->Name) + "'");
			break;

		case tkEnumeration:
			if(PyUnicode_Check(O))
				TValue::Make(GetEnumValue(TypeInfo, PyUnicode_AsUnicode(O)), TypeInfo, Result);
			if(PyLong_Check(O))
				TValue::Make(PyLong_AsLong(O), TypeInfo, Result);
			break;

		case tkSet:
			TValue::Make(StringToSet(TypeInfo, PyUnicode_AsUnicode(O)), TypeInfo, Result);
			break;

		case tkInteger:
			Result = TValue::From(PyLong_AsUnsignedLongMask(O));
			break;

		case tkUString:
		case tkString:
		case tkLString:
		case tkWString:
			Result = TValue::From(String(PyUnicode_AsUnicode(O)));
			break;

		case tkChar:
		case tkWChar:
			if(PyUnicode_GetSize(O) != 1)
				throw EPyVclError("Expected string with one character");
			Result = TValue::From(PyUnicode_AsUnicode(O)[0]);
			break;

		case tkFloat:
			Result = TValue::From(PyFloat_AsDouble(O));
			break;

		case tkRecord:
		{
			TRttiType *Type = Context.GetType(TypeInfo);
			std::vector<BYTE> Data(Type->TypeSize);
			DynamicArray<TRttiField*> Fields = Type->GetFields();
			if(PyTuple_Size(O) != Fields.Length)
			  throw EPyVclError("Expected tuple with " + IntToStr(Fields.Length) + " elements");
			for(int I = 0; I < Fields.Length; I++)
				Fields[I]->SetValue(&Data[0], ToValue(PyTuple_GetItem(O, I), Fields[I]->FieldType->Handle));
			TValue::Make(&Data[0], TypeInfo, Result);
			break;
		}

		case tkInt64:
			Result = TValue::From(PyLong_AsLongLong(O));
			break;

		case tkPointer:
      if(AnsiString(TypeInfo->Name) == "PWideChar")
    		TValue::Make(reinterpret_cast<int>(PyUnicode_AsUnicode(O)), TypeInfo, Result);
      else
    		throw EPyVclError("Cannot convert Python object of type '" + String(O->ob_type->tp_name) + "' to '" + AnsiString(TypeInfo->Name) + "'");
      break;

		case tkVariant:
		case tkArray:
		case tkInterface:
		case tkDynArray:
		case tkClassRef:
		case tkProcedure:
		case tkUnknown:
		case tkMethod:
		default:
			throw EPyVclError("Cannot convert Python object of type '" + String(O->ob_type->tp_name) + "' to '" + AnsiString(TypeInfo->Name) + "'");
	}
	if(PyErr_Occurred())
		throw EPyVclError("Cannot convert Python object of type '" + String(O->ob_type->tp_name) + "' to '" + AnsiString(TypeInfo->Name) + "'");
	return Result;
}
//---------------------------------------------------------------------------
/** Convert the content for a tuple to a vector of TValue that can be passed to the Delphi RTTI system.
 *  \param O: This must be a tuple. Not checked.
 *  \param Values: vector filled with the return values.
 *  \param Parameters: This must be the expected types for the converted values. The number of elements 
 *         must be the same as the number of items in the tuple.
 */
void TupleToValues(PyObject *O, std::vector<TValue> &Values, const DynamicArray<TRttiParameter*> &Parameters)
{
	Values.clear();
	unsigned Size = PyTuple_Size(O);
	for(unsigned I = 0; I < Size; I++)
		Values.push_back(ToValue(PyTuple_GetItem(O, I), Parameters[I]->ParamType->Handle));
}
//---------------------------------------------------------------------------
PyObject* ToPyObject(bool Value)
{
	return PyBool_FromLong(Value);
}
//---------------------------------------------------------------------------
PyObject* ToPyObject(int Value)
{
	return PyLong_FromLong(Value);
}
//---------------------------------------------------------------------------
PyObject* ToPyObject(long long Value)
{
	return PyLong_FromLongLong(Value);
}
//---------------------------------------------------------------------------
PyObject* ToPyObject(wchar_t Value)
{
  return PyUnicode_FromUnicode(&Value, 1);
}
//---------------------------------------------------------------------------
PyObject* ToPyObject(double Value)
{
	return PyFloat_FromDouble(Value);
}
//---------------------------------------------------------------------------
PyObject* ToPyObject(const std::wstring &Str)
{
  return PyUnicode_FromUnicode(Str.c_str(), Str.size());
}
//---------------------------------------------------------------------------
PyObject* ToPyObject(const String &Str)
{
	return PyUnicode_FromUnicode(Str.c_str(), Str.Length());
}
//---------------------------------------------------------------------------
PyObject* ToPyObject(const Func32::TComplex &Value)
{
	return Value.imag() ? PyComplex_FromDoubles(Value.real(), Value.imag()) : PyFloat_FromDouble(Value.real());
}
//---------------------------------------------------------------------------
PyObject* ToPyObject(TObject *Object)
{
	return VclObject_Create(Object, false);
}
//---------------------------------------------------------------------------
PyObject* ToPyObject(const char *Str)
{
	return PyUnicode_DecodeASCII(Str, strlen(Str), NULL);
}
//---------------------------------------------------------------------------
PyObject* ToPyObject(const wchar_t *Str)
{
  return PyUnicode_FromWideChar(Str, -1);
}
//---------------------------------------------------------------------------
/** Convert a TValue from the Delphi RTTI system to a PyObject*.
 *  \param V: The value to convert.
 *  \return New reference to a converted object.
 *  \throw EPyVclError if the conversion fails.
 */
PyObject* ToPyObject(const Rtti::TValue &V)
{
	Rtti::TValue &Value = const_cast<Rtti::TValue&>(V);
	if(Value.IsEmpty)
		Py_RETURN_NONE;
	switch(Value.Kind)
	{
		case tkInteger:
			return ToPyObject(Value.AsInteger());

		case tkUString:
		case tkString:
		case tkLString:
		case tkWString:
			return ToPyObject(Value.AsString());

		case tkEnumeration:
		{
			if(AnsiString(Value.TypeInfo->Name) == "Boolean")
				return ToPyObject(static_cast<bool>(Value.AsOrdinal()));
			TRttiEnumerationType *Type = static_cast<TRttiEnumerationType*>(Context.GetType(Value.TypeInfo));
			int Ordinal = Value.AsOrdinal();
			if(Ordinal >= Type->MinValue && Ordinal <= Type->MaxValue)
			  //GetEnumName() lacks range check
				return ToPyObject(GetEnumName(Value.TypeInfo, Ordinal));
			return ToPyObject("0x" + IntToHex(Ordinal, 2));
		}

		case tkClass:
			return VclObject_Create(Value.AsObject(), false);

		case tkSet:
			return ToPyObject(SetToString(Value.TypeInfo, *static_cast<int*>(Value.GetReferenceToRawData()), false));

		case tkChar:
			return ToPyObject(Value.AsType<char>());

		case tkWChar:
			return ToPyObject(Value.AsType<wchar_t>());

		case tkFloat:
			return ToPyObject(Value.AsExtended());

		case tkRecord:
		{
			void *Data = Value.GetReferenceToRawData();
			TRttiType *Type = Context.GetType(Value.TypeInfo);
			DynamicArray<TRttiField*> Fields = Type->GetFields();
			PyObject *Tuple = PyTuple_New(Fields.Length);
			for(int I = 0; I < Fields.Length; I++)
				PyTuple_SET_ITEM(Tuple, I, ToPyObject(Fields[I]->GetValue(Data)));
			return Tuple;
		}

		case tkMethod: //Event
		{
			TMethod Method;
			Value.ExtractRawDataNoCopy(&Method);
			if(Method.Code == NULL)
				Py_RETURN_NONE;
			TObject *Object = static_cast<TObject*>(Method.Data);
			if(TMethodImplementation *Impl = dynamic_cast<TMethodImplementation*>(Object))
			{
				PyObject *Result = static_cast<PyObject*>(Impl->FUserData);
				Py_IncRef(Result);
				return Result;
			}
			String Name = Object->MethodName(Method.Code);
			TRttiType *Type = Context.GetType(Object->ClassType());
			return VclMethod_Create(NULL, Object, Type->GetMethods(Name));
		}

		case tkInt64:
			return PyLong_FromLongLong(Value.AsInt64());

		case tkVariant:
		case tkArray:
		case tkInterface:
		case tkDynArray:
		case tkClassRef:
		case tkPointer:
		case tkProcedure:
		case tkUnknown:
		default:
			throw EPyVclError("Unable to convert type '" + AnsiString(Value.TypeInfo->Name) + "'");
	}
}
//---------------------------------------------------------------------------
template<> double FromPyObject<double>(PyObject *O)
{
	if(PyComplex_Check(O))
    if(PyComplex_ImagAsDouble(O) != 0)
      PyErr_SetString(PyExc_TypeError, "complex number has an imaginary part");
		else
      return PyComplex_RealAsDouble(O);
  return PyFloat_AsDouble(O);
}
//---------------------------------------------------------------------------
template<> Func32::TComplex FromPyObject<Func32::TComplex>(PyObject *O)
{
  Py_complex V = PyComplex_AsCComplex(O);
	return Func32::TComplex(V.real, V.imag);
}
//---------------------------------------------------------------------------
template<> std::wstring FromPyObject<std::wstring>(PyObject *O)
{
	return PyUnicode_AsUnicode(O);
}
//---------------------------------------------------------------------------
template<> int FromPyObject<int>(PyObject *O)
{
	return PyLong_AsLong(O);
}
//---------------------------------------------------------------------------
/** Exception handling helper function. Called from a catch(...) section and converts an active
 *  C++/Delphi exception to a Python exception.
 */
PyObject* PyVclHandleException()
{
	try
	{
		throw;
	}
	catch(EListError &E)
	{
		SetErrorString(PyExc_IndexError, E.Message);
	}
	catch(Exception &E)
	{
		SetErrorString(PyVclException, E.Message);
	}
	catch(std::exception &E)
	{
		PyErr_SetString(PyVclException, E.what());
	}
	catch(...)
	{
		PyErr_SetString(PyVclException, "Unknown exception");
	}
	return NULL;
}
//---------------------------------------------------------------------------
} //namespace Python
