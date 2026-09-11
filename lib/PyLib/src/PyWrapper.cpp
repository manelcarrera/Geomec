#include "PyWrapper.h"
#include "Printer.h"
#include "PyLibHandler.h"
#include "LoadedLibs.h"
//#include "ConfigFile.h"
#include "PyInc.h"
#include <frameobject.h>

// Confussion with Qt file with the same name
//#include "object.h" //Py_RETURN_NONE

#include <string>

//typedef ConfigFile cfg;
typedef PyLibHandler lib;

namespace PythonObjectToBuild //https://www.tutorialspoint.com/python/python_further_extensions.htm
{
	static const char* String		= "s";
	static const char* Integer		= "i";
	static const char* Double		= "d";
	static const char* Object		= "O";

	//static const char* Reservoir	= "OOOO"; // not used

	static const char* Command	= "iO";
}

//------------------------------------------------------------------------------------ parse : tuple

char* PyWrapper::_parse_char_ptr( PyTuple *tuple )
{
	char* ignorestring;
	pyd::PyArg_ParseTuple(tuple, PythonObjectToBuild::String, &ignorestring ); //FIXME: what happens with the reference to the char*?
	return ignorestring;
}

std::string PyWrapper::_parse_string( PyTuple *tuple ){ return std::string( _parse_char_ptr( tuple ) ); }

int PyWrapper::_parse_int( PyTuple *tuple )
{
	int number;
	pyd::PyArg_ParseTuple( tuple, PythonObjectToBuild::Integer, &number );
	return number;
}

double PyWrapper::_parse_double( PyTuple *tuple )
{
	double number;
	pyd::PyArg_ParseTuple( tuple, PythonObjectToBuild::Double, &number );
	return number;
}

TCommandP PyWrapper::_parse_cmd( PyTuple *tuple )
{
	int cmd_id;
	//PyTuple* params;
	PyObject* params = nullptr;
	pyd::PyArg_ParseTuple( tuple, PythonObjectToBuild::Command, &cmd_id, &params );
	//return std::make_pair( (Command::eType)cmd_id, params );
	return TCommandP{ cmd_id, params };
}

PyObject* PyWrapper::get_tuple_item( PyTuple *tuple, int idx )
{
	PyObject* obj = nullptr;

	bool is_tuple = PyTuple_Check( tuple );

	if( is_tuple )
	{
		ssize_t size = Py_ssize_t PyTuple_GET_SIZE( tuple );

		if( idx < size )
			obj = pyd::PyTuple_GetItem( tuple,  idx );
	}
	return obj;
}


TData PyWrapper::_parse_array( PyTuple *tuple )
{
	PyObject* _obj = nullptr;
	if( pyd::PyArg_ParseTuple( tuple, PythonObjectToBuild::Object, &_obj ) ) //FIXME
		return _parse_obj_array( _obj );
	else
		return TData(); //FIXME
}

//------------------------------------------------------------------------------------ parse : obj
int PyWrapper::_parse_obj_int( PyObject* obj ){				return pyd::PyInt_AsLong( obj ); }
double PyWrapper::_parse_obj_double( PyObject* obj ){		return pyd::PyInt_AsLong( obj ); } //check this out
std::string PyWrapper::_parse_obj_string( PyObject* obj ){	return std::string( pyd::PyString_AsString( obj ) );}

TData PyWrapper::_parse_obj_array( PyObject *_obj )
{
	//FIXME
	PyArrayObject* np_ret	= reinterpret_cast< PyArrayObject* >( _obj );
	int dim					= PyArray_NDIM(		np_ret );
	// numpy from v1.7
	int len					= static_cast< int >( PyArray_SHAPE(	np_ret )[ 0 ] );
	int width				= static_cast< int >( PyArray_SHAPE(	np_ret )[ 1 ] ); // FIXME: what if it's just one dimension?
	// numpy previous to v1.7
	//int len					= PyArray_DIMS(	np_ret )[ 0 ];
	//int width				= PyArray_DIMS(	np_ret )[ 1 ]; // FIXME: what if it's just one dimension?
	double* c_out			= reinterpret_cast< double* >( PyArray_DATA( np_ret ) );

	return TData{ len, width, c_out, DataType::DoubleV }; //FIXME: dim != cols
}

