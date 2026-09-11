// #include <QtGui/QGuiApplication>
// #include <QtCore/QProcess>

#include <QGuiApplication>
#include <QProcess>

#include "ModGMBus.h"
#include <iostream>

#include "ProcessManager.h"

#include "Printer.h"
#include "QUtil.h"

#include "GlobalMessage.h"
#include "GlobalMessage_CLI.h"

#include "SettingsIni.h"

#include "LauncherBusClient.h"

#include <QDateTime>
#include <QFile>
#include <QProcessEnvironment>

#ifdef _DEBUG
#define MAX_BACKEND_PROCESSES 1
#else
#define MAX_BACKEND_PROCESSES 1
#endif

namespace {
// static QString PRINTER_LOGS_FILE = QString::fromStdString( Printer::DEFAULT_FOLDER ) + "/geomec.log";
}

// global in case we want to (test the) use the CtrlHandler

ProcessManager *procMan = nullptr;

#if 0
BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
  printf("got %d\n", fdwCtrlType);
  if (procMan)
  procMan->stop();
  switch (fdwCtrlType)
  {
  // Handle the CTRL-C signal. 
  case CTRL_C_EVENT:
  //printf("Ctrl-C event\n\n");
  //Beep(750, 300);
  return TRUE;

  // CTRL-CLOSE: confirm that the user wants to exit. 
  case CTRL_CLOSE_EVENT:
  //Beep(600, 200);
  //printf("Ctrl-Close event\n\n");
  return TRUE;

  // Pass other signals to the next handler. 
  case CTRL_BREAK_EVENT:
  //Beep(900, 200);
  //printf("Ctrl-Break event\n\n");
  return TRUE;

  case CTRL_LOGOFF_EVENT:
  //Beep(1000, 200);
  //printf("Ctrl-Logoff event\n\n");
  return FALSE;

  case CTRL_SHUTDOWN_EVENT:
  //Beep(750, 500);
  //printf("Ctrl-Shutdown event\n\n");
  return FALSE;

  default:
  return FALSE;
  }
}
#endif

// we can use this to suppress qDebug output, but it won't suppress Qt internal messages
// for that we need preprocessor definitions
void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg) {}

// QString LOGS_FILE = QString::fromStdString( Printer::DEFAULT_FOLDER ) + "/geomec_qdebug.log";
// QFile g_qdebug_out_file( LOGS_FILE );
// QFile g_qdebug_out_file;
void qdebug_msg_handler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
  std::string url = Printer::instance()->folder() + "\\" + "geomec_qdebug.log";
  QFile g_qdebug_out_file(url.c_str());

  QString txt;

  // QString s_time = QDateTime::currentDateTime().toTimeSpec(Qt::OffsetFromUTC).toString(Qt::ISODate);
  QString s_time = QDateTime::currentDateTime().toString("yyyy-MM-dd, hh:mm:ss");

  switch (type) {
  // (%s:%u, %s)
  // context.file, context.line, context.function
  case QtDebugMsg:
    txt = QString("Debug: %1: %2").arg(s_time, msg);
    break;
  case QtWarningMsg:
    txt = QString("Warning: %1").arg(msg);
    break;
  case QtCriticalMsg:
    txt = QString("Critical: %1").arg(msg);
    break;
  case QtFatalMsg:
    txt = QString("Fatal: %1").arg(msg);
    break;
  }

  g_qdebug_out_file.open(QIODevice::WriteOnly | QIODevice::Append);
  QTextStream ts(&g_qdebug_out_file);
  ts << txt << endl;
  g_qdebug_out_file.close(); // ????
}

/*void pid_to_file( quint64 pid )
{
  QString filename = QString::fromStdString( Printer::instance()->folder() + "/pid.txt" );
  QFile file( filename );
  if ( file.open(QIODevice::ReadWrite) )
  {
    QTextStream stream( &file );
    stream << pid << endl;
  }
  Printer::instance()->info( "ProcessManager > main [file:%s pid=%d]", file.fileName().toStdString().c_str(), pid );
}*/

void token_to_file(quint64 pid, std::string file_) {
  QString filename = QString::fromStdString(Printer::instance()->folder() + "/" + file_);
  QFile file(filename);
  if (file.open(QIODevice::ReadWrite)) {
    QTextStream stream(&file);
    stream << pid << endl;
  }
  Printer::instance()->info("ProcessManager > main [file:%s pid=%d]", file.fileName().toStdString().c_str(), pid);
}

