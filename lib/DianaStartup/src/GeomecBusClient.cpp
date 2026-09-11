#include "GeomecBusClient.h"

#include "Printer.h"

#include <QImage>
#include <QBuffer>

#include "DianaStartUp.h"

const std::vector< std::string > GeomecBusClient::m_status_s_v = { "Stopped", "Running", "Succeded", "Error" };

GeomecBusClient::GeomecBusClient(const QString& base, const QString& name, QObject *parent)
  : ModGMLocalBusClient(base, name, parent), 
	m_result_available( false ),
	m_diana_running( false ),
	m_status( Stopped )
{
	connect(this, &GeomecBusClient::receivedMessage, this, &GeomecBusClient::onReceivedMessage, Qt::DirectConnection );
	connect(this, &GeomecBusClient::shmError, this, &GeomecBusClient::onReceivedError);

	Printer::instance()->info( "[Geomec]BC > Constructor" );
}
GeomecBusClient::~GeomecBusClient(){}

void GeomecBusClient::onReceivedError(const QString& error)
{ 
	Printer::instance()->info( "[Geomec]BC > onReceivedError [error:'%s']", error.toStdString().c_str() ); 
}

void GeomecBusClient::send( const QString& to, BusClientCmd cmd )
{
	Printer::instance()->info( 
		"[Geomec]BC > send ['%s'->'%s','%s'] ->", 
		m_name.toStdString().c_str(),
		to.toStdString().c_str(), 
		g_bus_client_cmd_s[ cmd ].c_str() );

	m_msg.setFrom(m_name.toStdString().c_str());
	m_msg.setTo( to.toStdString().c_str() );
	m_msg.setPayload( cmd );

	m_shm_socket.write( m_msg );
}


//void GeomecBusClient::send_run_diana_request( const QString& path )
void GeomecBusClient::send_run_diana_request()
{
	Printer::instance()->info( "[Geomec]BC > RUN request:%s ", m_path.toStdString().c_str() );

	data( m_path ); // write data that diana app could need in a named-shared-memory"
						// in this minimal version, the file id: '11072' 

	m_status = Running;
	m_result_available =  false;

	send( DIANA_ID, BusClientCmd::RunDiana_Req );
}

void GeomecBusClient::onReceivedMessage(const ModGMBusMessage& message)
{
	static int x = 0;
	QString from = message.from();
	int id = message.payloadAsInt();

	//CDianaStartUp::result_available( true );
	Printer::instance()->info( 
		"[Geomec]BC > on_rec ['%s'->'%s','%s'] <-", 
		from.toStdString().c_str(), 
		m_name.toStdString().c_str(),
		g_bus_client_cmd_s[ id ].c_str() );
	
	switch( id )
	{
		case BusClientCmd::StopDiana_Ack:	
			m_status = Stopped;
			m_diana_running = false; 
			break;

		case BusClientCmd::LaunchDiana_Ack: 
			m_status = Running;
			m_diana_running = true; 
			break;		

		case BusClientCmd::RunDiana_Done:	
			m_status = Succeded;
			m_result_available = true; 
			break;

		case BusClientCmd::DianaHeartbeat:
		{
			m_heartbeat_timestamp = QDateTime::currentDateTime();

			/*Printer::instance()->info( 
				"[Geomec]BC > onReceivedMessage > keep alive > %s", 
				m_heartbeat_timestamp.toString("hh:mm:ss.zzz").toStdString().c_str() );*/

			break;
		}
		default:
			break;
	}

	if( id != BusClientCmd::DianaHeartbeat )
		Printer::instance()->info( "[Geomec]BC > on_rec > status:'%s'", m_status_s_v[ m_status ].c_str() ); 
}


void GeomecBusClient::data( const QString& path )
{
	Printer::instance()->info( "[Geomec]BC > data > [path:%s]", path.toStdString().c_str() );

	m_shm.setKey(SHARED_MEMORY_RUN_DIANA_KEY);

    if( m_shm.isAttached() )
        m_shm.detach();

	if( !m_shm.create( path.size() ) ) 
	{
		Printer::instance()->info( "[Geomec]BC > loadFromFile > KO" );
        return;
	}

	//Printer::instance()->info( "[Geomec]BC > set_shared_memory > Ok [size:%d]", m_shm.size() );

    m_shm.lock();
    
	char* shm_data = (char*)m_shm.data();
	memcpy( shm_data, path.toStdString().c_str(), path.size() );

    m_shm.unlock();
}

///////////////////////////////////////////////// Playing aroung passing data in a named-shared-memory /////////////////////////////////////////////////////////  

/*
void GeomecBusClient::set_shared_memory_01()
{
	qDebug() << "[Geomec]BC > set_shared_memory";

	m_shm.setKey( SHARED_MEMORY_RUN_DIANA_KEY );

    if( m_shm.isAttached() )
        m_shm.detach();

	//const long long int SIZE = 10*1000*1000;
	const int SIZE = 10*1000;
	
	char data[ SIZE ];

	memset(data,' ', SIZE-1);
	data[ SIZE-1 ] = 0;
	data[ SIZE-2 ] = 'a';

	if( !m_shm.create( SIZE ) ) 
	{
		qDebug() << "[Geomec]BC > loadFromFile > KO";
        return;
	}

	qDebug() << QString::asprintf("[Geomec]BC > set_shared_memory > Ok [size:%d]", m_shm.size());
	Printer::instance()->info( "[Geomec]BC > set_shared_memory > Ok [size:%d]", m_shm.size() );

    m_shm.lock();
    
	char* shm_data = (char*)m_shm.data();
	memcpy( shm_data, data, SIZE );

    m_shm.unlock();
}


void GeomecBusClient::set_shared_memory_02()
{
	qDebug() << "[Geomec]BC > set_shared_memory_02";

	m_shm.setKey( SHARED_MEMORY_RUN_DIANA_KEY );

    if( m_shm.isAttached() )
        m_shm.detach();

	QString fileName = "C:\\aWork\\aGeomec\\DSA_0401\\x64\\ReleaseWithDebug\\michael_jordan.jpg";

    QImage image;
    image.load( fileName );

	 // load into shared memory
    QBuffer buffer;
    buffer.open( QBuffer::ReadWrite );
    QDataStream out( &buffer );
    out << image;
    int size = buffer.size();

    if( !m_shm.create( size ) ) 
	{
		qDebug() << "[Geomec]BC > loadFromFile > KO";
        return;
	}

	qDebug() << QString::asprintf("[Geomec]BC > set_shared_memory_02 > Ok [size:%d]", m_shm.size());
	Printer::instance()->info( "[Geomec]BC > set_shared_memory_02 > Ok [size:%d]", m_shm.size() );

    m_shm.lock();
    
	char* shm_data = (char*)m_shm.data();
    const char* data = buffer.data().data();
	memcpy( shm_data, data, qMin( m_shm.size(), size) );
	
    m_shm.unlock();
}
*/