#include "DianaInterface.h"

//qt
#include <QProcess> 
#include <QCoreApplication> //applicationPid()
#include <QFileInfo>
//own
#include "Printer.h" 
#include "DianaStartUp.h" 
#include "Global.h" 
#include "ISettings.h"  // hb timeout
//std
#include <thread>

#ifndef WIN32
#include <unistd.h>  // pid_t getpgid(pid_t pid);
#endif

namespace
{
  Printer* printer = Printer::instance(Printer::DiaI);

#ifdef WIN32
  const QString DIANA_APP = "gm_diana_app.exe";
#else
  const QString DIANA_APP = "gm_diana_app";
#endif

  static const int NOT_DEFINED = -1;
  static const int NOT_FOUND = -1;
}

////////////////////////////////////////////////////
//
//				functions
//
////////////////////////////////////////////////////

namespace gm{
namespace di{

qint64 launch( QString bin, const QStringList& args )
{
  QProcess p1;
  p1.setProgram( bin );
  p1.setArguments( args );

  qint64 pid_ = 0;
  bool res1 = p1.startDetached(&pid_);

  if( res1 && pid_ )
  {
    printer->info("%s launched", bin.toStdString().c_str());
    return pid_;
  }
  else
  {
    printer->error("launch : '%s' not found ... pid:%lld",bin.toStdString().c_str(), pid_);
    return 0;
  }

}

#ifdef KK
//#include "Windows.h"
//
// DO NOT USE
//
// it doesn't work so far
//
bool exe( 
  const QString& bin, 
  QStringList params,
  QString& result )
{
  QProcess p1;
  p1.setProgram( bin );
  p1.setArguments( params );

  //
  // it doesn't compile
  //
  /*p1.setCreateProcessArgumentsModifier(
        [](QProcess::CreateProcessArguments *args) {
    args->flags |= CREATE_NEW_CONSOLE;
    args->startupInfo->dwFlags &=~ STARTF_USESTDHANDLES;
  });*/

  //p1.start( bin, params );
  p1.start();
  if (!p1.waitForStarted())
    return false;

  if( !p1.waitForFinished() )
    return false;
  QByteArray result_ = p1.readAll();
  //QString output = p1.readAllStandardOutput(); // this doesn't make it work
  result = QString::fromStdString( result_.toStdString() );

  return true;
}

//
//
// DO NOT USE
//
// this doesn't work either in any of the thoussand ways I've formatted the command
//
std::string exec( const char* cmd ) 
{
  //std::array<char, 128> buffer;
  std::array<char, 512> buffer;
  std::string result;

  std::unique_ptr< FILE, decltype(&_pclose) > pipe( _popen(cmd, "r"), _pclose );

  if (!pipe)
    throw std::runtime_error("popen() failed!");
  
  while( fgets( buffer.data(), (int)buffer.size(), pipe.get() ) != nullptr )
    result += buffer.data();

  return result;
}
#endif
//
// this works fine
//
// Ex. running("gm_diana_app.exe")
//
// args:
// 
// /FI: filter
// /FO: output format
// /NH: no headers in the output
//
bool running(const QString &process) 
{
  QProcess p;
  p.start(
    "tasklist",
    QStringList()	<< "/NH" 
            << "/FO" << "CSV" 
            << "/FI" 
            << QString("IMAGENAME eq %1").arg(process));
  p.waitForFinished();
  QString output = p.readAllStandardOutput();
  return output.startsWith(QString("\"%1").arg(process));
}

// windows-only
bool running_(const qint64& pid) 
{
  QString pid_qs = QString::fromStdString( std::to_string( pid ) );

  QProcess p;
  QStringList params = QStringList()	
  << "/NH" 
  << "/FO" 
  << "CSV" 
  << "/FI" 
  << QString("PID eq %1").arg(pid_qs);

  p.start("tasklist", params );

  p.waitForFinished();
  QString output = p.readAllStandardOutput();

  printer->debug("running: tasklist %s / %s", 
    params.join(" ").toStdString().c_str(), 
    output.toStdString().c_str() );

  return output.contains( pid_qs );
}

bool running(const qint64& pid) 
{
  bool res;
#ifdef WIN32
  res = running_(pid); 
#else
  res = getpgid(pid) >= 0;
#endif
  printer->debug("running: %s",res?"yes":"no");
  return res;
}

//
// /f: force
// /t: kill also all secondary processes linked to the process
// /im: image name
//
void kill(const QString &process) 
{
  QProcess::execute("taskkill /f /t /im " + process);
}

//
// TODO: In Linux, kill also the descendants processes: 'gm42' and 'conhost'
//
void kill(const qint64& pid) 
{
#ifdef WIN32
  QString cmd = "taskkill /f /t /pid " + QString::fromStdString( std::to_string( pid ) );
#else
  QString cmd = "kill -9 " + QString::fromStdString( std::to_string( pid ) );
#endif

  printer->debug("kill: pid:%lld, cmd:%s",pid,cmd.toStdString().c_str()); 

  QProcess::execute(cmd);
}

std::string now_s(const std::string& format)
{
  auto now = std::chrono::system_clock::now();
  auto in_time_t = std::chrono::system_clock::to_time_t(now);

  std::stringstream ss;
  ss << std::put_time(std::localtime(&in_time_t), format.c_str());
  return ss.str();
}


} // namespace di
} // namespace gm

