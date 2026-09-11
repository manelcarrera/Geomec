#include <QtCore/QCoreApplication>

#include <QDebug>

#include "GlobalMessage_CLI.h"
#include "GlobalMessage.h"
#include "Printer.h"

int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);

  qDebug() << "qDebug";
  GlobalMessage::init( new GlobalMessage_CLI() );
  GlobalMessage::instance()->msg("GlobalMessage");
  Printer::instance()->debug("Printer");

  return a.exec();
}