#include "PyConfigFile.h"

#include "Printer.h"
#include "util_.h"

#include <string>
#include <vector>

#include <qstring> //QT

PyConfigFile::Data::Data( const Data& data )
{
  app_data		= data.app_data;
  python_root		= data.python_root;
  python_dll		= data.python_dll;
  base_path		= data.base_path;
  log_file		= data.log_file;
  py_files		= data.py_files;
  python_path		= data.python_path;
  module			= data.module;
  function		= data.function;
}

PyConfigFile::Data::Data( std::string _app_data,
            std::string _python_root,
            std::string _python_dll,
            std::string _base_path,
            std::string _log_file,
            std::string _py_files,
            std::string _python_path,
            std::string _module,
            std::string _function )
{
  app_data		= _app_data;
  python_root		= _python_root;
  python_dll		= _python_dll;
  base_path		= _base_path;
  log_file		= _log_file;
  py_files		= _py_files;
  python_path		= _python_path;
  module			= _module;
  function		= _function;
}

std::vector<std::string> PyConfigFile::Data::values_v()
{
  std::vector<std::string> v;

  v.push_back( app_data );
  v.push_back( python_root );
  v.push_back( python_dll );
  v.push_back( base_path );
  v.push_back( log_file );
  v.push_back( py_files );
  v.push_back( python_path );
  v.push_back( module );
  v.push_back( function );

  return v;
}

//------------------------------------------------------------------------------------------------

PyConfigFile::Data* PyConfigFile::m_data = nullptr;

const std::string PyConfigFile::Data::APPDATA		= std::string( getenv("APPDATA") ); //FIXME: Linux ???
const std::string PyConfigFile::Data::PYTHONROOT	= "e:\\Anaconda2";
const std::string PyConfigFile::Data::PYTHON_DLL	= "%PYTHONROOT%\\python27.dll";
const std::string PyConfigFile::Data::BASE_PATH		= "%APPDATA%\\Geomec\\plugins\\Inversion tool";
const std::string PyConfigFile::Data::LOG_FILE		= "%BASE_PATH%\\IT_cpp.log";
const std::string PyConfigFile::Data::PY_FILES		= "%BASE_PATH%\\Python files";
const std::string PyConfigFile::Data::PYTHONPATH_	= "%PYTHONROOT%\\libs;%PYTHONROOT%\\DLLs;%PYTHONROOT%\\Lib;%PYTHONROOT%\\Lib\\site-packages;%PYTHONROOT%\\Lib\\lib-tk;%PY_FILES%";
const std::string PyConfigFile::Data::MODULE		= "inversion_GUI12_f";
const std::string PyConfigFile::Data::FUNCTION		= "main";

const PyConfigFile::Data PyConfigFile::Data::DEFAULT_DATA = 
  PyConfigFile::Data( APPDATA, PYTHONROOT, PYTHON_DLL, BASE_PATH, LOG_FILE, PY_FILES, PYTHONPATH_, MODULE, FUNCTION );

const std::vector<std::string> PyConfigFile::Data::DEFAULT_DATA_V = 
  { Data::APPDATA, Data::PYTHONROOT, Data::PYTHON_DLL, Data::BASE_PATH, Data::LOG_FILE, Data::PY_FILES, Data::PYTHONPATH_, Data::MODULE, Data::FUNCTION };


void PyConfigFile::exitWithError(const std::string &error) 
{
  Printer::instance()->debug( (char*)error.c_str() );
}

void PyConfigFile::print_keys()
{
  Printer::instance()->debug("KEYS:");
  for( auto it = contents.cbegin(); it != contents.cend(); ++it )
    Printer::instance()->debug("%s = %s", it->first.c_str(), it->second.c_str() );
}

//FIXME: Everything is hard-coded
void PyConfigFile::create_default_file(const std::string &name)
{
  if( util::create_dir( m_data->base_path ) )
  {
  }
  else
  {
    //FIXME: uncomment

     // Failed to create directory.
    Printer::instance()->debug("Config file ERROR: %s [%s]", util::GetLastErrorAsString().c_str(),  m_data->base_path.c_str() );
  }

  //std::vector<std::string> keys_v		= {"APPDATA", "PYTHONROOT","PYTHON_DLL","BASE_PATH", "LOG_FILE", "PY_FILES", "PYTHONPATH", "MODULE", "FUNCTION"};
  std::vector<std::string> keys_v		= {"PYTHONROOT","PYTHON_DLL","BASE_PATH", "LOG_FILE", "PY_FILES", "PYTHONPATH", "MODULE", "FUNCTION"};
  std::vector<std::string> values_v	= m_data ? m_data->values_v() : Data::DEFAULT_DATA_V;
  
  std::ofstream outfile( name );
  for( int i = eData::APPDATA+1 ; i <= eData::FUNCTION; i++ )
    outfile << keys_v[ i ] << " " << KEY_VALUE_DELIMITER << " " << values_v[ i ] << std::endl;
  outfile.close();
}

