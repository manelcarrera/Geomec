#pragma once

#include <vector>
#include <string>
#include <map>

class CXml;

class CPlugins
{
public:
	typedef std::vector<std::string> TPluginsV;
	/*typedef std::vector<std::string> TFoldersV;
	typedef std::vector<std::string> TFilesV;

	enum eFile{ Name, FullPath };
	enum eUsrDir{ Base,	Goemec,	Plugins, Changelog };*/

private:

  std::string m_plugins_path;
  TPluginsV   m_plugins;

  typedef std::map<std::string, std::string> TManualsM;
  TManualsM m_manuals_map;

public:

  static const std::string DEF_PLUGINS_SUB_MENU_NAME;
  static const std::string NOT_FOUND;
  static const std::string INSTALL_EXT;
  static const std::string INSTALLED_EXT;
  static const std::string ERROR_EXT;
  static const std::string PYTHON_CONFIG_FILE;
  static const std::string PLUGIN_CONFIG_FILE;
  static const std::string LOCAL_PLUGINS_REPOSITORY;

private:

	std::string dir( std::string name );			// full path from plugin name

  CPlugins(const std::string& plugins_path);

  void create_manuals_map();

public:

  static CPlugins *instance();

  void install_repository_plugins();

  std::string plugins_path() const { return m_plugins_path; }

	void init();									// fills plugins list from the directory tree
	bool install( std::string package );			// unpack zip + refresh
	bool uninstall(  int pos  );					// remove dir + update list

	const std::string name( int pos );			// plugin name
	const std::size_t size();							// plugins list size

	const TPluginsV& get(){ return m_plugins; }; // plugins list

	//void rename( std::string file_name, std::string to_replace, std::string replace_by );	// change file extension

	bool installed( std::string name );

	std::string manual( std::string plugin_name );
	std::string manual( int idx );

	bool has_manual( std::string plugin_name );
	bool has_manual( int idx );

	//static
	static std::string python_path();
	static void mk_python_config( std::string dir );
	static bool provide_pythonroot( std::string dir );
	static bool new_release( CXml& cfg_previous, CXml& cfg );

	//static : util
	/*static TFoldersV folders( std::string path, eFile type_ );
	static TFilesV files( std::string path, std::string filter, eFile type_ );	// file full path
	static bool file_exists( std::string path );
	static bool dir_exists( std::string path );

	static std::string usr_dir( eUsrDir );
	static std::string native_sepatators( std::string s );
	static bool copy( std::string orig, std::string des );*/
};
