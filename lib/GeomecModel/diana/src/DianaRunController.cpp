#include "DianaRunController.h"

#include "BranchState.h"
#include "DepletionStage.h"
#include "FileCopier.h"
#include "GeomecCompactionDianaRunner.h"
#include "GeomecUtils.h"

#include "AnalysisLogger.h"
#include "ConsistencyGuard.h"
#include "DianaLicenseCheck.h"
#include "GeomecDianaRunner.h"
#include "Global.h"
#include "GlobalMessage.h"
#include "IDCAnalysisStatus.h"
#include "IRetrieveDianaFileNames.h"
#include "ISaveModel.h"
#include "TimeInterval.h"
#include "dimple.h"
#include "lbfl.h"

#include "Global.h"
#include "IProgressFactory.h"
#include "QUtil.h" // util::concurrency::threads::quit()
using namespace util::concurrency;

// std
#include <functional> // std::bind
#include <memory>
#include <thread>

namespace {
//
// FIXME: dangerous if someone extends the enum !!!
//
const char *g_run_res_cp[] = {"Success",         "RunError",        "EndDepletionError", "Cancelled",
                              "StartCheckError", "GuardCheckError", "RunFirstError",     "RunNextError",
                              "UndefinedError",  "EndStageError",   "StartRunnerError"};

const char *g_check_error_cp[] = {"Start", "End", "Guard"};
const char *run_step_cp[] = {"All", "First", "Next"};
} // namespace

namespace controller {
namespace print {
void depletions(const CModelBase &model) {
  Printer *p = Printer::instance(Printer::RunC);

  //
  //
  //
  int nD = model.NrOfDepletionStages();
  p->info("depletions: %dx: ", nD);
  /*CDepletionStage* d = &m_model.InitialDepletionStage();
  while(!d->Last())
  {
    m_printer->info( QString::asprintf( "%p-%p-%p", d->Previous_(), d, d->Next_()).toStdString().c_str() );
    d = d->Next_();
  }*/
  //
  //
  //
  CDepletionStage *d = (CDepletionStage *)&model.InitialDepletionStage();
  int i = 0;
  while (d) {
    p->info(QString::asprintf("%d: %s (%s)", i++, d->Name().toStdString().c_str(),
                              CDepletionStage::m_output_type_cp[d->OutputType()])
                .toStdString()
                .c_str());

    d = (CDepletionStage *)d->Next_();
  }
}
void depletion(const CDepletionStage *d, int i) {
  Printer *p = Printer::instance(Printer::RunC);

  p->info(QString::asprintf("--stage-- %d: %s (%s)", i++, d->Name().toStdString().c_str(),
                            CDepletionStage::m_output_type_cp[d->OutputType()])
              .toStdString()
              .c_str());
}
} // namespace print
} // namespace controller

// do a cleanup of old results every so many restarts
const int CLNRES_EVERY = 3;

//////////////////////////////////////////////////////////////////////////
//
//
//						CControllerHelper
//
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
//
//						CDianaRunController
//
//
//////////////////////////////////////////////////////////////////////////

CDianaRunController::CDianaRunController(const controller::Params &p)
    : CDianaRunController(p.model, p.antype, p.bWriteInputFiles, p.bWriteQuadDat, p.bWriteOutputStreamFile,
                          p.strTempPath) {}

//
// emits: RunEnd / RunError
//
CDianaRunController::CDianaRunController(CModelBase &model, const CAnalysisType &antype, bool bWriteInputFiles,
                                         bool bWriteQuadDat, bool bWriteOutputStreamFile, const QString &strTempPath)
    : IListener(CEvents::Controller_EH, {CancelRun, RunSuccess, RunEnd, RunController}), m_model(model),
      m_antype(antype), m_bWriteInputFiles(bWriteInputFiles), m_bWriteQuadDat(bWriteQuadDat),
      m_bWriteOutputStreamFile(bWriteOutputStreamFile), m_pStartStage(0), m_pEndStage(0), m_strTempPath(strTempPath),
      m_pRunner(0), m_nLicenseRetry(0), m_bHaveResults(false), m_bCalculationResult(false),
      m_step(CDianaRunController::All), m_helper(this), m_th(nullptr), m_wait(CEvents::Controller_EH) {
  m_printer->info("new");
}

CDianaRunController::~CDianaRunController() {
  m_printer->debug("delete >>");

  m_wait.signal();

  //
  // testing: force exit without properly finishing the working thread
  // m_th->detach()
  // -> dont do so as it crashes
  //

  threads::quit(m_th, m_printer);

  m_printer->info("delete <<");
}

