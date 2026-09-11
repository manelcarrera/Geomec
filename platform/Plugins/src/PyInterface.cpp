#include "PyInterface.h"

#include "Plugins.h"
#include "Printer.h"
#include "Xml.h"

#include "IGmInterface.h"

#include "PyLibHandler.h"
#include "PyWrapper.h"

#include "IT_Command.h" //FIXME
#include "SafeQueue.h"

#include "QUtil.h"

typedef PyLibHandler lib;
typedef PyWrapper wrapper;

int PyInterface::launch(std::string name, IGmInterface *gm_iface) {
  std::string py_cfg_file = CPlugins::instance()->plugins_path() + "\\" + CPlugins::PYTHON_CONFIG_FILE;

  CXml py_cfg(py_cfg_file, {"PYTHONROOT", "PYTHON_DLL", "PYTHONPATH"});
  bool py_cfg_r = py_cfg.verify();

  std::string app_cfg_file = CPlugins::instance()->plugins_path() + "\\" + name + "\\" + CPlugins::PLUGIN_CONFIG_FILE;

  CXml app_cfg(app_cfg_file, {"MODULE", "FUNCTION"}, {"LOCATION", "LIBS"});
  bool app_cfg_r = app_cfg.verify();

  if (py_cfg_r && app_cfg_r) {
    Printer::instance()->info("'PYTHONROOT' found in system: '%s'", CPlugins::python_path().c_str());

    std::string pythonroot = py_cfg.value("PYTHONROOT").c_str();
    Printer::instance()->info("PYTHONROOT: %s", pythonroot.c_str());
    if (QUtil::exists(pythonroot, QUtil::Dir) && QUtil::exists(pythonroot + "/python.exe", QUtil::File)) {
      // (1) ENVAR : PYTHONPATH
      std::string PYTHONPATH_s = py_cfg.value("PYTHONPATH") + ";" + CPlugins::instance()->plugins_path() + "\\" + name;
      if (app_cfg.exist("LOCATION"))
        PYTHONPATH_s += (";" + app_cfg.value("LOCATION"));
      if (app_cfg.exist("LIBS")) {
        bool replace_r =
            app_cfg.set("LIBS", CXml::replace("PYTHONROOT", py_cfg.value("PYTHONROOT"), app_cfg.value("LIBS")));
        PYTHONPATH_s += (";" + app_cfg.value("LIBS"));
      }
      _putenv_s("PYTHONPATH", PYTHONPATH_s.c_str());
      Printer::instance()->info("PYTHONPATH: '%s'", PYTHONPATH_s.c_str());

      // (2) PYTHON_DLL
      std::string py_dll = py_cfg.value("PYTHON_DLL");
      if (QUtil::exists(py_dll, QUtil::File)) {
        lib::file(py_dll);
        Printer::instance()->info("PYTHON_DLL: '%s'", py_dll.c_str());

        // (3) LAUNCH APP
        const TPyModule py_module_conf = TPyModule{app_cfg.value("MODULE"), app_cfg.value("FUNCTION"), "", nullptr};
        const TCModuleV c_module_conf_v = TCModuleV{TCModule{gm_iface->module(), gm_iface->methods()}};
        wrapper::execute(py_module_conf, c_module_conf_v);

        gm_iface->requests_queue().enqueue(Command(Command::eType::IT1_Control_Quit, nullptr)); // FXIME

        return 0; // Ok
      } else {
        Printer::instance()->error("'PYTHON_DLL' not found: '%s'", py_dll.c_str());
        return 1; // Not Ok
      }
    } else {
      Printer::instance()->error("'PYTHONROOT' doesn't exist: '%s'", pythonroot.c_str());
      return 2; // Not Ok
    }
  } else {
    if (!py_cfg_r)
      Printer::instance()->error("Error PY config file: %s", py_cfg_file.c_str());
    if (!app_cfg_r)
      Printer::instance()->error("Error APP config file: %s", app_cfg_file.c_str());

    return 3;
  }
}

void PyInterface::finalize() { wrapper::finalize(); }