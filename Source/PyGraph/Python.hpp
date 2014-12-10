/* This is a wrapper around Python.h
 * Python.h is interpretting the define _DEBUG is we are using a debug version of Python, though we are not.
 * So we undef _DEBUG while including Python.h and redefine it afterwards if necessary. */
#ifdef _DEBUG
#define OLD_DEBUG _DEBUG
#undef _DEBUG
#endif

#include "Python.h"

#ifdef OLD_DEBUG
#define _DEBUG OLD_DEBUG
#endif
