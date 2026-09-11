#include "DianaBusClient.h"

#include "DianaStartUp.h"
#include "lbcp.h"

#include "Printer.h"

#include "DianaXWrapper.h"

#include "util_.h" //get_last_line

#include <QBuffer>
//#include <QImage>

#include <QDir>
#include <QProcess>
#include <QApplication>


#include <iostream>
//#include <fstream>
#include <cctype>
#include <string>

#include <thread>

namespace
{

// FIXME:
//static const QString DIANA_BIN_HC = "C:/Program Files/GEOMEC 5.7 - SVS/Diana";

static const QString END_1 = "/DIANA/DC/END";
static const QString END_2 = "STOP";

static const QString GM42_BIN = "..\\binseg\\ap\\gm42.exe";

void print_env()
{
	//diana
	char* DIAPATH = DiGetenv("DIAPATH");
	char* DIASHARE = DiGetenv("DIASHARE");
	char* DIALIB = DiGetenv("DIALIB");
	char* FFDIR = DiGetenv("FFDIR");
	char* STP0 = DiGetenv("STP0");
	//model
	char* FF = DiGetenv("FF");


	Printer::instance()->debug("[Diana]BC [PRINT]");
	//diana
	Printer::instance()->debug("DIAPATH: %s", DIAPATH );
	Printer::instance()->debug("DIASHARE: %s", DIASHARE );
	Printer::instance()->debug("DIALIB: %s", DIALIB );
	Printer::instance()->debug("FFDIR: %s", FFDIR );
	Printer::instance()->debug("STP0: %s", STP0 );
	//model
	Printer::instance()->debug("FF: %s", FF );
}

void set_diana_env()
{
	// quick copy from GeomecModel/src/DianaEnv.cpp
	// DIASLIB is responsible for errors about non-existing element types
	// DIALIB was not set in call from command line, so maybe not needed
	// DIAERRPATH was not needed when DIASLIB was set

	// 1. Diana ENVVARS

	QString DiaPath_ = qApp->applicationDirPath() + "/Diana";

	//Printer::instance()->debug(" [Diana]BC : set_diana_env : DiaPath_:%s", DiaPath_.toStdString().c_str() );

	//QString DiaPath_ = DIANA_BIN_HC;
	QString DiaShare_ = DiaPath_ + "/share";
	QString DiaLib_ = DiaPath_ + "/lib";
	QString DiaSLib_ = DiaShare_ + "/lib";

	QString FFDIR	= DiaPath_ + "/bin";
	QString STP0	= DiaPath_ + "/binseg/ap/mc41.exe";

	int res;
	res = vDiSetenv("%s=%s", "DIAPATH", DiaPath_.toStdString().c_str());
	res = vDiSetenv("%s=%s", "DIASHARE", DiaShare_.toStdString().c_str());
	res = vDiSetenv("%s=%s", "DIALIB", DiaLib_.toStdString().c_str());
	res = vDiSetenv("%s=%s", "DIASLIB", DiaSLib_.toStdString().c_str());

	res = vDiSetenv("%s=%s", "FFDIR",           FFDIR.toStdString().c_str() );

	//res = vDiSetenv("%s=%s", "FFDIR",           ".");
	res = vDiSetenv("%s=%s", "STP0", STP0.toStdString().c_str() );


	// print
	/*Printer::instance()->debug("[Diana]BC [SET]");
	Printer::instance()->debug("DIAPATH: %s", DiaPath_.toStdString().c_str() );
	Printer::instance()->debug("DIASHARE: %s", DiaShare_.toStdString().c_str() );
	Printer::instance()->debug("DIALIB: %s", DiaLib_.toStdString().c_str() );
	Printer::instance()->debug("DIASLIB: %s", DiaSLib_.toStdString().c_str() );
	Printer::instance()->debug("FFDIR: %s", FFDIR.toStdString().c_str() );
	Printer::instance()->debug("STP0: %s", STP0.toStdString().c_str() );*/
}

void set_model_env( const QString& path )
{
	int pos = path.lastIndexOf(QChar('/'));

	QString TEMP	= path.left( pos ); // Ej.: C:\Users\MANEL~1.CAR\AppData\Local\Temp\dra07288

	QString id = path.right( 5 );

	QString FF		= QString::asprintf( "%s\\ffa%s.ff", TEMP.toStdString().c_str(), id.toStdString().c_str() );

	int res = vDiSetenv("%s=%s", "FF",  FF.toStdString().c_str() );

	Printer::instance()->debug("FF:%s", FF.toStdString().c_str() );
}

} // namespace