//
// TODO: run, run_next, run_first, run_stage : code replicated, needs to be simplified
//

//
// only in step-by-step mode !!!
//
// in non-dsa already done in the runner
// in stagged mode done per stage in the controller
//
bool CDianaRunController::end() {
  if (m_step == CDianaRunController::All) {
    m_printer->error("retrieve: step-by-step only : now: stagged mode");
    return false;
  }

  if (!m_pRunner) {
    m_printer->error("retrieve: runner: nullptr");
    return false;
  }

  return m_pRunner->end();

  //
  // must this be done after runner->end ???
  //
  // m_pRunner->clean()
}

void log_(CAnalysisLogger &logger, int idx) {
  QDateTime time = QDateTime::currentDateTime();
  QString strTime;
  if (idx == 0) {

    strTime = time.toString("'*** Starting runner at' dd MMM yyyy, hh:mm '***'");
    logger.AddLine(strTime.toStdString(), true);
  } else if (idx == 1) {
    strTime = time.toString("'*** Runner returned at' dd MMM yyyy, hh:mm '***'");
    logger.AddLine(strTime.toStdString(), true);
  }
}

bool CDianaRunController::run_step(eRunStep step) {
  m_printer->info("run step : %s", run_step_cp[step]);

  CAnalysisLogger &logger = Model().Logger();

  const dia::IDianaRunner::RunParams &p = m_params;

  bool bRet = true;

  std::string FFDIR;
  std::string FF;

  auto check_results = [=]() {
    m_sLicenseError = m_pRunner->LicenseError();
    if (m_sLicenseError.isEmpty()) {
      //
      // mcr 2020-07-09
      //
      // FIXME: or not...
      //
      // in dsa results will be read once per stage (staged run) or step (rull all)
      //
      // problem with opening FF several times is still there ???
      //
      m_pRunner->GetResultInfo(m_bHaveResults, m_bCalculationResult, p.dsa);
    } else
      m_bHaveResults = m_bCalculationResult = false;
    return m_bHaveResults && m_bCalculationResult;
  };

  //
  //
  //

  // next-only
  dia::ITemperatureTable *pTempTable = nullptr;
  dia::IConcentrationTable *pConcTable = nullptr;
  dia::IDianaRunner::TDirectionMap *pDirectionMap = nullptr;

  m_pStartStage = step == First ? &m_model.InitialDepletionStage() : &m_pEndStage->Next();
  m_pEndStage = step == First ? &m_pStartStage->Next() : m_pStartStage;

  if (step == Next) {
    // folders
    FFDIR = m_pRunner->FFDIR();
    FF = m_pRunner->FF();

    // explicitly save tables
    pTempTable = m_pRunner->TemperatureTable();
    pConcTable = m_pRunner->ConcentrationTable();
    pDirectionMap = m_pRunner->DirectionMap();

    // TODO: why this if after that is deleteed ?
    // JH: because they are owned by the m_pRunner and would be deleted as well. This way we can just copy the pointers
    // to the new instance, instead of copying or recreating the entire data structures.
    m_pRunner->TemperatureTable(0);
    m_pRunner->ConcentrationTable(0);
    m_pRunner->DirectionMap(0);

    delete m_pRunner;
  }

  m_model.PrepareFaultsForAnalysis();

  m_pRunner = m_model.CreateDianaRunner(*this);
  m_pRunner->LicenseRetry(m_nLicenseRetry);
  m_pRunner->SetLocation(FFDIR);
  m_pRunner->SetFileName(FF);
  m_pRunner->AllowCleanUpAfterRun(false);

  if (step == Next) {
    m_pRunner->TemperatureTable(pTempTable);
    m_pRunner->ConcentrationTable(pConcTable);
    m_pRunner->DirectionMap(pDirectionMap);
  }

  m_pRunner->progress(_g->prog()->create(eProgress::Geo, "Prepare calculation"));

  log_(logger, 0);

  //
  //
  //
  m_pRunner->params(p);
  bRet = m_pRunner->start();
  //
  //
  //

  if (p.dsa)
    m_wait.wait();

  bool res1 = check_results();

  log_(logger, 1);

  if (step == First) {
    //
    // both defined in GeomecDianaRunner: SetLocation and SetFileName
    //
    // 'FFDIR' and 'FF' defined in dia::IDianaRunner
    // are not the same as
    // 'Location' and 'FileName' defined in CGeomecDianaRunnerBase -D CDianaRunner -D dia::IDianaRunner
    //
    m_pRunner->SetLocation(m_pRunner->FFDIR());
    m_pRunner->SetFileName(m_pRunner->FF());
  }

  return bRet;
}

