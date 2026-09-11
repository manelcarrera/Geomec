#include <QtCore/QCoreApplication>
#include <QtCore/QTimer>

//#include "HeartbeatClient.h"
#include "DianaBusClient.h"

#include "ModGMBus.h"





#include "DianaRunController.h"
#include "DianaXWrapper.h"

//#include "DianaRunnerImpl.h"

#include "IRetrieveDianaFileNames.h"
#include "ISaveModel.h"

#include <QApplication>
//#include <qapplication.h>

#include <iostream>

#include "Classes.h"
#include "SaveModel.h"
#include "RetrieveDianaFileNames.h"

//#include "ExecuteDianaDialogQt.h"

#include "Printer.h"

/*int main( int argc, char *argv[] )
{
	//int k;
	//std::cin >> k;

	CModelBase model;
	const CAnalysisType analysis;
	bool bWriteInputFiles = true;
	bool bWriteQuadDat = true;
	bool bWriteOutputStreamFile = true;
	const QString strTempPath = "tmp_path";

	QApplication* qapp = new QApplication( argc, argv ); // Needed

	CDianaRunController controller(	model,
									analysis,
									bWriteInputFiles,
									bWriteQuadDat,
									bWriteOutputStreamFile,
									strTempPath );


	//------------------------

	const QString getPathName = "path_name";
	CExecuteDianaDialogQt* exe_diana_dlg = new CExecuteDianaDialogQt();
	CDianaXWrapper* dianaXWrapper = new CDianaXWrapper( exe_diana_dlg );
	const std::string title = "title";
	CSaveModel saveModel;
	CRetrieveDianaFileNames retrieveDianaFileNames;
	dia::IAnalysisStatusContainer *pAnalysisStatusContainer = 0;
	bool bSilent = false;

	controller.Run(	getPathName,
					dianaXWrapper,
					title,
					saveModel,
					retrieveDianaFileNames,
					pAnalysisStatusContainer,
					bSilent);

	return 0;
}*/

//#include <QtCore\QtDebug>
//#include <QtCore\QDebug>
#include <QDebug>
//#include <QtCore\QtGlobal>
#include <QtGlobal>
#include <QFile>
#include <QDateTime>

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
	Printer::multiprocess( true );

  //Printer::instance()->file("geomec_diana.log");
  Printer::instance()->file("geomec.log");
  Printer::instance()->info("%s", argv[0]);

  // TODO: run diana_app directly, not by the geomec_launcher and its processes manager
  /*
  QString id_s;
  if( argc > 1 )
  {
	  id_s = argv[ 1 ];
  }

	//QString path = "C:\\Users\\MANEL~1.CAR\\AppData\\Local\\Temp\\dra31952";
	QString path = "C:\\Users\\MANEL~1.CAR\\AppData\\Local\\Temp\\dra"+id_s;
	DianaBusClient::run_diana( path );
	return 0;
	*/

//#ifdef KK

  QCoreApplication a(argc, argv);

  g_qdebug_out_file.setFileName( QString::fromStdString( Printer::instance()->folder() +  "/geomec_qdebug.log" ) );
  qInstallMessageHandler( qdebug_msg_handler );

  quint64 ppid = ModGMLocalBusHelper::getPid(argc, argv);

  Printer::instance()->info("");
  

  // if we're started from ModGMMain, our interface name is sent as argument
  QString name("Diana");

  for (int i = 1; i < argc; ++i)
  {
    QStringList l = QString(argv[i]).split('=');
    if (l.size() == 2 && l[0] == "name")
      name = l[1];
  }
  Printer::instance()->info(" [DianaApp] > main [pid:%d name:%s]", ppid, name.toStdString().c_str() );

  // set up the client and call the setup-slot
  //HeartbeatClient client(ModGMLocalBusHelper::getBase(ppid), name, &a);
  DianaBusClient client(ModGMLocalBusHelper::getBase(ppid), name, &a);

  QObject::connect(&client, SIGNAL(finished()), &a, SLOT(quit()), Qt::QueuedConnection);
  QTimer::singleShot(0, &client, SLOT(setup()));

  // and then start the heartbeat messages after 5 seconds
  //QTimer::singleShot(5000, &client, SLOT(heartbeat_setup()));

  //QTimer::singleShot( 0, &client, SLOT( setup_() ) );

  return a.exec();

//#endif
}