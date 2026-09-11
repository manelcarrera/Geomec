#if !defined(Sender_h)
#define Sender_h

#include <QtNetwork/QTcpSocket>

#include <QtCore/QObject>

class CSender : QObject {
  Q_OBJECT

  QTcpSocket m_tcp_client;

  QString m_cmd;

public slots:

  void handle_reply(qint64);
  void display_error(QAbstractSocket::SocketError error_id);
  void host_found();
  void _connected();

public:
  CSender();
  virtual ~CSender();

  void connect(const QString &cmd);
  // void send_cmd();

  // void listen();
};

#endif //! defined(Sender_h)
