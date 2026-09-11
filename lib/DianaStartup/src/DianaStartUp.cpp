// DianaStartUp.cpp: implementation of the CDianaStartUp class.
//
//////////////////////////////////////////////////////////////////////

#include <assert.h>
//diana
#include "lbcp.h"
//own
#include "DianaStartUp.h"
#include "GeomecBusClient.h"
#include "RunDianaWorker.h"
#include "Printer.h"
#include "Environment.h"
#include "DianaInterface.h"
#include "Global.h"
#include "util_.h"
#include "IListenerIpc.h" // GI
//qt
#include <QtWidgets/QApplication>
#include <QFile>
#include <QThread>
#include <QTimer>
#include <QFileInfo>
#include <QDir>

namespace
{
  Printer* printer = Printer::instance(Printer::RunU);
}

#ifndef _MAX_PATH
#define _MAX_PATH PATH_MAX
#endif  // _MAX_PATH

// Need to be different
diana_info_t di_info;
diana_info_t di_info1;

// 1) set via 'm_vars_m' map
const QString CDianaStartUp::BASE =			"BASE";
const QString CDianaStartUp::DISPLAY =		"DISPLAY";
const QString CDianaStartUp::USER_LEADER =	"USER_LEADER";
const QString CDianaStartUp::USER_TRAILER =	"USER_TRAILER";

// 2) set via diana envvars
const QString CDianaStartUp::FFDIR =		"FFDIR";
const QString CDianaStartUp::FF =			"FF";
const QString CDianaStartUp::STP0 =			"STP0";


namespace diana{
  namespace cb{ // callbacks

// 3) fix vars (connstants all along the program) are set at launch time via diana envars and taken fron 'Environment' singleton 
// (from system ENVVARs first and then from config file )

static void GetSeparateStrings( QString txt, std::vector<QString>  &RetVal )
{
  int prev = 0;
  for( int i=0; i<(int)txt.length(); ++i )
  {
    if( txt[i] == '\n' || txt[i] == '\r' )
    {
      if( i == prev )
      {
        ++prev;
        continue;
      }
      QString str=txt.mid(prev,i - prev);
      RetVal.push_back( str );
      prev = i + 1;
    }
  }
  int l = txt.length();
  while ( l && ( (txt[l-1] == '\r') || (txt[l-1] == '\n') ) )l--;

  QString str=txt.mid(prev,l - prev);
  RetVal.push_back(str);
}

static int UserMessageHandler( const char *txt )
{
  CDianaStartUp* dsu = CDianaStartUp::instance();

  std::vector<QString> Lines;
  GetSeparateStrings( txt, Lines );
  for( size_t i=0; i < Lines.size(); ++i )
    dsu->on_diana_msg(eDianaMsg::Gen,Lines[i]);

  return 0;
}

static int AppSpcMessageHandler( const char *txt )
{
  CDianaStartUp* dsu = CDianaStartUp::instance();

  std::vector<QString> Lines;
  GetSeparateStrings( txt, Lines );
  for( size_t i=0; i < Lines.size(); ++i )
    dsu->on_diana_msg(eDianaMsg::AppSpc,Lines[i]);

  return 0;
}

static int ReportProgressHandler( const char *txt )
{
  return 0;
}

static int DisplayDianaErrorMessage( const char *txt )
{
  CDianaStartUp* dsu = CDianaStartUp::instance();

  const char *p = strstr( txt, " ERRORMSG." );
  const char *q = strstr(txt, "WARNING");

  std::vector<QString>  Lines;

  if ( p )
  {
    p += 10;
    if ( p[0] && (p[1] == ':') )
    {
      switch ( p[0] )
      {
      case 'A':
      case 'F':
        GetSeparateStrings( txt, Lines );
        for( size_t i=0; i<Lines.size(); ++i )
          dsu->on_diana_msg(eDianaMsg::Error,Lines[i]);
        break;
      case 'W':
        GetSeparateStrings( txt, Lines );
        for( size_t i=0; i<Lines.size(); ++i )
          dsu->on_diana_msg(eDianaMsg::Warning,Lines[i]);
        break;
      default:
        GetSeparateStrings( txt, Lines );
        for( size_t i=0; i<Lines.size(); ++i )
          dsu->on_diana_msg(eDianaMsg::Gen,Lines[i]);
        break;
      }
    }
  }
  if(q)
  {
    GetSeparateStrings( txt, Lines );
    for( size_t i=0; i<Lines.size(); ++i )
      dsu->on_diana_msg(eDianaMsg::Warning,Lines[i]);
  }
  return 0;
}

static int UserFunc()
{
  CDianaStartUp* dsu = CDianaStartUp::instance();
  
  bool StopExecuting = dsu->quit();

  dsu->on_diana_msg(eDianaMsg::StopExe,"",StopExecuting);

  if( StopExecuting )
  {
    printer->info("quit diana requested");

    StopExecutingSegments();
    ResetDiInfo( &di_info );
  }

  return StopExecuting==true ? 1:0 ;
}

static void DefMsgHandler( const char *buf, int size )
{
  //printer->info("diana cb msg");

  if ( size <= 0 ) return;
  char *p = (char *)malloc( size+1 );
  if ( !p ) return;
  strncpy( p, buf, size );
  p[size] = '\0';
  QString Buf = p;
  free( p );

  std::vector<QString> Lines;
  GetSeparateStrings( Buf, Lines );

  CDianaStartUp* dsu = CDianaStartUp::instance();
  for( size_t i=0; i<Lines.size(); ++i )
    dsu->on_diana_msg(eDianaMsg::Gen,Lines[i]);
}

} // namespace cb
} // namespace diana

