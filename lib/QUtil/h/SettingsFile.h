#ifndef _QtFileRegistry_h_
#define _QtFileRegistry_h_

#include <QSettings>

#include "ISettings.h"

class CSettingsFile : public ISettings
{
public:
  CSettingsFile(	const QString& usr_filename, 
					const QString& sys_filename);

  static void init( const QString& usr_filename, 
					const QString& sys_filename);

  virtual void setProfileName(const QString& profileName);
  virtual void setRegistryKey(const QString& registryKey);

  ////////////// [1] user

  virtual bool writeProfileString(
	  const QString& section, 
	  const QString& entry,
	  const QString& value);

  virtual QString getProfileString(
	  const QString& section, 
	  const QString& entry,
      const QString& aDefault = QString()) const;

  virtual bool writeProfileInt(
	  const QString& section, 
	  const QString& entry,
	  int value);

  virtual unsigned int getProfileInt(
	const QString& section,
	const QString& entry, 
	int aDefault = 0) const;

  ////////////// [2] system

  virtual QString getSystemString(
	const QString& section, 
	const QString& entry,
    const QString& aDefault = QString()) const;

  virtual unsigned int getSystemInt(
	const QString& section,
	const QString& entry, 
	int aDefault) const;


private:
  CSettingsFile(const CSettingsFile& rhs);
  CSettingsFile& operator = (const CSettingsFile& rhs);

  QSettings m_usr;
  QSettings m_sys;
};

#endif  // _QtFileRegistry_h_
