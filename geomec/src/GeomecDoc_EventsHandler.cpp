#include "stdafx.h"

//std
#include <thread>
#include <future> //async
#include <cctype> // std::isdigit


#include "GeomecDoc_EventsHandler.h"
//
#include "ProgressWrapper_Data.h"
#include "Global.h"
#include "Printer.h"

#include "RunAnalysis.h"
#include "RunAnalysis_NonDsa.h"
#include "IExeDianaDlg.h"
#include "IProgressBase.h"

#include "IProgressFactory.h"
#include "ProgressFactory_MFC.h"

#include "ExecuteDianaDialogQt.h"
#include "ExecuteDianaDialog.h"

#include "FemAppMainWindow.h"

#include "ProgressDlg_Qt.h"
#include "GeoProgress.h"

#include "DianaStartUp.h"

#include "DianaRunController.h"

#include "Test.h"
#include "IListenerIpc.h"

//qt
#include <QtWidgets\QMessageBox>


#include "QuestionThreadWrapper.h"
#include "GlobalMessage_GUI.h"
#include "GmHelper.h" // gm::send::status()

#include "DatFileDialog.h"

#include "GeomecDoc.h"

#include "AnalysisLogger.h"

#include "QUtil.h" // namespace util::concurrency
using namespace util::concurrency;

#include "TestInterface.h" // IpcListener to comunicate with Tests app

#include "GmCommand.h"
#include "MesherDlg.h"

namespace
{
  Printer* printer = Printer::instance(Printer::Doc);
}

template< class C >
void delete_(C*& c)
{
  if(c)
  {
    delete c;
    c=nullptr;
  }
}

//
// not used
//
template< class C >
C* factory_ra()
{
  C* obj = new C();
  //obj->start_thread();

  return obj;
};

CGeomecDoc_EventsHandler::CGeomecDoc_EventsHandler()
: 
  m_ra( nullptr ),
  m_dlg( nullptr ),
  m_prg( nullptr ),
  m_th( nullptr ),
  m_test( nullptr ),
  m_ti( nullptr )
{
  m_dsu = CDianaStartUp::instance();
}

CGeomecDoc_EventsHandler::~CGeomecDoc_EventsHandler()
{
  delete_( m_dlg );
  delete_( m_prg );
}

void CGeomecDoc_EventsHandler::start(	
  const CAnalysisType::TAnalysisType antype, 
  bool bWriteInputFiles, 
  bool bWriteQuadDat
)
{
  m_doc = GetGeomecDoc();

  if( _g->status()!=CGlobal::Idle )
  {
    QString title = "Not possible to launch a new analysis";
    QString msg = _g->status()==CGlobal::Cancelling ? 
      "Analysis being cancelled, hold on please..." : 
      "Analysis running, not posible to run simultaneous analysis...";
    gm::doc_::gui::popup(title,msg,2000);
    return;
  }

  //
  // FIXME: this shouldn't be needed
  //
  delete_( m_ra );

  if(_g->dsa())
    m_ra = factory_ra<CRunAnalysis>();
  else
    m_ra = factory_ra<CRunAnalysis_NonDsa>();

  delete_( m_dlg );
  delete_( m_prg );

  _e->broadcast(make_cmd(SetParams, new CRunAnalysis::RunParams{antype, bWriteInputFiles, bWriteQuadDat} ));
  _e->broadcast(Start);
}

//
// memory:
//		msg sender allocates memory for p2 param, it needs to be released here
//
// post: 
//		broker queue condition variable needs to be released here
//		signal to broker must be done, otehrwise msg queue is locked forever
//
// non-sequencial:
//		MFC queue is not sequencial so a synchro mechanism needs to be implemented if we want it sequencial 
//		sequencial: dont execute an event till previous one has been dispatched
//

