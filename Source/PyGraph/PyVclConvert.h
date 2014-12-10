//---------------------------------------------------------------------------
#ifndef PyVclConvertH
#define PyVclConvertH
#include "PythonBind.h"
//---------------------------------------------------------------------------
namespace Rtti
{
	struct TValue;
	struct TRttiContext;
	struct TRttiParameter;
}

namespace Python
{
	class EPyVclError : public Exception
	{
	public:
		EPyVclError(const String &Str) : Exception(Str) {}
	};

	extern Rtti::TRttiContext Context;
	extern _object *PyVclException;

	Rtti::TValue ToValue(_object *O, TTypeInfo *TypeInfo);
	void TupleToValues(_object *O, std::vector<Rtti::TValue> &Values, const DynamicArray<Rtti::TRttiParameter*> &Parameters);

	PyObject* ToPyObject(bool Value);
	PyObject* ToPyObject(int Value);
	PyObject* ToPyObject(long long Value);
	PyObject* ToPyObject(wchar_t Value);
	PyObject* ToPyObject(unsigned Value) {return ToPyObject(static_cast<int>(Value));}
	PyObject* ToPyObject(double Value);
	PyObject* ToPyObject(long double Value) {return ToPyObject(static_cast<double>(Value));}
	PyObject* ToPyObject(const std::wstring &Str);
	PyObject* ToPyObject(const String &Str);
	PyObject* ToPyObject(const Func32::TComplex &Value);
	PyObject* ToPyObject(const Rtti::TValue &Value);
	PyObject* ToPyObject(TObject *Object);
	PyObject* ToPyObject(const char *Str);
	PyObject* ToPyObject(const wchar_t *Str);

	template<typename T> T FromPyObject(PyObject *O);
	template<> int FromPyObject<int>(PyObject *O);
	template<> double FromPyObject<double>(PyObject *O);
	template<> unsigned FromPyObject<unsigned>(PyObject *O) {return FromPyObject<int>(O);}
	template<> TColor FromPyObject<TColor>(PyObject *O) {return static_cast<TColor>(FromPyObject<int>(O));}
	template<> long double FromPyObject<long double>(PyObject *O) {return FromPyObject<double>(O);}
	template<> Func32::TComplex FromPyObject<Func32::TComplex>(PyObject *O);
	template<> std::wstring FromPyObject<std::wstring>(PyObject *O);
	template<typename T> bool FromPyObject(PyObject *O, T &Value) {Value = FromPyObject<T>(O); return !PyErr_Occurred();}

	PyObject* PyVclHandleException();
}
#endif
