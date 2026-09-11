#include <cstdlib>
#include <iostream>

#include <QString>

#include "geomec_shell.h"
#include "ArgumentParser.h"
#include "InterfacePy.h"

#include "PyWrapper.h"
//#include "PyConfigFile.h"
#include "PyLibHandler.h"

#include "PyInc.h"

#include "Printer.h"

//typedef PyConfigFile config;
typedef PyLibHandler lib;
typedef PyWrapper wrapper;

const std::string PYTHON_MODULE			= "module";
const std::string FUNCTION				= "main";
const std::string PARAM					= "";
PyTuple* PARAM_PY						= nullptr;

const char* C_MODULE = "module";

static PyObject* send_cmd( PyObject *self, PyObject *args ){	return wrapper::none(); };
static PyObject* get_result( PyObject *self, PyObject *args ){ return wrapper::none(); };

static PyMethodDef methods[] =
{
	{ "send_cmd",	send_cmd,	METH_VARARGS,	"Sends a request"			},
	{ "get_result",	get_result,	METH_VARARGS,	"Returns the cmd result"	},
	{ NULL,	NULL, 0, NULL }
};

const TPyModule& PyModuleDefault	= TPyModule{ PYTHON_MODULE, FUNCTION, PARAM, PARAM_PY };
const TCModule& CModuleDefault		= TCModule{ C_MODULE, methods };

namespace
{

int script(int argc, char* argv[])
{
	return 0;
}

} // anonymous namespace


namespace gm_shell
{

const char *CInterfacePy::ShortDescription() const
{
  return "Py";
}

bool CInterfacePy::AcceptParameters(CArgumentParser& argParser, bool bForced)
{
  std::string key;
  std::string value;

  m_argParser = &argParser;

  for (size_t i = 0; i < argParser.ArgumentSize(); ++i)
  {
    argParser.GetArgument(i, key, value);
    if (value == "script")
    {
      m_commands.push(PY_SCRIPT);
      return true;
    }
  }

  if (bForced)
  {
    m_error = 1;
    m_commands.push(IInterfaceBase::USAGE);
    m_commands.push(IInterfaceBase::ABORT);
    return true;
  }

  m_argParser = 0;

  return false;
}

void CInterfacePy::PrintUsage() const
{
  std::cerr << "geomec_shell script" << std::endl;
}

void CInterfacePy::Startup() //FIXME: Load DLL
{
	std::cout << "using interface Py" << std::endl << std::endl;

  if (!m_initialized)
  {
	  // TODO: MCR 2018-10-17: This needs to be redone as in Geomec using 'QUtil' lib 'Xml' facilities

    /*QString BASE_PATH_qs = QString::fromStdString(config::Data::BASE_PATH);
    char* APPDATA = std::getenv("APPDATA");
    BASE_PATH_qs.replace("%APPDATA%", APPDATA);

    const std::string CONFIG_FILE = BASE_PATH_qs.toStdString() + "\\" + "interface_py.cfg";

    config::file(CONFIG_FILE);

    static const std::string LOG_FILE = "%BASE_PATH%\\interface_py.log";

    config::file(CONFIG_FILE);

    PyConfigFile::Data data = PyConfigFile::Data::DEFAULT_DATA;
    data.log_file = LOG_FILE;
    PyConfigFile::data(&data);

    config::instance();
    _putenv_s("PYTHONPATH", config::instance().getValueOfKey<std::string>("PYTHONPATH").c_str());

    //printer conf
    Printer::instance()->file(config::instance().getValueOfKey<std::string>("LOG_FILE"));
    Printer::instance()->debug("Launch IT");

    //DLL
    lib::file(config::instance().getValueOfKey<std::string>("PYTHON_DLL"));*/

    m_initialized = true;
  }
}

void CInterfacePy::HandleLastCommand()
{
  int command = m_commands.front();
  m_commands.pop();

  int retval = 1;

  switch(command)
  {
  case PY_SCRIPT:

	  //FIXME

	  /*

	//const TPyModule py_module	= PyModuleDefault;
	const TCModuleV c_modules_v	= {CModuleDefault};

	const TPyModule& py_module_conf = TPyModule{	config::instance().getValueOfKey<std::string>("MODULE"), 
													config::instance().getValueOfKey<std::string>("FUNCTION"), 
													PARAM, 
													PARAM_PY };

	wrapper::execute( py_module_conf, c_modules_v );
	*/
    break;
  }

  if (retval != 0)
  {
    m_error = retval;
    m_commands.push(IInterfaceBase::USAGE);
    m_commands.push(IInterfaceBase::ABORT);
  }

}




}