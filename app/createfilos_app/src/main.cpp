#include <QtCore/QCoreApplication>
#include <QtCore/QTimer>

//#include "HeartbeatClient.h"
#include "CreateFilosBusClient.h"

#include "ModGMBus.h"

#include "ModelBase.h"
#include "TetraModel.h"

#include <QApplication>
#include <QDebug>
#include <QtGlobal>
#include <QFile>
#include <QDateTime>

#include <iostream>

#include "Printer.h"


//QString LOGS_FILE = "C:/Users/Manel.CarreraRuibal/AppData/Roaming/geomec_qdebug.log";
//QString LOGS_FILE = QString::fromStdString( Printer::DEFAULT_FOLDER ) + "/geomec_qdebug.log";
QFile g_qdebug_out_file;
void qdebug_msg_handler( QtMsgType type, const QMessageLogContext & context, const QString & msg )
{
    QString txt;

	//QString s_time = QDateTime::currentDateTime().toTimeSpec(Qt::OffsetFromUTC).toString(Qt::ISODate);
	QString s_time = QDateTime::currentDateTime().toString("yyyy-MM-dd, hh:mm:ss");

    switch (type) 
	{
		// (%s:%u, %s)
		// context.file, context.line, context.function
		case QtDebugMsg:	txt = QString("Debug: %1: %2").arg( s_time, msg); break;
		case QtWarningMsg:	txt = QString("Warning: %1").arg(msg); break;
		case QtCriticalMsg: txt = QString("Critical: %1").arg(msg); break;
		case QtFatalMsg:	txt = QString("Fatal: %1").arg(msg); break;
    }
    
    g_qdebug_out_file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts( &g_qdebug_out_file );
    ts << txt << endl;
	g_qdebug_out_file.close(); // ????
}


int main(int argc, char *argv[])
{
  //Printer::instance()->file("geomec_diana.log");
  Printer::instance()->file("geomec.log");

  QCoreApplication a(argc, argv);

  g_qdebug_out_file.setFileName( QString::fromStdString( Printer::instance()->folder() +  "/geomec_qdebug.log" ) );
  qInstallMessageHandler( qdebug_msg_handler );

	void* memory_chunck = nullptr;
	//CModelBase *pModel = reinterpret_cast< CModelBase * >( memory_chunck ); 
	CTetraModel *pModel = reinterpret_cast< CTetraModel* >( memory_chunck ); 

	int b=0;

  quint64 ppid = ModGMLocalBusHelper::getPid(argc, argv);

  Printer::instance()->info("");
  

  // if we're started from ModGMMain, our interface name is sent as argument
  QString name("CreateFilos");

  for (int i = 1; i < argc; ++i)
  {
    QStringList l = QString(argv[i]).split('=');
    if (l.size() == 2 && l[0] == "name")
      name = l[1];
  }
  Printer::instance()->info("CreateFilosApp > main [pid:%d name:%s]", ppid, name.toStdString().c_str() );

  // set up the client and call the setup-slot
  //HeartbeatClient client(ModGMLocalBusHelper::getBase(ppid), name, &a);
  CreateFilosBusClient client(ModGMLocalBusHelper::getBase(ppid), name, &a);

  QObject::connect(&client, SIGNAL(finished()), &a, SLOT(quit()), Qt::QueuedConnection);
  QTimer::singleShot(0, &client, SLOT(setup()));

  // and then start the heartbeat messages after 5 seconds
  //QTimer::singleShot(5000, &client, SLOT(heartbeat_setup()));

  //QTimer::singleShot( 0, &client, SLOT( setup_() ) );


  return a.exec();

}