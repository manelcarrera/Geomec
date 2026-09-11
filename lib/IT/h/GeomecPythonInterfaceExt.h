#ifndef _GeomecPythonInterfaceExt
#define _GeomecPythonInterfaceExt

template< class T >
class SafeQueue;

#include "PyTypes.h"	// PyObject, PyTuple

class GeomecPythonInterfaceExt
{

	//static const TCModule CModuleDefault;

public:

	GeomecPythonInterfaceExt( void );
	~GeomecPythonInterfaceExt( void );

	static const char* C_MODULE;
	static PyMethodDef GeomecMethods[];

	static PyTuple* func( PyObject *self, PyObject *args );
	static PyTuple* reservoir( PyObject *self, PyObject *args );
	static PyTuple* displacement( PyObject *self, PyObject *args );
};

#endif //_GeomecPythonInterfaceExt