DianaBusClient::DianaBusClient(const QString& base, const QString& name, QObject *parent)
  : ModGMLocalBusClient(base, name, parent), m_diana_running( false )
{
	Printer::instance()->info( " [Diana]BC > Constructor" );

	connect(this, &DianaBusClient::receivedMessage, this, &DianaBusClient::onReceivedMessage);

	connect( &m_process, SIGNAL( readyReadStandardOutput() ), this, SLOT( read_output() ) );
	connect( &m_process, SIGNAL( readyReadStandardError() ), this, SLOT( read_output() ) );
}

DianaBusClient::~DianaBusClient(){}

// protect m_msg with a mutex
void DianaBusClient::send_keep_alive()
{
	while( m_diana_running )
	{
		//Printer::instance()->info( " [Diana]BC > send_keep_alive" );
		send( GEOMEC_ID, BusClientCmd::DianaHeartbeat );
		std::this_thread::sleep_for( std::chrono::seconds( 2 ) );
	}
	//Printer::instance()->info( " [Diana]BC > send_keep_alive > END" );
}

// must be non-blocking
void DianaBusClient::onReceivedMessage(const ModGMBusMessage& message)
{
	static int x = 0;
	QString from = message.from();
	int id = message.payloadAsInt();


	Printer::instance()->info( 
		" [Diana]BC > on_rec ['%s'->'%s','%s'] <-", 
		from.toStdString().c_str(), 
		m_name.toStdString().c_str(),
		g_bus_client_cmd_s[ id ].c_str() );

	switch( id )
	{
		case BusClientCmd::RunDiana_Req:
		{
			// TODO: all this in a sdeparated thead
			m_url = data(); // for testing purposed: print the content of given in the received memmory chunck
			set_diana_env();
			set_model_env( m_url );
			//print_env();
			//std::thread t0( notify_diana_end );

			m_diana_running = true;
			std::thread t0( [=] { send_keep_alive(); } );
			t0.detach(); //this is dangerous

			std::thread t1_( [=] { run_diana( m_url ); } );
			t1_.detach();

			break;
		}
		default:
			break;
	}
	//Printer::instance()->info( " [Diana]BC > onReceivedMessage > END" );
}

QString DianaBusClient::data()
{
	Printer::instance()->info(" [Diana]BC > get_shared_memory");

	m_shm.setKey( SHARED_MEMORY_RUN_DIANA_KEY );

    if( !m_shm.attach() ) 
	{
		Printer::instance()->info(" [Diana]BC > get_shared_memory > !m_shm.attach() !!!! ]");
        return QString(); //FIXME: Treat the error
	}

	m_shm.lock();

	QString data = QString( (char*)m_shm.constData() );

	int size = m_shm.size();

	m_shm.unlock();
	m_shm.detach();

	//qDebug() << qPrintable( QString::asprintf(" [Diana]BC > get_shared_memory [size:%d]", size ) );
	Printer::instance()->info(" [Diana]BC > get_shared_memory [size:%d, path:%s]", size, m_url.toStdString().c_str() );

	return data;
}

void DianaBusClient::read_output()
{
	Printer::instance()->info(" [Diana]BC : process : read_output" );
	Printer::instance()->info(" [Diana]BC : process : read_output : std error : '%s'", m_process.readAllStandardError().toStdString().c_str() );
	Printer::instance()->info(" [Diana]BC : process : read_output : std out : '%s'", m_process.readAllStandardOutput().toStdString().c_str() );
}