//------------------------------------------------------------------------------------ parse : tuple

TPyObjectPtrV PyWrapper::_parse_tuple( PyTuple *tuple )
{
	TPyObjectPtrV obj_v;

	bool is_tuple = PyTuple_Check( tuple );

	if( is_tuple )
	{
		Py_ssize_t size = PyTuple_GET_SIZE( tuple );

		if( size == 1 ) // Format: one only param for cmd_id and params list -> send_cmd( (cmd_id, (param0, param1,) ) )
		{
			obj_v = _parse_tuple( pyd::PyTuple_GetItem( tuple,  0 ) );
		}
		else // Format: one params for cmd_id and a second param for the params list -> send_cmd( cmd_id, (param0, param1,) )
		{
			for( Py_ssize_t pos = 0; pos < size; pos++ )
				obj_v.push_back( pyd::PyTuple_GetItem( tuple,  pos ) );
		}
	}
	else
		obj_v.push_back( (PyObject*)tuple ); //actually it's not a tuple but an object

	return obj_v;
}


//------------------------------------------------------------------------------- build : obj

PyObject* PyWrapper::_build_int( int i ){					return pyd::Py_BuildValue( PythonObjectToBuild::Integer, i );	}					
PyObject* PyWrapper::_build_double( double i ){				return pyd::Py_BuildValue( PythonObjectToBuild::Double, i ); }					
PyObject* PyWrapper::_build_char_ptr( const char* cp ){		return pyd::Py_BuildValue( PythonObjectToBuild::String,	cp );	}					
//PyObject* PyWrapper::_build_cmd( Command cmd ){				return Py_BuildValue( PythonObjectToBuild::Command, cmd.cmd(), cmd.param() );	}	

//PyObject* PyWrapper::_build_obj( void* _obj ){				return Py_BuildValue( PythonObjectToBuild::Object, PyWrapper::tuple( (PyObject*)_obj ) ); }	
PyObject* PyWrapper::_build_obj( void* _obj ){				return pyd::Py_BuildValue( PythonObjectToBuild::Object, _obj ); }	

PyObject* PyWrapper::_build_array( size_t rows, size_t cols, TMemoryChunkPtr memory_chunck, DataType data_type )
{
	int ND = cols ? 2 : 1;

	npy_intp* dims = new npy_intp[ ND ]; //FIXME: delete
	
	dims[ 0 ] = rows;
	dims[ 1 ] = cols;

	//FIXME: correct way for linked LIB
	//import_array1( 0 ); // !!!!!!

	//FIXME: correct way if LIB is dinamically loaded
	PyObject *numpy = pyd::PyImport_ImportModule("numpy.core.multiarray");
	PyObject *c_api = pyd::PyObject_GetAttrString(numpy, "_ARRAY_API");
	PyArray_API = (void **)pyd::PyCObject_AsVoidPtr(c_api);


	//types py: https://docs.scipy.org/doc/numpy/reference/c-api.dtype.html

	PyArrayObject* np_array	= reinterpret_cast< PyArrayObject* >( 
		PyArray_SimpleNewFromData( 
			ND, dims,
			data_type == DataType::DoubleV ?	NPY_DOUBLE : NPY_INT, //NPY_STRING
			reinterpret_cast< void* >( memory_chunck ) ) );

	delete[] dims;

	return _build_obj( (void*)np_array );
}

