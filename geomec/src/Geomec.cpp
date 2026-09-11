#include "stdafx.h"
//own
#include "Geomec.h"
#include "TipDlg.h"
#include "Splah.h"
#include "MainFrm.h"
#include "openglsplitview.h"
#include "GeomecDoc.h"
#include "versioninfo.h"
#include "ModelBase.h"
#include "KeyFile.h"
//diana
#include "lbcp.h"
//own
#include "version.h"
#include "FemAppMainWindow.h"
#if LICENSING
#include "LicenseData.h"
#endif
#include "MessageReporter.h"
//
#include "resource.h"
#include "resourceIDS.h"
//
#include "GeomecStringTable.h"
//
#include "SettingsFile.h"
#include "WindowsPlatform.h"
#include "RegistryCleanupWrapper.h"
//oiv
#include "Inventor\Gui\SoGui.h"
#include "MeshVizXLM\mapping\MoMeshViz.h"
#include <MeshViz/graph/PoGroup3Axis3.h>
#include "MigrateProfileSettings.h"
//std
#include <iostream>
#include <fstream>
//own
#include "Printer.h"
#include "ChangelogDlg.h"
#include "GeomecHelper.h" // HandleChangelog, HandlePlugins, qdebug_msg_handler
#include "GeomecUtils.h" //GetGeomecTempPathExt
#include "QUtil.h"
#include "DianaStartUp.h"
#include "CustomCmdLineInfo.h"
#include "GlobalMessage.h"
#include "GlobalMessage_GUI_MFC.h"
#include "GlobalMessage_GUI.h"
#include "AboutDlg.h"
#include "SettingsIni.h"
#include "Global.h"
#include "ISettings.h"

//own-progress
#include "ProgressFactory.h" 
#include "ProgressFactory_MFC.h"
//qt
#include <QApplication>
#include <QDir>
#include <QTimer> //singleshot
#include <QtCore\QtDebug>
#include <QtCore\QtGlobal>
#include <QDateTime>



#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif

#ifndef _DEBUG
// the following line can be commented out TEMPORARILY for testing purposes
// it will allow the application to run without valid license checkout
// all releases MUST exit without a valid license checkout or borrow
//#define EXIT_WITHOUT_LICENSE
#endif

static std::auto_ptr <CRegistryCleanupWrapper>
  registryCleanupWrapper(new CRegistryCleanupWrapper());

/////////////////////////////////////////////////////////////////////////////
// CGeomecApp

