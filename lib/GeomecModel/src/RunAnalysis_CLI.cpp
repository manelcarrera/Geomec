#include "RunAnalysis_CLI.h"
//
#include "DianaExecuter.h"
#include "DianaStartUp.h"
#include "Global.h"
#include "Printer.h"

namespace {
static const IListenerDefinition RunDianaCli_ListenerDefinition{
    CEvents::NonGuiApp_EH,
    {//
     RunSuccess, RunEnd, ChecksError, RunError, DianaMsg, QuitDiana_Done}};

static const char *scenarios_cp[] = {"DianaRunController", "DianaXWrapper", "DianaStartUp"};
} // namespace

void CRunAnalysis_CLI::init() {
  m_res = false;
  //
  m_quit = false;
  //
  // regular case
  //
  m_drc = nullptr;
  m_step = CDianaRunController::All;
  //
  // MatParamDlg-only
  //
  m_dxw = nullptr;
}

//
// cases in which CDianaExecuter is used:
//
// - gui LicenseCheck
// - DianaLicenseCheck
// - TestLib
// - CORA
// - RGI
//
// in most of these cases teh executer is passed as a parameter to a controller
//

//
// emits no event
//
CRunAnalysis_CLI::CRunAnalysis_CLI(CDianaRunController *drc, CDianaRunController::eRunStep step)
    : IRunAnalysis(RunDianaCli_ListenerDefinition), m_wait(CEvents::NonGuiApp_EH), m_scenario(DianaRunController) {
  init();

  m_drc = drc;
  m_step = step;

  m_drc->run(step); // non-blocking
}

//
// Special case for ModelOperations.StartDiana
//
CRunAnalysis_CLI::CRunAnalysis_CLI()
    : IRunAnalysis(RunDianaCli_ListenerDefinition), m_wait(CEvents::NonGuiApp_EH), m_scenario(DianaStartUp) {
  init();

  CDianaStartUp *dsu = CDianaStartUp::instance();
  dsu->RunDiana(); // non-blocking
}

//
// Special case, only used in MatParamDlg
//
CRunAnalysis_CLI::CRunAnalysis_CLI(IDianaXWrapper *dx)
    : IRunAnalysis(RunDianaCli_ListenerDefinition), m_wait(CEvents::NonGuiApp_EH), m_scenario(DianaXWrapper) {
  init();
  m_dxw = dx;
  m_dxw->ExecuteDiana(); // non-blocking
}

CRunAnalysis_CLI::~CRunAnalysis_CLI() {}

void CRunAnalysis_CLI::wait() { m_wait.wait(); }

//
// by now it does nothing
//
void CRunAnalysis_CLI::on_diana_msg(Cmd cmd) {
  void *data_ = cmd.second;
  std::string msg = *reinterpret_cast<std::string *>(data_);
  delete data_;

  //
  // FIXME:
  //
  //	never should be empty,
  //	at least first char with he msg typr should be present
  //
  int len = msg.length();
  if (len) {
    if (len > 1)
      m_printer->debug("diana msg: %c : %s", msg.front(), msg.substr(1).c_str());
    else
      m_printer->debug("diana msg: %c", msg.front());
  } else
    m_printer->error("diana msg");

  //
  // emit evt to dlg
  //
  /*CExecuteDianaDialog* dlg = dynamic_cast<CExecuteDianaDialog*>(c->dlg);
  if (dlg)
  {
  const std::map< QString, CDianaStartUp::eMsg >& m = CDianaStartUp::dsa_msg_type_map;
  QString type_s = QString(QChar(msg.front()));
  if (m.find(type_s) != m.end())
  {
  //
  // compile error, why?
  //
  //CDianaStartUp::eMsg type = m[ type_s ];

  CDianaStartUp::eMsg type = m.at(type_s);

  QString msg_qs;
  if (msg.length()>1)
  msg_qs = QString::fromStdString(msg.substr(1));

  printer->info("diana msg : emit evt");

  switch (type)
  {
  //
  // TODO
  //
  case CDianaStartUp::Gen:		emit dlg->OnMessage(msg_qs); break;
  case CDianaStartUp::AppSpc:		emit dlg->OnAppSpcMessage(msg_qs); break;
  case CDianaStartUp::Error:		emit dlg->OnError(msg_qs); break;
  case CDianaStartUp::Warning:	emit dlg->OnWarning(msg_qs); break;

  case CDianaStartUp::StopExe:
  {
  //
  // TODO: what is the 'a' for?
  //
  static bool a;
  emit dlg->OnStopExecuting(a);
  break;
  }

  case CDianaStartUp::Finished:	emit c->dlg->OnFinished(); break;

  default:						printer->error("diana msg : evt not emitted"); break;
  }
  }
  else
  {
  printer->error("diana msg : type '%s' not found", msg.front()); break;
  }
  }*/
}

