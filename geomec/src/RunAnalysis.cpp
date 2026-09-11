#include "stdafx.h"

#include "RunAnalysis.h"

#include "Printer.h"

#include <QTimer>

#include "GeomecDoc.h"

#include "ExecuteDianaDialog.h"
#include "ExecuteDianaDialogQt.h"

#include "GlobalMessage.h"

#include "TimeInterval.h"
#include "ConsistencyGuard.h"

// to run the model from the GUI app
#include "DianaXWrapper.h"
#include "SaveModelGUI.h"
#include "RetrieveDianaFileNamesGUI.h"
#include "RetrieveDianaFileNames.h"
#include "DianaRunController.h"
#include "SaveModel.h"

#include "DianaLicenseCheck.h"

#include "GeomecUtils.h" //GetGeomecTempPathExt

#include "resourceIDS.h" //IDS_USE_EXISTING_RESULTS

#include "ModelBase.h"
#include "BranchState.h" // for the preliminary checks

// to check license
#include "DianaExecuter.h"
#include "SaveModelConsole.h"
#include "RetrieveDianaFileNamesConsole.h"

#include "ProgressWrapper.h"
#include "ProgressFactory.h"
#include "ProgressFactory_MFC.h"

#include "QuestionThreadWrapper.h"

#include "DianaStartUp.h"
#include "Global.h"

///////////////////////////////////////////////////////////////////////////////////////////
//
//					!!!!!!! IMPORTANT !!!!!!
//
// diffs dsa / non-dsa:
//
//	dlg: 
//		dsa: MFC / non-dsa: Qt (empty of logic, it only forwards cmds to diana )
//		dlg events: dsa: forwarded to doc / non-dsa: just executed
// 
//	workflow:
//		- non-dsa: controller does run (it also does retrieve) + save all in a row
//		- dsa: controller does run, retrieve and save all in an async way
//
//	threads:
//		- non-dsa: everything happens in the gui thread 
//			-> working on improve this, it would maybe be enought by:
//				1) forward events to doc
//				2) keep using MFC dlg?
//		- dsa
//				non-gui processes run in any thread
//				gui actiosn forwarded to main gui thread: doc
//
///////////////////////////////////////////////////////////////////////////////////////////


namespace
{
  static const QString LICENSE_CHECK_MODEL_FILE = "LicenseCheck.gm4";

  static const IListenerDefinition RunAnalysis_ListenerDefinition{ 
    CEvents::RA_EH, 			
    {						// usr triggers next two events, they come from doc
      SetParams,
      Start,
      //
      DisableViews_Done,	// then run can be performed, it comes from gui (doc)
      RunEnd,				// analysis end end
      //
      CancelRun
    } 
  };

  std::string results_url(const QString& path_)
  {
    CString path(path_.toStdString().c_str());

    if (path.IsEmpty())
    {
      char* home = DiGetenv("HOME");
      CString strHome;
      if (home)
      {
        strHome = home;
      }
      else
      {
        char* homedrive = DiGetenv("HOMEDRIVE");
        char* homepath = DiGetenv("HOMEPATH");
        if (homedrive && homepath)
          strHome = CString(homedrive) + homepath;
      }

      if (strHome.IsEmpty())
      {
        char cwd[_MAX_PATH];
        _getcwd(cwd, _MAX_PATH);
        strHome = cwd;
      }

      path.Format("%s\\Untitled." GEOMEC_DEFAULT_EXTENSION, strHome);
    }

    CString strBase, strExt;

    int iDot = path.ReverseFind('.');
    if (iDot >= 0)
      strBase = path.Left(iDot);

    strExt = "." GEOMEC_DEFAULT_EXTENSION;

    strBase += "_results";
    path = strBase + strExt;

    int i = 0;

#ifdef WIN32
    struct _stat64 buf;
    while (_stat64(path, &buf) == 0) // file exists
#else
    struct stat buf;
    while (stat(strPath, &buf) == 0) // file exists
#endif
    {
      path.Format("%s%d%s", strBase, i++, strExt);
    }
    return std::string(path);
  }

