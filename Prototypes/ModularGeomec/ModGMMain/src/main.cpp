#include <QtCore/QProcess>
#include <QtGui/QGuiApplication>

#include "ModGMBus.h"
#include "ProcessManager.h"
#include <iostream>

#ifdef _DEBUG
#define MAX_BACKEND_PROCESSES 1
#else
#define MAX_BACKEND_PROCESSES 1
#endif

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

int main(int argc, char *argv[]) {
  QGuiApplication a(argc, argv);

  // BOOTSTRAP 1: set control handlers and get relevant input

  // SetConsoleCtrlHandler(CtrlHandler, TRUE);
  // qInstallMessageHandler(myMessageOutput);

  quint64 pid = a.applicationPid();

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

  procMan = new ProcessManager(pid);

  for (int i = 0; i < MAX_BACKEND_PROCESSES; ++i) {
    QString p = QString("ModGMBackend.exe:Bcknd%1").arg(i);
    procMan->RegisterProcess(p, false);
  }
  procMan->RegisterProcess("ModGMGui.exe", true);

  // BOOTSTRAP 3: set up listener, bus and connect them to each other as well as process manager

  ModGMLocalBusManager server(ModGMLocalBusHelper::getBase(pid), procMan, &a);

  QObject::connect(&server, &ModGMLocalBusManager::quit, procMan, &ProcessManager::stop);
  QObject::connect(&a, &QGuiApplication::aboutToQuit, procMan, &ProcessManager::stop);
  QObject::connect(procMan, &ProcessManager::quit, &a, &QGuiApplication::quit);

  // BOOTSTRAP DONE
  // start the processes and run the event loop

  procMan->start();

  return a.exec();
}