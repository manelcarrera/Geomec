#include "InterfaceContext.h"

#include <QDir>
#include <QString>

#include "BranchState.h"
#include "ConsistencyGuard.h"
#include "DepletionStage.h"
#include "DianaExecuter.h"
#include "DianaRunController.h"
#include "GeomecShellUtils.h"
#include "GeomecUtils.h"
#include "GoCadModel.h"
#include "IProgressFactory.h"
#include "ModelBase.h"
#include "ModelLifetimeFacade.h"
#include "RetrieveDianaFileNamesConsole.h"
#include "SKUAParseData.h"
#include "SaveModelConsole.h"
#include "TetraMesh.h"
#include "TetraSubBoundary.h"
#include "TetraSubHorizon.h"
#include "ValidateModel.h"
#include "VersionNumbers.h"

#include "RunAnalysis_CLI.h"

// FIXME
#include "IDCDianaRunner.h"

#include "Global.h"
#include "Printer.h"

namespace {
Printer *printer = Printer::instance(Printer::Shell);
}

namespace gm_shell {

//
// FIXME: why needs to be static in this case
//
// static Printer* printer = Printer::instance(Printer::Shell);

CInterfaceModelContext::CInterfaceModelContext(const QString &app_version)
    : m_pModel(0), m_VersionManager(app_version), m_created(false), m_app_version(app_version), m_pDianaExecuter(0),
      m_pSaveModel(0), m_pRetrieveDianaFileNames(0), m_drc(0), m_nStage(0), m_bHasRun(false), m_bAutoExport(false),
      m_nLicenseRetry(0) {
  printer->info("context >>");
}

CInterfaceModelContext::~CInterfaceModelContext() {
  printer->info("context : delete");

  //
  // multiple deletes
  //
  ShutdownDianaRunEnvironment();

  printer->info("context <<");
}

void CInterfaceModelContext::Load(const QString &model, IModelDiagnostics *pModelDiagnostics) {
  m_pModel = IModelLifetimeFacade::LoadMain(CModelBase::CModelLoadSaveDefault(0, pModelDiagnostics), model, m_logger);

  ProcessModelPath(model);
}

void CInterfaceModelContext::Create(const QString &model) {
  m_created = true;

  m_pModel = IModelLifetimeFacade::NewMain(GOCAD_MODEL, m_logger, m_VersionManager);

  m_pModel->Name(model.left(model.size() - 4));
  m_pModel->NewModel(false);

  CTetraSubHorizon *top = new CTetraSubHorizon("Top Horizon", false, *m_pModel);
  CTetraSubHorizon *bottom = new CTetraSubHorizon("Bottom Horizon", false, *m_pModel);
  CTetraSubBoundary &boundary = dynamic_cast<CTetraModel *>(m_pModel)->SubBoundary();

  boundary.TopHorizon().LinkTo(*top);
  boundary.BottomHorizon().LinkTo(*bottom);

  ProcessModelPath(model);
}

bool selectGocadSolidDlg(const std::vector<const gm_skua::SKUAParseData *> &solids, std::vector<int> &vcSolids) {
  vcSolids.clear();
  vcSolids.push_back(1);

  return true;
}

void CInterfaceModelContext::Create(const QString &model, const QString &skua) {
  m_created = true;

  m_pModel = IModelLifetimeFacade::NewMain(GOCAD_MODEL, m_logger, m_VersionManager);

  m_pModel->Name(model.left(model.size() - 4));
  m_pModel->NewModel(false);

  CTetraMesh &mesh = static_cast<CTetraMesh &>(m_pModel->Mesh());
  mesh.ImportMeshFromGoCadFile(skua, selectGocadSolidDlg);

  ProcessModelPath(model);
}

bool CInterfaceModelContext::Validate(bool forRockMech) {
  if (m_pModel) {
    CValidateModel v(m_pModel);

    return v.checkModel(forRockMech);
  }
  return false;
}

void CInterfaceModelContext::OutputPath(const QString &outputPath) { m_output_path = outputPath; }

QString CInterfaceModelContext::OutputPath() const { return m_output_path; }

void CInterfaceModelContext::TempPath(const QString &tempPath) { m_temp_path = tempPath; }

QString CInterfaceModelContext::TempPath() const { return m_temp_path; }

QString CInterfaceModelContext::ModelFullPath() const { return m_model_full_path; }

QString CInterfaceModelContext::ModelPath() const { return m_model_path; }

QString CInterfaceModelContext::ModelName() const { return m_model_name; }

QString CInterfaceModelContext::ModelExtension() const { return m_model_extension; }

CModelBase *CInterfaceModelContext::Model() { return m_pModel; }

void CInterfaceModelContext::SwitchTo(CModelBase *pModel) {
  m_pModel = pModel;
  m_pModel->GetConsistencyGuard()->SwitchToModel(*m_pModel);
}

void CInterfaceModelContext::SwitchToParent() {
  CModelBase *pModel = dynamic_cast<CModelBase *>(m_pModel->parentModel());
  if (pModel) {
    m_pModel = pModel;
    m_pModel->GetConsistencyGuard()->SwitchToModel(*m_pModel);
  }
}

bool CInterfaceModelContext::Created() const { return m_created; }

void CInterfaceModelContext::CopyFiles() {
  QString source = QDir::toNativeSeparators(m_model_full_path);
  QString destination = QDir::toNativeSeparators(m_output_path + "/" + m_model_name + "." + m_model_extension);

  copyFile(destination, source);
}

void CInterfaceModelContext::MoveFiles() {
  QString destination = QDir::toNativeSeparators(m_model_full_path);
  QString source = QDir::toNativeSeparators(m_output_path + "/" + m_model_name + "." + m_model_extension);

  moveFile(destination, source);
}

void CInterfaceModelContext::Stage(int nStage) { m_nStage = nStage; }

int CInterfaceModelContext::Stage() const { return m_nStage; }

void CInterfaceModelContext::Type(const QString &type) { m_type = type; }

QString CInterfaceModelContext::Type() const { return m_type; }

void CInterfaceModelContext::AutoExport(bool flag) { m_bAutoExport = flag; }

bool CInterfaceModelContext::AutoExport() const { return m_bAutoExport; }

void CInterfaceModelContext::LicenseRetry(int nLicenseRetry) { m_nLicenseRetry = nLicenseRetry; }

int CInterfaceModelContext::LicenseRetry() const { return m_nLicenseRetry; }

QString CInterfaceModelContext::LicenseError() const { return m_sLicenseError; }

bool CInterfaceModelContext::HaveResults() const { return m_bHaveResults; }

bool CInterfaceModelContext::GetCalculationResult() const { return m_bCalculationResult; }

bool CInterfaceModelContext::Run(CInterfaceLogWrapper *log) {
  printer->info("conetxt : Run");

  SetupDianaRunEnvironment();

  // setup branch state
  CDepletionStage *pStage = &m_pModel->DepletionStageEntry().LastStage();
  while (pStage && !pStage->IsPhaseStartStage()) {
    if (pStage->Initial())
      pStage = 0;
    else
      pStage = &pStage->Previous();
  }

  assert(pStage);
  if (!pStage) {
    if (log)
      log->AddLine("Unable to determine first depletion stage for analysis", false, true);
    return false;
  }

  if (pStage->Initial())
    m_pModel->BranchState().ClearBranch();

  if (log)
    log->AddLine("Starting GEOMEC non-linear analysis");
  bool bRet = false;
  try {
    CAnalysisType::TAnalysisType type = CAnalysisType::AT_NONLIN;

    if (m_type == "linear")
      type = CAnalysisType::AT_LINEAR;
    else if (m_type == "heat")
      type = CAnalysisType::AT_HEAT;
    else if (m_type == "mixture")
      type = CAnalysisType::AT_MIXTURE;
    else if (m_type == "containment")
      type = CAnalysisType::AT_MIXTURE_CONTAINMENT;

    QString strTempPath = GetGeomecTempPathExt(CTempPath::TEMP_CALCULATION);
    m_drc = new CDianaRunController(*m_pModel, type, false, false, false, strTempPath);
    m_drc->SetLicenseRetry(m_nLicenseRetry);

    dia::IDianaRunner::RunParams p = {m_model_full_path,
                                      m_model_name.toStdString(),
                                      strTempPath.toStdString(),
                                      m_pDianaExecuter,
                                      m_pSaveModel,
                                      m_pRetrieveDianaFileNames,
                                      nullptr,
                                      false,
                                      _g->dsa()};
    m_drc->params(p);

    //
    //
    //
    if (p.dsa) {
      CRunAnalysis_CLI ra(m_drc, CDianaRunController::All);
      ra.wait();
      bRet = ra.res();
    } else {
      bRet = m_drc->run();
    }
    //
    //
    //

    m_sLicenseError = m_drc->LicenseError();
    m_bHaveResults = m_drc->HaveResults();
    m_bCalculationResult = m_drc->GetCalculationResult();

    if (m_bAutoExport) {
      CConsistencyGuard *guard = m_pModel->GetConsistencyGuard();
      guard->AutoResultExport(*m_pModel, type);
    }
  }
  //
  // why this?
  //
  catch (CProgressCancel *p) {
    delete p;
    bRet = false;
    if (log)
      log->AddLine("GEOMEC analysis cancelled", false, true);
  }
  if (log)
    log->AddLine("GEOMEC non-linear analysis ended");

  m_bHasRun = true;

  return bRet;
}

//
// not used by now
//
void CInterfaceModelContext::End() {
  if (_g->dsa())
    m_drc->end();
  else
    printer->error("context : end : no sense because no dsa");
}

bool CInterfaceModelContext::RunStep(CInterfaceLogWrapper *log) {
  printer->info("conetxt : RunStep");

  if (!m_pDianaExecuter)
    SetupDianaRunEnvironment();

  bool bRet = false;

  if (!m_bHasRun) // First
  {
    // setup branch state
    CDepletionStage *pStage = &m_pModel->DepletionStageEntry().LastStage();
    while (pStage && !pStage->Initial()) {
      pStage->setOutputType(CDepletionStage::PHASE);
      pStage = &pStage->Previous();
    }

    pStage = &m_pModel->DepletionStageEntry().LastStage();
    while (pStage && !pStage->IsPhaseStartStage()) {
      if (pStage->Initial())
        pStage = 0;
      else
        pStage = &pStage->Previous();
    }

    assert(pStage);
    if (!pStage) {
      if (log)
        log->AddLine("Unable to determine first depletion stage for analysis", false, true);
      return false;
    }

    if (pStage->Initial())
      m_pModel->BranchState().ClearBranch();

    if (log)
      log->AddLine("Starting GEOMEC non-linear analysis");
    try {
      m_temp_path = GetGeomecTempPathExt(CTempPath::TEMP_CALCULATION);
      m_drc = new CDianaRunController(*m_pModel, CAnalysisType::AT_NONLIN, false, false, false, m_temp_path);
      m_drc->SetLicenseRetry(m_nLicenseRetry);

      dia::IDianaRunner::RunParams p = {m_model_full_path,
                                        m_model_name.toStdString(),
                                        m_temp_path.toStdString(),
                                        m_pDianaExecuter,
                                        m_pSaveModel,
                                        m_pRetrieveDianaFileNames,
                                        nullptr,
                                        false,
                                        _g->dsa()};
      m_drc->params(p);

      //
      //
      //
      if (p.dsa) {
        CRunAnalysis_CLI ra(m_drc, CDianaRunController::First);
        ra.wait();
        bRet = ra.res();
      } else {
        bRet = m_drc->run(CDianaRunController::First);
      }
      //
      //
      //

      m_sLicenseError = m_drc->LicenseError();
      m_bHaveResults = m_drc->HaveResults();
      m_bCalculationResult = m_drc->GetCalculationResult();
    }
    //
    // why this?
    //
    catch (CProgressCancel *p) {
      delete p;
      bRet = false;
      if (log)
        log->AddLine("GEOMEC analysis cancelled", false, true);
    }
    if (log)
      log->AddLine("GEOMEC non-linear analysis ended");
  } else // Next
  {
    if (log)
      log->AddLine("Starting GEOMEC non-linear analysis");
    try {
      //
      //
      //
      if (_g->dsa()) {
        CRunAnalysis_CLI ra(m_drc, CDianaRunController::Next);
        ra.wait();
        bRet = ra.res();
      } else {
        bRet = m_drc->run(CDianaRunController::Next);
      }
      //
      //
      //

      m_sLicenseError = m_drc->LicenseError();
      m_bHaveResults = m_drc->HaveResults();
      m_bCalculationResult = m_drc->GetCalculationResult();
    } catch (CProgressCancel *p) {
      delete p;
      bRet = false;
      if (log)
        log->AddLine("GEOMEC analysis cancelled", false, true);
    }
    if (log)
      log->AddLine("GEOMEC non-linear analysis ended");
  }

  m_bHasRun = true;

  return bRet;
}

bool CInterfaceModelContext::HasRun() const { return m_bHasRun; }

void CInterfaceModelContext::CleanUpAfterRun() {
  m_drc->CleanUpAfterRun(m_output_path, m_pDianaExecuter, m_model_name.toStdString(), *m_pSaveModel,
                         *m_pRetrieveDianaFileNames);
}

void CInterfaceModelContext::Save() {
  // Not implemented yet
}

void CInterfaceModelContext::SaveAs(const QString &fileName) {
  IModelLifetimeFacade::SaveMain(CModelBase::CModelLoadSaveDefault(m_app_version), m_pModel->RootModel(), fileName,
                                 true);
}

CDianaExecuter *CInterfaceModelContext::GetDianaExecuter() { return m_pDianaExecuter; }
CSaveModelConsole *CInterfaceModelContext::GetSaveModel() { return m_pSaveModel; }
CRetrieveDianaFileNamesConsole *CInterfaceModelContext::GetRetrieveDianaFileNames() {
  return m_pRetrieveDianaFileNames;
}
QString CInterfaceModelContext::ModelWorkPath() {
  QString destination = QDir::toNativeSeparators(m_output_path + "/" + m_model_name + "." + m_model_extension);
  return destination;
}

void CInterfaceModelContext::ProcessModelPath(const QString &model) {
  m_model_full_path = model;

  QString fileName;
  SplitPathAndFileName(m_model_full_path, m_model_path, fileName);

  m_model_name = RemoveExtension(fileName);
  m_model_extension = fileName.right(fileName.size() - m_model_name.size() - 1);
}

void CInterfaceModelContext::SetupDianaRunEnvironment() {
  m_pDianaExecuter = new CDianaExecuter();
  m_pSaveModel = new CSaveModelConsole(*m_pModel, m_app_version);
  m_pRetrieveDianaFileNames = new CRetrieveDianaFileNamesConsole();
}

void CInterfaceModelContext::ShutdownDianaRunEnvironment() {
  delete m_drc;
  delete m_pRetrieveDianaFileNames;
  delete m_pSaveModel;
  delete m_pDianaExecuter;
}

} // namespace gm_shell