  std::pair< bool, QString > not_converged_msg(const dia::IAnalysisStatusContainer& statuses)
  {
    // we're trying to give some meaningful warning as a summary of what's happening in the analysis log

    //dia::IAnalysisStatusContainer& statuses = m_dsa_context->out.statuses;

    size_t totalPhases = statuses.Phases();

    QString warning = "Please check the analysis log and the results.";
    bool showWarning = false;

    size_t firstPhase = 0;
    size_t lastPhase = 0;

    if (totalPhases > 1)
    {
      for (; statuses.Phase(firstPhase)->Converged(); ++firstPhase);
      for (lastPhase = totalPhases - 1; statuses.Phase(lastPhase)->Converged(); --lastPhase);
    }

    size_t totalSteps = 0;
    size_t totalConvergedSteps = 0;
    size_t totalNonConvergededInitializations = 0;
    size_t totalNonConvergedFirstSteps = 0;

    for (size_t i = firstPhase; i <= lastPhase; ++i)
    {
      dia::IAnalysisStatus *pStatus = statuses.Phase(i);

      size_t steps = pStatus->TimeSteps();
      size_t convergedSteps = pStatus->TimeStepsConverged();

      bool convergedFirstSteps = true;
      size_t firstSteps = 3;
      if (firstSteps > steps)
        firstSteps = steps;
      for (size_t j = 0; j < firstSteps; ++j)
        convergedFirstSteps = convergedFirstSteps && pStatus->TimeStepConverged(i);

      totalSteps += steps;
      totalConvergedSteps += convergedSteps;

      if (!pStatus->InitializationConvergence())
        ++totalNonConvergededInitializations;

      if (!convergedFirstSteps)
        ++totalNonConvergedFirstSteps;
    }


    double percentageConverged = 1;
    if (totalSteps > 0)
      percentageConverged = (double)totalConvergedSteps / (double)totalSteps;

    if (totalNonConvergededInitializations > 0 || totalNonConvergedFirstSteps > 0)
    {
      warning += " There is non-convergence in the initialization and/or the first few time steps.";
      showWarning = true;
    }
    if ((showWarning && totalConvergedSteps < totalSteps) || percentageConverged < .95)
    {
      warning += QString(" Only %1 of %2 time steps converged.").arg(totalConvergedSteps).arg(totalSteps);
      showWarning = true;
    }
    if (showWarning)
    {
      int firstPhaseNumber = statuses.Phase(firstPhase)->PhaseNumber();
      int lastPhaseNumber = statuses.Phase(lastPhase)->PhaseNumber();
      if (firstPhaseNumber != lastPhaseNumber)
        warning += QString(" Several phases are affected (first phase %1 and last phase %2).").arg(firstPhaseNumber).arg(lastPhaseNumber);
      else
        warning += QString(" See phase %1.").arg(firstPhaseNumber);
    }
    return std::pair<bool, QString>(showWarning, warning);
  }

