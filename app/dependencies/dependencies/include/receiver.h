#if !defined(Receiver_h)
#define Receiver_h

//listen
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>


#include <QtCore/QObject>

#include <QtCore/QList>
#include <QtCore/QString>

#include "_thread.h"

class CReceiver : QObject
{
  Q_OBJECT

  QTcpServer m_tcp_server;
  QTcpSocket* m_tcp_server_connection;

  CThread& m_thread;

  //QList< QString > m_cmds;

public slots:

  void accept_connection();
  void handle_command();
  void accept_error( QAbstractSocket::SocketError socketError );

public:


  CReceiver( CThread& _thread );
  virtual ~CReceiver();


  void listen();
};



#endif //!defined(Receiver_h)
