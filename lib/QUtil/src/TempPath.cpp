// TempPath.cpp: implementation of the CTempPath class.
//
//////////////////////////////////////////////////////////////////////

#include "TempPath.h"

#include "Environment.h"
#include "GeomecUtils.h"
#include "ISettings.h"

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
// #define new DEBUG_NEW
#endif

//
// default value is the one in the env
//
CTempPath::CTempPath() { GetEnvPath(); }

CTempPath::CTempPath(const CTempPath &rhs) {
  for (int c = TEMP_GENERAL; c < _TEMP_NR_OF_CATEGORIES; ++c)
    m_usr_path_v[c] = rhs.m_usr_path_v[c];

  m_b_usr_path = rhs.m_b_usr_path;
  m_env_path = rhs.m_env_path;
}

CTempPath CTempPath::operator=(const CTempPath &rhs) {
  for (int c = TEMP_GENERAL; c < _TEMP_NR_OF_CATEGORIES; ++c)
    m_usr_path_v[c] = rhs.m_usr_path_v[c];

  m_b_usr_path = rhs.m_b_usr_path;
  m_env_path = rhs.m_env_path;

  return *this;
}

CTempPath::~CTempPath() { WriteConfig(); }

void CTempPath::Path(Category category, QString strPath) { m_usr_path_v[category] = strPath; }

QString CTempPath::Path(Category category) {
  if (m_b_usr_path) {
    switch (category) {
    case TEMP_CALCULATION: {
      bool defined = !m_usr_path_v[TEMP_CALCULATION].isEmpty();

      return m_usr_path_v[defined ? TEMP_CALCULATION : TEMP_GENERAL];
    }
    case TEMP_GENERAL:
    default:
      return m_usr_path_v[TEMP_GENERAL];
    }
  } else {
    return m_env_path;
  }
}

void CTempPath::UserPath(bool bUserPath) { m_b_usr_path = bUserPath; }

bool CTempPath::UserPath() { return m_b_usr_path; }

void CTempPath::GetEnvPath() {
  for (const auto key : {"TEMP", "TMP"}) {
    m_env_path = CEnvironment::instance()->get(key);
    if (!m_env_path.isEmpty())
      break;
  }
}

bool CTempPath::PathValid(QString &strPath) {
  if (strPath.isEmpty())
    return false; // FALSE;

  return DirExists(strPath);
}

bool CTempPath::UserPathsValid() {
  for (int c = TEMP_GENERAL; c < _TEMP_NR_OF_CATEGORIES; ++c) {
    if (!PathValid(m_usr_path_v[c]))
      return false;
  }
  return true;
}

bool CTempPath::EnvPathValid() { return PathValid(m_env_path); }

//
// from config file to members
// before: from registry
//
void CTempPath::ReadConfig() {
  ISettings *config = ISettings::instance();

  QString section = "Settings";

  m_b_usr_path = config->getProfileInt(section, "UserTemp", 0) != 0;

  m_usr_path_v[TEMP_GENERAL] = config->getProfileString(section, "UserTempPath");
  m_usr_path_v[TEMP_CALCULATION] = config->getProfileString(section, "CalcTempPath");
}

// from members to config
void CTempPath::WriteConfig() {
  ISettings *config = ISettings::instance();

  QString section = "Settings";

  config->writeProfileInt(section, "UserTemp", m_b_usr_path ? 1 : 0);
  config->writeProfileString(section, "UserTempPath", m_usr_path_v[TEMP_GENERAL]);
  config->writeProfileString(section, "CalcTempPath", m_usr_path_v[TEMP_CALCULATION]);
}
