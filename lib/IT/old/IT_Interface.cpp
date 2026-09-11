#include "IT_Interface.h"
#include "GeomecPythonInterface.h"
#include "GeomecPythonInterfaceExt.h"
#include "PyWrapper.h"
#include "Printer.h"
#include "SafeQueue.h"
#include "PyConfigFile.h"
#include "PyLibHandler.h"
#include "IT_Command.h"

#include "Shlobj.h"

#include <qstring>

//#include <stdlib.h>  //setenv

typedef PyConfigFile config;
typedef PyLibHandler lib;

typedef PyWrapper wrapper;

const std::string IT_Interface::PYTHON_MODULE			= "module";
const std::string IT_Interface::FUNCTION				= "main";
const std::string IT_Interface::PARAM					= "";
PyTuple* IT_Interface::PARAM_PY							= nullptr;

const TPyModule& IT_Interface::PyModuleDefault = TPyModule{	IT_Interface::PYTHON_MODULE,
															IT_Interface::FUNCTION,
															IT_Interface::PARAM,
															IT_Interface::PARAM_PY };

const TCModule& IT_Interface::CModuleDefault = TCModule{	GeomecPythonInterface::C_MODULE,
															GeomecPythonInterface::GeomecMethods };

const TCModule& IT_Interface::CModuleExtended = TCModule{	GeomecPythonInterfaceExt::C_MODULE,
															GeomecPythonInterfaceExt::GeomecMethods };

IT_Interface::IT_Interface( void ){}
IT_Interface::~IT_Interface( void ){}

int IT_Interface::launch(	TPyModule py_module, 
							TCModuleV c_modules_v ) //FIXME: not taken in account
{
	QString BASE_PATH_qs = QString::fromStdString( config::Data::BASE_PATH ); 
	char* APPDATA = std::getenv( "APPDATA" );  
	BASE_PATH_qs.replace("%APPDATA%", APPDATA );

	const std::string CONFIG_FILE = BASE_PATH_qs.toStdString() + "\\" + "inversion_tool.cfg";

	config::file( CONFIG_FILE );

	PyConfigFile::Data data = PyConfigFile::Data::DEFAULT_DATA;
	PyConfigFile::data( &data );

	// load file and replace replecement strings: '%SOMETHING%' 
	config::instance();
	_putenv_s("PYTHONPATH", config::instance().getValueOfKey<std::string>("PYTHONPATH").c_str() ); 

	//printer conf
	Printer::instance()->file(	config::instance().getValueOfKey<std::string>("LOG_FILE") );
	Printer::instance()->debug("Launch IT");

	//DLL
	lib::file( config::instance().getValueOfKey<std::string>("PYTHON_DLL") );

	//launch
	const TPyModule& py_module_conf = TPyModule{	config::instance().getValueOfKey<std::string>("MODULE"), 
													config::instance().getValueOfKey<std::string>("FUNCTION"), 
													IT_Interface::PARAM, 
													IT_Interface::PARAM_PY };
	
	wrapper::execute( py_module_conf, c_modules_v );

	//end
	GeomecPythonInterface::m_requests_queue.enqueue( Command( Command::eType::IT1_Control_Quit, nullptr ) );
	return 0; //FIXME
}

void IT_Interface::finalize( void ){ wrapper::finalize(); }