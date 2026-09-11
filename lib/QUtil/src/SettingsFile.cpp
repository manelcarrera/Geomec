
#include <cassert>

#include "SettingsFile.h"

#include "Printer.h"

CSettingsFile::CSettingsFile(
  const QString& usr_filename,
  const QString& sys_filename )
: ISettings()
, m_usr( usr_filename, QSettings::IniFormat )
, m_sys( sys_filename, QSettings::IniFormat )
{
  //Printer::instance()->debug("reg : usr : %s", m_usr.fileName().toStdString().c_str());
  //Printer::instance()->debug("reg : sys : %s", m_sys.fileName().toStdString().c_str());
}

void CSettingsFile::init(	
  const QString& usr_filename,
  const QString& sys_filename )
{
  assert(m_singleton == 0);
  if(m_singleton)
    delete m_singleton;
  m_singleton = new CSettingsFile( usr_filename, sys_filename );
}

void CSettingsFile::setProfileName(const QString& /*profileName*/)
{
}

void CSettingsFile::setRegistryKey(const QString& /*registryKey*/)
{
}
////////////////////////////////////////////////////////
//
//		[1] user
//
////////////////////////////////////////////////////////

bool CSettingsFile::writeProfileString(
  const QString& section,
  const QString& entry, 
  const QString& value)
{
  /*Printer::instance()->debug("set : %s/%s:%s", 
    section.toStdString().c_str(), 
    entry.toStdString().c_str(), 
    value.toStdString().c_str() );*/

  m_usr.setValue(section + '/' + entry, value);

  return true;
}

QString CSettingsFile::getProfileString(
  const QString& section,
  const QString& entry, 
  const QString& aDefault) const
{
  /*Printer::instance()->debug(" get : %s/%s:%s", 
    section.toStdString().c_str(), 
    entry.toStdString().c_str(), 
    aDefault.toStdString().c_str() );*/

  return m_usr.value(section + '/' + entry, aDefault).toString();
}

bool CSettingsFile::writeProfileInt(
  const QString& section,
  const QString& entry, 
  int value)
{
  /*Printer::instance()->debug("set : %s/%s:%d", 
    section.toStdString().c_str(), 
    entry.toStdString().c_str(), 
    value );*/

  m_usr.setValue(section + '/' + entry, value);

  return true;
}

unsigned int CSettingsFile::getProfileInt(
  const QString& section,
  const QString& entry, 
  int aDefault) const
{
  return m_usr.value(section + '/' + entry, aDefault).toInt();
}

////////////////////////////////////////////////////////
//
//		[2] system
//
// string: so far used only five times, all in the model
// int: not used
////////////////////////////////////////////////////////

QString CSettingsFile::getSystemString(
  const QString& section,
  const QString& entry, 
  const QString& aDefault) const
{
  /*Printer::instance()->debug(" get_sys : %s/%s:%s", 
    section.toStdString().c_str(), 
    entry.toStdString().c_str(), 
    aDefault.toStdString().c_str() );*/

  return m_sys.value(section + '/' + entry, aDefault).toString();
}

// not used so far
unsigned int CSettingsFile::getSystemInt(
  const QString& section,
  const QString& entry, 
  int aDefault) const
{
  return m_sys.value(section + '/' + entry, aDefault).toInt();
}
