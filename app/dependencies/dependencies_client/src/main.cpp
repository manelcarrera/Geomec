#include "sender.h"
#include <QtCore/QThread>

#include <QtCore/QCoreApplication>

int main(int argc, char *argv[]) {
  QCoreApplication a(argc, argv);

  int i = 0;

  CSender sender;

  QString cmd = argv[1];

  sender.connect(cmd); // FIXME

  // sender.send_cmd();

  return a.exec();
}