//
// -> dsa:
//
//	- sends an event per stage being run
//	- waits till diana_app finishes running the stage: this can take long time
//	- non-blocking as it's executed in a different thread
//
//	-> runner->end:
//
//		- dsa: done per stage so no need to do it at the very end
//		- non-dsa: done inside the runner also per stage so also no need to do it either
//
//	-> runner life-cycle:
//
//		is it always 'stage', regardless of dsa/non-dsa and stagged/non-stagged?
//
CDianaRunController::eRunRes CDianaRunController::run_stage(std::string &FFDIR, std::string &FF) {
  m_printer->info("run stage ... end stage: %s",
                  m_antype == CAnalysisType::AT_NONLIN ? "next branch stage" : "last stage");

  //
  // test-only
  //
  // if(true)
  //	throw new CProgressCancel;

  CAnalysisLogger &logger = Model().Logger();

  //
  //
  //
  const dia::IDianaRunner::RunParams &p = m_params;

  eRunRes res = Success;

  auto check_results = [=]() {
    m_sLicenseError = m_pRunner->LicenseError();
    if (m_sLicenseError.isEmpty())
      m_pRunner->GetResultInfo(m_bHaveResults, m_bCalculationResult, false);
    else
      m_bHaveResults = m_bCalculationResult = false;

    return m_bHaveResults && m_bCalculationResult;
  };

  //
  // TODO: mcr 2020-07-26, this needs to be explainned !!!
  //
  m_pEndStage = m_antype == CAnalysisType::AT_NONLIN ? &m_pStartStage->GetNextBranchStage(*m_pStartStage)
                                                     : &m_pStartStage->LastStage();

  if (!check(End)) // needs to go after end stage is got
  {
    m_printer->error(g_check_error_cp[End]);
    return EndDepletionError;
  }

#if 0
  // not active yet: we break on handling the Diana messages
  if (!CDianaLicenseCheck::instance()->FoundLicense())
  {
    _m()->warn("No Diana license found. Please check before continuing.");
  }
#endif

  //
  // 2020-07-22 mcr:
  // shuoln't this be done out of the lopp or only for non-dsa?
  //
  // execute
  m_pRunner = m_model.CreateDianaRunner(*this);
  m_pRunner->LicenseRetry(m_nLicenseRetry);

  //
  // 2020-07-22 mcr:
  // why this?
  // shouldn't it be only for non-dsa?
  //
  m_pRunner->SetLocation(FFDIR);
  m_pRunner->SetFileName(FF);

  m_pRunner->progress(p.bSilent ? _g->prog()->create(eProgress::Silent)
                                : _g->prog()->create(eProgress::Geo, "Prepare calculation"));

  if (p.ana_status_container) {
    p.ana_status_container->NewPhase();
    m_pRunner->AnalysisStatus(p.ana_status_container->LastPhase());
  }

  if (!p.bSilent)
    log_(logger, 0);

  //
  // shouldn't this be out of the loop, before the loop?
  //
  m_pRunner->params(p);

  //
  //
  //

  //
  // TODO: to be tested ... don't know how to raise an exception from the runner
  //
  bool bRet = false;
  try {
    bRet = m_pRunner->start();

    if (!bRet)
      return StartRunnerError;
  } catch (...) {
    m_printer->error("runner exception");
  }

  //
  //
  //
  if (m_pRunner->Canceled()) {
    m_pRunner->clean();

    delete m_pRunner;
    m_pRunner = nullptr;

    return Cancelled;
  }
  //
  //
  //
  bool b_cleaned = false;
  if (p.dsa) {
    m_wait.wait();

    //
    // 2020-07-10: this seems to work for both staged and not staged run
    //
    //	what about?
    //
    //		- the end process done at the end in dsa
    //		- cleanup after stages
    //
    if (m_pRunner->Canceled()) {
      b_cleaned = m_pRunner->clean();
      return Cancelled;
    }
    //
    // FIXME: HC
    //
    if (p.path == "LicenseCheck.gm4") {
      m_printer->info("end skipped : license check model");
    } else {
      bRet = m_pRunner->end(); // retrieve()
      if (!bRet)
        return EndStageError;
    }
    b_cleaned = m_pRunner->clean();
  }

  //
  // what if false?
  //
  // why this is done here when FF has already been cleaned (removed)?
  //
  bool res1 = check_results();

  if (!p.bSilent)
    log_(logger, 1);

  if (m_pRunner->Canceled())
    res = Cancelled;

  FFDIR = m_pRunner->FFDIR();
  FF = m_pRunner->FF();

  delete m_pRunner;
  m_pRunner = nullptr;

  if (!bRet)
    return RunError_;

  m_printer->info("--cleaned-- %s", b_cleaned ? "yes" : "no");
  m_printer->info("end stage : is last: %s", m_pEndStage->Last() ? "yes" : "no");

  //
  // TODO: mcr 2020-07-26, this needs to be explainned ... why end->next instead of start->next
  //
  m_pStartStage = m_pEndStage->Last() ? 0 : &m_pEndStage->Next();
  //
  //
  //

  return res;
}

