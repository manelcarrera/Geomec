#include "Plugins.h"

#include <QDir>
#include <QDirIterator>
#include <QVersionNumber>
#include <qfile>

#include "Printer.h"
#include "System.h"
#include "Xml.h"
// #include "Item.h"		//FIXME: 'lib' including 'binary'

#include "GeomecUtils.h"

#include "Release.h"

#include "QUtil.h"

#include <sstream>

#include <QFileDialog>
#include <QMessageBox>

const std::string CPlugins::DEF_PLUGINS_SUB_MENU_NAME = "Plugins";

const std::string CPlugins::NOT_FOUND = "NOT_FOUND";

const std::string CPlugins::INSTALL_EXT = "xml";
const std::string CPlugins::INSTALLED_EXT = "installed";
const std::string CPlugins::ERROR_EXT = "ERROR";
const std::string CPlugins::PYTHON_CONFIG_FILE = "python.xml";
const std::string CPlugins::PLUGIN_CONFIG_FILE = "app.xml";
const std::string CPlugins::LOCAL_PLUGINS_REPOSITORY = QUtil::url(GetAppPath(), "packages").toStdString();

namespace {
Printer *printer = Printer::instance(Printer::App);
}

CPlugins::CPlugins(const std::string &plugins_path) : m_plugins_path(plugins_path) { init(); }

std::string CPlugins::manual(std::string plugin_name) {
  return has_manual(plugin_name) ? m_manuals_map[plugin_name] : NOT_FOUND;
}

std::string CPlugins::manual(int idx) { return idx < m_plugins.size() ? manual(m_plugins[idx]) : NOT_FOUND; }

bool CPlugins::has_manual(std::string plugin_name) { return m_manuals_map.find(plugin_name) != m_manuals_map.end(); }

bool CPlugins::has_manual(int idx) { return idx < m_plugins.size() ? has_manual(m_plugins[idx]) : false; }

void CPlugins::create_manuals_map() {
  m_manuals_map.clear();
  for (int i = 0; i < m_plugins.size(); i++) {
    std::string name = m_plugins[i];
    std::string dir = QUtil::url(QUtil::usr_dir(QUtil::Plugins), name);
    std::string cfg_file = QUtil::url(dir, PLUGIN_CONFIG_FILE);
    std::string manual = CXml::get(cfg_file, "manual");
    if (manual == CXml::NOT_FOUND) // look for the manual in the plugin folder
    {
      QUtil::TFilesV files_v = QUtil::files(dir, "*.pdf", QUtil::FullPath);
      std::size_t nE = files_v.size();
      if (nE) {
        m_manuals_map[name] = files_v[0];
        if (nE > 1)
          printer->info("plugin: %s : mnore than one '.pdf' file : taking the first found", name.c_str());
      }
    } else // manual from config file
    {
      m_manuals_map[name] = manual;
    }
  }
}

void CPlugins::init() {
  m_plugins = QUtil::folders(m_plugins_path, QUtil::Name);
  create_manuals_map();
}

CPlugins *CPlugins::instance() {
  static CPlugins _instance(QUtil::usr_dir(QUtil::Plugins));
  return &_instance;
}

const std::size_t CPlugins::size() { return m_plugins.size(); }

const std::string CPlugins::name(int pos) { return pos < m_plugins.size() ? m_plugins[pos] : NOT_FOUND; };

std::string CPlugins::dir(std::string name) {
  QDirIterator it(QString::fromStdString(m_plugins_path));

  while (it.hasNext()) {
    std::string dir = QDir(it.next()).dirName().toStdString();

    if (dir == name)
      return it.filePath().toStdString();
  }
  return NOT_FOUND;
}

std::string CPlugins::python_path() {
  CSystem sys;

  std::string res = sys.exe("where python");

  std::istringstream iss(res);
  std::string line;
  std::getline(iss, line);

  // res.erase(std::remove(res.begin(), res.end(), '\n'), res.end());
  line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());

  QFileInfo info(QString::fromStdString(res));
  std::string path = info.path().toStdString();

  printer->debug("res: %s -> %s", res.c_str(), path.c_str());

  // return "E:\\Anaconda2";
  return path;
}

bool CPlugins::provide_pythonroot(std::string dir) {
  bool found = false;
  bool exit = false;
  while (!found && !exit) {
    QString dir_ = QFileDialog::getExistingDirectory(nullptr /*parent*/, "Select PYRHONROOT directory", "c:/",
                                                     QFileDialog::ShowDirsOnly);
    if (dir_.isEmpty()) // user quites dialog
    {
      exit = true;
    } else // user selects python interpreter path
    {
      std::string file_ = QUtil::url(dir_, "python.exe").toStdString();

      QFileInfo info(QString::fromStdString(file_));
      if (info.exists()) {
        std::string py_cfg_file = QUtil::url(dir, PYTHON_CONFIG_FILE);
        CXml::update(py_cfg_file, "PYTHONROOT", dir_.toStdString());
        found = true;
      } else
        QMessageBox::warning(nullptr, QString("PYTHONROOT"), QString("PYTHONROOT not correct"));
    }
  }
  if (exit) {
    std::string plugins_xml = QUtil::url(CPlugins::instance()->plugins_path(), PYTHON_CONFIG_FILE);
    QMessageBox::warning(
        nullptr, QString("PYTHONROOT"),
        QString::fromStdString("PYTHONROOT not provided.\n\nProvide it editing\n\n 'PYTHONROOT' var in\n\n'" +
                               plugins_xml));
  }
  return !exit;
}