// path example: path:C:\Users\MANEL~1.CAR\AppData\Local\Temp\dra25952
void DianaBusClient::run_diana( const QString& path )
{
	//Printer::instance()->info( " [Diana]BC > run_diana" );

	// 2. Model ENVVARS
	// 3. System call

	const char* FFDIR_ = DiGetenv( "FFDIR" );
	//Printer::instance()->info(" [Diana]BC : run_diana : current_dir : FFDIR_:%s", FFDIR_ );

	QDir::setCurrent( FFDIR_ );
	//Manel.CarreraRuibal -> doesn't work
	Printer::instance()->debug(" [Diana]BC : run_diana : set CURRENT DIR: %s", FFDIR_ );

	QString TEMP	= path.left( path.lastIndexOf(QChar('/')) ); // Ej.: C:\Users\MANEL~1.CAR\AppData\Local\Temp\dra07288
	//QString logfile = TEMP + "\\a.txt";
	QString logfile = TEMP + ".txt";

	int num_tries = 0;
	int max_tries = 1; // no retries
	bool is_ok = false;

	QString cmd = GM42_BIN + " > " + logfile + " 2>&1";


	while( !is_ok && num_tries < max_tries )
	{
		Printer::instance()->info(" [Diana]BC : run_diana : system : cmd:'%s'", cmd.toStdString().c_str() );
		
		// sometimes it works and sometimes not

		// console visible
		//system( cmd.toStdString().c_str() );

		// console hidden
		std::string cmd_p = "cmd /c " + cmd.toStdString();
		QProcess process;

		// 'start' doesn't work, 'execute' works
		// 'execute' is blicking
		// 'start' also works but process needs to be global or if local a wait needs to be set after start

		process.start( QString::fromStdString( cmd_p ) );
		//process.execute( QString::fromStdString( cmd_p ) );
		//int res = m_process.execute( QString::fromStdString( cmd_p ) );

		// This also works fine
		bool res = process.waitForFinished( -1 ); // -1: no timeout, 30s. by default

		/*
		// this doesn't work with execute, but it does with 'start'
		if( !process.waitForStarted() )
		{
			Printer::instance()->info(" [Diana]BC : run_diana : process : error starting" );
		}
		else
		{
			Printer::instance()->info(" [Diana]BC : run_diana : process : Qt rocks!" );
			process.closeWriteChannel();

			if (!process.waitForFinished())
				Printer::instance()->info(" [Diana]BC : run_diana : process : error finishing" );

			QByteArray result = process.readAll(); // nothing read, maybe bacause all goes to a file -> no, it retrieves nothing even not dumping results to a file
			Printer::instance()->info(" [Diana]BC : run_diana : process : read:'%s'", result.toStdString().c_str() );
		}*/


		// analyze result
		std::string last_line;
		util::get_last_line( logfile.toStdString(), last_line );
		QString qs = QString::fromStdString( last_line ).simplified();
		QStringList list = qs.split(" ");
		QString result = list.at( 0 );

		is_ok = list[ 0 ] == END_1 && 
				list[ list.size()-1 ] == END_2;

		Printer::instance()->info(" [Diana]BC : run_diana : result:%d (num_tries:%d)", is_ok, num_tries );

		if( !is_ok )
		{
			Printer::instance()->info(" [Diana]BC : run_diana : result: LINE_0   :%s", list[ 0 ].toStdString().c_str() );
			Printer::instance()->info(" [Diana]BC : run_diana : result: LINE_LAST:%s", list[ list.size()-1 ].toStdString().c_str() );
		}

		num_tries++;
	}

	send( GEOMEC_ID, BusClientCmd::RunDiana_Done ); // end if diana run

	m_diana_running = false;

	//m_cond_var.notify_one();
	//send_response_to_geomec();
}

/*void DianaBusClient::notify_diana_end()
{
	std::unique_lock< std::mutex > l( m_mutex );
	m_cond_var.wait( l );
}*/

void DianaBusClient::send( const QString& to, BusClientCmd id )
{
	Printer::instance()->info( 
		"[Diana]BC > send ['%s'->'%s','%s'] ->", 
		m_name.toStdString().c_str(),
		to.toStdString().c_str(), 
		g_bus_client_cmd_s[ id ].c_str() );

	m_msg.setFrom(m_name.toStdString().c_str());
	m_msg.setTo( to.toStdString().c_str() );
	m_msg.setPayload( id ); // Any number...

	m_shm_socket.write( m_msg );
}


///////////////////////////////////////////////// Playing aroung passing data in a named-shared-memory /////////////////////////////////////////////////////////  

