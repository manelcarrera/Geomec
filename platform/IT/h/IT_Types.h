#ifndef _IT_Types
#define _IT_Types

#include <vector> 

typedef struct _object PyObject;
struct PyMethodDef;
typedef PyObject PyTuple;


typedef std::vector< PyObject* > TPyObjectPtrV;

typedef struct {	std::string module;
          std::string function;
          std::string param; } TPyModule;

typedef struct {	std::string name; 
          PyMethodDef* methods; } TCModule;

typedef std::vector< TCModule > TCModuleV;



#endif //_IT_Types
