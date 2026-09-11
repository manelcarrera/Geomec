#ifndef _PythonWrapper
#define _PythonWrapper

#include "Data.h"
#include "PyTypes.h"

typedef std::vector< PyObject* > TPyObjectPtrV;

typedef struct {	std::string module;
          std::string function;
          std::string param;// = std::string();
          PyTuple* param_py;// = nullptr;
          } TPyModule;

typedef struct {	std::string name; 
          PyMethodDef* methods; } TCModule;

typedef std::vector< TCModule > TCModuleV;

typedef struct {	int cmd_id; 
          PyTuple* tuple_ptr; } TCommandP;

class PyWrapper
{
  //----------------------------------------------------- parse (tuple)
  static int _parse_int(				PyTuple *tuple );
  static double _parse_double(		PyTuple *tuple );
  static TCommandP _parse_cmd(		PyTuple *tuple );
  static char* _parse_char_ptr(		PyTuple *tuple );
  static std::string _parse_string(	PyTuple *tuple );

  static TData _parse_array(	PyTuple *tuple );

  static TPyObjectPtrV _parse_tuple(	PyTuple *tuple );
  //----------------------------------------------------- parse (obj)
  static int _parse_obj_int(				PyObject* obj );
  static double _parse_obj_double(		PyObject* obj );
  static TData _parse_obj_array(	PyObject* obj );
  static std::string _parse_obj_string(	PyObject* obj );

  //----------------------------------------------------- build : FIXME: check out which ones of these methods already constructs a tuple
  static PyObject* _build_int(		int i );
  static PyObject* _build_double(		double i );
  static PyObject* _build_char_ptr(	const char* cp );
  //static PyObject* _build_cmd(		Command cmd );
  static PyObject* _build_obj(		void* _obj );
  static PyObject* _build_array(		TData array );
  //-----------------------------------------------------
  static PyObject* _build_array(		size_t rows, size_t cols, TMemoryChunkPtr memory_chunck, DataType data_type = DataType::Double );
  static PyObject* _build_tuple(		TPyObjectPtrV objs );

public: //FIXME

  static PyObject* get_tuple_item( PyTuple *tuple, int idx );


public:

  PyWrapper(){};

  //----------------------------------------- parse: tuple
  template< typename T >
  static T parse( PyTuple *tuple );

  template<>
  static int parse< int >( PyTuple *tuple ){					return _parse_int( tuple ); }

  template<>
  static double parse< double >( PyTuple *tuple ){				return _parse_double( tuple ); }

  template<>
  static char* parse< char* >( PyTuple *tuple ){				return _parse_char_ptr( tuple ); }

  template<>
  static std::string parse< std::string >( PyTuple *tuple ){	return _parse_string( tuple ); }

  /*template<>
  static Command  parse< Command >( PyTuple *tuple ){			return _parse_cmd( tuple ); }*/

  template<>
  static TCommandP  parse< TCommandP >( PyTuple *tuple ){			return _parse_cmd( tuple ); }

  template<>
  static TData  parse< TData >( PyTuple *tuple ){ return _parse_array( tuple ); }

  template<>
  static TPyObjectPtrV  parse< TPyObjectPtrV >( PyTuple *tuple ){ return _parse_tuple( tuple ); }

  //----------------------------------------- parse: object

  template< typename T >
  static T parse_obj( PyObject *obj );

  template<>
  static int parse_obj< int >( PyObject *obj ){					return _parse_obj_int( obj ); }

  template<>
  static double parse_obj< double >( PyObject *obj ){				return _parse_obj_double( obj ); }

  template<>
  static std::string parse_obj< std::string >( PyObject *obj ){	return _parse_obj_string( obj ); }

  template<>
  static TData  parse_obj< TData >( PyObject *obj ){ return _parse_obj_array( obj ); }

  //----------------------------------------- build : obj
  template< typename T >
  static PyObject* build( T t );

  template<>
  static PyObject* build< int >( int i ){			return _build_int( i ); }

  template<>
  static PyObject* build< double >( double i ){			return _build_double( i ); }

  template<>
  static PyObject* build< const char* >( const char* cp ){	return _build_char_ptr( cp ); }

  template<>
  static PyObject* build< std::string >( std::string s ){	return _build_char_ptr( s.c_str() ); }

  //template<>
  //static PyObject* build< Command >( Command cmd ){ return _build_cmd( cmd ); }

  template<>
  static PyObject*  build< TData >( TData array ){ return _build_array( array ); } //FIXME

  //----------------------------------------- build : tuple
  template<>
  static PyTuple*  build< TPyObjectPtrV >( TPyObjectPtrV obj_v ){ return _build_tuple( obj_v ); }

  //----------------------------------------- more

  static PyObject* none();
  static void xdecref( PyObject* );
  static void decref( PyObject* );

  //------------------------------------------- C++ uses Python

  static PyObject* execute(	TPyModule py_module,
                TCModuleV c_modules_v={},
                int (*_callback)(PyTuple*)  = nullptr );

  static void finalize( void );
};

#endif //_PythonWrapper