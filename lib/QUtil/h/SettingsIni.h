#pragma once

#include <QString>

class CSettingsIni
{
  QString m_usr_url;
  QString m_sys_url;

  QString m_env_DIAPATH;

private:

  CSettingsIni();
  ~CSettingsIni();

public:

  void update();

  static CSettingsIni* instance();

  void init();
  QString create_merged();
  void print_();
  QString version_str();

  QString usr_url(){ return m_usr_url; }
  QString sys_url(){ return m_sys_url; }

  QString env_DIAPATH(){ return m_env_DIAPATH; }
};
