#include "PyLibHandler.h"


const std::string PyLibHandler::DEFAULT_FILE = "e:\\Anaconda2\\python27.dll";

std::string PyLibHandler::m_file = PyLibHandler::DEFAULT_FILE;


//typedef ConfigFile config;
//std::string PYTHONPATH_s = config::instance().getValueOfKey<std::string>("PYTHONPATH"); 
//_putenv_s("PYTHONPATH", PYTHONPATH_s.c_str() ); 



//typedef struct _object PyObject;
//struct PyMethodDef;
//typedef PyObject PyTuple;

/*static const int DLL_FUNCTIONS_N = 100;
typedef int (__stdcall* func_ptr_t)();
func_ptr_t g_func_ptr[ DLL_FUNCTIONS_N ];

enum eFuncId
{
	e_Py_Initialize,
	e_PyArg_ParseTuple,
	e_Py_InitModule
};

const char* DLL_FUNCTION_NAMES [DLL_FUNCTIONS_N] = 
{
  "Py_Initialize",
  "PyArg_ParseTuple",
  "Py_InitModule4_64"
};

typedef struct
{
  int  (__stdcall* f_Py_Initialize)();
  int  (__stdcall* f_PyArg_ParseTuple)(PyObject *, const char *, ...);
  PyObject* (__stdcall* f_Py_InitModule4)(const char *name, PyMethodDef *methods, const char *doc, PyObject *self, int apiver);
} functions_struct;

typedef union
{
  functions_struct  by_type;
  func_ptr_t        func_ptr [DLL_FUNCTIONS_N];
} functions_union;

functions_union functions;

#define dll_Py_Initialize		(functions.by_type.f_Py_Initialize)		// _ptr
#define dll_PyArg_ParseTuple	(functions.by_type.f_PyArg_ParseTuple)
#define dll_Py_InitModule4			(functions.by_type.f_Py_InitModule4)*/

//f_Py_Initialize Py_Initialize;



//https://stackoverflow.com/questions/297654/what-is-stdcall

//define PyAPI_FUNC(RTYPE) __declspec(dllimport) RTYPE
//PyAPI_DATA(RTYPE) extern __declspec(dllimport) RTYPE

//__stdcall : standard
//__cdecl, __thiscall, __fastcall, __naked

typedef int (__stdcall *f_PyArg_ParseTuple)(PyObject *args, const char * format, ...);

typedef int (__stdcall *f_PyArg_VaParse)(PyObject *args, const char * format, va_list vargs);

//typedef PyAPI_FUNC(void) (__stdcall *f_Py_Initialize)();
typedef void (__stdcall *f_Py_Initialize)();

typedef PyObject* (__stdcall *f_Py_InitModule4_64)(const char *name, PyMethodDef *methods, const char *doc, PyObject *self, int apiver);

//__declspec(dllimport) void PySys_SetArgv(int, char **)
//PyAPI_FUNC(void) PySys_SetArgv(int, char **);
//extern "C" { typedef void (__stdcall *f_PySys_SetArgv)(int, char **); }

