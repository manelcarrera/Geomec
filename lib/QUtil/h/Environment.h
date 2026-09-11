#pragma once

#include <QString>

class CEnvironment {
public:
  static CEnvironment *instance();

  static QString DIAPATH;
  static QString DIASHARE;
  static QString DIALIB;
  static QString DIASLIB;
  static QString DianaExeName;
  static QString InputExeName;
  static QString FgvExeName;
  static QString STP0;
  static QString DIAERRPATH;
  static QString APP_SPC_LOGGING;

  QString get(QString key) const;

private:
  static CEnvironment *m_singleton;

private:
  CEnvironment();
  CEnvironment(const CEnvironment &rhs);
  CEnvironment &operator=(const CEnvironment &rhs);
};