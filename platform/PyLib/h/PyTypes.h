#ifndef _PyTypes
#define _PyTypes

typedef struct _object PyObject;
struct PyMethodDef;
typedef PyObject PyTuple;

#include <vector>
typedef std::vector< PyObject* > TPyObjectPtrV;

#endif