typedef void (*f_PySys_SetArgv)(int, char **);
typedef long (__stdcall *f_PyInt_AsLong)(PyObject *obj);
typedef char* (__stdcall *f_PyString_AsString)(PyObject *obj);
typedef PyObject * (__stdcall *f_Py_BuildValue)(const char *format, ...);
typedef PyObject * (__stdcall *f_Py_VaBuildValue)(const char *format, va_list vargs);
typedef PyObject * (__stdcall *f_PyTuple_GetItem)(PyObject *, Py_ssize_t);
typedef PyObject * (__stdcall *f_PyTuple_New)(Py_ssize_t size);
typedef int (__stdcall *f_PyTuple_SetItem)(PyObject* obj, Py_ssize_t size, PyObject *obj2);
typedef PyObject * (__stdcall *f_PyString_FromString)(const char * cp);
typedef PyObject * (__stdcall *f_PyImport_Import)(PyObject *name);
typedef PyObject * (__stdcall *f_PyObject_GetAttrString)(PyObject *obj, const char *cp);
typedef int (__stdcall *f_PyCallable_Check)(PyObject *obj);
typedef PyObject * (__stdcall *f_PyObject_CallObject)(PyObject *callable_object, PyObject *args);
typedef PyObject * (__stdcall *f_PyErr_Occurred)(void);
typedef void (__stdcall *f_PyErr_Print)(void);
typedef void* (__stdcall *f_PyCObject_AsVoidPtr)(PyObject *);
typedef void (__stdcall *f_PyErr_SetString)(PyObject *obj, const char *cp);
typedef PyObject * (__stdcall *f_PyErr_Format)(PyObject *, const char *, ...);
typedef void (__stdcall *f_Py_Finalize)(void);
typedef PyObject* (__stdcall *f_PyImport_ImportModule)(const char *name);
typedef void (__stdcall *f_PyEval_InitThreads)(void);
typedef PyThreadState* (__stdcall *f_Py_NewInterpreter)(void);
typedef void (__stdcall *f_Py_EndInterpreter)(PyThreadState*);
//PyAPI_FUNC(int) PyRun_SimpleStringFlags(const char *, PyCompilerFlags *);
typedef int (__stdcall *f_PyRun_SimpleStringFlags)(const char *, PyCompilerFlags *);
typedef void (__stdcall *f_PyErr_Fetch)(PyObject **ptype, PyObject **pvalue, PyObject **ptraceback);


//typedef PyObject _Py_NoneStruct;
//PyAPI_DATA(PyObject) _Py_NoneStruct
// -> extern __declspec(dllimport) PyObject _Py_NoneStruct
//typedef PyObject (__stdcall *f__Py_NoneStruct)();

typedef PyObject* f__Py_NoneStruct;
typedef PyObject f__Py_NoneStruct_obj;
//f__Py_NoneStruct_obj _Py_NoneStruct = *( f__Py_NoneStruct )GetProcAddress( PyLibHandler::instance().handler(), "_Py_NoneStruct" );
f__Py_NoneStruct _Py_NoneStruct_ptr;
//PyObject _Py_NoneStruct;
namespace pyd
{

typedef PyTypeObject* f_PyCObject_Type;
typedef PyTypeObject f_PyCObject_Type_obj;
//f_PyCObject_Type_obj PyCObject_Type = *( f_PyCObject_Type )GetProcAddress( PyLibHandler::instance().handler(), "PyCObject_Type" );
f_PyCObject_Type_obj PyCObject_Type;

typedef PyObject* f_PyExc_AttributeError;
//f_PyExc_AttributeError PyExc_AttributeError = ( f_PyExc_AttributeError )GetProcAddress( PyLibHandler::instance().handler(), "PyExc_AttributeError" );
f_PyExc_AttributeError PyExc_AttributeError;

typedef PyObject* f_PyExc_ImportError;
//f_PyExc_ImportError PyExc_ImportError = ( f_PyExc_ImportError )GetProcAddress( PyLibHandler::instance().handler(), "PyExc_ImportError" );
f_PyExc_ImportError PyExc_ImportError;

typedef PyObject* f_PyExc_RuntimeError;
//f_PyExc_RuntimeError PyExc_RuntimeError = ( f_PyExc_RuntimeError )GetProcAddress( PyLibHandler::instance().handler(), "PyExc_RuntimeError" );
f_PyExc_RuntimeError PyExc_RuntimeError;



void Py_InitVars()
{
	_Py_NoneStruct_ptr = ( f__Py_NoneStruct )GetProcAddress( PyLibHandler::instance().handler(), "_Py_NoneStruct" );
	PyCObject_Type = *( f_PyCObject_Type )GetProcAddress( PyLibHandler::instance().handler(), "PyCObject_Type" );
	PyExc_AttributeError = ( f_PyExc_AttributeError )GetProcAddress( PyLibHandler::instance().handler(), "PyExc_AttributeError" );
	PyExc_ImportError = ( f_PyExc_ImportError )GetProcAddress( PyLibHandler::instance().handler(), "PyExc_ImportError" );
	PyExc_RuntimeError = ( f_PyExc_RuntimeError )GetProcAddress( PyLibHandler::instance().handler(), "PyExc_RuntimeError" );
}

}


