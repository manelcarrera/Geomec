#include "ModGMGui.h"
#include <QtWidgets/QApplication>
#include <QtCore/QTimer>

#include "ModGMBus.h"



int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  ModGMGui w;

  // default interface name is Gui; unlikely to be changed through arguments

  QString name("Gui");

  for (int i = 1; i < argc; ++i)
  {
      QStringList l = QString(argv[i]).split('=');
      if (l.size() == 2 && l[0] == "name")
    name = l[1];
  }


  // client

  quint64 ppid = ModGMLocalBusHelper::getPid(argc, argv);

  ModGMLocalBusClient client(ModGMLocalBusHelper::getBase(ppid), name, &a);

  // we read messages from the backend, show errors, and we can send a command to all other processes (see form)

  QObject::connect(&w, &ModGMGui::finished, &client, &ModGMLocalBusClient::quit);
  QObject::connect(&client, SIGNAL(finished()), &a, SLOT(quit()));

  QObject::connect(&client, &ModGMLocalBusClient::receivedMessage, &w, &ModGMGui::receivedMessage);
  QObject::connect(&client, &ModGMLocalBusClient::shmError, &w, &ModGMGui::onReceivedError);
  QObject::connect(&w, &ModGMGui::handleMessage, &client, &ModGMLocalBusClient::handleMessage);


  // call setup slot, show window and start event loop
  QTimer::singleShot(0, &client, SLOT(setup()));

  w.show();

  return a.exec();
}
