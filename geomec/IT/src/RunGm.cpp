#include "stdafx.h"

#include "RunGm.h"

// RunStep:
#include "DianaExecuter.h"
#include "DianaRunController.h"
#include "GeomecUtils.h"
#include "IProgressFactory.h"
#include "RetrieveDianaFileNamesConsole.h"
#include "SaveModelConsole.h"

#include "BranchState.h"
#include "DepletionStage.h"
#include "GeomecDoc.h"
#include "ModelBase.h"
#include "TempPath.h"

#include "Global.h" //dsa

// AppVersion
#include "version.h"
#include <QString>
namespace {
const QString APPLICATION = "geomec";
const QString VERSION =
    xstr(GM_MAJOR) "." xstr(GM_MINOR) "." xstr(GM_BUILD) "." xstr(GM_REVISION) " " GM_BUILD_PREFIX
                                                                               " " xstr(GM_BUILD_STRING);

const QString AppVersion() { return APPLICATION + " " + VERSION; }
} // namespace

CRunGm::CRunGm() {
  m_doc = GetGeomecDoc();
  m_app_version = AppVersion();
  init();
}

CRunGm::~CRunGm() {
  delete m_pDianaRunController;
  m_pDianaRunController = nullptr;

  if (m_pDianaExecuter) {
    delete m_pDianaExecuter;
    m_pDianaExecuter = nullptr;
  }
  if (m_pSaveModel) {
    delete m_pSaveModel;
    m_pSaveModel = nullptr;
  }
  if (m_pRetrieveDianaFileNames) {
    delete m_pRetrieveDianaFileNames;
    m_pRetrieveDianaFileNames = nullptr;
  }
}

void CRunGm::init() {
  m_bHasRun = false;
  m_model = (CModelBase *)(m_doc->Model());
  m_temp_path = GetGeomecTempPathExt(CTempPath::TEMP_CALCULATION);
  m_pDianaRunController = new CDianaRunController(*m_model, CAnalysisType::AT_NONLIN, false, false, false, m_temp_path);

  m_model_full_path = m_doc->GetPathName();
  QString fileName;
  SplitPathAndFileName(m_model_full_path, m_model_path, fileName);
  m_model_name = RemoveExtension(fileName);

  m_pDianaExecuter = nullptr;
  m_pSaveModel = nullptr;
  m_pRetrieveDianaFileNames = nullptr;

  m_bHasRun = false;
}

CRunGm *CRunGm::instance() {
  static CRunGm _instance;
  return &_instance;
}
void CRunGm::setup_diana_run_env() {
  m_pDianaExecuter = new CDianaExecuter();
  m_pSaveModel = new CSaveModelConsole(*m_model, m_app_version);
  m_pRetrieveDianaFileNames = new CRetrieveDianaFileNamesConsole();
}

bool CRunGm::step() {
  bool bRet = false;

  if (!m_pDianaExecuter)
    setup_diana_run_env();

  if (!m_bHasRun) {
    // setup branch state
    CDepletionStage *pStage = &m_model->DepletionStageEntry().LastStage();
    while (pStage && !pStage->Initial()) {
      pStage->setOutputType(CDepletionStage::PHASE);
      pStage = &pStage->Previous();
    }

    pStage = &m_model->DepletionStageEntry().LastStage();
    while (pStage && !pStage->IsPhaseStartStage())
      pStage = pStage->Initial() ? 0 : &pStage->Previous();

    assert(pStage);
    if (pStage) {
      if (pStage->Initial())
        m_model->BranchState().ClearBranch();

      // if (log) log->AddLine("Starting GEOMEC non-linear analysis");
      try {
        m_temp_path = GetGeomecTempPathExt(CTempPath::TEMP_CALCULATION);
        m_pDianaRunController =
            new CDianaRunController(*m_model, CAnalysisType::AT_NONLIN, false, false, false, m_temp_path);

        dia::IDianaRunner::RunParams p = {m_model_full_path,
                                          m_model_name.toStdString(),
                                          m_temp_path.toStdString(),
                                          m_pDianaExecuter,
                                          m_pSaveModel,
                                          m_pRetrieveDianaFileNames,
                                          nullptr,
                                          false,
                                          _g->dsa()};
        m_pDianaRunController->params(p);

        bRet = m_pDianaRunController->run(CDianaRunController::First);
      } catch (CProgressCancel *p) {
        delete p;
        bRet = false;
      }
      m_bHasRun = true;
    }
  } else {
    try {
      bRet = m_pDianaRunController->run(CDianaRunController::Next);
    } catch (CProgressCancel *p) {
      delete p;
      bRet = false;
    }
  }

  return bRet;
}