const char* DI::m_kill_diana_res_cp[] = { "DianaNotRunning", "DianaKilled", "UnableToKillDiana" };

const char* DI::m_diana_status_cp[] = { "Stopped", "Running" };

//
// finally it doesn't receive any event from outside
// so no event handler needed
// everything is automanaged
// constructor launches diana_app and RunDiana cmd
// on RunDiana_Done it sdends RunSuccess and kills diana_app
//
//
// --WHAT DOES THIS DO-- 
//
// waits for cmds in the cmds queue
// when available cmd is executed
// cmds:
// -0: launch diana_app
// -others: transfered to dian_app to be run there
//
//
// --PROBLEM WHEN diana_app DIES--
//
// mutex stays corrupt after a diana_app crash
// after that nobody else can lock the mutex
//
// unlock() doesn't release the mutex
//
// doing notify in the cond var protected by the mutex in order to free the wait causes a crash
//
// deleting and creating again shared memory doesn't work either
//
//
// --POSIX SOLUTION--
//
// mutex is not release when diana process dies !!!! 
// can't be fixed with boost, only with POSIX threads
DI::DI()
  :	IListener( CEvents::DI_EH, {HeartBeat, HeartBeat_Error} ), // both emitted by HB
    IListenerIpc(IListenerIpc::Server),
    m_diana_app_pid(0),
    m_diana_status(Stopped),
    m_hb_timeout(NOT_DEFINED),
    m_hb(CEvents::HB_EH,CEvents::DI_EH)
{
  printer->debug("new");
  //
  m_dsu = CDianaStartUp::instance();
  //
  // wait till shm is well allocated before launching diana_app: with 10ms. it works Ok
  //
  // otherwise race condition
  //
  std::this_thread::sleep_for(std::chrono::milliseconds( 10 )); // don't remove it, needed !!!
  //
  push_( eCmd::LaunchDiana );
}

DI::~DI()
{
  printer->debug("delete >>");

  //
  // 1 sec. should be by far enough
  //
  int nRetries=0;
  while(diana_status()==Running && nRetries++ < 10)
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // to avoid race condition

  printer->info("diana_app status: %s", m_diana_status_cp[diana_status()]);

  //
  // in principle not needed
  // just in case diana_app thread dies but process still alive
  //
  //std::this_thread::sleep_for(std::chrono::milliseconds(500)); // to avoid race condition
  //

  //
  // is there any inpact on mutexes or cond_vars with killing diana_app in a forced way?
  //
  // I don't think so, all them have been released in the lines above
  //
  //
  // 'if' is needed as it takes a bit to diana_app quitting after sending DianaQuit_Ack
  // don't remoev it !!!
  //
  if( diana_status()==Running )
  {
    //
    // is it better to kill it or to leave a zombie process?
    //
    eKillDianaRes res = kill_diana( diana_app_pid() );
    printer->info("%s",m_kill_diana_res_cp[res]);
  }

  printer->debug("delete <<");
  printer->info("--di released--");
}

