#include "receiver.h"

#include <QtWidgets/QApplication>
#include <QtCore/QThread>
#include <QtCore/QByteArray>
#include <QtNetwork/QHostAddress>

static const int DefaultServerPort = 10000;

CReceiver::CReceiver( CThread& _thread )
:	m_thread( _thread )
{
  connect( &m_tcp_server, SIGNAL( newConnection() ), this, SLOT( accept_connection() ) );
  //connect( &m_tcp_server, SIGNAL( acceptError(QAbstractSocket::SocketError socketError) ), this, SLOT( accept_error( QAbstractSocket::SocketError socketError ) ) );
}

CReceiver::~CReceiver(){}


void CReceiver::listen()
{
  while( !m_tcp_server.isListening() && !m_tcp_server.listen( QHostAddress::LocalHost, DefaultServerPort ) ){} 
    //QThread::sleep( 1 );

  //m_tcp_server.waitForNewConnection();

  int i=0;
}
void CReceiver::accept_error( QAbstractSocket::SocketError socketError )
{
  qDebug() << socketError;
}

void CReceiver::accept_connection()
{
  m_tcp_server_connection = m_tcp_server.nextPendingConnection();

  QObject::connect( m_tcp_server_connection, SIGNAL( readyRead() ),							this, SLOT( handle_command() ) );

  //FIXME:
  //connect( m_tcpServerConnection, SIGNAL( error(QAbstractSocket::SocketError)),	this, SLOT( displayError( QAbstractSocket::SocketError ) ) );

  m_tcp_server.close();
}

/**
* 
*/

void CReceiver::handle_command()
{
  //int bytesReceived += (int)m_tcp_server_connection->bytesAvailable();
 
  QString cmd = QString( m_tcp_server_connection->readAll() ); //QByteArray

  qDebug() << "Received:" << cmd;

  //m_cmds << cmd;


  //FIXME:
  m_thread.m_mutex.lock();
  /*for( int i=1 ; i < 10 ; i++ )
  {
    QString cmd = QString::asprintf("cmd_%d", i);
    m_thread.m_cmds << cmd;
  }*/
  m_thread.m_cmds << cmd;
  m_thread.m_mutex.unlock();
  
  m_thread.m_cmds_available.wakeAll();
  

  //print_results(	CDependencies::PRINT_PROJECT ); //FIXME

  m_tcp_server_connection->close(); //FIXME: Mirar si comando viene troceado

  /*if( bytesReceived == TotalBytes ) 
  {
    m_tcp_server_connection->close();
  }*/
}


