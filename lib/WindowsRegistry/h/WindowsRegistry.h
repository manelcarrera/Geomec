#pragma once

#include <QString>

#include "ISettings.h"

class CWindowsRegistry : public ISettings
{
public:
  CWindowsRegistry();

  static void init();

  virtual void setProfileName(const QString& profileName);
  virtual void setRegistryKey(const QString& registryKey);

  virtual bool writeProfileString(const QString& section, const QString& entry,
    const QString& value);
  virtual QString getProfileString(const QString& section, const QString& entry,
    const QString& aDefault = QString()) const;

  virtual bool writeProfileInt(const QString& section, const QString& entry,
    int value);
  virtual unsigned int getProfileInt(const QString& section,
    const QString& entry, int aDefault = 0) const;

  virtual QString getSystemString(const QString& section, const QString& entry,
    const QString& aDefault = QString()) const;

private:
  CWindowsRegistry(const CWindowsRegistry& rhs);
  CWindowsRegistry& operator = (const CWindowsRegistry& rhs);

  HKEY getApplicationSystemRegistryKey() const;
  HKEY getSystemSectionKey(const QString& section) const;

  QString m_profileName;
  QString m_registryKey;
};