/*std::string dll_full_path = "e:\\python27.dll";
HINSTANCE m_hGetProcIDDLL = LoadLibrary( dll_full_path.c_str() );
//HINSTANCE m_hGetProcIDDLL = PyLibHandler::instance().handler();
f_Py_Initialize local_Py_Initialize = ( f_Py_Initialize )GetProcAddress(m_hGetProcIDDLL, "Py_Initialize" );
int a=0;
#define Py_Initialize() local_Py_Initialize();*/

/*
f_PyArg_ParseTuple local_PyArg_ParseTuple	= ( f_PyArg_ParseTuple )GetProcAddress( PyLibHandler::instance().handler(), "PyArg_ParseTuple" );
#define PyArg_ParseTuple(tuple,cp,...) local_PyArg_ParseTuple( tuple,cp,__VA_ARGS__ );
*/


//PyAPI_FUNC(PyThreadState *) PyEval_SaveThread(void);
typedef PyThreadState* (__stdcall *f_PyEval_SaveThread)(void);
//PyAPI_FUNC(void) PyEval_RestoreThread(PyThreadState *);
typedef void (__stdcall *f_PyEval_RestoreThread)(PyThreadState*);

// MACROS:
// PyArray_SimpleNewFromData -> no need

namespace pyd
{

//typedef int (__stdcall *f_PyArg_ParseTuple)(PyObject *, const char *, ...);
int PyArg_VaParse( PyObject* args , const char* format, va_list vargs )
{
	f_PyArg_VaParse PyArg_VaParse_ptr = ( f_PyArg_VaParse )GetProcAddress( PyLibHandler::instance().handler(), "PyArg_VaParse" );
	return PyArg_VaParse_ptr( args, format, vargs );
}
int PyArg_ParseTuple( PyObject* args , const char* format, ... )
{
	va_list vargs;
    va_start(vargs, format);
	int res = pyd::PyArg_VaParse( args, format, vargs );
    va_end(vargs);
	return res;
}
void Py_Initialize()
{
	f_Py_Initialize Py_Initialize_ptr = ( f_Py_Initialize )GetProcAddress( PyLibHandler::instance().handler(), "Py_Initialize" );
	Py_Initialize_ptr();
}
PyObject* Py_InitModule4_64( const char *name, PyMethodDef *methods, const char *doc, PyObject *self, int apiver )
{
	f_Py_InitModule4_64 Py_InitModule4_64_ptr = ( f_Py_InitModule4_64 )GetProcAddress( PyLibHandler::instance().handler(), "Py_InitModule4_64" );
	return Py_InitModule4_64_ptr(  name, methods, doc, self, apiver  );
}
void PySys_SetArgv(int argc, char **argv)
{
	f_PySys_SetArgv PySys_SetArgv_ptr = ( f_PySys_SetArgv )GetProcAddress( PyLibHandler::instance().handler(), "PySys_SetArgv" );
	PySys_SetArgv_ptr(argc,argv);
}
long PyInt_AsLong(PyObject *obj)
{
	f_PyInt_AsLong PyInt_AsLong_ptr = ( f_PyInt_AsLong )GetProcAddress( PyLibHandler::instance().handler(), "PyInt_AsLong" );
	return PyInt_AsLong_ptr(obj);
}
char* PyString_AsString(PyObject *obj)
{
	f_PyString_AsString PyString_AsString_ptr = ( f_PyString_AsString )GetProcAddress( PyLibHandler::instance().handler(), "PyString_AsString" );
	return PyString_AsString_ptr(obj);
}
PyObject* Py_VaBuildValue( const char* cp, va_list vargs )
{
	f_Py_VaBuildValue Py_VaBuildValue_ptr = ( f_Py_VaBuildValue )GetProcAddress( PyLibHandler::instance().handler(), "Py_VaBuildValue" );
	return Py_VaBuildValue_ptr( cp, vargs );
}
PyObject* Py_BuildValue( const char* cp, ... )
{
	va_list args;
    va_start(args, cp);
	PyObject* res = Py_VaBuildValue( cp, args );
    va_end(args);
	return res;
}
PyObject* PyTuple_GetItem(PyObject *obj, Py_ssize_t pos)
{
	f_PyTuple_GetItem PyTuple_GetItem_ptr = ( f_PyTuple_GetItem )GetProcAddress( PyLibHandler::instance().handler(), "PyTuple_GetItem" );
	return PyTuple_GetItem_ptr(obj,pos);
}
PyObject* PyTuple_New(Py_ssize_t size)
{
	f_PyTuple_New PyTuple_New_ptr = ( f_PyTuple_New )GetProcAddress( PyLibHandler::instance().handler(), "PyTuple_New" );
	return PyTuple_New_ptr(size);
}
int PyTuple_SetItem(PyObject* obj, Py_ssize_t size, PyObject *obj2)
{
	f_PyTuple_SetItem PyTuple_SetItem_ptr = ( f_PyTuple_SetItem )GetProcAddress( PyLibHandler::instance().handler(), "PyTuple_SetItem" );
	return PyTuple_SetItem_ptr(obj, size, obj2);
}
PyObject* PyString_FromString(const char * cp)
{
	f_PyString_FromString PyString_FromString_ptr = ( f_PyString_FromString )GetProcAddress( PyLibHandler::instance().handler(), "PyString_FromString" );
	return PyString_FromString_ptr(cp);
}
PyObject* PyImport_Import(PyObject *name)
{
	f_PyImport_Import PyImport_Import_ptr = ( f_PyImport_Import )GetProcAddress( PyLibHandler::instance().handler(), "PyImport_Import" );
	return PyImport_Import_ptr(name);
}
PyObject* PyObject_GetAttrString(PyObject *obj, const char *cp)
{
	f_PyObject_GetAttrString PyObject_GetAttrString_ptr = ( f_PyObject_GetAttrString )GetProcAddress( PyLibHandler::instance().handler(), "PyObject_GetAttrString" );
	return PyObject_GetAttrString_ptr(obj,cp);
}
int PyCallable_Check(PyObject *obj)
{
	f_PyCallable_Check PyCallable_Check_ptr = ( f_PyCallable_Check )GetProcAddress( PyLibHandler::instance().handler(), "PyCallable_Check" );
	return PyCallable_Check_ptr(obj);
}
PyObject * PyObject_CallObject(PyObject *callable_object, PyObject *args)
{
	f_PyObject_CallObject PyObject_CallObject_ptr = ( f_PyObject_CallObject )GetProcAddress( PyLibHandler::instance().handler(), "PyObject_CallObject" );
	return PyObject_CallObject_ptr(callable_object,args);
}
PyObject * PyErr_Occurred(void)
{
	f_PyErr_Occurred PyErr_Occurred_ptr = ( f_PyErr_Occurred )GetProcAddress( PyLibHandler::instance().handler(), "PyErr_Occurred" );
	return PyErr_Occurred_ptr();
}
void PyErr_Print(void)
{
	f_PyErr_Print PyErr_Print_ptr = ( f_PyErr_Print )GetProcAddress( PyLibHandler::instance().handler(), "PyErr_Print" );
	PyErr_Print_ptr();
}
void* PyCObject_AsVoidPtr(PyObject * obj)
{
	f_PyCObject_AsVoidPtr PyCObject_AsVoidPtr_ptr = ( f_PyCObject_AsVoidPtr )GetProcAddress( PyLibHandler::instance().handler(), "PyCObject_AsVoidPtr" );
	return PyCObject_AsVoidPtr_ptr(obj);
}
void PyErr_SetString(PyObject * obj, const char *cp)
{
	f_PyErr_SetString PyErr_SetString_ptr = ( f_PyErr_SetString )GetProcAddress( PyLibHandler::instance().handler(), "PyErr_SetString" );
	PyErr_SetString_ptr(obj,cp);
}
PyObject* PyErr_Format( PyObject *obj, const char *format, ... )
{
	char buf[ 200 ];     // this should really be sized appropriately
						// possibly in response to a call to vsnprintf()
    va_list vl;
    va_start( vl, format );
    vsnprintf( buf, sizeof( buf ), format, vl );
    va_end( vl );
	pyd::PyErr_SetString( obj, buf);
	return nullptr;
}
void Py_Finalize(void)
{
	f_Py_Finalize Py_Finalize_ptr = ( f_Py_Finalize )GetProcAddress( PyLibHandler::instance().handler(), "Py_Finalize" );
	Py_Finalize_ptr();
}
PyObject* PyImport_ImportModule(const char *name)
{
	f_PyImport_ImportModule PyImport_ImportModule_ptr = ( f_PyImport_ImportModule )GetProcAddress( PyLibHandler::instance().handler(), "PyImport_ImportModule" );
	return PyImport_ImportModule_ptr( name );
}
PyThreadState* PyEval_SaveThread()
{
	f_PyEval_SaveThread PyEval_SaveThread_ptr = ( f_PyEval_SaveThread )GetProcAddress( PyLibHandler::instance().handler(), "PyEval_SaveThread" );
	return PyEval_SaveThread_ptr();
}
void PyEval_RestoreThread( PyThreadState* state)
{
	f_PyEval_RestoreThread PyEval_RestoreThread_ptr = ( f_PyEval_RestoreThread )GetProcAddress( PyLibHandler::instance().handler(), "PyEval_RestoreThread" );
	return PyEval_RestoreThread_ptr( state );
}
/*void* PyCapsule_GetPointer(PyObject *capsule, const char *name)
{
	f_PyCapsule_GetPointer PyCapsule_GetPointer_ptr = ( f_PyCapsule_GetPointer )GetProcAddress( PyLibHandler::instance().handler(), "PyCapsule_GetPointer" );
	return PyCapsule_GetPointer_ptr( capsule, name );
}*/
void PyEval_InitThreads(void)
{
	f_PyEval_InitThreads PyEval_InitThreads_ptr = ( f_PyEval_InitThreads )GetProcAddress( PyLibHandler::instance().handler(), "PyEval_InitThreads" );
	PyEval_InitThreads_ptr();
}
PyThreadState* Py_NewInterpreter(void)
{
	f_Py_NewInterpreter Py_NewInterpreter_ptr = ( f_Py_NewInterpreter )GetProcAddress( PyLibHandler::instance().handler(), "Py_NewInterpreter" );
	return Py_NewInterpreter_ptr();
}
void Py_EndInterpreter(PyThreadState* state)
{
	f_Py_EndInterpreter Py_EndInterpreter_ptr = ( f_Py_EndInterpreter )GetProcAddress( PyLibHandler::instance().handler(), "Py_EndInterpreter" );
	Py_EndInterpreter_ptr(state);
}
int PyRun_SimpleStringFlags(const char *cp, PyCompilerFlags *flags)
{
	f_PyRun_SimpleStringFlags PyRun_SimpleStringFlags_ptr = ( f_PyRun_SimpleStringFlags )GetProcAddress( PyLibHandler::instance().handler(), "PyRun_SimpleStringFlags" );
	return PyRun_SimpleStringFlags_ptr(cp, flags);
}

void PyErr_Fetch(PyObject **ptype, PyObject **pvalue, PyObject **ptraceback)
{
	f_PyErr_Fetch PyErr_Fetch_ptr = ( f_PyErr_Fetch )GetProcAddress( PyLibHandler::instance().handler(), "PyErr_Fetch" );
	PyErr_Fetch_ptr(ptype, pvalue, ptraceback);
}

} // namespace pyd