#ifdef kk

void DianaBusClient::get_shared_memory_01()
{
	Printer::instance()->info(" [Diana]BC > get_shared_memory");

	qDebug() << " [Diana]BC > get_shared_memory";

	m_shm.setKey( SHARED_MEMORY_RUN_DIANA_KEY );

    if( !m_shm.attach() ) 
	{
		qDebug() << " [Diana]BC > get_shared_memory > !m_shm.attach() !!!! ]";
		Printer::instance()->info(" [Diana]BC > get_shared_memory > !m_shm.attach() !!!! ]");
        return;
	}

	/*
    QBuffer buffer;
    //QDataStream in(&buffer);
    //QImage image;

    m_shm.lock();

    buffer.setData( (char*)m_shm.constData(), m_shm.size() );
    buffer.open( QBuffer::ReadOnly );


	int size = buffer.size();
    //in >> image;
	*/

	m_shm.lock();

	//const int SIZE = 10*1000*1000;
	const int SIZE = 10*1000;
	char* cp  = (char*)m_shm.constData();
	char c = cp[ SIZE-2 ];
	//int size_2 = sizeof( cp );
	int size = m_shm.size();

	m_shm.unlock();
	m_shm.detach();


	//qDebug() << " [Diana]BC > loadFromMemory [size:" << size << "]";
	
	qDebug() << " [Diana]BC > loadFromMemory [char received]";
	qDebug() << " [Diana]BC > loadFromMemory [c:" << c << "]";

	//qDebug() << " [Diana]BC > loadFromMemory [size:" << size << "]";
	qDebug() << qPrintable( QString::asprintf(" [Diana]BC > loadFromMemory [size:%d]", size ) );

	//qDebug() << " [Diana]BC > loadFromMemory [size_2:" << size_2 << "]";

	Printer::instance()->info(" [Diana]BC > loadFromMemory [size:%d]", size );
}



//////////////////////////////////////////////////////// Some tests and trials //////////////////////////////////////////////////////////////

/*void DianaBusClient::run_diana()
{
  qDebug() << "Client diana";

  	QString Exe = "C:/Program Files/GEOMEC 5.7 - SVS/Diana/binseg/ap/gm42.exe";
	QString Path = "C:/Users/Manel.CarreraRuibal/AppData/Local/Temp/dra11072";
  //Path = "E:/Temp/c/dra12836";
	QString Dat;
	QString Com;
	QString Filos = "ff" + Path.right(6) + ".ff"; 
	QString Base = "DIANA";
	bool DisplayDefMessages = false;
	QString UserLeader;
	QString UserTrailer;

  // quick copy from GeomecModel/src/DianaEnv.cpp
  // DIASLIB is responsible for errors about non-existing element types
  // DIALIB was not set in call from command line, so maybe not needed
  // DIAERRPATH was not needed when DIASLIB was set
  QString DiaPath_ = "C:/Program Files/GEOMEC 5.7 - SVS/Diana";
  QString DiaShare_ = DiaPath_ + "/share";
  QString DiaLib_ = DiaPath_ + "/lib";
  QString DiaSLib_ = DiaShare_ + "/lib";

  int res;
  res = vDiSetenv("%s=%s", "DIAPATH", DiaPath_.toStdString().c_str());
  res = vDiSetenv("%s=%s", "DIASHARE", DiaShare_.toStdString().c_str());
  res = vDiSetenv("%s=%s", "DIALIB", DiaLib_.toStdString().c_str());
  res = vDiSetenv("%s=%s", "DIASLIB", DiaSLib_.toStdString().c_str());

  QString TEMP = "C:/Users/Manel.CarreraRuibal/AppData/Local/Temp";
  QString FF = TEMP + "/dra11072/ffa11072.ff";
  QString FFDIR = DiaPath_ + "/bin";
  QString STP0 = DiaPath_ + "/binseg/ap/mc41.exe";

	res = vDiSetenv("%s=%s", "FF",  FF.toStdString().c_str() );
	//res = vDiSetenv("%s=%s", "FFDIR",           ".");
	res = vDiSetenv("%s=%s", "FFDIR",           FFDIR.toStdString().c_str() );
	res = vDiSetenv("%s=%s", "STP0", STP0.toStdString().c_str() );


	

	QDir::setCurrent( FFDIR );
	system("..\\binseg\\ap\\gm42.exe");


	//QProcess process;
	//process.setWorkingDirectory( FFDIR );
	//process.start("..\\binseg\\ap\\gm42.exe", QStringList() << "gui");
	//process.start("..\\binseg\\ap\\gm42.exe");
	//process.startDetached("..\\binseg\\ap\\gm42.exe");
	//if( process.waitForStarted() ) 
	//{
	//	int a=0;
		// Now your app is running.
	//}
}*/

