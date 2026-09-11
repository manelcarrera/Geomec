// TempPath.h: interface for the CTempPath class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <qstring.h>

//
//
//
//
//
//
//
//

class CTempPath {
public:
  typedef enum { TEMP_GENERAL = 0, TEMP_CALCULATION, _TEMP_NR_OF_CATEGORIES } Category;

  CTempPath();
  CTempPath(const CTempPath &rhs);
  CTempPath operator=(const CTempPath &rhs);

  virtual ~CTempPath();

  void Path(Category category, QString strPath);
  QString Path(Category category);

  void UserPath(bool bUserPath);
  bool UserPath();

  bool UserPathsValid();
  bool EnvPathValid();
  void ReadConfig();
  void WriteConfig();

private:
  void GetEnvPath();
  bool PathValid(QString &strPath);

  QString m_usr_path_v[_TEMP_NR_OF_CATEGORIES];
  bool m_b_usr_path;
  QString m_env_path;
};