//////////////////////////////////////////////////////////////////////
//
//			notify run diana dlg [Ini]
//
//////////////////////////////////////////////////////////////////////

void CDianaStartUp::on_diana_msg( eDianaMsg type_, const QString& str, bool bStop ) 
{ 
  printer->debug("%s: %s",g_diana_msg_type[type_].toStdString().c_str(), str.toStdString().c_str() );

  if( m_gi )
  {
    m_gi->push_ipc(	Cmd(	
      DianaMsg, 
      new std::string( (g_diana_msg_type[type_]+(type_==eDianaMsg::StopExe ? bStop ? "1":"0" : str)).toStdString() ))); 
  }
  //
  // only non-dsa
  // signals can't be sent between processes: from diana_app to gm
  //
  else
  {
    switch(type_)
    {
      case Gen:		emit OnMessage(str); break;
      case AppSpc:	emit OnAppSpcMessage(str); break;
      case Error:		emit OnError(str);  break;
      case Warning:	emit OnWarning(str); break;			
      case StopExe:	emit OnStopExecuting(bStop); break; 
      case Finished:	emit OnFinished(); break;
      default:		printer->error("on diana msg : msg type undefined"); break;
    }
  }
}

//////////////////////////////////////////////////////////////////////
//
//			notify run diana dlg [End]
//
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//CDianaStartUp* CDianaStartUp::m_pThis = 0;


CDianaStartUp::CDianaStartUp() :
  //m_pid( NO_PID ),
  m_di( nullptr ),
  m_gi(nullptr ),
  m_quit(false)
{
  //assert(!m_pThis);
  //m_pThis=this;

  // to avoid crash on DiGetenv when FFDIR or FF are not initialized
  SetDianaEnv( CDianaStartUp::FFDIR, "" );
  SetDianaEnv( CDianaStartUp::FF, "" );
}

CDianaStartUp::~CDianaStartUp()
{
}

CDianaStartUp* CDianaStartUp::instance()
{
  static CDianaStartUp* _instance = new CDianaStartUp();
  return _instance;

  /*if(!m_pThis)
    new CDianaStartUp();

  assert(m_pThis);
  return m_pThis;*/
}


////////////////////////////////////////////////////////////////////////////////////////////////////////