//
// progress notiufications from a thread other than the GUI one
//
void CGeomecDoc_EventsHandler::on_prg(const Cmd& cmd)
{
  void* data_ = cmd.second;
  CProgressData data = *reinterpret_cast< CProgressData* >(data_);
  delete data_;

  if(m_ra && m_ra->context()->more.silent)
    return;

  //
  // FIXME: this shouldn't happen, but it does
  //
  // FIXME: dlg type is baseless and it shouldn't be 
  //
  // which dlg type must be chosen?
  //
  if (!m_prg && data.m_cmd != CProgressData::New)
  {
    printer->error("progress : nullptr");
    if( _g->mfc( CGlobal::Progress ) )
      m_prg = new CGeoProgress(FemAppGetMainWnd()->GetActiveWindow(), data.m_msg, data.m_yes_no);
    else
      m_prg = new CProgressDlg_Qt( eProgress::Geo, "Unknown title", false /*cancel btn*/, 1 /*jobs*/);
  }

  switch (data.m_cmd)
  {
    case CProgressData::New:
    {
      //
      // this shouln't be needed
      //
      if(m_prg)
      {
        printer->error("progress : new : progress not nullptr");
        delete m_prg;
        m_prg = nullptr;
      }

      if(_g->mfc( CGlobal::Progress ))
      {
        CProgressFactory_MFC factory; 
        //
        // in this case create = create_imp
        //
        m_prg = factory.create(	data.m_type, data.m_msg, data.m_yes_no,	data.m_number);
      }
      else
      {
        m_prg = new CProgressDlg_Qt( data.m_type, data.m_msg, data.m_yes_no, data.m_number); // jobs
      }

      printer->info("progress : new : %s", data.m_msg.toStdString().c_str());
      break;
    }
    case CProgressData::Delete:
    {
      //
      // FIXME
      //
      printer->debug("progress : delete");
      if(m_prg)
      {
        delete m_prg;
        m_prg = nullptr;
      }
      else
        printer->error("progress : delete : progress nullptr");
      break;
    }
    case CProgressData::AddSteps_:	m_prg->AddSteps(data.m_number); break;
    case CProgressData::Step_:		m_prg->Step(data.m_number); break;
    case CProgressData::Status_:	m_prg->StatusMessage(data.m_msg); break;
    case CProgressData::NextJob_:	m_prg->NextJob(data.m_msg); break;
    case CProgressData::Enable_:	m_prg->Enable(data.m_yes_no);	break;
      //
      // FIXME:
      //
    //case CProgressWrapper::Current:		m_prg->SetProgress(data.m_number); break;
    //
    // TODO
    //
    case CProgressData::Cancel:
      break;
    default:
      break;
  }
}