void PyConfigFile::replace_config_wildcards()
{
  print_keys();

  QString PYTHONROOT_qs	= QString::fromStdString( contents["PYTHONROOT"] ); 
  QString PYTHON_DLL_qs	= QString::fromStdString( contents["PYTHON_DLL"] ); 
  QString BASE_PATH_qs	= QString::fromStdString( contents["BASE_PATH"] ); 
  QString LOG_FILE_qs		= QString::fromStdString( contents["LOG_FILE"] ); 
  QString PY_FILES_qs		= QString::fromStdString( contents["PY_FILES"] ); 
  QString PYTHONPATH_qs	= QString::fromStdString( contents["PYTHONPATH"] ); 

  PYTHON_DLL_qs.replace("%PYTHONROOT%", PYTHONROOT_qs );

  char* APPDATA = std::getenv( "APPDATA" );  
  //const char* APPDATA = contents["APPDATA"].c_str();

  BASE_PATH_qs.replace("%APPDATA%", APPDATA );

  LOG_FILE_qs.replace("%BASE_PATH%", BASE_PATH_qs );
  PY_FILES_qs.replace("%BASE_PATH%", BASE_PATH_qs );

  PYTHONPATH_qs.replace("%PYTHONROOT%", PYTHONROOT_qs );
  PYTHONPATH_qs.replace("%PY_FILES%", PY_FILES_qs );

  modify_value("PYTHON_DLL",	PYTHON_DLL_qs.toStdString());	// PYTHONROOT
  modify_value("BASE_PATH",	BASE_PATH_qs.toStdString());	// APPDATA ENVVAR
  modify_value("LOG_FILE",	LOG_FILE_qs.toStdString());		// BASE_PATH
  modify_value("PY_FILES",	PY_FILES_qs.toStdString());		// BASE_PATH
  modify_value("PYTHONPATH",	PYTHONPATH_qs.toStdString());	// PYTHONROOT, PY_FILES

  print_keys();

  // if python files doesn't exist
  if( !util::dir_exists( PY_FILES_qs.toStdString() ) )
  {
    util::create_dir( PY_FILES_qs.toStdString() );
    //SHCreateDirectoryEx(NULL, PY_FILES_qs.toStdString().c_str(), NULL);
    Printer::instance()->debug("Default python files folder created: %s", PY_FILES_qs.toStdString().c_str() );
  }

  // if 'module.py' with 'def function()' doesn't exist
  std::ifstream file;
  std::string module_full_path = contents["PY_FILES"] + "\\" + contents["MODULE"] + ".py";
  file.open( module_full_path.c_str() );
  if (!file)
  {
    std::ofstream outfile( module_full_path );

    //QString default_module_qs = QString::asprintf( "def %s(): return 0", contents["FUNCTION"].c_str() );

    QString default_module_qs = 
      "from Tkinter import *\n" \
      "def main():\n" \
      "	root = Tk()\n" \
      "	w = Label(root, text='Hello, world!')\n"
      "	w.pack()\n"
      "	root.mainloop()";

    outfile << default_module_qs.toStdString() << std::endl;
    outfile.close();
    Printer::instance()->debug("Default MODULE.FUNCTION()[%s.%s()] created, contect: [%s]", 
      contents["MODULE"].c_str(), contents["FUNCTION"].c_str(), default_module_qs.toStdString().c_str() );
  }
}
void PyConfigFile::ExtractKeys()
{
  std::ifstream file;
  file.open(m_name.c_str());
  if (!file)
    create_default_file( m_name );

  ConfigFile::ExtractKeys();
  
  replace_config_wildcards();
}

void PyConfigFile::reload()
{
  contents.clear();
  ExtractKeys();
}


PyConfigFile::PyConfigFile(const std::string &name )
{
  ExtractKeys();
}