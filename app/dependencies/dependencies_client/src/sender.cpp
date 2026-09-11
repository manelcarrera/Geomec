#include "sender.h"

#include <QtWidgets/QApplication>

#include <QtCore/QThread>
#include <QtCore/QString>

#include <QtCore/QByteArray>
#include <QtNetwork/QHostAddress>

static const int DefaultServerPort = 10000;

CSender::CSender()
{
  QObject::connect( &m_tcp_client, SIGNAL(	bytesWritten( qint64 ) ),				this, SLOT( handle_reply( qint64 )));
  QObject::connect( &m_tcp_client, SIGNAL(	error(QAbstractSocket::SocketError)),	this, SLOT( display_error(QAbstractSocket::SocketError)));
  QObject::connect( &m_tcp_client, SIGNAL(	hostFound() ) ,							this, SLOT( host_found() ) ); //qint64
  QObject::connect( &m_tcp_client, SIGNAL(	connected() ) ,							this, SLOT( _connected() ) ); //qint64
}

CSender::~CSender(){}

void CSender::connect( const QString& cmd )
{
  m_cmd = cmd;
  qDebug() << "Connecting...";

  // m_tcp_client.waitForConnected( 1000 ) // It doesn't work
  
  while( m_tcp_client.state() != QAbstractSocket::ConnectedState )
  {
    if( m_tcp_client.state() != QAbstractSocket::HostLookupState ) 
      m_tcp_client.connectToHost( QHostAddress::LocalHost, DefaultServerPort );
  
    m_tcp_client.waitForConnected( -1 ); // it doesn't wait but it's enecessary

    QThread::sleep( 1 );
  }
}

//void CSender::send_cmd(){}



void CSender::display_error( QAbstractSocket::SocketError error_id )
{ 
  //qDebug() << "display_error:" << m_tcp_client.error();
}

void CSender::handle_reply( qint64 )
{
}


void CSender::host_found()
{
  int i=0;
}

void CSender::_connected()
{
  qDebug() << "...Connected";

  //QString cmd = "cmd";

  //qDebug() << "Sending" << m_cmd;

  for( int i=1 ; i < 10 ; i++ )
  {
    QString cmd = QString::asprintf("cmd_%d", i);
    qDebug() << "Sending" << cmd;
    m_tcp_client.write( cmd.toUtf8() );
    //m_tcp_client.write( m_cmd.toUtf8() );
    //QThread::sleep( 1 );
  }
}