void CGeomecDoc_EventsHandler::on_diana_msg(const Cmd& cmd)
{
  void* data_ = cmd.second;
  std::string msg = *reinterpret_cast< std::string* >(data_);
  delete data_;

  //
  // FIXME: MatParamDlg scenario has no ra so no context !!!
  //
  if( m_ra )
  {
    DSA_Context* c = m_ra->context(); // only used here
    if(c->more.silent)
      return;
  }

  if(!m_dlg)
  {
    if(_g->mfc( CGlobal::RunDiana ))
    {
      //
      // FIXME: pass parent ?
      //
      //	1) AfxGetApp()->m_pMainWnd
      //	2) FemAppGetMainWnd()->GetActiveWindow()
      //
      CExecuteDianaDialog* dlg_ = new CExecuteDianaDialog();
      
      dlg_->init();
      m_dlg = dlg_;
    }
    else
      m_dlg = new CExecuteDianaDialogQt();
  }
  IExeDianaDlg* dlg = m_dlg;


  if (!dlg)
    return;

  const std::map< QString, eDianaMsg >& m = g_diana_msg_type_map;
  QString type_s = QString(QChar(msg.front()));

  if (m.find(type_s) == m.end())
  {
    printer->error("diana msg : type '%s' not found", msg.front());
    return;
  }

  eDianaMsg type = m.at(type_s);

  QString msg_qs;
  if (msg.length())
    msg_qs = QString::fromStdString(msg.substr(1));

  printer->debug("%s: %s",g_diana_msg_type[type].toStdString().c_str(), msg_qs.toStdString().c_str() );

  //
  // this does currently nothing ... functionality needs to be restored
  //
  static bool b_stop = false;

  //
  // main mfc implementation
  //
  auto mfc_emit = [=]() 
  {
    switch(type)	
    {
      case Gen:		emit m_dsu->OnMessage(msg_qs); break;
      case AppSpc:	emit m_dsu->OnAppSpcMessage(msg_qs); break;
      case Error:		emit m_dsu->OnError(msg_qs);  break;
      case Warning:	emit m_dsu->OnWarning(msg_qs); break;			
      case StopExe:	emit m_dsu->OnStopExecuting(b_stop); break; 
      case Finished:	
      {
        //
        // this has logic and does thongs with runner whereas we only want the dlg without any functionality
        //
        emit m_dsu->OnFinished(); 
        //

        //
        // this closes the dlg for ever
        //
        //m_run_dlg_mfc->CloseDialog(); // it makes show = false

        //
        // this doesn't hide the dlg
        //
        //m_run_dlg_mfc->show(false);

        //dlg->clear();
        break;
      }
      default:		printer->error("on diana msg : msg type undefined"); break;
    }
  };

  //
  // alternative mfc implementation
  //
  auto mfc_handle = [=]() 
  {
  bool log = true;
    switch(type)	
    {
      case Gen:		dlg->OnMessage(msg_qs); break;
      case AppSpc:	dlg->OnAppSpcMessage(msg_qs); break;
      case Error:		dlg->OnError(msg_qs);  break;
      case Warning:	dlg->OnWarning(msg_qs); break;			
      case StopExe:	dlg->OnStopExecuting(b_stop); log = false; break;
      case Finished:	
      {
    log = false;
        //
        // it happens several times, no only at the very end
        //

        //
        // FIXME: debug
        //
        printer->info("diana msg : Finished");
        //
        // this has logic and does things with runner whereas we only want the dlg without any functionality
        //
        dlg->OnFinished(); 

        //
        // FIXME: this needs to be called only at the very end
        //
        //dlg->end(0); // 0: Ok
        //

        //
        // this closes the dlg for ever
        //
        //m_run_dlg_mfc->CloseDialog(); // it makes show = false

        //
        // this doesn't hide the dlg
        //
        //m_run_dlg_mfc->show(false);

        //dlg->clear();
        break;
      }
      default:		printer->error("on diana msg : msg type undefined"); log = false; break;
    }
  if (log)
      m_doc->Logger().AddLine(msg_qs.toStdString());
  };

  //
  // qt implementation
  //
  auto qt_ = [=]()
  {
    switch (type)
    {
      case eDianaMsg::Gen:
      case eDianaMsg::AppSpc:
      case eDianaMsg::Error:
      case eDianaMsg::Warning:
      {
        dlg->msg(msg_qs,type); 
        dlg->show_();  // only if it's hidden
        break;
      }
      //
      // TODO
      //
      case eDianaMsg::StopExe:
      {
        printer->error("qt dlg : StopExe : to be implemented");
        break;
      }
      case eDianaMsg::Finished:
      {
        dlg->clear();
        //
        // the original one (MFC) also hides the dlg
        //
        dlg->hide_();
        break;
      }
      default: printer->error("diana msg : evt not emitted"); break;
    }
  };

  if(_g->mfc( CGlobal::RunDiana ))
  {
    //mfc_emit_();
    mfc_handle();
  }
  else
    qt_();
}


// gui
std::string CGeomecDoc_EventsHandler::error_msg(const Cmd& cmd)
{
  DSA_Context* c = m_ra->context();
  c->out.is_success = false;

  void* data_ = cmd.second;

  CEvents::ErrorData data = *reinterpret_cast< CEvents::ErrorData* >(data_);
  delete data_;


  CEvents::eEventsHandler sender =  data.sender;

  std::string msg;
  switch(sender)
  {
    case CEvents::Controller_EH:
    {
      CDianaRunController::eRunRes error_id = (CDianaRunController::eRunRes)data.error_id;
      switch (error_id)
      {

        //case CDianaRunController::Success:
        case CDianaRunController::RunError_: 
        case CDianaRunController::EndDepletionError:
        case CDianaRunController::Cancelled:
        case CDianaRunController::StartCheckError:
        case CDianaRunController::GuardCheckError:
        case CDianaRunController::RunFirstError:
        case CDianaRunController::RunNextError:
        case CDianaRunController::UndefinedError:
        case CDianaRunController::EndStageError:
        default: msg = "Unknown error"; break;
      }
      break;
    }
    case CEvents::DI_EH:
    {
      eDsaError error_id = (eDsaError)data.error_id; // defined in Inetrprocess.h
      switch (error_id)
      {
        case UnableToLaunchDiana:		msg = "Unable to launch 'diana_app'"; break;
        case HeartBeatError:			msg = "Heartbeat error"; break;
        case EndDepletionCheckError:	msg = "End depletion check error"; break;
        default:						msg = "Unknown error"; break;
      }
      QMessageBox::critical( nullptr,	QString("Running diana process"), QString::fromStdString( msg ) );
      break;
    }
    default:
      break;
  }

  return msg; 
}

