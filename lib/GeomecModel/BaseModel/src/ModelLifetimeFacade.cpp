#include "ModelLifetimeFacade.h"

#include "ModelIO.h"
#include "VersionNumbers.h"

#include "HexaModel.h"
#include "GoCadModel.h"
#include "EclipseModel.h"
#include "WellZoomInModel.h"
#include "WellCasingModel.h"


void IModelLifetimeFacade::NewMainDecorators()
{
  CValueTypeFactory::instance()->reset();
}


CModelBase *IModelLifetimeFacade::NewMain(int type, CAnalysisLogger& logger, const CVersionManager& versionManager)
{
  CModelBase *pModel = 0;

  switch (type)
  {
  case HEXA_MODEL:
  pModel = new CHexaModel(logger, versionManager);
  break;
  case GOCAD_MODEL:
  pModel = new CGoCadModel(logger, versionManager);
  break;
  case ECLIPSE_MODEL:
  pModel = new CEclipseModel(logger, versionManager);
  break;
  }

  if (pModel)
  NewMainDecorators();

  return pModel;
}

CModelBase *IModelLifetimeFacade::NewChild(int type, CAnalysisLogger& logger, const CVersionManager& versionManager)
{
  switch (type)
  {
  case HEXA_MODEL:
  return new CHexaModel(logger, versionManager);
  case GOCAD_MODEL:
  return new CGoCadModel(logger, versionManager);
  case ECLIPSE_MODEL:
  return new CEclipseModel(logger, versionManager);
  }

  return 0;
}

CModelBase *IModelLifetimeFacade::NewChild(int type, CWellZoomInModel& parent, CAnalysisLogger& logger, const CVersionManager& versionManager)
{
  if (type == WELL_CASING_MODEL)
  return new CWellCasingModel(parent, logger, versionManager);

  return 0;
}

CModelBase *IModelLifetimeFacade::NewChild(int type, CModelBase& parent, CNewWellPath& wp, CAnalysisLogger& logger, const CVersionManager& versionManager)
{
  if (type == WELL_ZOOMIN_MODEL)
  return new CWellZoomInModel(parent, wp, logger, versionManager);

  return 0;
}

CModelBase *IModelLifetimeFacade::NewChild(int type, CModelBase& parent, CWellPath& wp, CAnalysisLogger& logger, const CVersionManager& versionManager)
{
  if (type == WELL_ZOOMIN_MODEL)
  return new CWellZoomInModel(parent, wp, logger, versionManager);

  return 0;
}


CModelBase* IModelLifetimeFacade::LoadMain(const CModelBase::IModelLoadSave& modelLoadSave, const QString& strFileName, CAnalysisLogger& logger, bool bSilent)
{
  // TODO: we may have to add a store/load for the CValueTypeFactory ONCE (if) we add custom types per model
  return privateModelIO::LoadModel(modelLoadSave, strFileName, logger, true, bSilent);
}

CModelBase* IModelLifetimeFacade::LoadChild(const CModelBase::IModelLoadSave& modelLoadSave, const QString& strFileName, CAnalysisLogger& logger)
{
  return privateModelIO::LoadModel(modelLoadSave, strFileName, logger, false);
}

bool IModelLifetimeFacade::SaveMain(const CModelBase::IModelLoadSave& modelLoadSave, CModelBase& model, const QString& strFileName, bool bSwitch)
{
  return privateModelIO::SaveModel(modelLoadSave, model, strFileName, bSwitch);
}


void IModelLifetimeFacade::Close(CModelBase *model)
{
  model->CloseModel();
}

void IModelLifetimeFacade::Delete(CModelBase *model)
{
  model->destroy();
}

