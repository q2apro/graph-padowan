//---------------------------------------------------------------------------
#ifndef PyVclRefH
#define PyVclRefH
//---------------------------------------------------------------------------
namespace Python
{
	extern PyTypeObject VclRefType;
	PyObject* VclRef_Create(TValue *Value);
}
#endif