//
// gui
//
// Error caused by:
//	- Preliminar checks
//	- Progress bar exception !!! not implemented
//
void CGeomecDoc_EventsHandler::on_checks_error(const Cmd& cmd)
{
  DSA_Context* c = m_ra->context();
  c->out.is_success = false;

  void* data_ = cmd.second;

  CRunAnalysis::eChecks error_id = *reinterpret_cast< CRunAnalysis::eChecks* >(data_);

  if( error_id != CRunAnalysis::HasResults &&
    error_id != CRunAnalysis::Input )
  {
    QMessageBox::critical(
      nullptr,
      QString("Run analysis"),
      QString::asprintf("An error occurred during the preliminary checks regarding: %s", CRunAnalysis::m_checks_cp[error_id]));
  }

  delete data_;
}

// gui
void CGeomecDoc_EventsHandler::on_log(const Cmd& cmd)
{
  void* data_ = cmd.second;

  std::vector< std::pair< std::string, bool > > msg_v =
    *reinterpret_cast< std::vector< std::pair< std::string, bool > >* >(data_);
  delete data_;

  for (const auto& item : msg_v)
    m_doc->Logger().AddLine(item.first, item.second);
}

void CGeomecDoc_EventsHandler::on_status(const Cmd& cmd)
{
  void* data_ = cmd.second;
  std::string message = *reinterpret_cast< std::string* >(data_);
  delete data_;

  gm::send::status(QString::fromStdString(message));
}

