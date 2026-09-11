#include "stdafx.h"

#include <cassert>

#include "WindowsRegistry.h"

CWindowsRegistry::CWindowsRegistry() : ISettings(), m_registryKey() {}

void CWindowsRegistry::init() {
  assert(m_singleton == 0);
  m_singleton = new CWindowsRegistry();
}

/*
 * This sets the application name in the registry path:
 * HKEY_CURRENT_USER\Software\<company name>\<application name>\<section name>\
 *   <value name>.
 */

void CWindowsRegistry::setProfileName(const QString &profileName) { m_profileName = profileName; }

/*
 * This sets the company name in the registry path:
 * HKEY_CURRENT_USER\Software\<company name>\<application name>\<section name>\
 *   <value name>.
 */

void CWindowsRegistry::setRegistryKey(const QString &registryKey) { m_registryKey = registryKey; }

bool CWindowsRegistry::writeProfileString(const QString &section, const QString &entry, const QString &value) {
  return AfxGetApp()->WriteProfileString(section.toStdString().c_str(), entry.toStdString().c_str(),
                                         value.toStdString().c_str());
}

QString CWindowsRegistry::getProfileString(const QString &section, const QString &entry,
                                           const QString &aDefault) const {
  return (LPCSTR)AfxGetApp()->GetProfileString(section.toStdString().c_str(), entry.toStdString().c_str(),
                                               aDefault.toStdString().c_str());
}

bool CWindowsRegistry::writeProfileInt(const QString &section, const QString &entry, int value) {
  return AfxGetApp()->WriteProfileInt(section.toStdString().c_str(), entry.toStdString().c_str(), value);
}

unsigned int CWindowsRegistry::getProfileInt(const QString &section, const QString &entry, int aDefault) const {
  return AfxGetApp()->GetProfileInt(section.toStdString().c_str(), entry.toStdString().c_str(), aDefault);
}

QString CWindowsRegistry::getSystemString(const QString &section, const QString &entry, const QString &aDefault) const {
  assert(!section.isEmpty());
  assert(!entry.isEmpty());
  assert(!m_registryKey.isEmpty());

  HKEY sectionKey = getSystemSectionKey(section);

  if (sectionKey == NULL) {
    return aDefault;
  }

  QString value;
  DWORD type, count;
  LONG result = RegQueryValueEx(sectionKey, entry.toStdString().c_str(), NULL, &type, NULL, &count);

  if (result == ERROR_SUCCESS) {
    unsigned char *data = new unsigned char[count];
    assert(type == REG_SZ);
    result = RegQueryValueEx(sectionKey, entry.toStdString().c_str(), NULL, &type, data, &count);
    value = reinterpret_cast<char *>(data);
    delete[] data;
  }

  RegCloseKey(sectionKey);

  if (result == ERROR_SUCCESS) {
    assert(type == REG_SZ);
    return value;
  }

  return aDefault;
}

//  private

HKEY CWindowsRegistry::getApplicationSystemRegistryKey() const {
  assert(!m_registryKey.isEmpty());
  assert(!m_profileName.isEmpty());

  HKEY applicationKey = NULL;
  HKEY softwareKey = NULL;
  HKEY companyKey = NULL;

  if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE", 0, KEY_READ, &softwareKey) == ERROR_SUCCESS) {
    DWORD disposition;

    if (RegCreateKeyEx(softwareKey, m_registryKey.toStdString().c_str(), 0, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_READ,
                       NULL, &companyKey, &disposition) == ERROR_SUCCESS) {
      RegCreateKeyEx(companyKey, m_profileName.toStdString().c_str(), 0, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_READ,
                     NULL, &applicationKey, &disposition);
    }
  }

  if (softwareKey != NULL) {
    RegCloseKey(softwareKey);
  }

  if (companyKey != NULL) {
    RegCloseKey(companyKey);
  }

  return applicationKey;
}

HKEY CWindowsRegistry::getSystemSectionKey(const QString &section) const {
  assert(!section.isEmpty());

  HKEY sectionKey = NULL;
  HKEY applicationKey = getApplicationSystemRegistryKey();

  if (applicationKey == NULL) {
    return NULL;
  }

  DWORD disposition;

  RegCreateKeyEx(applicationKey, section.toStdString().c_str(), 0, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_READ, NULL,
                 &sectionKey, &disposition);
  RegCloseKey(applicationKey);

  return sectionKey;
}
