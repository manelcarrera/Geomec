#pragma once

#include <qstring.h>

namespace settings{
	//
	// get
	//
	QString get_s(const char* section, const char* key);
	bool get(const char* section, const char* key, bool default_=false);
	int get_i(const char* section, const char* key, int default_=0);
	//
	// set
	//
	void set_i(const char* section, const char* key, int val );
} //namespace settings


class ISettings
{
public:
  virtual ~ISettings();

  static ISettings* instance();
  static void cleanup();

  virtual void setProfileName(const QString& profileName) = 0;
  virtual void setRegistryKey(const QString& registryKey) = 0;

	virtual bool writeProfileString(
		const QString& section, 
		const QString& entry,
		const QString& value) = 0;

	virtual QString getProfileString(
		const QString& section, 
		const QString& entry,
		const QString& aDefault = QString()) const = 0;

	virtual bool writeProfileInt(
		const QString& section, 
		const QString& entry,
		int value) = 0;

	virtual unsigned int getProfileInt(
		const QString& section,
		const QString& entry, 
		int aDefault = 0) const = 0;

	virtual QString getSystemString(
		const QString& section, 
		const QString& entry,
		const QString& aDefault = QString()) const = 0;

protected:
  ISettings();

  static ISettings* m_singleton;

private:
  ISettings(const ISettings& rhs);
  ISettings& operator = (const ISettings& rhs);
};