  //
  // uses: 
  //	- doc logger -> post messaged to the treated in the GUI thread
  //	- GlobalMessage
  //
  // better return lines and do doc->logger->add_line later?
  //
  // GlobalMessage: is interactive in any case here?
  //
  std::vector< std::pair< std::string, bool > >* log(CRunAnalysis::eRunLogs step, DSA_Context* c)
  {
    QString str;

    QStringList* list = new QStringList();

    std::vector< std::pair< std::string, bool > >* msg_v = new std::vector< std::pair< std::string, bool > >();

    switch (step)
    {
    case CRunAnalysis::Step_1:
    {
      c->time.start = QDateTime::currentDateTime();
      str = c->time.start.toString("'*** GEOMEC analysis started at' dd MMM yyyy',' hh':'mm '***'");
      break;
    }
    case CRunAnalysis::Step_2:
    {
      _m()->status("Checking for Diana license");
      str = "*** Checking for Diana license ***";
      break;
    }
    case CRunAnalysis::Step_3:
    {
      _m()->msg("Couldn't find a Diana license. Please check.");
      str = "*** GEOMEC analysis canceled: no license found ***";
      break;
    }
    case CRunAnalysis::Step_4:
    {
      QDateTime timeEnd = QDateTime::currentDateTime();
      str = timeEnd.toString("'*** GEOMEC analysis ended at' dd MMM yyyy',' hh':'mm '***'");

      *list << str;

      TTimeInterval timespan = calculateTimeInterval(c->time.start, timeEnd);
      str = QString("*** Analysis took %1 days, %2 hours, %3 minutes, %4 seconds ***").arg(timespan.days).arg(timespan.hours).arg(timespan.minutes).arg(timespan.seconds);

      *list << str;
      *list << "";

      break;
    }

    case CRunAnalysis::Step_5:
    {
      str = c->time.save.start.toString("'*** Automatic save started at' dd MMM yyyy',' hh':'mm '***'");
      break;
    }

    case CRunAnalysis::Step_6:
    {
      CString strLine;

      if (c->out.save_succeed)
        strLine.Format("*** Results file automatically saved as '%s' ***", c->out.path.c_str());
      else
        strLine.Format("*** Failed to save results file to '%s' ***", c->out.path.c_str());

      msg_v->push_back(std::make_pair((LPCTSTR)strLine, false));

      c->time.save.end = QDateTime::currentDateTime();
      str = c->time.save.end.toString("'*** Automatic save ended at' dd MMM yyyy',' hh':'mm '***'");
      msg_v->push_back(std::make_pair(str.toStdString(), true));

      TTimeInterval timespan = calculateTimeInterval(c->time.save.start,
        c->time.save.end);

      str = QString("*** Automatic save took %1 days, %1 hours, %3 minutes, %4 seconds ***").arg(timespan.days).arg(timespan.hours).arg(timespan.minutes).arg(timespan.seconds);

      msg_v->push_back(std::make_pair(str.toStdString(), true));
      break;
    }
    default: // ERROR
    {
      break;
    }
    }
    if (step != CRunAnalysis::Step_4 && step != CRunAnalysis::Step_6)
      msg_v->push_back(std::make_pair(str.toStdString(), false));

    return msg_v;
  }
}


const char* CRunAnalysis::m_checks_cp[] = { "HasResults", "Model", "Input", "License", "NoError" };

CRunAnalysis::CRunAnalysis()
  :	IRunAnalysis( RunAnalysis_ListenerDefinition ),
    m_dsa_context( nullptr )
{
  m_printer->info("new");
  m_printer->info("--run analysis: START-- >>>>>>>>>>>>>>>>>");

  m_doc = GetGeomecDoc();
  m_model = (CModelBase*)m_doc->Model();

  if(_g->dsa())
    _g->prog(new CProgressFactory);

  //m_thread = new std::thread( &IRunAnalysis::run, this );

  //
  // needed to call the derived class function
  //
  // this can't be done in the constructor, it's to early...
  //
  //m_thread = new std::thread( [this]{ this->exe(); } );
}

/*void CRunAnalysis::start_thread()
{
  m_thread = new std::thread( [this]{ this->run(); } );
}*/

CRunAnalysis::~CRunAnalysis()
{
  m_printer->info("delete >>");

  delete_();



  //
  // if not dsa running always MFC progress
  //
  //if(	_g->dsa() && _g->mfc(CGlobal::Progress))
    _g->prog(new CProgressFactory_MFC);

  m_printer->info("delete <<");
  m_printer->info("<<<<<<<<<<<<<<<<<<<<<< --run analysis: ENDED--");
}

//
// run entry: start
//