DI::eKillDianaRes DI::kill_diana()
{
  if( !gm::di::running( DIANA_APP ) )
    return DianaNotRunning;

  printer->info("found '%s' running : kill it", DIANA_APP.toStdString().c_str());
  gm::di::kill( DIANA_APP );

  return gm::di::running( DIANA_APP ) ? UnableToKillDiana : DianaKilled;
}

DI::eKillDianaRes DI::kill_diana(const qint64& pid_ )
{
  if( !gm::di::running( pid_ ) )
    return DianaNotRunning;

  printer->info("found pid:%lld running : kill it", pid_);
  gm::di::kill( pid_ );

  return gm::di::running( pid_ ) ? UnableToKillDiana : DianaKilled;
}

void DI::on_launch_diana()
{
  qint64 pid = gm::di::launch( 
    DIANA_APP, 
    QStringList() 
      << QString::fromStdString( token() )	// for shm naming
      << QString::number( Printer::pid() ) );	// for named mutex

  diana_app_pid(pid);

  if( !diana_app_pid() )
  {
    printer->error("not possible to launch '%s'", DIANA_APP.toStdString().c_str());
    _e->broadcast(make_cmd(RunError, new CEvents::ErrorData{ CEvents::DI_EH, UnableToLaunchDiana } )); // joins and deletes this thread
  }
}

QStringList DI::run_diana_params()
{
  QFileInfo STP0_info( m_dsu->GetDianaEnv( CDianaStartUp::STP0 ) );
  QString STP0_filename(STP0_info.fileName());

  QStringList params = QStringList()
    // env
    << m_dsu->GetDianaEnv( CDianaStartUp::FFDIR )
    << m_dsu->GetDianaEnv( CDianaStartUp::FF )
    << STP0_filename
    // vars
    << m_dsu->var( CDianaStartUp::BASE ) 
    << m_dsu->var( CDianaStartUp::DISPLAY )
    << m_dsu->var( CDianaStartUp::USER_LEADER )
    << m_dsu->var( CDianaStartUp::USER_TRAILER ) ;

  auto print_=[=](){
  printer->debug("size:%d --FFDIR-- %s --FF-- %s --STP0-- %s --BASE-- %s --DISPLAY-- %s --USER_LEADER-- %s --USER_TRAILER-- %s", 
    params.join("*").toStdString().size(),
    params[0].toStdString().c_str(),
    params[1].toStdString().c_str(),
    params[2].toStdString().c_str(),
    params[3].toStdString().c_str(),
    params[4].toStdString().c_str(),
    params[5].toStdString().c_str(),
    params[6].toStdString().c_str() );
  };
  print_();

  return params;
}

void DI::on_diana_response(const Cmd& cmd)
{
  printer->debug("on diana response");

  eCmd cmd_id = (eCmd)cmd.first;

  switch( cmd_id )
  {
    case eCmd::LaunchDiana_Ack_:
    {
      diana_status(Running);
      //
      // order:
      //	1. m_hb_timeout
      //	2. geomec.ini Dsa-HB_TIMEOUT
      //		exist 
      //			0 -> disabled
      //			not 0 -> take this
      //		don't exist -> take default (1000 ms.)
      //
      auto start_hb=[=]()
      {
        if(m_hb_timeout==NOT_DEFINED)
        {
          int timeout = settings::get_i("Dsa", "HB_TIMEOUT",NOT_FOUND); 
          if(timeout!=NOT_FOUND)
            m_hb.hb_timeout(timeout);
        }
        else
        {
          m_hb.hb_timeout(m_hb_timeout);
        }
      
        if(m_hb.hb_timeout())
          m_hb.push_( Start_HB );
      };
      start_hb();
      //
      //
      //
      push( Cmd( RunDiana, new std::string( run_diana_params().join("*").toStdString() ) ) );
      //
      break;
    }
    case eCmd::RunDiana_Done_:	
    {
      m_hb.push_(	Quit_HB );
      //
      push_( QuitDiana );
      //
      _e->broadcast(RunSuccess); //End step
      break;
    }
    case eCmd::QuitDiana_Ack:	diana_status(Stopped); break;
    default:					printer->error("cmd not handled : %s", g_cmd_s[ cmd.first ].c_str()); break;
  }
}

