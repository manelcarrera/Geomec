#include "stdafx.h"

#include "dependencies.h"
#include "receiver.h"
#include "consumer.h"

#include <QtCore/QMap>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QDateTime>
#include <QtCore/QThread>
#include <QtCore/QCoreApplication>

static const QString DefaultLogFile = "C:\\dev\\dependencies.txt";

static const QList< QString > DefaultExcludeProjects = QList< QString >() << "DSealect" << "tbbTachyon" << "tachyon";


void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
  QString txt;
  //QString moment = QDateTime::currentDateTime().toTimeSpec(Qt::OffsetFromUTC).toString(Qt::ISODate);
  //txt = QString("%1 %2").arg(moment,msg);
  txt = QString("%2").arg(msg);
  QFile outFile( DefaultLogFile );
  outFile.open(QIODevice::WriteOnly | QIODevice::Append);
  QTextStream ts(&outFile);
  ts << txt << endl;
}

int _tmain(int argc, char* argv[])
{
  QCoreApplication a(argc, argv);


  QTime myTimer;
  myTimer.start();
  
  qInstallMessageHandler(myMessageOutput); 
  
  QString moment = QDateTime::currentDateTime().toTimeSpec(Qt::OffsetFromUTC).toString(Qt::ISODate);
  qDebug() << QString(); 
  qDebug() << moment << "Dependencies->";

  QMap< QString, CDependencies::CProject > projects;

  CDependencies d;
  d.run( DefaultProjetPath, DefaultExcludeProjects );


  CThread _thread; // mutex & condition vars to synchro producer and consumer

  CConsumer consumer( _thread, d );
  consumer.start();

  CReceiver receiver( _thread );
  receiver.listen();

  qDebug().noquote() << "Elapsed time 1:" << myTimer.elapsed()/1000;


  /*qDebug().noquote() << "Elapsed time 1:" << myTimer.elapsed()/1000;

  d.print_results(	CDependencies::PRINT_PROJECT );
  
  //d.print_results(	CDependencies::PRINT_NOT_FOUND );
  
  //d.print_results();

  qDebug().noquote() << QString("Dependent projects:"); 
  qDebug().noquote() << QString("==================="); 
  d.new_print_results( CDependencies::CProject::ProjectsType::Dependent );
  qDebug().noquote() << QString(); 
  qDebug().noquote() << QString("Linked-by projects:"); 
  qDebug().noquote() << QString("==================="); 
  d.new_print_results( CDependencies::CProject::ProjectsType::LikedBy );
  qDebug().noquote() << QString(); 
  qDebug().noquote() << QString("Dependencies not found:"); 
  qDebug().noquote() << QString("======================="); 
  d.new_print_results( CDependencies::CProject::ProjectsType::DependenciesNotFound );
  qDebug().noquote() << QString(); 
  qDebug().noquote() << QString("Parsing errors:"); 
  qDebug().noquote() << QString("==============="); 
  for( int i=0 ; i < d.m_parsing_errors.size() ; i++ )
    qDebug().noquote() << i << d.m_parsing_errors[ i ]; 

  qDebug().noquote() << "Elapsed time 2:" << myTimer.elapsed()/1000;*/



  //d.print_data( "GeomecModel", CDependencies::DataType::Headers );
  //qDebug() << QString("DerivedResult.h") << ":" << d.find_dependent_project( QString("DerivedResult.h") );

  //qDebug() << "number of projects:" << projects.size();

  return a.exec();
}

