#include "RetrieveDianaFileNamesConsole.h"

#include <QDir>

CRetrieveDianaFileNamesConsole::CRetrieveDianaFileNamesConsole(const QString workingdir)
: IRetrieveDianaFileNames(),
  m_strWorkingDir(workingdir)
{
}

CRetrieveDianaFileNamesConsole::~CRetrieveDianaFileNamesConsole()
{
}

bool CRetrieveDianaFileNamesConsole::retrieveDianaFileNames(QString& newTitle,
  const std::string& title) const
{
  newTitle = QString::fromStdString(title);
  if(newTitle.right(4) == ".gm4" || newTitle.right(4) == ".gmp")
  newTitle = newTitle.left(newTitle.length() - 4);

  if(!m_strWorkingDir.isEmpty())
  {
  newTitle = m_strWorkingDir + QDir::separator() + newTitle;
  }

  return true;
}