//
// gui
//
// FIXME: never use GlobalMessage here, otherwise deadlock !!! (mcr 2020-08-24)
//
void CGeomecDoc_EventsHandler::on_question(const Cmd& cmd)
{
  CQuestionThreadWrapper::Data* data = reinterpret_cast< CQuestionThreadWrapper::Data* >(cmd.second);
  int question_id = data->id;

  int response = -1;
  std::string response_s = std::string();

  switch(question_id)
  {
    case CQuestionThreadWrapper::MesherDlg:
    {
      std::vector< std::string > d = *reinterpret_cast< std::vector< std::string >* >(data->data);
      delete data->data;

      CMesherDlg dlg;
      dlg.data( d );
      dlg.DoModal(); // 'OnExport' and 'OnExportPoints' happens inside
      break;
    }
    case CQuestionThreadWrapper::GlobalMessage:
    {
      GlobalMessage_GUI::GMData d = *reinterpret_cast< GlobalMessage_GUI::GMData* >(data->data);
      delete data->data;

      std::string s = d.message;
      bool is_number = !s.empty() && std::find_if(s.begin(), s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();

      if(is_number)
        response = AfxMessageBox( std::stoi(s), d.style, d.contextID);
      else
        response = AfxMessageBox( s.c_str(), d.style, d.contextID);
      break;
    }
    case CQuestionThreadWrapper::DatFileDlg:
    {
      CDatFileDialog::Data d = *reinterpret_cast< CDatFileDialog::Data* >(data->data);
      delete data->data;

      auto remove_extension=[](const QString& path_)
      {
        return path_.split( ".", QString::SkipEmptyParts).at( 0 );
      };

      //
      // test: it doesn't work as EndDialog() is not closing CDatFileDialog
      //
      auto implementation_01=[=](
        CDatFileDialog::Data& d, 
        int& response, 
        std::string& response_s)
      {
        if(m_test)
          d.timeout=1000;

        CDatFileDialog dlgdat(&d);
        response = dlgdat.DoModal();
        if(response==IDOK)
        {
          response_s = dlgdat.GetPathName();
        }
        else // IDCANCEL
        {
          if(m_test)
            response_s = remove_extension( ra()->context()->more.path ).toStdString();
        }
      };

      //
      // test: skip the question and assume the response
      //
      auto implementation_02=[=](
        CDatFileDialog::Data& d, 
        int& response, 
        std::string& response_s)
      {
        if(m_test)
        {
          response=IDOK;
          response_s = remove_extension( ra()->context()->more.path ).toStdString();
        }
        else
        {
          CDatFileDialog dlgdat(&d);
          response = dlgdat.DoModal();
          if(response==IDOK)
            response_s = dlgdat.GetPathName();
        }
      };

      //implementation_01(d,response,response_s);
      implementation_02(d,response,response_s);

      break;
    }
    default:
      break;
  }
  

  printer->info("question: %s ... response:%d response_s:%s", CQuestionThreadWrapper::m_question_cp[question_id], response, response_s.c_str() );

  _e->broadcast(make_cmd(
    Response,
    new CQuestionThreadWrapper::DataRes{ response, response_s }));

  delete cmd.second;
}

void CGeomecDoc_EventsHandler::on_hb()
{
  //
  // Implementation 1
  //
  auto implementation_01=[](){
    static const int MaxTics = 500;
    static int tics = 0;
    tics++;
    gm::send::status( QString( tics, QChar('.') ) );
    tics = tics == MaxTics ? 0 : tics;
  };
  //
  // Implementation 2 and 3
  //
  auto elapsed_time=[=](){
    if(!m_ra)
      return QString();

    DSA_Context* c = m_ra->context();
    QDateTime now = QDateTime::currentDateTime();
    qint64 secs = c->time.start.secsTo(now);

    QString qs = QTime(0,0,0).addSecs(secs).toString("hh:mm:ss");
    return qs;
  };

  auto implementation_02=[=](){
    gm::send::status( QString("Elapsed time: ") + elapsed_time(), gm::send::eStatusType::Text );
  };
  auto implementation_03=[=](){
    gm::send::status( elapsed_time(), gm::send::eStatusType::ElapsedTime );
  };
  //
  // all the bar splashes from time to time: not nice ... refresh maybe less often
  //
  auto implementation_04=[=](){
    m_doc->set_dsa_icon(Running);
  };

  implementation_01();
  //implementation_02(); // not compatible with 01
  implementation_03();
  //implementation_04();
}

//
// mcr 2020-07-17
//
// TODO: end analysis needs to be implemented properly
//
// controller: cond vars and threads in join
//

void CGeomecDoc_EventsHandler::on_quit_ra()
{
  //
  // can't be deleted before deleting ra !!!
  //
  DSA_Context* c = m_ra->context();

  //
  // just in case mfc dlg stays in visible in a doModal state
  //
  delete_(m_dlg);
  delete_(m_prg);
  //
  //
  //
  if (_g->dsa())
    m_dsu->delete_di();
  //
  // clean status bar
  //
  gm::send::status( getStringTableEntry(AFX_IDS_IDLEMESSAGE), gm::send::eStatusType::Text );
  //
  m_doc->set_dsa_icon(_g->dsa()?_2P:_1P);
  //
  // can also be done at the analysis start
  //
  //const IListenerDefinition& d = RunDianaGui_ListenerDefinition;
  //_e->set( d.eh_id, d.events_v ); 
  //
  // by now don't delete the elapsed time at the end of the analysis
  //
  //msg::gui::mfc::status( "", msg::gui::mfc::ElapsedTime );

  //
  // also deletes the objs
  //

  auto task=[](CRunAnalysis*& ra)
  {
    QString title="Run analysis";
    QString msg="Finishing...";
    //int timeout=2000;
    //
    gm::send::status(title+": "+msg);
    //
    // not Ok -> don't do it -> for this to work it should run in the gui thread
    //
    //gm::doc_::gui::popup(title,msg,timeout);
    //
    delete ra;
    ra = nullptr;
    //
    _e->notify(eCmd::Quit_RA_Done,CEvents::GuiApp_EH);
    //
    msg="Finished";
    //
    gm::send::status(title+": "+msg);
    //gm::doc_::gui::popup(title,msg,timeout);
  };

  // Destroy dialog in main/GUI thread
  if (c->dlg)
  {
  delete c->dlg;
  c->dlg = nullptr;
  }

  //
  // This works fine
  //
  threads::quit(m_th,printer);
  m_th = new std::thread( task, std::ref(m_ra) );

  //
  // this doesnt crash... not bloquing ... but it looks like using gui thread instead of a new one
  //
  //auto result_doesnt_matter = std::async( task, std::ref(m_ra) );

  //
  // this crashes
  //
  //std::auto_ptr< std::thread > delete_ra( new std::thread( task, std::ref( m_ra ) ) );

  //
  // FIXME: commented temporarily
  //
  //delete m_ra; // RA_DeleteContext(->NO) + Quit_RA
  //m_ra = nullptr;
}

//
// pre: m_test exists
//
// this is bloking
//
void CGeomecDoc_EventsHandler::on_test(Cmd& cmd)
{
  enum eRes{Success,End,Error};
  auto execute_test_cmd=[=]()
  {
    int idx = m_test->idx();

    if( idx > m_test->num()-1 ) // end
      return End;
    //
    auto notify=[=]()
    {
      CTest::eCmd cmd = m_test->current_cmd(); 
      if(cmd==CTest::Label || cmd==CTest::Goto)
        return;
      QString msg = m_test->cmd_s( idx );
      if(cmd==CTest::Run || cmd==CTest::Export)
        msg += QString::asprintf(": %s", CTest::ana_type_cp[ cmd ] );

      _e->notify( make_cmd( Test_Step, new std::string(msg.toStdString() ) ), CEvents::TI_EH );
    };
    if(unittest())
      notify();
    //
    bool res = m_test->execute( idx );
    if( !res )
    {
      gm::doc_::gui::popup("Tests","Wrong format",2000);
      return Error;
    }
    return Success;
  };

  //
  // 1) 
  //	1a) create test, or 
  //	1b) increment idx
  //
  if(m_test) // 1b) increment idx
  {
    m_test->idx( m_test->idx() + 1 );
  }
  else // 1a) create
  {
    m_test = new CTest();

    //
    // unittest scenario with token as a parameter
    //
    void* data_ = cmd.second;
    if(data_)
    {
      std::string token = *reinterpret_cast< std::string* >(data_);
      free(data_);
      //
      m_ti = new TI(token); // neede to send feedback to the unittest ModelOperations.GUI
      //
      gm::send::minimize();
      //
      std::this_thread::sleep_for(std::chrono::milliseconds( 100 )); // wait for shm to be instantiated before be accessed by Receiver -> is it needed in the case ?
      //
      _e->notify( make_cmd( Test_Ack, new std::string(m_test->url().toStdString() ) ),CEvents::TI_EH);
    }
  }
    
  //
  // 2) execute command
  //
  eRes res = execute_test_cmd();
  //eRes res = End;

  //
  // 3) result: 
  //		3a) success: resent Tests event to execute next command
  //		3b) exit: end or error
  //
  switch(res)
  {
    //
    // 3a) success -> continue
    //
    case Success: 
    {
      CTest::eCmd cmd_ = m_test->current_cmd();
      //
      // run model emits Quit_RA_Done at the end so test event is emitted when Quit_RA_Done occurs
      //
      if(	cmd_ == CTest::Run ) // execute next cmd
        break;
      else
        _e->notify(eCmd::Test, CEvents::GuiApp_EH);
      break;
    }
    //
    // 3b) exit: end or error
    //
    case Error:
    case End:
    {
      delete_(m_test); // end test
      //
      _e->notify(Test_End,CEvents::TI_EH);
      //
      std::this_thread::sleep_for(std::chrono::milliseconds( 100 )); // wait for the ipc mesg to be sent first by the Receiver
      //
      // close Geomec only if unittest
      //
      if(unittest())
      {
        printer->info("post msg ID_APP_EXIT");
        //
        // FIXME: close file if any open to avoid the save/no save pop-up? (I don't know how to skip it other than this way)
        //
        m_doc->ForceFileClose(); // it works!
        //
        gm::send::quit();
        //
        std::this_thread::sleep_for(std::chrono::milliseconds( 100 ));
        //
        delete_(m_ti);
      }
      break;
    }
    default:
      break;
  }
}

void CGeomecDoc_EventsHandler::handle(const Cmd& cmd)
{
  int cmd_id = cmd.first;

  //
  // FIXME: 2020-09-29, before this was not needed, cancel scenario
  //
  //
  // FIXME: Test, Quit_RA_Done
  //
  if(!m_ra && 
    //
    cmd_id != eCmd::Test && 
    cmd_id != eCmd::Quit_RA_Done &&
    cmd_id != eCmd::Question)
    //
  {
    auto needs_ra=[=]()
    {
      int cmd_id = cmd.first;
      switch(cmd_id)
      {
      case eCmd::SetParams:
      case eCmd::Start:
      case eCmd::Run:
      case eCmd::Save:
        return true;
      default:
        return false;
      }
    };
    printer->error("doc evennts handler : ra : nullptr : cmd: %s", _cmd_s(cmd) );
    return;
  }

  switch (cmd_id)
  {
    case eCmd::Quit_RA:			on_quit_ra(); break;
    ////////////////////////////////////////
    //
    // not in dsa multi-thread, only non-dsa main gui thread
    //
    ////////////////////////////////////////
    case eCmd::SetParams:		m_ra->on_params(cmd); break;
    case eCmd::Start:			m_ra->on_start(cmd); break;
    case eCmd::DisableViews_Done:	m_doc->push_(Run); break;
    case eCmd::Run:				m_ra->on_run(cmd); break;
    case eCmd::Save:			m_ra->save(); break;
    /////////////////////////////////////////////////////
    //
    //	responses
    //
    /////////////////////////////////////////////////////
    //
    // not in dsa
    //
    case eCmd::RunEnd:			m_ra->on_run_end(); break; // end of whole analysis (not only an iteration) 
    ////////////////////////////////////////
    //
    // gui events, both dsa and non-dsa
    //
    ////////////////////////////////////////
    //
    //	enable / disable scene to avoid updates on each result update
    //
    case eCmd::UpdateViews:		m_doc->update_views(); break;
    case eCmd::DisableViews:	m_doc->disable_views(); break; // -> then Run
    case eCmd::Save_Done:		
    {
      //
      // FIXME: just a patch
      //
      // mcr 2020-09-29: this needs to be handle differently
      //
      if(_g->dsa() && _g->mfc( CGlobal::RunDiana ))
      {
        CExecuteDianaDialog* dlg = dynamic_cast<CExecuteDianaDialog*>(m_dlg);
        if(dlg)
          dlg->end( 1 );
      }

      m_doc->push_({	UpdateViews, Quit_RA});	 // kill me !!!
      break; 
    }
    //
    //	error messages
    //
    case eCmd::ChecksError:		on_checks_error(cmd); break;
    case eCmd::RunError:
    {
      std::string msg = error_msg(cmd); 
      _e->broadcast(Quit_RA); // delete ra and context
      break;
    }
    //
    //	tracking messages
    //
  case eCmd::DianaMsg:		on_diana_msg(cmd); break;
    case eCmd::Progress:		on_prg(cmd); break;
    case eCmd::Log:				on_log(cmd); break;
    case eCmd::Question:		on_question(cmd); break;
    case eCmd::Status:			on_status(cmd); break;
    case eCmd::HeartBeat_Ack:	on_hb(); break;
    case eCmd::QuitDiana_Done:	break; // when diana finishes execution forcely
    case eCmd::CancelRun:
    {
      //
      // this arrives too late, filter impelemented in doc handle methos instead
      //
      //_e->remove( CEvents::GuiApp_EH, {Progress,DianaMsg} ); // many many (2600x) progress events has been enqueued before this
      delete_(m_dlg);
      delete_(m_prg);
      break;
    }
    case eCmd::Quit_RA_Done:
    {
      if(m_test)
        _e->notify(eCmd::Test, CEvents::GuiApp_EH);
      break;
    }
    case eCmd::Test: on_test((Cmd&)cmd); break;
    ////////////////////////////////////////
    //
    // 
    //
    ////////////////////////////////////////
    default:
    {
      printer->error("event not handled: %d, %s", cmd.first, g_cmd_s[cmd.first].c_str());
      break;
    }
  }
}