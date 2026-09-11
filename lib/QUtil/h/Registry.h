#pragma once

class QSettings;
#include <string>

//-------------------------- CRegistryBase -----------------------------

class CRegistryBase {
protected:
  QSettings *m_settings;
  std::string m_path;

public:
  CRegistryBase(std::string path);
  ~CRegistryBase();

  std::string path();

  bool modify(std::string key, std::string value);
  bool create(std::string key, std::string value);

  std::string value(std::string key);
};

//-------------------------- CRegistry -----------------------------

class CRegistry : public CRegistryBase {
  static std::string SETTINGS_PATH;

  std::string m_release;
  std::string m_build;

public:
  static std::string BUILD_KEY;
  enum eRelease { Same, NewRelease, NewBuild };

public:
  CRegistry(std::string release, std::string build);
  eRelease check();
};