void CRunAnalysis::on_params( const Cmd& cmd )
{
  void* data_ = cmd.second;
  RunParams data = *reinterpret_cast< RunParams* >(data_);
  delete data_;

  //
  // FIXME: dangerous !!! move it to CAnalysisType
  //

  CAnalysisType::TAnalysisType ana_type_id =  data.ana_type;

  static const char* ana_type_cp[]={"First_&_Linear","NonLinear","Heat","Mixture","Last_&_MixtureContainment"};
  m_printer->info("--params-- ana:%s, write input files:%s, write quad dat:%s", ana_type_cp[data.ana_type],data.is_write_input_files ? "yes":"no", data.is_write_quad_dat ? "yes":"no" );

  // save context:
  m_dsa_context = new DSA_Context(); // just in case
  DSA_Context* c = m_dsa_context;

  c->input = { data.ana_type , data.is_write_input_files , data.is_write_quad_dat };

  if(_g->dsa())
    c->more = { LICENSE_CHECK_MODEL_FILE, "", true, DSA_Context::LicenseCheck };
  else
    c->more = { (LPCSTR)GetGeomecDoc()->GetPathName(), (LPCSTR)GetGeomecDoc()->GetTitle(), false, DSA_Context::Regular };
}

//
// dsa:
//
//		runs 'OnEndAnalysis', overloaded in the runner implementatcion 
//		takes model results from FF (file) and sets them into the memory
//		not in dia::IDianaRunner but in 'CGeomecDianaRunenrBase' in GeomecModel'
//
// not dsa:
//
//		no need as r->DoRun does r->DoRun_START + r->DoRun_END (AFTER REFACTOR NAMES HAVE CHANGED!!!)
//
void CRunAnalysis::end_run()
{
  m_printer->info("on_end");
  m_dsa_context->drc->end();
}

//
// model from memory to '.gm5' file
//
// FIXME: still reference to doc, send evetn instead
//
void CRunAnalysis::save()
{
  m_printer->info("save");

  DSA_Context* c = m_dsa_context;

  // retrieve context
  const CAnalysisType ana_type			= c->input.ana_type;
  dia::IAnalysisStatusContainer& statuses = c->out.statuses;

  bool bSuccess			= c->out.is_success;

  bool bWriteInputFiles	= c->input.is_write_input_files;
  bool bWriteQuadDat		= c->input.is_write_quad_dat;

  _e->broadcast(make_cmd(Log, log(Step_4, c)));

  if(bSuccess && !bWriteInputFiles && !bWriteQuadDat)
  {
    std::string url = results_url(c->more.path);

    m_printer->info("--url-- %s",url.c_str());

    c->out.path = url;
    c->time.save.start = QDateTime::currentDateTime();

    _e->broadcast(make_cmd(Log, log(Step_5, c)));

    c->out.save_succeed = m_doc->SaveModel(url.c_str(), 0, false);;

    //m_dsa_context->out.save_succeed = m_doc->SaveModel( url.c_str(), 0, false );

    _e->broadcast(make_cmd(Log, log(Step_6, c)));

    CConsistencyGuard *guard = m_model->GetConsistencyGuard();
    guard->AutoResultExport(*m_model, ana_type.AnalysisType());
  }

  //
  // FIXME
  //
  m_doc->SetModifiedFlag();

  if( !statuses.Converged() )
  {
    std::pair< bool, QString > msg_p = not_converged_msg(c->out.statuses ); // pair: [ show/dont_show, msg ]
    if( msg_p.first )
      _m()->msg( msg_p.second );
  }
  _e->broadcast(Save_Done); // after save, update views and neatly quit ra
}

//
// strategy, factory
//
// needed by other class: CMatParamDlg
//
IExecuteDianaDialog* CRunAnalysis::create_dlg()
{
  IExecuteDianaDialog* dlg = nullptr;
  if(!_g->dsa())
    dlg = new CExecuteDianaDialog();

  return dlg;
}

void CRunAnalysis::delete_dlg( IExecuteDianaDialog* dlg )
{
  if( dlg )
    delete dlg;
}