//------------------------------------------------------------------------------- build : tuple
PyTuple* PyWrapper::_build_tuple( TPyObjectPtrV obj_v )
{
	PyObject* tuple = pyd::PyTuple_New( obj_v.size() );
	for( int i = 0; i < obj_v.size(); i++   )
		pyd::PyTuple_SetItem( tuple, i, reinterpret_cast< PyObject* >( obj_v[ i ] ) );
	return tuple;
}



PyObject* PyWrapper::_build_array( TData array ){ return _build_array( array.rows, array.cols, array.data, array.data_type ); }


//------------------------------------------------------------------------------- execute
PyObject* PyWrapper::execute(	TPyModule py_module,
									TCModuleV c_modules_v,
									int (*_callback)(PyTuple*) )
{
	Printer::instance()->info("CPython : execute : module:'%s' funcion:'%s' param:'%s' param_py:'%p'", py_module.module.c_str(), py_module.function.c_str(), py_module.param.c_str(), py_module.param_py );

	PyObject *pName, *pModule, *pFunc;
	PyObject *pArgs, *pValue;

	//CLoadedLibs libs;
	//libs.save();

	static bool is_init = false;

	//char* PYTHON_PATH_ENV_VAR;                                                                        
    //PYTHON_PATH_ENV_VAR = std::getenv( "PYTHONPATH" );  

	lib::instance().load(); // -> reload: (1)free + (2)load
	
	pyd::Py_InitVars();

	pyd::Py_Initialize();

	//std::ostringstream cmd_s;
	//cmd_s << "import sys; sys.path.append('" << PYTHONPATH_s << "')\n";
	//std::string copyOfStr = stringStream.str();
	//PyRun_SimpleString( cmd_s.str().c_str() );

	/*PyRun_SimpleString( "import sys" );
	PyRun_SimpleString( "sys.path.append('e:\Anaconda2\libs')" );
	PyRun_SimpleString( "sys.path.append('e:\Anaconda2\DLLs')" );
	PyRun_SimpleString( "sys.path.append('e:\Anaconda2\Lib')" );
	PyRun_SimpleString( "sys.path.append('e:\Anaconda2\Lib\site-packages')" );
	PyRun_SimpleString( "sys.path.append('e:\Anaconda2\Lib\lib-tk')" );
	PyRun_SimpleString( "sys.path.append('c:\dev\Geomec_IT\lib\InversionTool')" );*/


	//PyThreadState* mainThreadState = nullptr;
	//PyThreadState* myThreadState = nullptr;

	/*if( !is_init )
	{
		Py_Initialize();
		//PyEval_InitThreads();
		//mainThreadState = PyEval_SaveThread();
		//myThreadState = Py_NewInterpreter();
		//int a=1;
		is_init = true;
	}*/
	/*else
	{

		//PyEval_AcquireLock();
		myThreadState = Py_NewInterpreter(); //TODO: this doesn't work either
		int a=1;
	}*/

	//FIXME: why this ? Otherwise tkinter can't be instaantiated
	char* argv[ 100 ] = {"argv0","argv1"};
	int argc = 2;
	pyd::PySys_SetArgv( argc, argv );

	for( int i = 0; i < c_modules_v.size() ; i++ )
		//Py_InitModule( c_modules_v[ i ].name.c_str(), c_modules_v[ i ].methods );
		pyd::Py_InitModule4_64(c_modules_v[ i ].name.c_str(), c_modules_v[ i ].methods, (char *)NULL, (PyObject *)NULL, PYTHON_API_VERSION);

	pName = pyd::PyString_FromString( py_module.module.c_str() );

	pModule = pyd::PyImport_Import( pName );
	Py_DECREF( pName );

	if( pModule )
	{
		pFunc = pyd::PyObject_GetAttrString( pModule, py_module.function.c_str() );

		if( pFunc && pyd::PyCallable_Check( pFunc ) )
		{
			if( py_module.param_py )
			{
				pArgs = py_module.param_py;
			}
			else
			{
				if( py_module.param.empty() )
					pArgs = pyd::PyTuple_New( 0 );
				else //FIXME: This allows just one char* -> its functionallity is to limited so make it generic
					pArgs = PyWrapper::build< TPyObjectPtrV >( TPyObjectPtrV{ PyWrapper::build< const char* >( py_module.param.c_str() ) } ); 
			}

			pValue = pyd::PyObject_CallObject( pFunc, pArgs );
			Py_DECREF( pArgs );
			if( pValue )
			{
				if( py_module.param_py || !py_module.param.empty() )
				{
					if( _callback )				// not really used
						_callback( pValue );	// FIXME: just to compare results

					Printer::instance()->info("CPython : Ext Interface : return result");

					return pValue;	// this is the right way
									// FIXME: I need to Py_DECREF( pValue ) at the other side
									// IMPORTANT: http://stackoverflow.com/questions/27858519/how-to-copy-pyobject
				}
				else
				{
					//printf("Result of call: %ld\n", PyInt_AsLong( pValue ) );
					Printer::instance()->info("CPython : Result of call: '%ld'", pyd::PyInt_AsLong( pValue ));
				}
				Py_DECREF( pValue );
			}
			else
			{
				Py_DECREF( pFunc );
				Py_DECREF( pModule );
				pyd::PyErr_Print();
				Printer::instance()->error("CPython : Call failed");
				//return 1;
				Py_RETURN_NONE;
			}
		}
		else
		{
			if( pyd::PyErr_Occurred() )
				pyd::PyErr_Print();
			Printer::instance()->error("CPython : Cannot find function '%s'", py_module.function.c_str() );
		}
		Py_XDECREF( pFunc );
		Py_DECREF( pModule );
	}
	else
	{
		//PyErr_Print();

		PyObject *ptype, *pvalue, *ptraceback;
		pyd::PyErr_Fetch(&ptype, &pvalue, &ptraceback);
		//pvalue contains error message
		//ptraceback contains stack snapshot and many other information
		//(see python traceback structure)

		//Get error message
		char *pStrErrorMessage = pyd::PyString_AsString(pvalue);

		/*https://stackoverflow.com/questions/1796510/accessing-a-python-traceback-from-the-c-api
		PyTracebackObject* traceback = get_the_traceback();
		int line = ptraceback->tb_lineno;
		const char* filename = PyString_AsString(ptraceback->tb_frame->f_code->co_filename);*/

		Printer::instance()->error("CPython : Failed to load '%s' : '%s'", py_module.module.c_str(), pStrErrorMessage );

		Py_DECREF( ptype );
		Py_DECREF( pvalue );
		if( ptraceback != nullptr )
			Py_DECREF( ptraceback ); // FIXME: CRASH !!!!!

		//return 1;
		Py_RETURN_NONE;
		//none();
	}

	/*if( is_init )
	{
		Py_EndInterpreter( myThreadState );
		//PyEval_ReleaseLock();
	}
	else
	{
		//Py_EndInterpreter( myThreadState );
		is_init = true;
	}*/	
	
		
	//PyEval_RestoreThread(mainThreadState);
	//Py_Finalize();

	//--------------------------------------------------------------------
	//int start=libs.size();
	//libs.save();
	//libs.unload( start );
	//--------------------------------------------------------------------
	//PythonHandler::instance().free(); //this doesn't free the python interpreter, just the DLL
	//return 0;

	Py_RETURN_NONE;
	//none();
}

void PyWrapper::finalize( void ){ pyd::Py_Finalize(); }

//--------------------------------------------------------------------------------------------- more

PyObject* PyWrapper::none()
{				
	Py_RETURN_NONE; 

	//return Py_INCREF( (&pyd::_Py_NoneStruct) ), (&pyd::_Py_NoneStruct);
}

//http://stackoverflow.com/questions/4657764/py-incref-decref-when
void PyWrapper::xdecref( PyObject* obj ){	Py_XDECREF( obj ); };
void PyWrapper::decref( PyObject* obj ){	Py_DECREF( obj ); };