
#include <cassert>

#include "Settings.h"

CSettings::CSettings(const QString &organization, const QString &application)
    : ISettings(), m_user(organization, application), m_sys(QSettings::SystemScope, organization, application) {}

void CSettings::init(const QString &organization, const QString &application) {
  assert(m_singleton == 0);
  m_singleton = new CSettings(organization, application);
}

void CSettings::setProfileName(const QString & /*profileName*/) {}

void CSettings::setRegistryKey(const QString & /*registryKey*/) {}

bool CSettings::writeProfileString(const QString &section, const QString &entry, const QString &value) {
  m_user.setValue(section + '/' + entry, value);

  return true;
}

QString CSettings::getProfileString(const QString &section, const QString &entry, const QString &aDefault) const {
  return m_user.value(section + '/' + entry, aDefault).toString();
}

bool CSettings::writeProfileInt(const QString &section, const QString &entry, int value) {
  m_user.setValue(section + '/' + entry, value);

  return true;
}

unsigned int CSettings::getProfileInt(const QString &section, const QString &entry, int aDefault) const {
  return m_user.value(section + '/' + entry, aDefault).toInt();
}

QString CSettings::getSystemString(const QString &section, const QString &entry, const QString &aDefault) const {
  return m_sys.value(section + '/' + entry, aDefault).toString();
}