void CRunAnalysis::delete_()
{
  m_printer->info("delete objs");

  DSA_Context* c = m_dsa_context;

  if (c->dlg)
  {
    //
    // FIXME: this started crashing after the thread refactor
    //
    // 2020-04-30: still crashing
    //
    // Option 1: crashed with MFC dlg
    //
    delete c->dlg;
    c->dlg = nullptr;

    //
    // Option 2: this doesnt solve anything
    //
    //CRunAnalysis::delete_dlg( dlg );


  }
  if (c->dxw)
  {
    delete c->dxw;
    c->dxw = nullptr;
  }
  if (c->save)
  {
    delete c->save;
    c->save = nullptr;
  }
  if (c->retrieve)
  {
    delete c->retrieve;
    c->retrieve = nullptr;
  }
  if (c->drc) // last one
  {
    delete c->drc;
    c->drc = nullptr;
  }

  //
  //
  //
  delete c;
  //
  //
  //

  // Not sure the specific descructor of each object is being called 
#ifdef KK
  std::vector< void * > obj_v = { drc, /*dlg,*/ dxw, save, retrieve };
  for (auto obj : obj_v)
  {
    if (obj)
    {
      delete obj;
      obj = nullptr;
    }
  }
#endif

}

//
// create objs + save them in context
//
void CRunAnalysis::create__(DSA_Context::eType type)
{
  DSA_Context* c = m_dsa_context;

  switch (type)
  {
  case DSA_Context::LicenseCheck:
  {
    //
    // dx does almost nothig, it uses dsu to run diana
    //
    c->dxw = new CDianaExecuter(); //dxw -> passed as a param and used at drc->run (CDianaRunner->OnRunAnalysis)
    //
    CModelBase* model = CDianaLicenseCheck::instance()->model();
    //
    c->save = new CSaveModelConsole(*model, "");
    c->retrieve = new CRetrieveDianaFileNamesConsole();
    c->drc = new CDianaRunController(
      *model, 
      CAnalysisType::AT_LINEAR, 
      false, false, false, 
      GetGeomecTempPathExt(CTempPath::TEMP_CALCULATION));
    break;
  }
  case DSA_Context::Regular:
  {
    c->dlg = create_dlg();
    //
    // dsa: dxw it's similar to dx so it also does almost nothig, it uses the dlg which uses dsa to run diana
    //
    // non-dsa: MFC dlg does more than dsa's one
    //
    if(_g->dsa())
      c->dxw = nullptr;
    else
      c->dxw = new CDianaXWrapper(c->dlg);
    //
    c->save = new CSaveModel(new CSaveModelGUI());
    c->retrieve = new CRetrieveDianaFileNames(
      new CRetrieveDianaFileNamesGUI(
        c->input.ana_type,
        c->input.is_write_input_files,
        c->input.is_write_quad_dat,
        *m_model));
    c->drc = new CDianaRunController(
      *m_model,
      c->input.ana_type,
      c->input.is_write_input_files,
      c->input.is_write_quad_dat,
      m_model->WriteOutputStreamFile(),
      GetGeomecTempPathExt(CTempPath::TEMP_CALCULATION));
    break;
  }
  default:
    break;
  }
}

//
// creates needed objects (dlg,xwrapper,controller,...) and does controller->run
//
// runner is created by controller
// 
bool CRunAnalysis::run_controller()
{
  m_printer->info( "run_controller");

  DSA_Context* c = m_dsa_context;
  /*	if (c->more.type == DSA_Context::LicenseCheck) // check license
    create_objs_simple();
  else
    create_objs();*/

  create__(c->more.type);


  //
  // tell runner about dsa value
  //
  // runner is in 'DCore' lib that doesn't know about 'DSU' lib
  //
  // runner here still mot created, try with an static var... 
  //
  dia::IDianaRunner::RunParams p = { 
    c->more.path, c->more.title, c->drc->tmp().toStdString(),
    c->dxw, c->save, c->retrieve, 
    &c->out.statuses, 
    c->more.silent, 
    _g->dsa() };

  c->drc->params(p);

  bool res=false;
  if(_g->dsa())
  {
    _e->broadcast( make_cmd(
      RunController, 
      new dia::IDianaRunner::RunParams( p ) ) ); // no need to pass the params but it can be useful in the future

    res = true;
  }
  else
  {
    res = c->drc->run(); // in dsa cmd is queued and method exists immediatelly ... to be implemented: cmd 'RunController'
  }

  c->out.is_success = res;

  return res;;
}