BEGIN_MESSAGE_MAP(CGeomecApp, CTnoWinApp)
  ON_COMMAND(CG_IDS_TIPOFTHEDAY, ShowTipOfTheDay)
  //{{AFX_MSG_MAP(CGeomecApp)
  ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
    // NOTE - the ClassWizard will add and remove mapping macros here.
    //    DO NOT EDIT what you see in these blocks of generated code!
  //}}AFX_MSG_MAP
  // Standard file based document commands
  ON_COMMAND(ID_FILE_NEW, OnFileNew)
  ON_COMMAND(ID_FILE_OPEN, CTnoWinApp::OnFileOpen)
  // Standard print setup command
  ON_COMMAND(ID_FILE_PRINT_SETUP, CTnoWinApp::OnFilePrintSetup)
  ON_COMMAND(ID_HELP, OnHelp)
  ON_COMMAND(ID_CONTEXT_HELP, OnContextHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGeomecApp construction

namespace
{
  Printer* printer = Printer::instance(Printer::App);
}

// Geomec multiple instances
namespace
{
//static const bool MULTIPLE_INSTANCES = true;
static const QString GEOMEC_INST_PATH = "C:/Program Files/Geomec"; //FIXME

void print_installations()
{
  QDir dir( 
    GEOMEC_INST_PATH, 
    "", //no filter 
    QDir::Name, 
    QDir::Dirs | QDir::NoDot | QDir::NoDotDot ); // ????

  QStringList list = dir.entryList();
  for (int i = 0; i < list.size(); ++i) 
    printer->debug("dir: %s", 
      list[ i ].toStdString().c_str() );
}

} //namespace

namespace gm{

namespace license{
  bool check()
  {
    // get the application version
    CString str = "geomec.exe";
    CVersionInfo versioninfo;
    versioninfo.GetVersionInfo((char*)LPCTSTR(str));

    // create the license manager

    // build the app string with separate characters so the name cannot be modified in the executable
    CString sApp;
    sApp += 'G';
    sApp += 'E';
    sApp += 'O';
    sApp += 'M';
    sApp += 'E';
    sApp += 'C';
    LicenseData* pLicenseMgr = LicenseData::CreateInstance(sApp, versioninfo.m_strFixedProductVersion);

    // check the availability of the license
    if(!pLicenseMgr || !pLicenseMgr->SetupLicense(TRUE, FALSE))
    {
      // see comment for EXIT_WITHOUT_LICENSE above when you need to
      // run a release build without a license server for testing
#ifdef EXIT_WITHOUT_LICENSE
      return false;
#endif
    }
    return true;
  }
} //namespace license

namespace oiv{
  bool init( int nTotalAttempts, int nOIVLicenseAttempt, CString strProductName )
  {
    // now we check for Open Inventor licenses; it's a pity that we can't update the splash screen (yet)
    bool bOIV_retry = false;
    bool bOIV_initialized = false;

    do
    {
    if (!SoGui::isInitialized())
      SoGui::init();

    if (SoGui::isInitialized() && !MoMeshViz::isInitialized())
      MoMeshViz::init();

    bOIV_initialized = SoGui::isInitialized() && MoMeshViz::isInitialized();

    if (bOIV_initialized)
    {
      // If we create PoGroup3Axis3 later, OIV will check for a license, even though it should
      // already have been covered by the MoMeshViz::init above. No other init seems to do the
      // trick, so we do it here.
      // We expect this check to succeed, otherwise OIV will probably hand us its assert window.
      // Just in case, we surround it with a try-catch.

      try
      {
      new PoGroup3Axis3; // OIV will handle delete
      }
      catch(...)
      {
      bOIV_initialized = false;
      }
    }

    if (!bOIV_initialized)
    {
      const QString pattern = "Geomec can attempt different settings for OIV licensing.\nPlease press Retry for attempt %1 of %2 and start Geomec again.\n\n"
                  "If you have borrowed licenses, press Cancel and check them.";
      MessageReporter m;

      ISettings* settings = ISettings::instance();

      switch (nOIVLicenseAttempt)
      {
      case 0:
      if (m.ShowRetryBox(pattern.arg(1).arg(nTotalAttempts).toStdString().c_str(), strProductName))
        settings->writeProfileInt("Settings", "OIVLicenseAttempt", 1);
      break;
      case 1:
      if (m.ShowRetryBox(pattern.arg(2).arg(nTotalAttempts).toStdString().c_str(), strProductName))
        settings->writeProfileInt("Settings", "OIVLicenseAttempt", 5 - nTotalAttempts);
      else
        settings->writeProfileInt("Settings", "OIVLicenseAttempt", 0);
      break;
      case 2:
      if (m.ShowRetryBox(pattern.arg(3).arg(nTotalAttempts).toStdString().c_str(), strProductName))
        settings->writeProfileInt("Settings", "OIVLicenseAttempt", 3);
      else
        settings->writeProfileInt("Settings", "OIVLicenseAttempt", 0);
      break;
      default:
      bOIV_retry = m.ShowRetryBox("Open Inventor licenses not found. Please contact local support.", strProductName);
      settings->writeProfileInt("Settings", "OIVLicenseAttempt", 0);
      break;
      }
    }
    } while (!bOIV_initialized && bOIV_retry);

    return bOIV_initialized;
  }
} //namespace oiv

namespace printer{
  void init()
  {
    Printer* printer_ = Printer::instance(Printer::App);

    std::string ver = gm::doc_::version(gm::doc_::App).replace("_build_", "_").toStdString();
    std::string url = QUtil::url(QUtil::eUsrDir::Goemec, "gm_" + ver + ".log");
    for( int i = 0; i < Printer::ModulesNum; i++)
      Printer::instance((Printer::eModule)i)->url( url );
  }
  void start()
  {
    Printer* printer_ = Printer::instance(Printer::App);

    printer_->info("");
    printer_->info("Geomec >> >>");
    printer_->info("");
  }
  void legend()
  {
    Printer* printer_ = Printer::instance(Printer::App);

    //
    // Legend not shown for non-activated modules
    //
    printer_->info( "--legend--" );
    for (int i = 0; i < Printer::ModulesNum; i++)
    {
      Printer* p = Printer::instance((Printer::eModule)i);
      std::pair<std::string, std::string>& t = p->m_cfg.modules_s_m.at((Printer::eModule)i);
      p->info( t.second.c_str() );
    }
    printer_->info("");
  }
} //namespace printer

namespace settings_{
  void init()
  {
    CSettingsIni::instance()->init();
  }
} //namespace settings

namespace global{

//
// mcr 2020-03-23 
//
// 'dsa': Diana Stand Alone
//
// global->dsa_available: 
// taken from config file ('geomec.ini'), section: 'Dsa', key: 'Available' -> enabled: 'Y', 'YES'
//
// global->dsa:
// to make diana run in a separate process: actiave it in 'Op3timizationSettings' dlg 
//
// run analysis:
//
//
//
// Printer for multiple processes:
//
// Enable it in 'logs.xml', creating adding '<multiprocess></multiprocess>'
//
// In run time not implemented to switch between one or multiple processes
//
// If one defined and multiple trying to write the same file: 
//
// Only first one ('Geomec') will achieve.
// It won't produce crash, only missing logs from other processes ('diana_app')
// 
//
// read config file (geomec.ini) and assign values to CGlobals vars
//
void read()
{
  _g->dsa_available(			settings::get("Dsa", "Available") );
  _g->dsa(					settings::get("Dsa", "ENABLED_BY_DEFAULT_IN_GUI") );

  _g->mfc(CGlobal::RunDiana,	settings::get("Dsa", "FORCE_MFC_DLG_RUNDIANA", true) );
  _g->mfc(CGlobal::Progress,	settings::get("Dsa", "FORCE_MFC_DLG_PROGRESS", true ) );
  //
  _g->qt_dlg_bg_color(		settings::get_s("Debug", "QT_DLG_BG_COLOR") );
}

} //namespace global

} //namespace gm

///////////////////////////////////////////////////////////////////////
//
//
//					CGeomecApp
//
//
///////////////////////////////////////////////////////////////////////
//##ModelId=3BAF4D1F0014
CGeomecApp::CGeomecApp()
: m_pQapp(0),
  m_pKeyFile(0)
{
  _g->gui_thread_id( std::this_thread::get_id() );

  // TODO: add construction code here,
  // Place all significant initialization in InitInstance

  //
  // Set logs file
  // one file per version
  //
  // 5.7.0.11_build_20041.1 -> 5.7.0.11_20041.1
  //
  // NOTE: 2020-07-22 mcr: why not just make url static instead of per instance? was it thought to use a file epr module? 
  //
  Printer::pid( QCoreApplication::applicationPid() );
  //
  //
  //
  gm::printer::init();
  gm::printer::start();
  gm::printer::legend();
  //
  printer->info("pid: %lld",Printer::pid());
  //
  CSettingsIni::instance()->init();
  CSettingsIni::instance()->print_();
  //
  CWindowsPlatform::init();
  //
  //_m()->msg("HELLO WORLD"); // add a unittest
  GlobalMessage::init( new GlobalMessage_GUI() );
  GlobalMessage::init_mfc( new GlobalMessage_GUI_MFC() );
  //
  EnableHtmlHelp();

    //
    // no need but useful to know
    //
  /*LPSTR cmd = GetCommandLine();

  std::ofstream file;
  file.open( std::string( getenv("APPDATA") ) + "\\geomec_params.txt" ); //C:\Users\Manel.CarreraRuibal\AppData\Roaming
  file << std::string( cmd ) << "\n";
  file.close();*/

  //
  // Don't use Printer before this line
  // Do it after creating CGeomecDoc in InitInstance 
  //

  //
  // FIXME: mcr 2020-03-10
  // Printer::multiprocess( QString( cmd ).contains( "pid", Qt::CaseInsensitive ) );
  // Printer::multiprocess( true );
  //

  // To simulate two processes
  // and debug firt step of run model: filos file creation
  /*Printer::multiprocess( true );
  CDianaStartUp* dsu = CDianaStartUp::instance();
  dsu->pid( 99 );
  dsu->dsa( true );*/
  //

  //printer->debug("params: '%s'", cmd ); //Geomec.exe  pid=6336
}

CGeomecApp::~CGeomecApp()
{

  if(m_pKeyFile) 
    delete m_pKeyFile;
  
  if(m_pQapp) 
    delete m_pQapp;

  if( m_TempPath )
    delete m_TempPath;

  CSettingsIni::instance()->update();

  printer->info("Geomec << <<");

  // so far no need: when 'geomec_launcher' crashes, 'diana_app' dies
  // solve first 'geomec_launcher' crash
  //if( CDianaStartUp::instance()->multiprocess() )
  //	QUtil::kill_process( "diana_app.exe" );
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CGeomecApp object

namespace Filos
{
  //
  // Define the DIANA Callbacks
  //
  extern "C"
  {

    void DebugPrint( const char *fmt, va_list args)
    {
      char buf[1024];
      _vsnprintf( buf, 1024, fmt, args );
      TRACE0( buf );
    }

    void Fatal( int erno, const char *errm, const char *proc, va_list args )
    {
      char buf[1024];
      _vsnprintf( buf, 1024, errm, args );
      TRACE0( buf );
      TRACE0( "\n" );
    }
  }
}


CGeomecApp theApp;


//
// not used
//
//#include <conio.h>
void EnablePrintfAtMFC()
{
  if (AttachConsole(ATTACH_PARENT_PROCESS))
  {
    FILE* pCout;
    freopen_s(&pCout, "CONOUT$", "w", stdout);
    std::cout.clear();
    std::wcout.clear();
  }
}

void set_env_MCSLMD_LICENSE_FILE( int& nTotalAttempts, int& nOIVLicenseAttempt )
{
    if (getenv("GM_MCSLMD_LICENSE_FILE"))
  setenv("MCSLMD_LICENSE_FILE", getenv("GM_MCSLMD_LICENSE_FILE"), 1);


  QStringList l = QString(getenv("MCSLMD_LICENSE_FILE")).split(';');
  QStringList europe_muntse;
  QStringList europe_others;
  QStringList others;
  QStringList servers;

  for (size_t i = 0; i < l.size(); ++i)
  {
  if (l[i].contains("europe", Qt::CaseInsensitive))
  {
      if (l[i].contains("muntse", Qt::CaseInsensitive))
    europe_muntse.append(l[i]);
      else
    europe_others.append(l[i]);
  }
  else
      others.append(l[i]);
  }

  if (europe_muntse.empty())
  ++nTotalAttempts;
  else
  servers.append(europe_muntse.join(';'));

  if (!europe_others.empty())
  servers.append(europe_others.join(';'));
  if (!others.empty())
  servers.append(others.join(';'));

  if (nOIVLicenseAttempt)
  {
  const QString europeServer = "3015@MUNTSE-S-09786.europe.shell.com";

  switch (nOIVLicenseAttempt)
  {
  case 2: // try to add Muntse
      servers.prepend(europeServer);
      /* Fall-through */
  case 1: // try reordering
      setenv("MCSLMD_LICENSE_FILE", servers.join(';').toStdString().c_str(), 1);
      break;
  case 3: // try only Muntse
      setenv("MCSLMD_LICENSE_FILE", europeServer.toStdString().c_str(), 1);
  default:
      // do nothing
      break;
  }
  }

}

//##ModelId=3BAF4D1F0015
BOOL CGeomecApp::InitInstance()
{
  // No need to do these twice: either here or later. But --version for Geomec.exe to console is SLOOOOOW, so probably don't use it
  //CCustomCmdLineInfo cmdInfo1;
  //ParseCommandLine(cmdInfo1);

  // trick write versionn to console (stdout?) and exit
  // needs to be tested
  /*if( cmdInfo1.m_version_request )
  {
  EnablePrintfAtMFC();
  printf("Hello world!\n");
  std::cout << "It works!" << endl;
  return FALSE;
  }*/

  //TODO: call AfxInitRichEdit2() to initialize richedit2 library.
   // disable reporting of memory leaks at the end of debug execution (takes ages)
   // comment this line out to get reports on memory leaks again
  ::AfxInitRichEdit2();
#ifdef _DEBUG
  AfxEnableMemoryTracking(FALSE);
#endif

  int argc = 0;
  char** argv = 0;
  m_pQapp = new QApplication(argc, argv); // Needed for QFont support

  //
  // next was used during 'ModGMBus' implementation of DSA
  // this implementation is no longer being used
  // 
  //qInstallMessageHandler( geomec::qdebug_msg_handler ); // -> geomec_qdebug.log

  EnableAutomation(); //use automation for DStabor etc..

  AfxEnableControlContainer();

#if LICENSING
  // =======================
  // FlexLM license checkout
  // =======================
  if( !settings::get("Debug", "DISABLE_FLEXLM") )
  {
    if( !gm::license::check() )
    return FALSE;
  }
#endif


  // Standard initialization
  // If you are not using these features and wish to reduce the size
  //  of your final executable, you should remove from the following
  //  the specific initialization routines you do not need.

#ifdef _AFXDLL
  Enable3dControls();			// Call this when using MFC in a shared DLL
#else
  Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
  if (!AfxOleInit())
  _m()->msg("Error");

  /*
   * This sets the company name in the settings path:
   * HKEY_CURRENT_USER\Software\<company name>\<application name>\
   *   <section name>\<value name>.
   */

  SetRegistryKey(_T("TNO"));

  ISettings* settings = ISettings::instance();

  settings->setRegistryKey(m_pszRegistryKey);

  // set the profile name for this version
  CString executable = m_pszExeName;
  executable += ".exe";
  LPTSTR path = new TCHAR[executable.GetLength()+1];
  _tcscpy(path, executable);

  CVersionInfo version;
  version.GetVersionInfo( path );
  delete path;

  // defines the settings key for this version, changes automatically with product version
  // for when it is installed next to a previous version

  CString strProductName;
  CString strBranchName = GM_BUILD_BRANCH_INFO;

  if (strBranchName.GetLength() > 0)
  {
  strBranchName.TrimRight();
  strBranchName = " " + strBranchName;
  }
  strProductName.Format("GEOMEC %u.%u%s", version.m_major, version.m_minor, strBranchName );

  // must be malloc'ed, because the MFC framework calls free on it
  free((void*) m_pszProfileName);
  m_pszProfileName = _tcsdup(strProductName);

  /*
   * This sets the application name in the settings path:
   * HKEY_CURRENT_USER\Software\<company name>\<application name>\
   *   <section name>\<value name>.
   */

  settings->setProfileName(m_pszProfileName);

  // FIXME: Some values need to be still migrated: bars, summary and Window size (even if multiple instances)
  // Migrate all always, it doesnt hurt
  CMigrateProfileSettings migrateProfileSettings;
  migrateProfileSettings.migrateProfileSettings( m_pszRegistryKey, m_pszProfileName, version.m_major, version.m_minor);

  CSplash* pSplash = new CSplash(IDB_SPLASH,4000,CSplash::KillOnClick);

  int nOIVLicenseAttempt = settings->getProfileInt("Settings", "OIVLicenseAttempt", 0);
  int nTotalAttempts = 2;

  // We've had so many problems with OIV license checking, and apparently roaming profiles of computers with different
  // GM versions (major.minor) can give us invalid environment settings, so we're just going to poke them in here
  setenv("OIV_LICENSE_FILE", (ExePath() + "\\password.dat").c_str(), 1);
#if defined(GM_BUILD_TYPE_RELEASE_NIGHTLY) || defined(GM_BUILD_TYPE_RELEASE)
  if (getenv("OIVHOME"))
  setenv("OIVHOME", "", 1);
  if (getenv("OIVARCH"))
  setenv("OIVARCH", "", 1);
#endif

  set_env_MCSLMD_LICENSE_FILE( nTotalAttempts, nOIVLicenseAttempt );

  if( !settings::get("Debug", "DISABLE_GUI") )
  {
    if( !gm::oiv::init( nTotalAttempts, nOIVLicenseAttempt, strProductName ) )
      return FALSE;
  }

  // Change extension for help file
  CString strHelpFile = m_pszHelpFilePath;
  strHelpFile.Replace(".HLP", ".chm");
  free((void*)m_pszHelpFilePath);
  m_pszHelpFilePath = _tcsdup(strHelpFile);

  // set location of help file in PATH variable so externally linked files
  // (e.g. pdfs) can be found
  int ipos = strHelpFile.ReverseFind('\\');
  if(ipos >= 0)
  {
  strHelpFile = strHelpFile.Left(ipos);
  CDianaStartUp* dsu = CDianaStartUp::instance();
  dsu->SetDianaEnv("PATH", dsu->GetDianaEnv("PATH") + ";" + QString( (LPCTSTR)strHelpFile) );
  }

  LoadStdProfileSettings();  // Load standard INI file options (including MRU)

  m_pKeyFile = new CKeyFile();

  // must be initialized before processing the shell commands
  m_TempPath = new CTempPath();
  //
  // needs to be done before creting the doc
  //
  TempPath()->ReadConfig();

  // to track the state when teh model is being run in a diffenet process
  // Register the application's document templates.  Document templates
  //  serve as the connection between documents, frame windows and views.

  CSingleDocTemplate* pDocTemplate;
  pDocTemplate = new CSingleDocTemplate(
  IDR_MAINFRAME,
  RUNTIME_CLASS(CGeomecDoc),
  RUNTIME_CLASS(CMainFrame),       // main SDI frame window
  RUNTIME_CLASS(COpenGLSplitView));
  AddDocTemplate(pDocTemplate);


  /////////////////////////////////////////////////////////////////////
  //
  //				globals + dsu
  //
  /////////////////////////////////////////////////////////////////////

  // section: "Dsa" / keys: "Available", "ENABLED_BY_DEFAULT_IN_GUI", "FORCE_MFC_DLG_RUNDIANA", "FORCE_MFC_DLG_PROGRESS"
  gm::global::read();


  //CDianaStartUp::TFunction function_cb = std::bind( &CGeomecDoc::set_dsa_icon, &GetDoc(), std::placeholders::_1 );
  //dsu->register_cb( function_cb );

  //printer->debug( "CGeomecApp : pid:%d", dsu->pid() );

  // Initialize a DIANA environment, so Filos library can work properly
  // Gets values in this order : 1)reg, 2)config file ... some values can be generated from 'DIAPATH' 
  // Then they are sedt to diana envvars system
  CDianaStartUp* dsu = CDianaStartUp::instance();
  dsu->SetDianaEnv();
  dsu->Print_DianaEnv();	// DIAPATH and the rest of Diana envvars are modified here
                          // Original values are saved in CSettings

  /*IProgressFactory* f = 
    _g->mfc(CGlobal::Progress) ? 
    new CProgressFactory_MFC : 
    new CProgressFactory;
  _g->prog(f);*/
  //
  // not running (load, for example): MFC ... running: by now only qt, since MFC cancel btn can't be pushed (FIXME)
  //
  _g->prog( new CProgressFactory_MFC );

  /////////////////////////////////////////////////////////////////////
  //
  //				command line
  //
  /////////////////////////////////////////////////////////////////////

  
  auto parse_args = [=](CCustomCmdLineInfo::Test& test) //FIXME
  {
    // Parse command line for standard shell commands, DDE, file open
    CCustomCmdLineInfo cmdInfo;
    ParseCommandLine(cmdInfo);

    // Dispatch commands specified on the command line
    if (cmdInfo.m_nShellCommand == CCommandLineInfo::FileOpen)
    {
    if(!ProcessShellOpenCommand(cmdInfo))
      return FALSE;
    }
    else
    {
    // Start with an empty project.
    if (!ProcessShellCommand(cmdInfo))
      return FALSE;
    }

    if (cmdInfo.Output())
    {
    GetDoc().SaveModel(cmdInfo.OutputFile());
    }

    if (cmdInfo.Exit())
    {
    exit(0);
    }

    test = cmdInfo.test();


    return TRUE;
  };
  CCustomCmdLineInfo::Test test={ false, "", "" };
  if( !parse_args(test) )
    return FALSE;

  if(test.enabled)
    _e->notify(make_cmd(eCmd::Test,new std::string(test.token)),CEvents::GuiApp_EH);

  /////////////////////////////////////////////////////////////////////
  //
  //				command line (end)
  //
  /////////////////////////////////////////////////////////////////////

  //InstallChangelog();
  gm::helper::plugins(); // IT + Plugins

  gm::helper::add_default_version_menu_item();

  // The one and only window has been initialized, so show and update it.
  m_pMainWnd->ShowWindow(SW_SHOW);
  m_pMainWnd->UpdateWindow();

  FemAppGetMainWnd(); // initialize

  // ProcessShellCommand succeeded, main frame parameters may be saved on exit
  ((CMainFrame *)m_pMainWnd)->CanSaveParametersOnExit(); // set to true

#ifndef _DEBUG
  // Disclaimer for test versions
//	_m()->msg("This is a pilot version of GEOMEC 3.2.\nFunctionalities are not fully tested.\nNo guarantee is given on results validity.");
#endif

  // CG: This line inserted by 'Tip of the Day' component.
//	ShowTipAtStartup();

  if(!test.enabled) // FIXME
  {
    CGeomecDoc* pDoc = &GetDoc();
    pDoc->changelog_dlg(
      gm::helper::changelog( (LPCTSTR)strProductName ) 
    );
  }

  GetDoc().set_dsa_icon(!_g->dsa_available() ? DSA_Icon::Empty :
  _g->dsa() ? DSA_Icon::_2P :
  DSA_Icon::_1P);
  
  return TRUE;
}

int CGeomecApp::ExitInstance()
{
  QString val = ISettings::instance()->getProfileString("Debug", "DISABLE_FLEXLM").toUpper();
  if (val != "Y")
  {
#if LICENSING
  LicenseData::DestroyInstance(); 
#endif
  }

  return CWinApp::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

//##ModelId=3BAF4D200207
class CAboutDlg : public CDialog
{
public:
  //##ModelId=3BAF4D200228
  CAboutDlg();
 
// Dialog Data
  //{{AFX_DATA(CAboutDlg)
  enum { IDD = IDD_ABOUTBOX };
  //}}AFX_DATA

  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CAboutDlg)
  protected:
  //##ModelId=3BAF4D200229
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:
  //{{AFX_MSG(CAboutDlg)
  //##ModelId=3C74E3FD02EE
  afx_msg void OnChangelog();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

void CAboutDlg::OnChangelog()
{ 
  ChangelogDlg dlg;

  if( dlg.verify() )
    dlg.exec();
  else
  {
    AfxMessageBox( "No changelog data found", MB_ICONERROR );
    printer->error( "changelog : no data found" );
  }
};

//##ModelId=3BAF4D200228
CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
  //{{AFX_DATA_INIT(CAboutDlg)
  //}}AFX_DATA_INIT
}

//##ModelId=3BAF4D200229
void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
  CString str;
  str = "geomec.exe";
  CVersionInfo *version = new CVersionInfo();
  version->GetVersionInfo((char*)LPCTSTR(str));
  str.Format("Geomec Version : %s", version->m_strFullVersion);
  delete version;

  CString strEdition;
#ifdef _WIN64
  strEdition = getStringTableEntry(IDS_64_EDITION);
#else
  strEdition = getStringTableEntry(IDS_32_EDITION);
#endif

  str += " " + strEdition;

  ((CStatic*)GetDlgItem(IDC_VERSION))->SetWindowText(str);

  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CAboutDlg)
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
  //{{AFX_MSG_MAP(CAboutDlg)
  ON_BN_CLICKED(IDC_CHANGELOG, OnChangelog)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
//##ModelId=3BAF4D1F0017
void CGeomecApp::OnAppAbout()
{
  CAboutDlg aboutDlg;
  aboutDlg.DoModal();

  // Blocks all events !!!! So dont let it open for ever if running a model
  //QAboutDlg dlg;
  //dlg.exec();
}

/////////////////////////////////////////////////////////////////////////////
// CGeomecApp message handlers


//##ModelId=3C74E3F601E4
CGeomecDoc& CGeomecApp::GetDoc() const
{
  POSITION pos = AfxGetApp()->GetFirstDocTemplatePosition();
  CDocTemplate *pDocTemplate = AfxGetApp()->GetNextDocTemplate(pos);
  POSITION docpos = pDocTemplate->GetFirstDocPosition();
  return *(CGeomecDoc*)(pDocTemplate->GetNextDoc(docpos));
}

void CGeomecApp::ShowTipAtStartup(void)
{
  // CG: This function added by 'Tip of the Day' component.

  CCommandLineInfo cmdInfo;
  ParseCommandLine(cmdInfo);
  if (cmdInfo.m_bShowSplash)
  {
    CTipDlg dlg;
    if (dlg.m_bStartup)
      dlg.DoModal();
  }

}

void CGeomecApp::ShowTipOfTheDay(void)
{
  // CG: This function added by 'Tip of the Day' component.

  CTipDlg dlg;
  dlg.DoModal();

}

// Global function to retrieve a pointer to the global WinApp derived object
CGeomecApp *GetGeomecApp()
{
  return (CGeomecApp *) AfxGetApp();
}

BOOL CGeomecApp::OnCmdMsg(unsigned int nID, int nCode, void *pExtra, AFX_CMDHANDLERINFO *pHandlerInfo)
{
  if(nCode != CN_EVENT) // don't do this for ActiveX events
  {
    CGeomecDoc *pDoc = &GetDoc();
    if(pDoc)
    {
      POSITION pos = pDoc->GetFirstViewPosition();
      while(pos)
      {
        CView *pView = pDoc->GetNextView(pos);
        if(pView) if(pView->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo)) return TRUE;
      }
    }
  }

  return CTnoWinApp::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

CTempPath* CGeomecApp::TempPath()
{
  return m_TempPath;
}

const CKeyFile &CGeomecApp::KeyFile() const
{
  return *m_pKeyFile;
}

BOOL CGeomecApp::ProcessShellOpenCommand(CCommandLineInfo& cmdInfo)
{
  // Because there is something wrong with the load structure of Geomec, first an empty
  // project will be created so all the windows are created and after that the project is loaded.
  CString strFile = cmdInfo.m_strFileName;
  cmdInfo.m_strFileName = "";
  cmdInfo.m_nShellCommand = CCommandLineInfo::FileNew;
  if (!ProcessShellCommand(cmdInfo))
    return FALSE;

  // MFC try/catch
  TRY
  {
  cmdInfo.m_strFileName = strFile;
  cmdInfo.m_nShellCommand = CCommandLineInfo::FileOpen;
  if (!ProcessShellCommand(cmdInfo))
      return FALSE;
  }
  CATCH (CInvalidArgException, pException)
  {
  // we have no good way to recover (yet)
  char msg[255];
  pException->GetErrorMessage(msg, 254);
  strcat(msg, " Exiting Geomec.");
  _m()->msg(msg);
  exit(-1);
  }
  END_CATCH

  return TRUE;
}