// FIXME: We asume result is Ok, just to test
/*void CDianaStartUp::on_finished()
{
  printer->debug( "dsu : on_finished" );
  //m_function_cb( res ? IDI_DIANA_2P : IDI_DIANA_CRASH ); // upate tools frame icon 
  m_function_cb( DSA_Icon::_2P );
}*/

int  CDianaStartUp::RunDiana()
{
  printer->info("run diana --%s-- --%s--", _g->dsa() ? "dsa":"non-dsa",util::GetAppFileName().c_str());

  quit(false);

  int res = _g->dsa() ?
    RunDiana_DSA() :
    //RunDiana_DSA_OLD() :
    RunDiana_ONE_PROCESS();

  return res;
}



/////////////////////////////////////////////////////////////////
//
//			CDianaStartUp
//
/////////////////////////////////////////////////////////////////

//
// mcr 2020-03-16, 16:00 -> not doing DI delete / new...
//
// conclusion: I don't know what I'm doing wrong but with HB I've deadlocks
//
// is nothing with memory in wrong state after crash, is the synchro mechanism itself: mutex, condition vars, etc.
//

void CDianaStartUp::delete_di() 
{ 
  if (m_di) 
    delete m_di; 
  m_di = nullptr; 
};

int  CDianaStartUp::RunDiana_DSA()
{
  printer->info("run diana request");

  //
  // look at the test app, I think no delay needed
  //
  const int DELAY = 100;

  if( m_di )
  {
    delete m_di; // quits all threads (3x)
    m_di = nullptr;
  }

  std::this_thread::sleep_for(std::chrono::milliseconds( DELAY ));

  if( !m_di )
    m_di = new DI();

  return 0;
}

//
// DON'T USE
//
// ModGMBus implementation
//
#ifdef kk
int  CDianaStartUp::RunDiana_DSA_OLD()
{
  QString FFDIR_ = GetDianaEnv( CDianaStartUp::FFDIR );

  printer->info( "[Geomec] CDianaStartUp > RunDiana [Path:%s]", FFDIR_.toStdString().c_str() );

  m_function_cb( DSA_Icon::Running ); // upate tools frame icon 

  // Qt thread, not wait or join, response to the main app is async

  QThread* thread = new QThread;
  RunDianaWorker* worker = new RunDianaWorker( FFDIR_, m_pid );
  worker->moveToThread(thread);
    
  // needed?
  // connect(worker, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
    
  connect(thread, SIGNAL(started()), worker, SLOT(process()));
    
  connect(worker, SIGNAL(finished()), thread, SLOT(quit()), Qt::DirectConnection);
  connect(worker, SIGNAL(finished()), this, SLOT(on_finished()), Qt::DirectConnection ); // this is not blocked by the processEvents
    
  // what does this do?
  connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
  connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

  thread->start();
  //printer->debug( "started" );
    
  // no need wait anymore as run model and show results are now async
  // even to exit from wait Qt::DirectConnection is mandatoty
  // thread->wait();
  // printer->debug( "after wait" );

  // result still no avaible here
  // we need to return 0
  // In this case 0 means Ok

  int res = 0; // !!!!!!! Look at the comments
  
  //int res = worker->m_res ? 0:1;
  

  // can't be deleted as they are still running
  // deleteLater will deleet them? check it out
  // delete worker;
  // delete thread;

  // fedback to the app not yet ready
  // m_function_cb( res ? IDI_DIANA_2P : IDI_DIANA_CRASH ); // upate tools frame icon 

  // printer->info( "[Geomec] CDianaStartUp > RunDiana : after 'run_diana_task' : res:%d", res );
  return res;
}
#endif

// Needs to be fixed
std::string get_env()
{
  std::string s;

  /*LPTSTR lpszVariable;
  LPVOID lpvEnv = GetEnvironmentStrings();
  if( lpvEnv)
  {
    for (lpszVariable = (LPTSTR) lpvEnv; *lpszVariable; lpszVariable++)
    {
      while (*lpszVariable)
        //putchar(*lpszVariable++);
        s += *lpszVariable++;
      //putchar('\n');
      s += '\n';
    }
  }*/
  return s;
}