CDianaRunController::eRunRes CDianaRunController::run_all() {
  const dia::IDianaRunner::RunParams &p = m_params;

  controller::print::depletions(m_model);
  //
  //
  //
  m_printer->info("run all --filename-- : %s", p.path.toStdString().c_str());
  //
  // checks
  //
  if (!check(Start)) // before getting start stage
  {
    QString msg = QString("Please define at least one depletion stage after the branch stage '%1'")
                      .arg(m_model.BranchState().BranchStage().Name());
    _m()->error(msg);
    m_printer->error(g_check_error_cp[Start]);
    return StartCheckError;
  }

  m_pStartStage = (m_model.BranchState().IsBranch() && m_antype == CAnalysisType::AT_NONLIN)
                      ? &m_model.BranchState().BranchStage().Next()
                      : &m_model.InitialDepletionStage();

  if (!check(Guard)) {
    _m()->error("Please check the analysis criteria and resolve the outstanding issues.");
    m_printer->error(g_check_error_cp[Guard]);
    return GuardCheckError;
  }

  m_model.PrepareFaultsForAnalysis(p.bSilent);

  std::string FFDIR, FF;

  eRunRes res = Success;
  int i = 0;
  while (m_pStartStage) {
    controller::print::depletion(m_pStartStage, i);

    res = run_stage(FFDIR, FF);

    // ERROR
    if (res == RunError_ || res == Cancelled || res == EndDepletionError || res == EndStageError ||
        res == StartRunnerError) {
      m_printer->error("run all : %s", g_run_res_cp[res]);
      if (res == EndDepletionError)
        _m()->error("The model needs to be saved before branch files can be created\n\nSave the model and restart the "
                    "analysis");

      return res;
    }
  }

  m_pStartStage = 0;
  m_pEndStage = 0;

  return res;
}

bool CDianaRunController::check(eDepletion val) {
  switch (val) {
  case eDepletion::Start: {
    if (m_model.BranchState().IsBranch() && m_antype == CAnalysisType::AT_NONLIN &&
        m_model.BranchState().BranchStage().Last())
      return false;
    break;
  }
  case eDepletion::End: {
    if (m_antype == CAnalysisType::AT_NONLIN &&
        (m_pEndStage->OutputType() == CDepletionStage::BRANCH ||

         m_pEndStage->OutputType() == CDepletionStage::PHASE) &&
        m_params.path.isEmpty())
      return false;
    break;
  }
  case Guard: {
    CConsistencyGuard *guard = m_model.GetConsistencyGuard();
    guard->Validate(m_model, true);
    if (!guard->CanCalculate(m_model, m_antype.AnalysisType())) {
      return false;
    }
    break;
  }
  default:
    break;
  }
  return true;
}

void CDianaRunController::SetLicenseRetry(int nLicenseRetry) { m_nLicenseRetry = nLicenseRetry; }

QString CDianaRunController::LicenseError() const { return m_sLicenseError; }

bool CDianaRunController::HaveResults() const { return m_bHaveResults; }

bool CDianaRunController::GetCalculationResult() const { return m_bCalculationResult; }

void CDianaRunController::CleanUpAfterRun(const QString & /*getPathName*/, IDianaXWrapper * /*dianaXWrapper*/,
                                          const std::string & /*title*/, ISaveModel & /*saveModel*/,
                                          IRetrieveDianaFileNames & /*retrieveDianaFileNames*/) {
  if (!m_pRunner)
    return;

  m_pRunner->Cleanup(m_pRunner->FFDIR());

  delete m_pRunner;
  m_pRunner = nullptr;

  m_pStartStage = nullptr;
  m_pEndStage = nullptr;
}

void CDianaRunController::clear(bool clean) {
  m_printer->debug("clear");

  if (m_pRunner) {
    if (clean)
      m_pRunner->clean(true);

    delete m_pRunner;
    m_pRunner = nullptr;
  }

  m_pStartStage = nullptr;
  m_pEndStage = nullptr;
}

CModelBase &CDianaRunController::Model() { return m_model; }