void DianaBusClient::run_diana_03()
{


  // Note: DianaEnv.cpp should be in Glue, but it depends on registry; we may want to refactor that altogether (later)

	
	/*int res1 = CDianaStartUp::GetInstance()->RunDiana(	 Exe,
											 Path,
											 Dat,
											 Com,
											 Filos,
											 Base,
											 DisplayDefMessages,
											 UserLeader,
											 UserTrailer );*/






	/*res = CDianaStartUp::GetInstance()->RunDiana(	 Exe,
											 Path,
											 Dat,
											 Com,
											 Filos,
											 Base,
											 DisplayDefMessages,
											 "UserLeader",
											 "UserTrailer" );*/

	/*while (true)
  {
    m_hb_message.setPayload(m_hb_id++);

    if (!m_shm_socket.write(m_hb_message))
    {
      ++m_dropped;

      if (m_dropped >= 1)
      {
        qDebug() << m_name << " dropped " << m_dropped;
        m_dropped = 0;
      }
    }
  }*/
}


void DianaBusClient::run_diana_02()
{
	/*CExecuteDianaDialogQt dlg;
	CDianaXWrapper* dianaXWrapper = new CDianaXWrapper( &dlg );


	QString strDiaPath = DiaPath();
	assert(!strDiaPath.isEmpty());

	QString strDiaShare = DiaSharePath();
	assert(!strDiaShare.isEmpty());

	QString strDiaLib = DiaLibPath();
	assert(!strDiaLib.isEmpty());

	QString strDiaSLib = DiaSLibPath();
	assert(!strDiaSLib.isEmpty());

	QString strExecutable = DianaExecutable();
	assert(!strExecutable.isEmpty());

	QString strStp0 = strDiaPath + "/binseg/ap/" + strExecutable;

	dianaXWrapper->SetProgressIsLifesign(true);

	// set DIANA environment for DianaX control
	dianaXWrapper->SetEnvironmentVar("DIAPATH",         strDiaPath.toStdString().c_str());
	dianaXWrapper->SetEnvironmentVar("DIASHARE",        strDiaShare.toStdString().c_str());
	dianaXWrapper->SetEnvironmentVar("DIALIB",          strDiaLib.toStdString().c_str());
	dianaXWrapper->SetEnvironmentVar("DIASLIB",         strDiaSLib.toStdString().c_str());
	dianaXWrapper->SetEnvironmentVar("FF",              FilosFileName().c_str());
	dianaXWrapper->SetEnvironmentVar("FFDIR",           WorkingDir().c_str());
	//dianaXWrapper->SetEnvironmentVar("FFDIR",           "."));

	//if(ApplicationSpecificLogging())
		//dianaXWrapper->SetEnvironmentVar("APP_SPC_LOGGING", "Yes");

	dianaXWrapper->SetShowDefaultMessages(DefaultMessages());
	dianaXWrapper->SetDisplayStopMessage(DisplayStopMessage());
	dianaXWrapper->SetAutoCloseDialog(AutoCloseDialog());

	dianaXWrapper->SetEnvironmentVar("STP0", strStp0.toStdString().c_str());

	dianaXWrapper->SetRunner(this);

	bool bRetry = true;
	bool bCalcResult = false;

	//if (bSilent)
	{
		dianaXWrapper->SetShowDefaultMessages(false);
		short res = dianaXWrapper->ExecuteDiana(strStp0.toStdString().c_str(), WorkingDir().c_str(), "", "", FilosFileName().c_str(), "DIANA");
		bCalcResult = GetCalculationResult();
	}*/
}

#endif
