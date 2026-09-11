#include "ModelData.h"
#include "ModelLifetimeFacade.h"

CModelData::CModelData(const QString& fileName)
: m_analysisLogger()
, m_modelBase(IModelLifetimeFacade::LoadMain(CModelBase::CModelLoadSaveDefault(), fileName, m_analysisLogger))
, m_fileName(fileName)
{
}

CModelBase* CModelData::operator () ()
{
  return m_modelBase;
}

const CModelBase* CModelData::operator () () const
{
  return m_modelBase;
}

const CAnalysisLogger& CModelData::getAnalysisLogger() const
{
  return m_analysisLogger;
}

const QString& CModelData::fileName() const
{
  return m_fileName;
}
