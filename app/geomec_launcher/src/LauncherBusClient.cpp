#include "LauncherBusClient.h"

#include "Printer.h"
#include "ProcessManager.h"

#include <QBuffer>
#include <QDir>
#include <QProcess>

LauncherBusClient::LauncherBusClient(const QString& base, const QString& name, QObject *parent)
  : ModGMLocalBusClient(base, name, parent)
{
  Printer::instance()->info( "   [Launcher]BC > Constructor" );
  connect(this, &LauncherBusClient::receivedMessage, this, &LauncherBusClient::onReceivedMessage);
}

LauncherBusClient::~LauncherBusClient(){}

void LauncherBusClient::onReceivedMessage(const ModGMBusMessage& message)
{
  static int x = 0;
  QString from = message.from();
  int id = message.payloadAsInt();

  Printer::instance()->info( 
    "   [Launcher]BC > on_rec ['%s'->'%s','%s'] <-", 
    from.toStdString().c_str(), 
    m_name.toStdString().c_str(),
    g_bus_client_cmd_s[ id ].c_str() );

  if( from == GEOMEC_ID )
  {
    switch( id )
    {
      case BusClientCmd::LaunchDiana_Req:
      {
        // 1.
        Printer::instance()->info( "   [Launcher]BC > do LAUNCH diana" );
        m_pm->RegisterProcess( DIANA_BIN, false);
        m_pm->start( DIANA_BIN );
        // FIXME: verify process is running
        std::this_thread::sleep_for( std::chrono::seconds( 2 ) );
        // wait diana to be started

        // 2.
        //send_response_to_geomec(); // communication from diana app to Geomec app via the bus communications system
        send( GEOMEC_ID, LaunchDiana_Ack );
        break;
      }
      case BusClientCmd::StopDiana_Req:
      {
        Printer::instance()->info( "[Launcher]BC > do STOP diana" );

        // 2.
        send( GEOMEC_ID, StopDiana_Ack );

        // 1.
        // TODO: verify if I need to unregister ... a priori I would say that it's not needed

        // FIXME MCR 2019-09-16: Testing
        m_pm->stop_( DIANA_BIN );
        // doing this next message never arrives to Geomec (StopDiana_Ack)
        //
        // Let's change the order and try... -> it's setnt but never arrived to GeoemcBC
  
        break;
      }
      default: // ERROR
        break;

    }
  }
}

void LauncherBusClient::send( const QString& to, BusClientCmd id )
{
  Printer::instance()->info( 
    "   [Launch]BC > send ['%s'->'%s','%s'] ->", 
    m_name.toStdString().c_str(),
    to.toStdString().c_str(), 
    g_bus_client_cmd_s[ id ].c_str() );

  m_msg.setFrom(m_name.toStdString().c_str());
  m_msg.setTo( to.toStdString().c_str() );
  m_msg.setPayload( id );

  m_shm_socket.write( m_msg );
}