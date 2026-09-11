#pragma once

#include <QSettings>

#include "ISettings.h"

class CSettings : public ISettings {
public:
  CSettings(const QString &organization, const QString &application);

  static void init(const QString &organization, const QString &application);

  virtual void setProfileName(const QString &profileName);
  virtual void setRegistryKey(const QString &registryKey);

  virtual bool writeProfileString(const QString &section, const QString &entry, const QString &value);

  virtual QString getProfileString(const QString &section, const QString &entry,
                                   const QString &aDefault = QString()) const;

  virtual bool writeProfileInt(const QString &section, const QString &entry, int value);

  virtual unsigned int getProfileInt(const QString &section, const QString &entry, int aDefault = 0) const;

  virtual QString getSystemString(const QString &section, const QString &entry,
                                  const QString &aDefault = QString()) const;

private:
  CSettings(const CSettings &rhs);
  CSettings &operator=(const CSettings &rhs);

  QSettings m_user;
  QSettings m_sys;
};
