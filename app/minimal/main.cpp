#include <QtCore/QCoreApplication>

#include "NumRangeValidator.h"

#include <QDebug>


int main(int argc, char *argv[])
{
  NumRangeValidator nrv;

  qDebug() << "qDebug";

  QCoreApplication a(argc, argv);
  return a.exec();
}