const CModelBase &CDianaRunController::Model() const { return m_model; }

const CAnalysisType &CDianaRunController::AnalysisType() const { return m_antype; }

bool CDianaRunController::WriteInputFiles() const { return m_bWriteInputFiles; }

bool CDianaRunController::WriteQuadDat() const { return m_bWriteQuadDat; }

bool CDianaRunController::WriteOutputStreamFile() const {
  return m_bWriteOutputStreamFile || WriteInputFiles() || WriteQuadDat();
}

bool CDianaRunController::CleanupOldResults(int iStartStage) const {
  if (!m_model.CleanupOldBranchResults() || iStartStage == 0)
    return false;

  int iRestartStage = 0;
  int iStage;
  CDepletionStageEntry::const_iterator it;
  for (it = m_model.DepletionStageEntry().begin(), iStage = 0;
       it != m_model.DepletionStageEntry().end() && iStage < iStartStage; ++it, ++iStage) {
    if (it->OutputType() == CDepletionStage::BRANCH || it->OutputType() == CDepletionStage::PHASE)
      ++iRestartStage;
  }

  return iRestartStage % CLNRES_EVERY == 0;
}

const CDepletionStage &CDianaRunController::StartStage() const {
  assert(m_pStartStage);
  return *m_pStartStage;
}

const CDepletionStage &CDianaRunController::EndStage() const {
  assert(m_pEndStage);
  return *m_pEndStage;
}