std::string CRunAnalysis_CLI::error_msg(Cmd cmd) {
  void *data_ = cmd.second;
  CEvents::ErrorData data = *reinterpret_cast<CEvents::ErrorData *>(data_);
  delete data_;

  CEvents::eEventsHandler sender = data.sender;

  std::string msg;
  switch (sender) {
  case CEvents::Controller_EH: {
    CDianaRunController::eRunRes error_id = (CDianaRunController::eRunRes)data.error_id;
    switch (error_id) {

    // case CDianaRunController::Success:
    case CDianaRunController::RunError_:
    case CDianaRunController::EndDepletionError:
    case CDianaRunController::Cancelled:
    case CDianaRunController::StartCheckError:
    case CDianaRunController::GuardCheckError:
    case CDianaRunController::RunFirstError:
    case CDianaRunController::RunNextError:
    case CDianaRunController::UndefinedError:
    case CDianaRunController::EndStageError:
    default:
      msg = "Unknown error";
      break;
    }
    break;
  }
  case CEvents::DI_EH: {
    eDsaError error_id = (eDsaError)data.error_id; // defined in Inetrprocess.h
    switch (error_id) {
    case UnableToLaunchDiana:
      msg = "Unable to launch 'diana_app'";
      break;
    case HeartBeatError:
      msg = "Heartbeat error";
      break;
    case EndDepletionCheckError:
      msg = "End depletion check error";
      break;
    default:
      msg = "Unknown error";
      break;
    }
    break;
  }
  default:
    break;
  }
  return msg;
}

void CRunAnalysis_CLI::handle(Cmd cmd) {
  static CDianaStartUp *dsu = CDianaStartUp::instance();

  switch (cmd.first) {
  case RunSuccess: // = end iteration
  {
    if (m_scenario != DianaRunController) {
      //
      // controller is the one that emits RunEnd, but here there is no controller
      //
      push_(RunEnd); // when run diana completely finished
    }
    break;
  }
  case RunEnd: // = end analysis
  {
    m_res = true;
    //
    dsu->delete_di(); // neatly quits all diana interface threads
    //
    m_wait.signal(); // -> DON'T REMOVE !!!  -> needed to quit from wait
    break;
  }
  //
  // Error caused by:
  //	- Preliminar checks
  //	- Progress bar exception
  //
  case eCmd::ChecksError: {
    std::string *msg = reinterpret_cast<std::string *>((void *)cmd.second);
    delete msg;
    //
    push_(Quit_RA);
    //
    m_wait.signal(); // Maybe needed in any of the cases
    break;
  }
  case eCmd::Quit_RA: {
    if (_g->dsa())
      dsu->delete_di();
    break;
  }
  //
  // it does:
  //	- delete runner
  //	- clean FFDIR with FF
  //	- m_pStartStage = 0
  //	- m_pEndStage = 0
  //
  // by now this case happens when diana_app_new.exe of previous execution didsn't end and for any reason can't be
  // killed but is intended to use by any reason... reason why is specified in the param
  case eCmd::RunError: {
    std::string msg = error_msg(cmd);
    //
    push_(Quit_RA);
    //
    m_wait.signal();
    break;
  }
  //
  // when diana finishes execution forcely
  //
  case eCmd::QuitDiana_Done: {
    quit(true);
    m_wait.signal();
    break;
  }
  case eCmd::DianaMsg: {
    on_diana_msg(cmd); // by now it does nothing
    break;
  }
  default: {
    m_printer->error("unknown evt");
    break;
  }
  }
}