void DI::execute(const Cmd& cmd)
{
  printer->debug("execute cmd");

  eCmd cmd_id = (eCmd)cmd.first;

  switch( cmd_id )
  {
    case LaunchDiana: 
      on_launch_diana(); 
      break;
    case DoNothing:	
      std::this_thread::sleep_for(std::chrono::milliseconds( 1*1000 )); 
      break;
    case HeartBeat_Error:
      _e->broadcast(make_cmd(RunError, new CEvents::ErrorData{ CEvents::DI_EH, HeartBeatError } ));
      break;
    default:				
      printer->error("cmd not handled : %s", g_cmd_s[ cmd.first ].c_str()); 
      break;
  }
}

void DI::received(Cmd cmd) //ipc
{
  eCmd cmd_id = (eCmd)cmd.first;

  switch(cmd_id)
  {
    //
    // broadcast it without handle it
    // 
    case DianaMsg: 
    case QuitDiana_Done:
    case RunDiana_Ack_: // this one used only in test 'ModelOperations.KillDianaApp'
    {
      _e->broadcast(cmd); 
      break;
    }
    case HeartBeat_Ack:
    {
      m_hb.push_(HeartBeat_Ack);
      _e->broadcast(HeartBeat_Ack);
      break;
    }
    //
    // handle it
    //
    default: 
    {
      push(cmd); 
      break;
    }
  }
}

void DI::handle(Cmd cmd)
{
  eCmd cmd_id = (eCmd)cmd.first;

  switch( cmd_id )
  {
    ////////////////////////////////////////////////////
    //
    //		1) to be execute localy by DI
    //
    ////////////////////////////////////////////////////
    case eCmd::LaunchDiana: // Launch diana_app
    case eCmd::Quit_Listen: 
    case eCmd::DoNothing: 
    case HeartBeat_Error:
      execute(cmd);
      break;
    ////////////////////////////////////////////////////
    //
    //		2) cmds to be forwarded to diana
    //
    ////////////////////////////////////////////////////
    case RunDiana:
    case HeartBeat: // forward it to diana_app
    case QuitDiana:
    {
      if(cmd_id == QuitDiana && diana_status() == Stopped)
      {
        printer->error("QuitDiana : no need : Stopped");
        break;
      }
      push_ipc( cmd );
      break;
    }
    ////////////////////////////////////////////////////
    //
    //		3) diana_app responses
    //
    ////////////////////////////////////////////////////
    case LaunchDiana_Ack_:
    case RunDiana_Ack_:
    case RunDiana_Done_:
    case QuitDiana_Ack:
      on_diana_response(cmd);
      break;
    ////////////////////////////////////////////////////
    //
    //		error
    //
    ////////////////////////////////////////////////////
    default:
    {
      printer->error("cmd not handled : %s", g_cmd_s[ cmd.first ].c_str());
      break;
    }
  } // switch
}

//
// don't do it any longer as can be other Geomec instances running diana_app
//
#ifdef KK
void DI::on_launch_diana()
{
  eKillDianaRes res = kill_diana( pid() );

  if(	res==DianaNotRunning || 
    res==DianaKilled)
  {
    pid( gm::di::launch( DIANA_APP ) );
  }
  else
  {
    printer->error("not possible to kill '%s'", DIANA_APP.toStdString().c_str());
    //
    push_( Quit_Listen ); // blocks following command so Ok
    push_( Quit_Exe );
    //
    _e->broadcast(make_cmd(RunError, new int(UnableToLaunchDiana))); // joins and deletes this thread
  }
}
#endif