bool CDianaRunController::OnWriteBranchFiles(const QString &getPathName, bool bCalcResult,
                                             ISaveModel &saveModel) const {
  m_printer->info("write branch files");

  bool bRet = true;

  assert(m_pRunner != 0); // should be called inside runner's Start context

  // get the path to the Filos file
  const std::string &strFilosFileName = m_pRunner->FF();
  const std::string &strFilosDirName = m_pRunner->FFDIR();
  std::string strFilosPath = strFilosDirName + "/" + strFilosFileName;

  // set the target location from the model's filename
  // removing the next superfluous assert: we simply save as Untitled_resultsX or require the user to save first in case
  // of branches assert(!getPathName.isEmpty()); // model should have been saved at least once

  QString strDocBase, strDocExt;
  int iDot = getPathName.lastIndexOf('.');
  if (iDot >= 0) {
    strDocBase = getPathName.left(iDot); // excluding dot
    strDocExt = getPathName.right(getPathName.length() - iDot);
  } else {
    strDocBase = getPathName; // no dot found
    strDocExt = ".gm4";
  }

  if (m_bWriteOutputStreamFile && &EndStage() == &Model().DepletionStageEntry().LastStage()) {
    QString sPath;
    QString sFileName;
    SplitPathAndFileName(getPathName, sPath, sFileName);
    std::string strStrPath = m_pRunner->FFDIR() + "\\" + sFileName.toStdString() + ".str";
    QString strStrTarget = strDocBase + ".str";
    QString strTitle;
    strTitle = QString("Copying '%1'").arg(strStrTarget);
    CFileCopier fcopier(strStrPath.c_str(), strStrTarget, strTitle);

    fcopier.Copy();
  }

  strDocBase = CreateBranchFileBaseName(strDocBase, EndStage().Index());

  if (bCalcResult && CreateBranchFiles() && EndStage().OutputType() == CDepletionStage::BRANCH) {
    // the filos file name
    QString strFilosTarget = strDocBase + ".ff";

    // the project file name
    QString strModelTarget = strDocBase + strDocExt;

    // copy the filos file and generate a model file

    // setup progress dialog for filos file copy
    QString strTitle;
    strTitle = QString("Copying '%1'").arg(strFilosTarget);
    CFileCopier fcopier(strFilosPath.c_str(), strFilosTarget.toStdString().c_str(), strTitle.toStdString().c_str());

    // setup logging
    CAnalysisLogger &logger = const_cast<CAnalysisLogger &>(Model().Logger());
    QDateTime timeCloseStart = QDateTime::currentDateTime();

    // close filos file
    dia::ff::close();

    QDateTime timeCloseEnd = QDateTime::currentDateTime();
    TTimeInterval timespan = calculateTimeInterval(timeCloseStart, timeCloseEnd);
    QString strTime = QString("*** Close analysis database took %1 days, %2 hours, %3 minutes, %4 seconds ***")
                          .arg(timespan.days)
                          .arg(timespan.hours)
                          .arg(timespan.minutes)
                          .arg(timespan.seconds);
    logger.AddLine(strTime.toStdString(), true);

    QDateTime timeCopyStart = QDateTime::currentDateTime();

    // copy it
    bRet = fcopier.Copy();

    QDateTime timeCopyEnd = QDateTime::currentDateTime();
    timespan = calculateTimeInterval(timeCopyStart, timeCopyEnd);
    strTime = QString("*** Copy branch analysis database took %1 days, %2 hours, %3 minutes, %4 seconds ***")
                  .arg(timespan.days)
                  .arg(timespan.hours)
                  .arg(timespan.minutes)
                  .arg(timespan.seconds);
    logger.AddLine(strTime.toStdString(), true);

    if (bRet) {
#define DOCOMPRESS 0
#if DOCOMPRESS
      std::auto_ptr<IProgressBase> prgCompress(
          _g->prog()->create(eProgress::Geo, "Compressing branch database", false));

      // not very sophisticated, but at least the user knows what's going on...
      prgCompress->AddSteps(4);
      prgCompress->StatusMessage(_T("Compressing branch database"));

      // change working dir
      char *pwd = new char[_MAX_PATH];
      bool bpwd = (_getcwd(pwd, _MAX_PATH) != 0);

      QString strPath;
      int iSlash = strDocBase.ReverseFind('\\');
      if (iSlash >= 0)
        strPath = strDocBase.Left(iSlash);

      if (!strPath.IsEmpty())
        chdir(strPath);

      QString strFilosFileName;
      iSlash = strFilosTarget.ReverseFind('\\');
      if (iSlash >= 0)
        strFilosFileName = strFilosTarget.Right(strFilosTarget.GetLength() - iSlash - 1);
      else
        strFilosFileName = strFilosTarget;

      // remember FF symbol
      QString strOldFFSymbol = DiGetenv("FF");

      // set new FF symbol
      QString strFF = "FF=" + strFilosFileName;
      vDiSetenv(strFF);

      // remove redundant stuff
      OpenFilos(strFilosFileName, O_RDWR);

      // results
      if (XistIndexed("/RESULT", 0))
        RemoveIndexedItem("/RESULT", 0);
      prgCompress->Step();

      // loads
      if (XistIndexed("/LOADS", 0))
        RemoveIndexedItem("/LOADS", 0);
      prgCompress->Step();

      closfc_();

      // open Filos file in read-write mode
      FCM *lfcm = f_openfc(strFilosFileName, O_RDWR);

      // open, compress and close datastructures
      OpenDs(lfcm->fc_ffcb->F_dscpru);
      dscmpr_();
      ClosDs(lfcm->fc_ffcb->F_dscpru);

      prgCompress->Step();

      // compress the file
      ftn_int_t Info[4];
      ftn_int_t iRes = cmprfl_(Info);

      // close it
      closfc_();

      // reset current dir
      if (bpwd)
        chdir(pwd);

      // reset FF symbol
      strFF = "FF=" + strOldFFSymbol;
      vDiSetenv(strFF);

      prgCompress->Step();
#endif
    } else {
      QString msg;
      msg = QString("Failed to copy the analysis database to '%1'\nAnalysis aborted").arg(strFilosTarget);
      _m()->error(msg);
      return false;
    }

    QDateTime timeOpenStart = QDateTime::currentDateTime();

    // and open the original again
    OpenFilos(strFilosPath.c_str(), O_RDONLY);

    QDateTime timeOpenEnd = QDateTime::currentDateTime();
    timespan = calculateTimeInterval(timeOpenStart, timeOpenEnd);
    strTime = QString("*** Reopen analysis database took %1 days, %2 hours, %3 minutes, %4 seconds ***")
                  .arg(timespan.days)
                  .arg(timespan.hours)
                  .arg(timespan.minutes)
                  .arg(timespan.seconds);
    logger.AddLine(strTime.toStdString(), true);

    if (bRet) {
      // temporarily override branch state to save the model file
      CBranchState::COverride BranchOverride(m_model.BranchState(), EndStage());

      QDateTime timeSaveStart = QDateTime::currentDateTime();

      bRet = saveModel.saveModel(strModelTarget, false);

      QDateTime timeSaveEnd = QDateTime::currentDateTime();
      timespan = calculateTimeInterval(timeSaveStart, timeSaveEnd);
      strTime = QString("*** Save branch model file took %1 days, %2 hours, %3 minutes, %4 seconds ***")
                    .arg(timespan.days)
                    .arg(timespan.hours)
                    .arg(timespan.minutes)
                    .arg(timespan.seconds);
      logger.AddLine(strTime.toStdString(), true);

      if (!bRet) {
        QString msg;
        msg = QString("Failed to create branch model file '%1'\nAnalysis aborted").arg(strModelTarget);
        _m()->error(msg);
      }
    }
  }
  m_printer->info("write branch files (end)");
  return bRet;
}

