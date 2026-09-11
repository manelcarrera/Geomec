
#include "VersionManager.h"
#include "GlobalMessage.h"
#include "VersionNumbers.h"
#include "resourceIDP.h"
#include <QObject>

CVersionManager::CVersionManager(const QString &applicationVersion)
    : m_file_version(VERSION_MAJOR, VERSION_MINOR, VERSION_REVISION), m_applicationVersion(applicationVersion) {}

CVersionManager::CVersionManager(const CVersionManager &rhs)
    : m_file_version(rhs.m_file_version), m_applicationVersion(rhs.m_applicationVersion) {}

CVersionManager &CVersionManager::operator=(const CVersionManager &rhs) {
  m_file_version = rhs.m_file_version;
  m_applicationVersion = rhs.m_applicationVersion;

  return *this;
}

QString &CVersionManager::AppVersion() { return m_applicationVersion; }

const QString &CVersionManager::AppVersion() const { return m_applicationVersion; }

void CVersionManager::AppVersion(const QString &applicationVersion) { m_applicationVersion = applicationVersion; }

CStreamVersion &CVersionManager::FileVersion() { return m_file_version; }

const CStreamVersion &CVersionManager::FileVersion() const { return m_file_version; }

void CVersionManager::FileVersion(const CStreamVersion &file_version) { m_file_version = file_version; }

CStreamVersion CVersionManager::CurrentVersion() const {
  return CStreamVersion(VERSION_MAJOR, VERSION_MINOR, VERSION_REVISION);
}

bool CVersionManager::CheckVersion(const CStreamVersion &version) const {
  // At least 3.0.0 is supported

  if (version.majorNr() >= 3) {
    if (CStreamVersion(VERSION_MAJOR, VERSION_MINOR, VERSION_REVISION) < version) {
      _m()->error(IDP_VERSIONTOOLOW);

      return false;
    }

    if (version.majorNr() == 3 && version.minorNr() == 7 &&
        version > CStreamVersion(VERSION_MAJOR_3, VERSION_MINOR_3, VERSION_REVISION_3)) {
      _m()->error(QObject::tr("This model was saved in a newer version of "
                              "GEOMEC 3 than is supported by this version of GEOMEC"));
      return false;
    }

    return true;
  }

  // Not supported

  _m()->error("This document version is a development version and it is not "
              "supported by Geomec");

  return false;
}
