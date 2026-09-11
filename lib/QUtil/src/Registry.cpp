#include "Registry.h"

#include <QSettings>
#include <QString>

//------------------------- CRegistryBase --------------------------------------------

CRegistryBase::CRegistryBase(std::string path) : m_path(path) {
  m_settings = new QSettings(QString::fromStdString(m_path), QSettings::NativeFormat);
}

CRegistryBase::~CRegistryBase() { delete m_settings; }

bool CRegistryBase::modify(std::string key, std::string value) {
  m_settings->setValue(QString::fromStdString(key), QString::fromStdString(value));
  return true;
}

bool CRegistryBase::create(std::string key, std::string value) { return modify(key, value); }

std::string CRegistryBase::path() { return m_path; }

std::string CRegistryBase::value(std::string key) {
  return m_settings->value(QString::fromStdString(key)).toString().toStdString();
}

//------------------------- CRegistry --------------------------------------------

std::string CRegistry::SETTINGS_PATH = "HKEY_CURRENT_USER\\Software\\TNO\\%s\\Settings";

std::string CRegistry::BUILD_KEY = "Build";

CRegistry::CRegistry(std::string release, std::string build)

    : m_release(release), m_build(build),

      CRegistryBase(QString::asprintf(

                        SETTINGS_PATH.c_str(), release.c_str())
                        .toStdString()) {}

CRegistry::eRelease CRegistry::check() {
  std::string build_reg = m_settings->value(QString::fromStdString(BUILD_KEY)).toString().toStdString();

  return build_reg.empty() ? NewRelease : build_reg == m_build ? Same : NewBuild;
}
