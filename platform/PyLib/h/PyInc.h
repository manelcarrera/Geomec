#ifndef _PyInc
#define _PyInc

#ifdef _DEBUG
  #undef _DEBUG
  #include <Python.h>
  #define _DEBUG
#else
  #include <Python.h>
#endif

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include "numpy/arrayobject.h"

#ifdef Py_RETURN_NONE
#undef Py_RETURN_NONE
#endif
extern PyObject *_Py_NoneStruct_ptr;

#define Py_RETURN_NONE return Py_INCREF(_Py_NoneStruct_ptr), _Py_NoneStruct_ptr

#endif