bool CDianaRunController::WriteCommands() const {
  CGeomecCompactionDianaRunner *pRunner = dynamic_cast<CGeomecCompactionDianaRunner *>(m_pRunner);
  assert(pRunner);

  PushDir();

  ChangeDir("/CMNDS");
  ftn_int_t idx = Inquire("SEGINF", "DIM");
  assert(idx > 0);

  bool bRet = true;

  const CDepletionStage *pStage = &m_model.BranchState().ActiveStage();
  while (pStage) {
    const CDepletionStage &endstage = pStage->GetNextBranchStage(*pStage);

    PushDir();

    // input commands

    // remove path from dat file name
    QString sDat = pStage->ModifiedFileName(m_sDatFile).c_str();
    int iDot = sDat.lastIndexOf('\\');
    if (iDot < 0)
      iDot = sDat.lastIndexOf('/');

    if (iDot >= 0)
      sDat = sDat.right(sDat.length() - iDot - 1);

    ChangeIndexedDir("SEGINF/", &idx);
    ChangeDir("INPUT");
    ChangeDir("READ");
    PutCharItem("FILE", sDat.toStdString().c_str());

    PopDir();

    idx += 10;

    PushDir();

    // geomec commands
    ChangeIndexedDir("SEGINF/", &idx);
    bRet = pRunner->DoWriteCommands(m_model.BranchState().ActiveStage(), endstage, pStage->Index());

    PopDir();

    idx += 10;

    if (endstage.Last() || !bRet)
      pStage = 0;
    else
      pStage = &endstage.Next(); // next branch
  }

  PopDir();

  return bRet;
}

bool CDianaRunController::OnModifyInputFileNames(std::string & /*sComfileName*/, std::string &sDatfileName) const {
  assert(WriteInputFiles() || WriteQuadDat());

  if (AnalysisType().AnalysisType() == CAnalysisType::AT_NONLIN)
    sDatfileName = m_pStartStage->ModifiedFileName(sDatfileName);

  return true;
}

bool CDianaRunController::OnQueryInputFileNames(const std::string &title, std::string &sComfileName,
                                                std::string &sDatfileName,
                                                IRetrieveDianaFileNames &retrieveDianaFileNames) const {
  if (m_sComFile.empty() && m_sDatFile.empty()) {
    QString newTitle;
    bool retrieved = retrieveDianaFileNames.retrieveDianaFileNames(newTitle, title);

    if (!retrieved) {
      return false;
    }

    if (!newTitle.isEmpty()) {
      sComfileName = newTitle.toStdString() + ".dcf";
      sDatfileName = newTitle.toStdString() + ".dat";
    }

    m_sComFile = sComfileName;
    m_sDatFile = sDatfileName;
  } else {
    sComfileName = m_sComFile;
    sDatfileName = m_sDatFile;
  }

  return true;
}

// private

bool CDianaRunController::CreateBranchFiles() const {
  return m_antype == CAnalysisType::AT_NONLIN && !m_bWriteInputFiles && !m_bWriteQuadDat;
}

QString CDianaRunController::CreateBranchFileBaseName(const QString &sBaseName, int nStageIndex) const {
  int iFound = -1;
  int i = sBaseName.indexOf("_D");
  while (i >= 0 && i < sBaseName.length() - 2) {
    // make sure there is at least one digit after _D
    if (sBaseName[i + 2].isDigit())
      iFound = i;
    i = sBaseName.indexOf("_D", i + 1);
  }

  // the basic rule...
  QString sNewBaseName = sBaseName;
  QString strAppend;
  strAppend = QString("_D%1").arg(EndStage().Index());
  sNewBaseName += strAppend;

  // do this if we found _Dxx in the filename
  if (iFound >= 0) {
    assert(iFound < sBaseName.length() - 2);
    QString sIndex;
    int j;
    for (j = iFound + 2; j < sBaseName.length() && sBaseName[j].isDigit(); ++j)
      sIndex += sBaseName[j];
    assert(!sIndex.isEmpty());

    // only replace if _Dxx is last part of the base name
    if (iFound + 2 + sIndex.length() == sBaseName.length()) {
      int idx = atoi(sIndex.toStdString().c_str());
      if (idx < nStageIndex) {
        QString sMask = sBaseName.left(iFound + 2) + "%1";
        sNewBaseName = QString(sMask).arg(nStageIndex);
      }
    }
  }

  return sNewBaseName;
}

