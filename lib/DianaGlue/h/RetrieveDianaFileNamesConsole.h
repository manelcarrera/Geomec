#pragma once

#include "IRetrieveDianaFileNames.h"
#include <QString>

class CRetrieveDianaFileNamesConsole : public IRetrieveDianaFileNames {
public:
  CRetrieveDianaFileNamesConsole(const QString workingdir = QString());
  virtual ~CRetrieveDianaFileNamesConsole();

  virtual bool retrieveDianaFileNames(QString &newTitle, const std::string &title) const;

  /*private:
    CRetrieveDianaFileNamesConsole(const CRetrieveDianaFileNamesConsole& rhs);
    CRetrieveDianaFileNamesConsole& operator = (
    const CRetrieveDianaFileNamesConsole& rhs);*/

private:
  QString m_strWorkingDir;
};
