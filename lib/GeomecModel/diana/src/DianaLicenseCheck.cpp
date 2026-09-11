#include "DianaLicenseCheck.h"

#include "GeomecUtils.h"
#include "ModelLifetimeFacade.h"
#include "DianaExecuter.h"
#include "SaveModelConsole.h"
#include "RetrieveDianaFileNamesConsole.h"
#include "DianaRunController.h"
#include "Printer.h"
#include "Global.h"

CDianaLicenseCheck::CDianaLicenseCheck()
  : m_active(false)
  , m_modelName("LicenseCheck.gm4")
  , m_lastCheck(tbb::tick_count::now())
  , m_lastResult(false)
{
  QString modelPath = EnsureBackslash(GetAppPath()) + "/" + m_modelName;

  m_pModel = IModelLifetimeFacade::LoadMain(CModelBase::CModelLoadSaveDefault(), modelPath, m_logger, true);
}

CDianaLicenseCheck::~CDianaLicenseCheck()
{
  if (m_pModel)
  {
  m_pModel->setDeleting(true);
  IModelLifetimeFacade::Close(m_pModel);
  }
}

CDianaLicenseCheck *CDianaLicenseCheck::instance()
{
  static CDianaLicenseCheck _instance;
  return &_instance;
}

void CDianaLicenseCheck::Activate()
{
  m_active = true;
}

bool CDianaLicenseCheck::FoundLicense()
{
  if (!m_active)
  return true;

  static bool _internal = false;

  if (_internal)
  return true;

  _internal = true;

  if (m_pModel)
  {
  if (!m_lastResult || (tbb::tick_count::now() - m_lastCheck).seconds() > 0)
  {
    Printer::instance(Printer::RunA)->info("check license");

      CDianaExecuter diaexec;
      CSaveModelConsole smc(*m_pModel, "");
      CRetrieveDianaFileNamesConsole retr;

      CDianaRunController drc(*m_pModel, CAnalysisType::AT_LINEAR, false, false, false, GetGeomecTempPathExt(CTempPath::TEMP_CALCULATION));

    dia::IDianaRunner::RunParams p = { m_modelName, "License Check", drc.tmp().toStdString(), &diaexec, &smc, &retr, nullptr, true, _g->dsa() };
    drc.params(p);
    m_lastResult = drc.run();

      m_lastCheck = tbb::tick_count::now();
  }
  }

  _internal = false;

  return m_lastResult;
}