void CDianaRunController::RemoveRedundantLoads() const {
  if (!m_pStartStage->Initial()) {
    int iStage;
    CDepletionStageEntry::const_iterator it;
    for (iStage = 0, it = m_model.DepletionStageEntry().begin();
         it != m_model.DepletionStageEntry().end() && &(*it) != m_pStartStage; ++it, ++iStage) {
      // remove /LOADS() directory if not initial stage and not last stage before current start stage
      if (!it->Initial() && &it->Next() != m_pStartStage) {
        ftn_int_t idx = iStage + 1; // 1-based filos index
        RemoveIndexedItem("/LOADS/", &idx);
      }
    }
  }
}

void CDianaRunController::params(const dia::IDianaRunner::RunParams &p) {
  m_params = p;
  m_printer->info("--params-- path:%s, title:%s, tmp:%s, silent:%s dsa:%s", p.path.toStdString().c_str(),
                  p.title.c_str(), p.rootdir.c_str(), p.bSilent ? "yes" : "no", p.dsa ? "yes" : "no");
};

///////////////////////////////////////////////////////////////////////////////
//
//
//					concurrency (start)
//
//
///////////////////////////////////////////////////////////////////////////////

//
// mcr 2020-05-09:
//
//		I think dsa works if there is only one depletion stage
//		for more depletion stages a thread managing a state machine must be implemented
//
//
//	if non-interactive (text) it must block execution
//
bool CDianaRunController::run(eRunStep step) {
  // m_printer->info("run: %s", run_step_cp[step]);

  m_step = step;

  if (m_params.dsa) {
    push_(step == First ? Run_First : step == Next ? Run_Next : Run_All);
    return true;
  } else {
    try {
      bool res = false;

      if (step == First || step == Next)
        res = run_step(step);
      else
        res = run_all() == Success;

      return res;
    } catch (...) {
      clear(true);
      return false;
    }
  }
}

//
// emits: RunEnd / RunError
//
void CDianaRunController::task(Cmd cmd) {
  eRunRes res = UndefinedError;

  eCmd cmd_id = (eCmd)cmd.first;

  switch (cmd_id) {
  case Run_First:
  case Run_Next: {
    CDianaRunController::eRunStep step = cmd_id == Run_First ? First : Next;
    bool res__ = run_step(step);
    res = res__ ? Success : RunNextError;
    break;
  }
  case Run_All: {
    res = run_all();
    break;
  }
  default: {
    m_printer->error("unknown evt");
    break;
  }
  }

  //
  // after running 'n' times diana...
  //
  // notify the workflow manager: iface context
  //
  if (res == Success) {
    _e->broadcast(eCmd::RunEnd); // not iteratio but the whole analysis
  } else {
    m_printer->error("tasK : %s", g_run_res_cp[res]);
    //
    // FIXME: don't make a different case
    //
    if (res != CDianaRunController::Cancelled)
      _e->broadcast(make_cmd(eCmd::RunError, new CEvents::ErrorData{CEvents::Controller_EH, res}));
  }
}

//
// dsa-only
//
// this runs in the controller thread
//
// sends an event when run is completely finished: success / error
//
// it only handles local events so no need to register neither push nor events
//
void CDianaRunController::handle(Cmd cmd) {
  eCmd cmd_id = (eCmd)cmd.first;

  // if( _g->status()==CGlobal::Cancelling && cmd_id !=CancelRun)
  //	return;

  switch (cmd_id) {
  case RunController: {
    void *data_ = cmd.second;
    dia::IDianaRunner::RunParams p = *reinterpret_cast<dia::IDianaRunner::RunParams *>(data_);
    delete data_;

    params(p);
    run(); // add command to the queue

    break;
  }
  case CancelRun: {
    if (runner())
      runner()->Cancel();
    //
    // testing:
    // delete runner(); // this crashes so don't do it
    //
    //
    m_wait.signal();
    break;
  }
  case RunSuccess: // = end analysis ... for gui and non-gui in drc case
  {
    m_wait.signal();
    break;
  }
  case RunEnd: // = analysis done ... step-by-step mode only !!!
  {
    if (step() != All)
      end();
    break;
  }
  case ClearController: // batch tests only
  {
    clear();
    break;
  }
  // Run_First
  // Run_Next
  // Run_All
  default: {
    //
    // FIXME: this can cause problems with multiple tasks requests
    //
    // in principle tassks are ordered sequencialy
    //
    threads::quit(m_th, m_printer);
    //
    m_th = new std::thread(&CDianaRunController::task, this, cmd);
    break;
  }
  }
}