// '0': Ok
// check that basic diana envs are set: 'STP0' (binary to run) / 'FFDIR' / 'DIAPARH' / 'FF'
// 'FireOnError' out of this method
CDianaStartUp::eError CDianaStartUp::checks()
{
  //------------------ 1) STP0
  QString STP0_ = GetDianaEnv( CEnvironment::STP0 );
  if( STP0_.isEmpty() ) // TODO: empty or doesn't exist
    return STP0_E;

  //------------------ 2) FFDIR
  QString FFDIR_ = GetDianaEnv( CDianaStartUp::FFDIR );
  if( !QDir( FFDIR_ ).exists() )
    return FFDIR_E;

  //------------------ 3) DIAPATH
  QString DIAPATH_ = GetDianaEnv( CEnvironment::DIAPATH );
  if( DIAPATH_.isEmpty() || 
    !QDir( DIAPATH_ ).exists() )
    return DIAPATH_E;

  //------------------ 4) FF = Filos file
  QString FF_ = GetDianaEnv( CDianaStartUp::FF );
  if( !QFileInfo( FFDIR_ + "/" + FF_ ).exists() )
    return FF_E;

  return NO_E;
}


void CDianaStartUp::var( const QString& key, const QString& val )
{
  m_vars_m[ key ] = val;
}

QString CDianaStartUp::var( const QString& key )
{
  if( is_var( key ) )
    return m_vars_m[ key ];
  return QString();
}

bool CDianaStartUp::is_var( const QString& key )
{
  return m_vars_m.find( key ) != m_vars_m.end();
}

