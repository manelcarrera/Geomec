#include "CreateFilosBusClient.h"


#include "Printer.h"
#include <QBuffer>

#include <QDir>
#include <QProcess>

#include "Interprocess.h"

//#include "lbcx.h"
#include "lbfl.h"

static const QString SHARED_MEMORY_KEY = "MICHAEL";

CreateFilosBusClient::CreateFilosBusClient(const QString& base, const QString& name, QObject *parent)
  : ModGMLocalBusClient(base, name, parent)
{
	Printer::instance()->info( "[CreateFilos]BusClient > Constructor" );

	connect(this, &CreateFilosBusClient::receivedMessage, this, &CreateFilosBusClient::onReceivedMessage);
}

CreateFilosBusClient::~CreateFilosBusClient(){}

void CreateFilosBusClient::onReceivedMessage(const ModGMBusMessage& message)
{
	static int x = 0;
	QString from = message.from();
	int id = message.payloadAsInt();

	Printer::instance()->info( "[CreateFilos]BusClient > onReceivedMessage [from:'%s' id='%d']", from.toStdString().c_str(), id );

	get_shared_memory(); // for testing purposed: print the content of given in the received memmory chunck
	create_filos(); 
	send_response_to_geomec(); // communication from diana app to Geomec app via the bus communications system
}


void CreateFilosBusClient::send_response_to_geomec()
{
	Printer::instance()->info( "[CreateFilos]BusClient > send_response_to_geomec" );

	m_msg.setFrom(m_name.toStdString().c_str());
	m_msg.setTo("Geomec");
	m_msg.setPayload( 21 ); // Any number...

	m_shm_socket.write( m_msg );
}

void CreateFilosBusClient::create_filos()
{
	Printer::instance()->info( "[CreateFilos]BusClient > run_diana" );

	std::string FF; // Filos File
	OpenFilos( FF.c_str(), O_RDONLY );
}

void CreateFilosBusClient::get_shared_memory()
{
	Printer::instance()->info("[CreateFilos]BusClient > get_shared_memory");

	m_shm.setKey(SHARED_MEMORY_RUN_DIANA_KEY);

    if( !m_shm.attach() ) 
	{
		Printer::instance()->info("[CreateFilos]BusClient > get_shared_memory > !m_shm.attach() !!!! ]");
        return;
	}

	m_shm.lock();

	m_path = QString( (char*)m_shm.constData() );

	int size = m_shm.size();

	m_shm.unlock();
	m_shm.detach();

	//qDebug() << qPrintable( QString::asprintf("[CreateFilos]BusClient > get_shared_memory [size:%d]", size ) );
	Printer::instance()->info("[CreateFilos]BusClient > get_shared_memory [size:%d, path:%s]", size, m_path.toStdString().c_str() );
}


///////////////////////////////////////////////// Playing aroung passing data in a named-shared-memory /////////////////////////////////////////////////////////  

void CreateFilosBusClient::get_shared_memory_01()
{
	Printer::instance()->info("[CreateFilos]BusClient > get_shared_memory");

	qDebug() << "[CreateFilos]BusClient > get_shared_memory";

	m_shm.setKey(SHARED_MEMORY_RUN_DIANA_KEY);

    if( !m_shm.attach() ) 
	{
		qDebug() << "[CreateFilos]BusClient > get_shared_memory > !m_shm.attach() !!!! ]";
		Printer::instance()->info("[CreateFilos]BusClient > get_shared_memory > !m_shm.attach() !!!! ]");
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


	//qDebug() << "[CreateFilos]BusClient > loadFromMemory [size:" << size << "]";
	
	qDebug() << "[CreateFilos]BusClient > loadFromMemory [char received]";
	qDebug() << "[CreateFilos]BusClient > loadFromMemory [c:" << c << "]";

	//qDebug() << "[CreateFilos]BusClient > loadFromMemory [size:" << size << "]";
	qDebug() << qPrintable( QString::asprintf("[CreateFilos]BusClient > loadFromMemory [size:%d]", size ) );

	//qDebug() << "[CreateFilos]BusClient > loadFromMemory [size_2:" << size_2 << "]";

	Printer::instance()->info("[CreateFilos]BusClient > loadFromMemory [size:%d]", size );
}