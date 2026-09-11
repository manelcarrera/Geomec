#include "StdAfx.h"

#include "SaveModel.h"

CSaveModel::CSaveModel(ISaveModel* strategy)
: ISaveModel()
, m_strategy(strategy)
{
}

CSaveModel::~CSaveModel()
{
  if (m_strategy)
  {
  delete m_strategy;
  }
}

bool CSaveModel::saveModel(const QString& fileName, bool bSwitch)
{
  assert(m_strategy != 0);

  return m_strategy->saveModel(fileName, bSwitch);
}
