#ifndef _PyConfigFile
#define _PyConfigFile

#include "ConfigFile.h"

#include <vector>

class PyConfigFile : public ConfigFile {

public:
  enum eData { APPDATA, PYTHONROOT, PYTHON_DLL, BASE_PATH, LOG_FILE, PY_FILES, PYTHONPATH_, MODULE, FUNCTION };

  class Data {
  public:
    static const std::string APPDATA;
    static const std::string PYTHONROOT;
    static const std::string PYTHON_DLL;
    static const std::string BASE_PATH;
    static const std::string LOG_FILE;
    static const std::string PY_FILES;
    static const std::string PYTHONPATH_;
    static const std::string MODULE;
    static const std::string FUNCTION;

  public:
    static const Data DEFAULT_DATA;
    static const std::vector<std::string> DEFAULT_DATA_V;

    std::string app_data;
    std::string python_root;
    std::string python_dll;
    std::string base_path;
    std::string log_file;
    std::string py_files;
    std::string python_path;
    std::string module;
    std::string function;

  public:
    Data(const Data &data);

    Data(std::string _app_data, std::string _python_root, std::string _python_dll, std::string _base_path,
         std::string _log_file, std::string _py_files, std::string _python_path, std::string _module,
         std::string _function);

    std::vector<std::string> values_v();
  };

  static PyConfigFile &instance() {
    static PyConfigFile instance;
    return instance;
  }

  static void data(Data *_data) { m_data = _data; }
  static Data *data() { return m_data; }

  void reload();

private:
  static Data *m_data;

  PyConfigFile(const std::string &name = m_name);

  void create_default_file(const std::string &name);
  void replace_config_wildcards();

  void modify_value(const std::string &key, std::string value) { contents[key] = value; };
  void print_keys();

  virtual void ExtractKeys();
  virtual void exitWithError(const std::string &error);
};

#endif // _PyConfigFile