// MCR 2019-10-31
// 1. No need to use a QByteArray instaed of a QString
// 2. Don't know the reason why but both 1)'vDiSetenv' and 2)DianaArguments need to be informed
int CDianaStartUp::RunDiana_ONE_PROCESS()
{
  printer->info( "run >> --%s--", util::GetAppFileName().c_str() ); //NOT DIFFERENT PROCESS

  QString current_path = QDir::currentPath();

  Print_DianaEnv();

  //------------------ 1) Checks
  eError res = checks();
  if( res != NO_E ) // ERROR
  {
    QString error_s = 
      QString("ERROR: ") + 
      res == STP0_E ?		CEnvironment::STP0 :
      res == FFDIR_E ?	CDianaStartUp::FFDIR :
      res == DIAPATH_E ?	CEnvironment::DIAPATH :
                CDianaStartUp::FF;
    on_diana_msg(Error,error_s);
    printer->error( (char*)error_s.toStdString().c_str() );
    QDir::setCurrent( current_path );
    return res;
  }

  QDir::setCurrent( GetDianaEnv( CDianaStartUp::FFDIR ) );
  
  //------------------ 2) Gather vars to pass to 'DianaArguments'
  QString STP0_ = GetDianaEnv( CEnvironment::STP0 ); // binary to execute

  QString FF_ = GetDianaEnv( CDianaStartUp::FF );

  QString BASE_="DIANA";
  if( is_var( CDianaStartUp::BASE ) )
    BASE_ = var( CDianaStartUp::BASE );

  QString DISPLAY_; // dont show messages by default
  if( is_var( CDianaStartUp::DISPLAY ) )
    if( var( CDianaStartUp::DISPLAY ).toLower() == "yes" )
      DISPLAY_ = "-m";
  //------------------
  const char *argv[7];

  /*
  // This doesn't work
  const int NUM = 7;
  const int LEN = 100;
  char argv[ NUM ][ LEN ];
  for( int i=0; i < NUM; i++ )
    memset( argv[ i ], 0, LEN );*/

  //strncpy( argv[0], "HOLA", 99 );
  //strncpy( argv[6], "HOLA----------------------------------------------------", 99 );

  int argc = 0;
  char **nargv;
  // order counts but I don't understand why
  for( auto value : { STP0_, DISPLAY_, BASE_, FF_ } )
  {
    if( !value.isEmpty() )		
      argv[ argc++ ] = strdup( value.toStdString().c_str() );
      //strncpy( argv[ argc++ ], value.toStdString().c_str(), LEN-1 );
  }
  argv[argc] = NULL;
  di_info.commandfile = vDiStrsave( "", "", NULL );
  // Data from 'argv' to 'di_info'
  DianaArguments( argc, (char **)argv, &di_info, &nargv );
  //------------------

  Print_DianaArgs( argv, argc );
  Print_DianaVars();

  //------------------ 3) Set CALLBACKS
  InstallDCMessageHandler(
    ASK_DC_HANDLE_ERRMSG,
    ASK_DC_TERMINATE_ERRMSG,
    diana::cb::DisplayDianaErrorMessage );

  InstallDCMessageHandler(
    ASK_DC_APPSPC_MSG,
    NULL,
    diana::cb::AppSpcMessageHandler );

  InstallDCMessageHandler(
  ASK_DC_REPORT_PROGRESS,
  ASK_DC_ENDOF_PROGRESSTEXT,
  diana::cb::ReportProgressHandler);
  
  if( is_var( CDianaStartUp::USER_LEADER ) && 
    is_var( CDianaStartUp::USER_TRAILER ) )
  {
    InstallDCMessageHandler(
      var( CDianaStartUp::USER_LEADER ).toStdString().c_str(),
      var( CDianaStartUp::USER_TRAILER ).toStdString().c_str(),
      diana::cb::UserMessageHandler );
  }

  InstallDCDefaultMessageHandler( diana::cb::DefMsgHandler);

  InstallDCUserFunction( diana::cb::UserFunc );

  SetDCReadNoDelay();

  di_info.size = sizeof( diana_info_t );
  di_info.windows = 1;

  //------------------ 4) Run diana
  int iret = DianaExecute( &di_info );

  printer->debug( "diana execute : res: %s (not reliable as allways is Ok)", iret ? "Not Ok" : "Ok" );

  Free_DianaArgs( argv, argc );

  ResetDiInfo( &di_info );
  QDir::setCurrent( current_path );

  if( iret != 0 ) // ERROR
  {
    on_diana_msg(Error,"Can't start Diana");
    return iret;
  }

  on_diana_msg(Finished);

  // NO NEED as FF and FFDIR are set on each retry
  // Defer cleanup of m_vars_m and ClearDianaEnv_Model, as we may need them if we have a license error and want to retry
  // RunDianaCleanup *must* be called by calling routine otherwise
  
  //------------------ 5) clear
  //m_vars_m.clear();		// vars differs for each execution
  //ClearDianaEnv_Model();	// also clear 'FF' and 'FFDIR'

  printer->info( "run <<" );
  return iret; // '0' is Ok
}

void CDianaStartUp::Print_DianaArgs( const char *argv[], int argc )
{
  for( int i = 0; i < argc; i++ )
    printer->debug("[DianaA] %s", argv[ i ] );
}

void CDianaStartUp::Free_DianaArgs( const char *argv[], int argc )
{
  for( int i = 0; i < argc; i++ )
    free( (void*)argv[ i ] );
}


void CDianaStartUp::Print_DianaVars()
{
  for( auto arg : m_vars_m )
  {
    printer->debug("[DianaV] %s: %s",	
      arg.first.toStdString().c_str(), 
      arg.second.toStdString().c_str() );
  }
}

void CDianaStartUp::Print_DianaEnv()
{
  CEnvironment* env = CEnvironment::instance();

  auto keys = {
    CEnvironment::DIAPATH,
    CEnvironment::DIASHARE,
    CEnvironment::DIALIB,
    CEnvironment::DIASLIB,
    CEnvironment::DIAERRPATH,
    CEnvironment::APP_SPC_LOGGING,
    CEnvironment::STP0 };

  for( auto key : keys )
    printer->debug("[Diana ] %s=%s",	
      key.toStdString().c_str(), 
      DiGetenv( key.toStdString().c_str() ));

  for( auto key : { 
    CDianaStartUp::FF, 
    CDianaStartUp::FFDIR } )
      printer->debug("[Diana+] %s=%s",	
        key.toStdString().c_str(), 
        DiGetenv( key.toStdString().c_str() ));
}