void CPlugins::mk_python_config(std::string dir) {
  std::string py_cfg_file = QUtil::url(dir, PYTHON_CONFIG_FILE);

  std::string py_cfg_file_tmp = QUtil::url(CPlugins::LOCAL_PLUGINS_REPOSITORY, PYTHON_CONFIG_FILE);

  bool res = QUtil::copy(py_cfg_file_tmp, py_cfg_file);

  if (res) {
    std::string path = python_path();
    if (path.empty() || path == ".")
      provide_pythonroot(dir);
    else // python interpreter found in PATH
      CXml::update(py_cfg_file, "PYTHONROOT", python_path());
  } else
    printer->error("create : %s", py_cfg_file.c_str());

  printer->debug("creating: %s", py_cfg_file.c_str());
}

void CPlugins::install_repository_plugins() {
  QString package_dir = QDir::toNativeSeparators(QString::fromStdString(LOCAL_PLUGINS_REPOSITORY));

  std::vector<QString> packages;
  Glob(package_dir, ".zip$", packages);

  if (packages.empty()) {
    printer->info("NO plugins in the repository %s", package_dir.toStdString().c_str());
  } else {
    std::string py_cfg_file = QUtil::url(m_plugins_path, PYTHON_CONFIG_FILE);
    if (QUtil::exists(py_cfg_file, QUtil::File)) {
      printer->info("py config file exists: %s", py_cfg_file.c_str());
    } else {
      QDir plugins_path(m_plugins_path.c_str());
      if (!plugins_path.exists())
        plugins_path.mkpath(".");

      mk_python_config(m_plugins_path);
    }

    for (std::vector<QString>::iterator it = packages.begin(); it != packages.end(); ++it)
      install(it->toStdString());
  }
}

//-------------------------------------- install
//--------------------------------------------------------------------------

bool CPlugins::installed(std::string name) { return QUtil::exists(QUtil::url(m_plugins_path, name), QUtil::Dir); }

bool CPlugins::new_release(CXml &cfg_previous, CXml &cfg) {
  if (cfg.exist("version") && cfg_previous.exist("version")) {
    CRelease r1 = CRelease(cfg_previous.value("version"));
    CRelease r2 = CRelease(cfg.value("version"));

    if (r1.valid() && r2.valid()) {
      if (r1 < r2) {
        return true;
      } else {
      }
    }
  } else {
    if (cfg.exist("version"))
      return true;
  }

  return false;
}

bool CPlugins::install(std::string package) {
  CSystem sys;

  printer->info("plugins : install : %s", package.c_str());
  bool retval = false;

  QString dir_qs = GetGeomecTempPathExt(CTempPath::TEMP_GENERAL);

  printer->info("tmp dir : '%s'", dir_qs.toStdString().c_str());

  if (QFileInfo(dir_qs).isDir()) {
    QString tmp_dir_path = QDir::toNativeSeparators(CreateTempDirExt(CTempPath::TEMP_GENERAL));

    if (QFileInfo(tmp_dir_path).isDir()) {
      bool res_u = sys.unpack(package, tmp_dir_path.toStdString(), PLUGIN_CONFIG_FILE);
      if (res_u) {
        std::string cfg_file = QUtil::url(tmp_dir_path.toStdString(), PLUGIN_CONFIG_FILE);
        CXml cfg(cfg_file, {"name"}, {"version"});

        bool cfg_r = cfg.verify();

        printer->info("cfg file : %s", cfg_file.c_str());

        if (cfg_r) {
          std::string dest = QUtil::url(m_plugins_path, cfg.value("name"));
          bool do_unpack = true;

          if (installed(cfg.value("name"))) {
            std::string old_cfg_file = QUtil::url(dest, PLUGIN_CONFIG_FILE);

            CXml cfg_old(old_cfg_file, {"name"}, {"version"});

            if (new_release(cfg_old, cfg)) {
              printer->info("new release of an existant plugin");
            } else {
              printer->error("this plugin is already installed");
              do_unpack = false;
            }
          }

          if (do_unpack) {
            RemoveDir(dest.c_str());
            retval = sys.unpack(package, dest);
            if (retval)
              init();
            else
              printer->error(
                  "installation of new plugin failed (and we may have removed an old one!)"); // If this happens (a
                                                                                              // lot), we should make
                                                                                              // this more user friendly
          }
        }
      }
      // tmp debug flag; TODO: remove
      const char *flag = getenv("GM_DEBUG_PLUGINS");
      if (flag && (flag[0] == 'y' || flag[1] == 'Y'))
        printer->debug("tmp dir is not removed : '%s'", dir_qs.toStdString().c_str());
      else
        RemoveDir(tmp_dir_path);
    } else {
      printer->error("tmp dir is not writable or you don't have enought rights : '%s'", dir_qs.toStdString().c_str());
    }
  } else {
    printer->error("tmp dir doesn't exist : '%s'", dir_qs.toStdString().c_str());
  }

  return retval;
}

//-------------------------------------- uninstall
//--------------------------------------------------------------------------

bool CPlugins::uninstall(int pos) // last pos = -1
{
  int nE = (int)m_plugins.size();

  pos = pos == -1 ? nE - 1 : pos;

  if (pos < nE) {
    std::string name = m_plugins[pos];
    std::string full_name = dir(name);
    QDir qdir(full_name.c_str());
    bool remove_dir_r = qdir.removeRecursively(); // FIXME: this doesnt return the proper result
    if (remove_dir_r) {
      printer->debug("plugins : remove : dir : '%s' -> '%s' : %s", name.c_str(), dir(name).c_str(),
                     remove_dir_r ? "OK" : "ERROR");
      m_plugins.erase(m_plugins.begin() + pos);
      printer->debug("plugins : count : %d", m_plugins.size());
      return true;
    }
  }
  return false;
}