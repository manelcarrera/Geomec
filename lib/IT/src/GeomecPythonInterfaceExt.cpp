#include "GeomecPythonInterfaceExt.h"

#include "IT_Command.h"
#include "IT_Data.h"	
//#include "Data.h"
#include "Printer.h"
#include "PyWrapper.h"

#include "PyInc.h"

//#include <string>

typedef PyWrapper wrapper;

const char* GeomecPythonInterfaceExt::C_MODULE = "geomec_ext";

static const int FAKE_NUMBER		= 4;
static const Command FAKE_COMMAND	= Command( Command::eType::IT1_List_Reservoir, new IT_Data() );

const TCModule CModuleDefault = TCModule{	GeomecPythonInterfaceExt::C_MODULE,
											GeomecPythonInterfaceExt::GeomecMethods };

PyMethodDef GeomecPythonInterfaceExt::GeomecMethods[] =
{
	{ "reservoir",		GeomecPythonInterfaceExt::reservoir,	METH_VARARGS,	"Asks for a reservoir."	},
	{ "displacement",	GeomecPythonInterfaceExt::displacement,	METH_VARARGS,	"Asks for a displacement."	},
	{ "func",			GeomecPythonInterfaceExt::func,			METH_VARARGS,	"Generic function call"	},

	{ NULL,	NULL, 0, NULL }
};

TData FAKE_DATA() //2 point with three colums (x,y,z)
{
	IT_Data::TPointsV v = { {1,2,3}, {1,2,3} };
	TData data = TData{2,3,v.data()};
	return data;
}

int _callback_reservoir( PyObject* tuple ) //TODO: provide the reservoir to the IT
{
	/*PyObject** obj_list = wrapper::parse< PyObject** >( tuple );
	//Printer::instance()->debug( wrapper::_parse_array_from_obj( obj_list[ 0 ] ).data, 2 );*/
	return 1;
}

int _callback_displacement( PyObject* tuple ){ return 1; }

PyTuple* GeomecPythonInterfaceExt::func( PyObject *self, PyObject *args )
{
	TPyObjectPtrV obj_v =  wrapper::parse< TPyObjectPtrV >( args );

	enum ePos{ Module, Function, Params	};

	static const std::string EMPTY_STRING = std::string(); 

	std::string module		= wrapper::parse_obj< std::string >( obj_v[ ePos::Module ] );
	std::string function	= wrapper::parse_obj< std::string >( obj_v[ ePos::Function ] );
	std::string param		= wrapper::parse_obj< std::string >( obj_v[ ePos::Params ] );

	PyTuple* params_t = wrapper::build< TPyObjectPtrV >( TPyObjectPtrV{ obj_v[ ePos::Params ] } );

	TPyModule py_module = TPyModule{ module, function, EMPTY_STRING, params_t }; //FIXME: check if it's needed to build a tuple or an object is enought

	PyTuple* tuple =  wrapper::execute(	py_module, TCModuleV{ CModuleDefault } );
	
	return tuple;

	/*TPyObjectPtrV res_obj_v;
	res_obj_v.push_back( wrapper::build< int >( eCommandExt::Cmd_Get_Generic ) );
	res_obj_v.push_back( tuple );
	return wrapper::build< TPyObjectPtrV >( res_obj_v );*/
}	


PyTuple* GeomecPythonInterfaceExt::reservoir( PyObject *self, PyObject *args )
{
	Printer::instance()->debug( "C++ : Ext : reservoir" );
	Printer::instance()->debug( "C++ : Ext : reservoir" );

	std::string module		= "IO_gocad";
	std::string function	= "readdata";

	std::string filename	= wrapper::parse<std::string>( args );

	TPyModule py_module = TPyModule{ module, function, filename };

	PyTuple* tuple =  wrapper::execute(	py_module,
										TCModuleV{ CModuleDefault },
										_callback_reservoir );
	return tuple;  
}	

PyTuple* GeomecPythonInterfaceExt::displacement( PyObject *self, PyObject *args )
{
	Printer::instance()->debug( "C++ : Ext : displacement" );

	std::string module		= "IO_gocad";
	std::string function	= "getmeasure";

	std::string filename	= wrapper::parse<std::string>( args );

	TPyModule py_module = TPyModule{ module, function, filename };

	PyTuple* tuple =  wrapper::execute(	py_module,
										TCModuleV{ CModuleDefault },
										_callback_displacement );
	return tuple;  
}	