//
// checks and logs
//
// also check License
//
// FIXME: this has gui code, move to doc ... HasResults pop-up temporarily commented !!!
//
bool CRunAnalysis::checks( eChecks type )
{
  DSA_Context* c = m_dsa_context;

  bool res = true;

  switch( type )
  {
  // 1) check if model has results
  case HasResults:
  {
    if( !c->input.is_write_input_files &&
      !c->input.is_write_quad_dat )
    {
      //
      // FIXME: sometimes crashes here !!! 
      //
      if (m_model->ResultRegister().ResultsAvailable(c->input.ana_type) &&
        (c->input.ana_type != CAnalysisType::AT_NONLIN ||
          !m_model->BranchState().IsBranch()))
      {
        if (_m()->msg(IDS_USE_EXISTING_RESULTS, MB_YESNO) == IDYES)
          res = false;
      }
    }
    break;
  }
  //
  // gui inside !!!
  //
  // in dsa mode this needs to be run in the gui thread
  //
  // there are plenty of calls to _m()->msg() in CModelBase and in derived classes as CTetraModel
  //
  // all issues above solved in 'GlobalMessage' implementation
  //
  case Input:
  {
    res = m_model->CheckForAnalysis(
      c->input.ana_type,
      c->input.is_write_input_files,
      c->input.is_write_quad_dat);
    break;
  }
  case License:
  {
    _e->broadcast(make_cmd(Log, log(Step_2,c)));

    //
    // for dsa this is done differently
    //
    if( !_g->dsa() )
      CDianaLicenseCheck::instance()->Activate();

    m_doc->Logger().Silent(true);
    
    bool bLicense = CDianaLicenseCheck::instance()->FoundLicense();

    m_doc->Logger().Silent(false);

    _m()->status("");

    if (!bLicense)
    {
      _e->broadcast(make_cmd(Log, log(Step_3, c)));
      res = false;
    }
    break;
  }
  default:
  {
    res = false;
    break;
  }
  }

  return res;
}

//
// runs all checks to verify execution is possible 
//
CRunAnalysis::eChecks CRunAnalysis::checks()
{
  m_printer->info("preliminar checks");

  DSA_Context* c = m_dsa_context;

  if (!checks(HasResults))
    return HasResults; // and user decides to use them and not to run the model again... dlg happens inside 'checks()'

  if (m_model)
    m_model->getMaterialErrorHandler().resetErrorHandler();
  else
    return Model;

  _e->broadcast(make_cmd(Log, log(Step_1, c)));

  if (!checks(Input))
    return Input;

  //
  // done differently
  //
  if ( !_g->dsa() && !checks(License))
    return License;

  return NoError;
}

//
// TODO: try two banches: 
//
//	1)licenese check model 
//	2)real model to run
//
//  maybe this way code is cleaner
//
void CRunAnalysis::on_start(const Cmd& cmd)
{
  DSA_Context* c = context();

  m_printer->info("--start-- (%s)", _g->dsa() ? "dsa" : "non dsa");

  CRunAnalysis::eChecks res = CRunAnalysis::NoError;
  if(	c->more.type != DSA_Context::LicenseCheck && 
    //
    // if test questions popups as a result of checks are skipped: 
    //	examples:
    //		sure to run linear even if model is branched?
    //		model has this kind of results, use those or calculate again?
    //
    !m_doc->test())
  {
    res = checks();
  }

  //
  // 1)pre-checks Ok
  //
  if (res == CRunAnalysis::NoError)
  {
    if (c->more.type != DSA_Context::LicenseCheck)
    {
      _e->broadcast(DisableViews); 
      //
      // Run send on DisableViews_Done
      //
    }
    else
      push_(Run);
  }
  //
  // 3) pre-checks error -> notify GUI
  //
  else
  {
    c->out.is_success = false;

    if (res != CRunAnalysis::HasResults && res != CRunAnalysis::License)
      _e->broadcast(make_cmd(ChecksError, new int(res)));

    if (c->more.type != DSA_Context::LicenseCheck)
      _e->broadcast(UpdateViews);

    _e->broadcast(Quit_RA);
  }
}

