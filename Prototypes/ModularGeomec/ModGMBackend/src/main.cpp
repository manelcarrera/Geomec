#include <QtCore/QCoreApplication>
#include <QtCore/QTimer>

#include "HeartbeatClient.h"
#include "DianaClient.h"

int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);

  quint64 ppid = ModGMLocalBusHelper::getPid(argc, argv);

  // if we're started from ModGMMain, our interface name is sent as argument
  QString name("Backend");

  for (int i = 1; i < argc; ++i)
  {
    QStringList l = QString(argv[i]).split('=');
    if (l.size() == 2 && l[0] == "name")
      name = l[1];
  }

  // set up the client and call the setup-slot
  //HeartbeatClient client(ModGMLocalBusHelper::getBase(ppid), name, &a);
  DianaClient client(ModGMLocalBusHelper::getBase(ppid), name, &a);

  QObject::connect(&client, SIGNAL(finished()), &a, SLOT(quit()), Qt::QueuedConnection);
  QTimer::singleShot(0, &client, SLOT(setup()));

  // and then start the heartbeat messages after 5 seconds
  //QTimer::singleShot(5000, &client, SLOT(heartbeat_setup()));
  QTimer::singleShot(5000, &client, SLOT(diana_setup()));

  return a.exec();
}

