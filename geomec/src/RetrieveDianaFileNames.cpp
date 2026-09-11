#include "StdAfx.h"

#include "RetrieveDianaFileNames.h"

CRetrieveDianaFileNames::CRetrieveDianaFileNames(
  IRetrieveDianaFileNames* strategy)
: IRetrieveDianaFileNames()
, m_strategy(strategy)
{
}

CRetrieveDianaFileNames::~CRetrieveDianaFileNames()
{
  if (m_strategy)
  {
    delete m_strategy;
  }
}

bool CRetrieveDianaFileNames::retrieveDianaFileNames(QString& newTitle,
  const std::string& title) const
{
  assert(m_strategy != 0);

  return m_strategy->retrieveDianaFileNames(newTitle, title);
}