//
// mcr 2020-02-22
//
// Linking errors:
//
// 1) adding GlobalMessage and GlobalMessage_CLI it doest link
//
// 2) now new classes added to QUtil can't be found:
//
// -SettingsFile
// -Environment
// -TempPath
//
int main(int argc, char *argv[]) {
  QGuiApplication a(argc, argv);

  GlobalMessage::init(new GlobalMessage_CLI());

  CSettingsIni::instance()->init();

  // Printer::multiprocess( true );
  std::string filename = "gm_" + CSettingsIni::instance()->version_str().toStdString() + ".log";
  Printer::instance()->url(QUtil::url(QUtil::eUsrDir::Goemec, filename));
  Printer::instance()->info("");
  Printer::instance()->info("gm_launcher > main");

  // Printer::instance()->file("geomec_pm.log");

  // no need, default is Ok
  // Printer::instance()->file( PRINTER_LOGS_FILE.toStdString() );
  Printer::instance()->info("%s", argv[0]);

  // Printer::instance()->info("");
  // Printer::instance()->info("ProcessManager > main");

  for (int i = 0; i < argc; ++i)
    Printer::instance()->debug("arg: %s", argv[i]);

  // BOOTSTRAP 1: set control handlers and get relevant input

  // SetConsoleCtrlHandler(CtrlHandler, TRUE);

  // No messages
  // qInstallMessageHandler(myMessageOutput);

  // MCR 2019-04-18: This makes app randomly crash
  qInstallMessageHandler(qdebug_msg_handler);

  quint64 pid = a.applicationPid();

  // not used any more, now pid is passed as a paremeter
  // pid_to_file( pid );
  // QUtil::token_to_file( pid, "pid.txt" );

  // BOOTSTRAP 2: read configuration and set up process manager
  // (we pre-define instead of reading any configuration)
  //    ModGMBackend just sends a heartbeat number (or time, based on what is compiled)
  //    With 20+ of them we can get impression of bus throughput
  //    The duplicate processes are currently distinguished by the :... behind the program name,
  //    which becomes its interface/service name
  //    All the heartbeats end up in the ModGMGui (which exposes its "Gui" interface/service)
  //    Multiple Gui's can be started, in case they would need different interface/service names as well
  //
  // Note: we have yet to determine if we will allow user-configurations

  for (auto process : {DIANA_BIN, GEOMEC_BIN})
    QUtil::kill_process(process.toStdString());

  procMan = new ProcessManager(pid);

  for (auto process : {GEOMEC_BIN, LAUNCHER_BIN})
    procMan->RegisterProcess(process, true);

  QString name(LAUNCHER_ID);
  QString base = ModGMLocalBusHelper::getBase(pid);

  LauncherBusClient *client = new LauncherBusClient(
      base, name); // this_->m_qapp -> no difference passing the parent in terms of events handling -> so don't pass it
  client->m_pm = procMan;

  QTimer::singleShot(0, client, SLOT(setup())); // needed to rergister the client into the bus
  // QApplication::instance()->processEvents();

  // BOOTSTRAP 3: set up listener, bus and connect them to each other as well as process manager

  ModGMLocalBusManager server(ModGMLocalBusHelper::getBase(pid), procMan, &a);

  QObject::connect(&server, &ModGMLocalBusManager::quit, procMan, &ProcessManager::stop);
  QObject::connect(&a, &QGuiApplication::aboutToQuit, procMan, &ProcessManager::stop);
  QObject::connect(procMan, &ProcessManager::quit, &a, &QGuiApplication::quit);

  // BOOTSTRAP DONE
  // start the processes and run the event loop

  // procMan->start();

  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

  QString key = "Path";
  QString path = env.value(key);
  env.remove(key);
  path += ";C:\\aWork\\aFlexLM\\lib64";
  env.insert(key, path);

  QStringList args = QStringList{QString("pid=%1").arg(pid)};
  if (argc > 1)
    args.append(QString("%s").arg(argv[1]));

  procMan->start(GEOMEC_BIN, args, &env);

  return a.exec();
}