void CRunAnalysis::on_run(const Cmd& cmd)
{
  m_printer->info("on run");

  DSA_Context* c = context();

  bool bSuccess = false;
  try
  {
    bSuccess = run_controller();
  }
  //
  // FIXME
  //
  //catch (CProgressCancel* e)
  catch ( std::exception& ex )
  {
    ex.what();

    m_printer->info("GeomecDoc > catch");
    //delete e;
    //
    // FIXME
    //
    //Logger().AddLine("*** GEOMEC analysis cancelled ***");

    bSuccess = false;
    c->out.is_success = false;
    _e->broadcast(make_cmd(ChecksError, new std::string("Unexpected crash catched during running analysis")));
    _e->broadcast({
      UpdateViews, // TODO: verify wether this is needed
      Quit_RA });
  }
  //
  // result ?
  //
  if (bSuccess)
  {
    if (_g->dsa())
    {
      //
      // wait for 'RunSuccess' evt
      //
    }
    else
    {
      // 
      // TODO:	dont trait differenty non-dsa in the runner 
      //			implement the same workflow for dsa and non-dsa
      //
      c->out.is_success = true;
      push_(Save); // on Save_done then UpdateViews + Quit_RA
    }
  }
}

//
// = end analysis (all iterations Ok)
//
void CRunAnalysis::on_run_end()
{
  DSA_Context* c = m_dsa_context;
  //
  // previously run a small model to check connectivity
  //
  if (c->more.type == DSA_Context::LicenseCheck)
  {
    //
    // TODO: delete temp folder
    //
    c->more = { (LPCSTR)GetGeomecDoc()->GetPathName(), (LPCSTR)GetGeomecDoc()->GetTitle(), false, DSA_Context::Regular };
    //
    push_({	Start });
  }
  //
  // real model to run
  //
  else
  {
    c->out.is_success = true;

    //
    // EndRun: no need, in gui run is always stagged, so never step-by-step one, data is retrieved per stage in the controller
    //
    push_({ Save });
  }
}

//
// FIXME: now all this is happening in the gui thread and it shouldn't !!!!
//
void CRunAnalysis::handle(Cmd cmd)
{
  int cmd_id = cmd.first;
  switch (cmd_id)
  {
    //////////////////////////////
    //
    //		commands
    //
    //////////////////////////////
    case eCmd::SetParams:			on_params(cmd); break;
    case eCmd::Start:				on_start(cmd); break;
    case eCmd::DisableViews_Done:	push_(Run); break;
    case eCmd::Run:					on_run(cmd); break;
    case eCmd::Save:				save(); break;
    //////////////////////////////
    //
    //		commands 
    //
    //	(executed in gui thread)
    //
    //////////////////////////////
    //case eCmd::UpdateViews:
    //case eCmd::DisableViews:
    //case eCmd::ChecksError:
    //case eCmd::RunError:
    //
    //case eCmd::Quit_RA:				-> can't kill himself
    //
    //////////////////////////////
    //
    //		responses
    //
    //////////////////////////////
    case eCmd::RunEnd:				on_run_end(); break; //= end analysis (all iterations Ok)
    case eCmd::CancelRun:
    {
      //
      // FIXME: send all three in a command
      //
      _e->notify(QuitDiana,CEvents::DI_EH);
      //
      // m_doc->clear(); // many progress and run diana msgs enqueued -> no need
      //
      _e->notify({ 
        UpdateViews,		// needs to be done before deleting ra as it depends on it
        Quit_RA},			//quits also DI
        //
        CEvents::GuiApp_EH); 
      break;
    }
    //////////////////////////////
    //
    //		process tracking (handled in gui)
    //
    //////////////////////////////
    //case eCmd::Progress:
    //case eCmd::Log:
    //case eCmd::DianaMsg:
    //
    //////////////////////////////
    //
    //		error
    //
    //////////////////////////////
    default:
    {
      m_printer->error("evt not handled : %s",_cmd_s(cmd));
      break;
    }
  }
}
