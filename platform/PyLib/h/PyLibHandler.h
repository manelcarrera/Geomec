#ifndef _PyLibHandler
#define _PyLibHandler

#include "PyInc.h"

// #include "modsupport.h"

#include <iostream>
#include <windows.h>

// https://stackoverflow.com/questions/1008019/c-singleton-design-pattern
class PyLibHandler {
  static const std::string PyLibHandler::DEFAULT_FILE;
  static std::string m_file;
  HINSTANCE m_hGetProcIDDLL;

public:
  static void file(std::string val) { m_file = val; }
  static std::string file() { return m_file; }

  static PyLibHandler &instance() {
    static PyLibHandler instance; // Guaranteed to be destroyed.
                                  // Instantiated on first use.
    return instance;
  }
  void load(std::string file = m_file) {
    if (m_hGetProcIDDLL)
      free();
    m_hGetProcIDDLL = LoadLibrary(m_file.c_str());
  }
  bool free() {
    if (m_hGetProcIDDLL)
      return FreeLibrary(m_hGetProcIDDLL) == TRUE;
    else
      return false; // FIXME
  }
  HINSTANCE &handler() {
    if (!m_hGetProcIDDLL)
      load();
    return m_hGetProcIDDLL;
  }
  /*void handler( HINSTANCE _handler )
  {
    m_hGetProcIDDLL = _handler;
  }*/
private:
  PyLibHandler() { load(); }

  /*        PythonHandle(PythonHandle const&);              // Don't Implement
      void operator=(PythonHandle const&); // Don't implement

    public:
      PythonHandle(PythonHandle const&)    = delete;
      void operator=(PythonHandle const&)  = delete;*/
};

namespace pyd {
void Py_InitVars();
int PyArg_VaParse(PyObject *args, const char *format, va_list vargs);
int PyArg_ParseTuple(PyObject *args, const char *format, ...);
void Py_Initialize();
PyObject *Py_InitModule4_64(const char *name, PyMethodDef *methods, const char *doc, PyObject *self, int apiver);
void PySys_SetArgv(int argc, char **argv);
long PyInt_AsLong(PyObject *obj);
char *PyString_AsString(PyObject *obj);
PyObject *Py_VaBuildValue(const char *cp, va_list vargs);
PyObject *Py_BuildValue(const char *cp, ...);
PyObject *PyTuple_GetItem(PyObject *obj, Py_ssize_t pos);
PyObject *PyTuple_New(Py_ssize_t size);
int PyTuple_SetItem(PyObject *obj, Py_ssize_t size, PyObject *obj2);
PyObject *PyString_FromString(const char *cp);
PyObject *PyImport_Import(PyObject *name);
PyObject *PyObject_GetAttrString(PyObject *obj, const char *cp);
int PyCallable_Check(PyObject *obj);
PyObject *PyObject_CallObject(PyObject *callable_object, PyObject *args);
PyObject *PyErr_Occurred(void);
void PyErr_Print(void);
void *PyCObject_AsVoidPtr(PyObject *obj);
void PyErr_SetString(PyObject *obj, const char *cp);
PyObject *PyErr_Format(PyObject *obj, const char *format, ...);
void Py_Finalize(void);
PyObject *PyImport_ImportModule(const char *name);
PyThreadState *PyEval_SaveThread();
void PyEval_RestoreThread(PyThreadState *state);
void PyEval_InitThreads(void);
PyThreadState *Py_NewInterpreter(void);
void Py_EndInterpreter(PyThreadState *state);
int PyRun_SimpleStringFlags(const char *cp, PyCompilerFlags *flags);
void PyErr_Fetch(PyObject **ptype, PyObject **pvalue, PyObject **ptraceback);

#define Py_BEGIN_ALLOW_THREADS                                                                                         \
  {                                                                                                                    \
    PyThreadState *_save;                                                                                              \
    _save = PyEval_SaveThread();

#define Py_BLOCK_THREADS PyEval_RestoreThread(_save);
#define Py_UNBLOCK_THREADS _save = PyEval_SaveThread();

#define Py_END_ALLOW_THREADS                                                                                           \
  PyEval_RestoreThread(_save);                                                                                         \
  }

#define Py_InitModule(name, methods) Py_InitModule4(name, methods, (char *)NULL, (PyObject *)NULL, PYTHON_API_VERSION)

// Py_InitModule4_64

} // namespace pyd

#endif