#ifndef _GeomecPythonInterface
#define _GeomecPythonInterface

template< class T >
class SafeQueue;

#include "PyTypes.h"
#include "IGmInterface.h"
#include "IT_Command.h"

class GeomecPythonInterface : public IGmInterface
{

public:
  static SafeQueue< Command > m_requests_queue;
  static SafeQueue< Command > m_results_queue;

  virtual SafeQueue< Command >& requests_queue(){ return m_requests_queue; };
  virtual SafeQueue< Command >& results_queue(){ return m_results_queue; };

  bool m_processing;

private:
  /* 
    Format: vector: 
    0:		cmd_id 
    1..n:	py objects that can be: 
          (1) int, 
          (2) double, 
          (3) string or 
          (4) a numpy array

    puts everything in a tuple before sending to python side
  */
  static PyTuple* _build_result( Command cmd );
  static PyTuple* _build_result_ack( Command cmd );
  static PyTuple* _build_result_nack( Command cmd );

  //static std::pair<IT_Data::TDataInput,IT_Data::TSettings> GeomecPythonInterface::_parse_params( TPyObjectPtrV& py_params_v );


public:

  GeomecPythonInterface() : m_processing( false ){};
  ~GeomecPythonInterface(){};

  /*
  https://docs.python.org/2/c-api/structures.html

  -> PyMethodDef

  Field		C Type			Meaning
  -------		------------	----------------------------------------------
  ml_name		char *			name of the method
  ml_meth		PyCFunction		pointer to the C implementation
  ml_flags	int				flag bits indicating how the call should be constructed
  ml_doc		char *			points to the contents of the docstring


  -> Py_InitModule

  PyObject* Py_InitModule(char *name, PyMethodDef *methods)
  Create a new module object based on a name and table of functions, returning the new module object.

  */
  static const char* C_MODULE;
  static PyMethodDef GeomecMethods[];

  const char* module(){ return C_MODULE; };
  PyMethodDef* methods(){ return GeomecMethods; };

  static PyObject* get_result( PyObject *self, PyObject *args );
  static PyObject* send_cmd(	PyObject *self, PyObject *args );
};





#endif //_GeomecPythonInterface