const QString CDianaStartUp::GetDianaEnv( const QString& key )
{
  return QString( DiGetenv( key.toStdString().c_str() ) );
}

void CDianaStartUp::SetDianaEnv( const QString& key, const QString& value )
{
  vDiSetenv("%s=%s",	
    key.toStdString().c_str(), 
    value.toStdString().c_str() );

  char* cp = DiGetenv( key.toStdString().c_str() );
  //std::string a = cp;
  //QString s = GetDianaEnv( key );
  //int a=1;
}

// when is this needed?
bool CDianaStartUp::SetDianaEnv_v2(QString varName, QString value)
{
#if (defined(_WIN32) || defined(_WIN64)) && !defined(NDEBUG)
  return (vDiSetenv("%s=%s", varName.toStdString().c_str(),
  value.toStdString().c_str()) == 0);
#else // (!defined(_WIN32) && !defined(_WIN64)) || defined(NDEBUG)
  return (vDiSetenv(
      DiStrsave(
        QString(varName + "=" + value).toStdString().c_str())) == 0);
#endif  // (defined(_WIN32) || defined(_WIN64)) && !defined(NDEBUG)
}


void CDianaStartUp::Print_Env()
{
  CEnvironment* env = CEnvironment::instance();

  auto keys = {
    CEnvironment::DIAPATH,
    CEnvironment::DIASHARE,
    CEnvironment::DIALIB,
    CEnvironment::DIASLIB,
    CEnvironment::DIAERRPATH,
    CEnvironment::APP_SPC_LOGGING,
    CEnvironment::STP0 };

  for( auto key : keys )
    printer->debug("%s=%s",	
      key.toStdString().c_str(), 
      env->get( key ).toStdString().c_str());
}


void CDianaStartUp::SetDianaEnv()
{
  const char *argv[1] = {"geomec"};
  int argc = 1;
  char **nargv;

  CEnvironment* env = CEnvironment::instance();

  auto keys = {
    CEnvironment::DIAPATH,
    CEnvironment::DIASHARE,
    CEnvironment::DIALIB,
    CEnvironment::DIASLIB,
    CEnvironment::DIAERRPATH,
    CEnvironment::APP_SPC_LOGGING,
    CEnvironment::STP0 };

  // 1)
  for( auto key : keys )
    SetDianaEnv( key, env->get( key ) );
  // 2)
  // MCR: Is this needed ????
  DianaArguments( argc, (char **)argv, &di_info1, &nargv );
}

void CDianaStartUp::RunDianaCleanup()
{
  //------------------ 5) clear
  m_vars_m.clear();		// vars differs for each execution
  ClearDianaEnv_Model();	// also clear 'FF' and 'FFDIR'
}

void CDianaStartUp::ClearDianaEnv_All() const
{
  //reset all diana env.
  
  for( auto var : 
    {"STP0",
    "DIASLIB",
    "DIALIB",
    "FEMGVX",
    "DIAPATH",
    "FGVWORK",
    "FF",
    "FFDIR",
    "FGVSTR",
    "DIABIN",
    "DIASHARE",
    "DIASCRIPT",
    "BINSEG",
    "DIALIB",
    "DIASLIB",
    "SEGPATH",
    "DIAERRPATH" } )
      vDiSetenv( "%s=", var );
}

void CDianaStartUp::ClearDianaEnv_Model() const
{
  //reset all diana env.
  
  for( auto var : 
      {	
        CDianaStartUp::FF, 
        CDianaStartUp::FFDIR
      } 
    )
      vDiSetenv( "%s=", var.toStdString().c_str() );
}