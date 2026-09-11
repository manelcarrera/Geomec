#pragma once


// Defines some classes for handling IPC and management
//
// 1. Message and Queue [ ModGMBusMessage, ModGMQueue ]
// 2. Listener and Connector for initial handshake [ ModGMListener, ModGMConnector ]
//    These use QLocalSocket (named pipes) which are local to the current machine
//    Hence the name "Local" in some of the class names below; we might add a non-local version later (eg tcp/ip)
// 3. End-point of two-way communication channel [ ModGMSharedMemorySocket ]
// 4. Managers for the client- and server-part of these end-points [ ModGMLocalBusClient, ModGMLocalBusServer ]
// 5. A bus and manager for the server side [ ModGMLocalBusManager ]
// 6. Miscellaneous [ ModGMLocalBusHelper, IConnectionManager ]

// THIS IS A TOY VERSION TO TEST STUFF LIKE ROBUSTNESS & SPEED
// QT HAS SOME LIMITATIONS THAT MAKE IT UNLIKELY TO BE FINAL CHOICE FOR THIS FUNCTIONALITY



#include "modgmbus_global.h"

#include <QtCore/QThread>
#include <QtCore/QQueue>
#include <QtNetwork/QLocalServer>
#include <QtNetwork/QLocalSocket>
#include <QtCore/QTimer>
#include <QtCore/QSystemSemaphore>
#include <QtCore/QSharedMemory>
#include <QtCore/QMutex>


#define BUS_LISTENER "listener"



// Simple message definition with a type, a to/from in text, and some payload
// It's small. Bigger data should be done through named shared memory and the payload the name
class MODGMBUS_EXPORT ModGMBusMessage
{
public:

  typedef enum Size : quint8
  {
  TYPE_SIZE = 1,
  NAME_SIZE = 8,
  PAYLOAD_SIZE = 15,
  TOTAL_SIZE = 32
  } Size;

  typedef enum Type : quint8
  {
  SIGNAL = 0,
  REQUEST = 1,
  RESPONSE = 2
  } Type;

  ModGMBusMessage();
  ModGMBusMessage(Type type, const char *from, const char *to);
  ModGMBusMessage(const ModGMBusMessage& rhs);

  Type type() const { return fields.type; }
  const char *from() const { return fields.from; }
  const char *to() const { return fields.to; }
  const char *payload() const { return fields.payload; }
  int payloadAsInt() const { return *(int *)fields.payload; }
  const char *data() const { return package; }

  void setType(Type type);
  void setFrom(const char *from);
  void setTo(const char *to);
  void setPayload(const char *data);
  void setPayload(int data);

private:
  struct Fields {
  Type   type;

  char from[NAME_SIZE];
  char to[NAME_SIZE];

  char payload[PAYLOAD_SIZE];
  }; //fields;

  union
  {
  /*struct fields {
      Type   type;

      char from[NAME_SIZE];
      char to[NAME_SIZE];

      char payload[PAYLOAD_SIZE];
  }; //fields;*/
  Fields fields; 
  char package[TOTAL_SIZE];
  };
};


#define MODGMBUS_QUEUE_SIZE 20

// Fixed size queue to pass along messages
struct MODGMBUS_EXPORT ModGMQueue
{
  int head;
  int tail;
  int size;
  ModGMBusMessage queue[MODGMBUS_QUEUE_SIZE];

  __inline bool empty() { return size == 0; }
  __inline bool full() { return size == MODGMBUS_QUEUE_SIZE; }
  __inline void init();
  __inline void push(const ModGMBusMessage& msg);
  __inline ModGMBusMessage pop();
  __inline void pop(ModGMBusMessage *msg);
};


// A process can connect to the listener via QLocalSocket (local named pipe on windows)
// These are some helper routines for that
class MODGMBUS_EXPORT ModGMLocalBusHelper
{
public:

  static quint64 getPid(int argc, char *argv[]);
  static QString getBase(quint64 pid);

  static QString createListenException(const QString& service);
  static QString readFrom(QLocalSocket *socket);
  static void writeTo(QLocalSocket *socket, const QString& message);

  static ModGMBusMessage readMessageFrom(QLocalSocket *socket);
  static void writeMessageTo(QLocalSocket *socket, const ModGMBusMessage& message);

  static QString error(QLocalSocket::LocalSocketError socketError, QLocalSocket *socket = nullptr);
};



// Interface for handling connections; used by ModGMLocalBusManager (implemented in ModGMMain)
class MODGMBUS_EXPORT IConnectionManager : public QObject
{
  Q_OBJECT

public:
  IConnectionManager(QObject *parent = nullptr)
  : QObject(parent)
  {
  }

  virtual void handleCommand(const QString&) = 0;

signals:
  void registerChannel(const QString&);
  void unregisterChannel(const QString&);
};


// The listener receives new connections, signals the command that was received and closes the connection
class MODGMBUS_EXPORT ModGMListener : public QObject
{
  Q_OBJECT

public:
  ModGMListener(const QString& base, IConnectionManager *connectionManager, QObject *parent = nullptr);
  ~ModGMListener();

private slots:
  void handleConnect();

private:
  QLocalServer *m_server;
  IConnectionManager *m_connectionManager;
};



// The connector sets up a connection with the listener, fires off a command and then expects to be disconnected
class MODGMBUS_EXPORT ModGMConnector : public QObject
{
  Q_OBJECT

public:
  explicit ModGMConnector(const QString& base, QObject *parent = nullptr);
  ~ModGMConnector();

  void connectToListener(const QString& command);

signals:
  void commandDelivered();

private slots:
  void handleConnect();
  void handleDisconnect();

private:
  QString m_base;
  QString m_command;
  QLocalSocket *m_socket;
};




// Once processes have connected to the bus via the listener, they will connect though shared memory sockets
// Communication is two-way, but read and write both have their own socket.
// So there will be four sockets per connection: Server.Read <-> Client.Write and Server.Write <-> Client.Read
// Synchronization is protected by system semaphores (also four, Queue.Empty and Queue.Full per socket pair)
//
// Naming is from the point of view of the client; for the server we just switch the pointers.
class MODGMBUS_EXPORT ModGMSharedMemorySocket : public QObject
{
  Q_OBJECT

public:

  typedef enum : int {
  CLIENT = 0,
  SERVER = 1
  } Type;

  explicit ModGMSharedMemorySocket(Type type = CLIENT);
  explicit ModGMSharedMemorySocket(const QString& name, Type type = CLIENT);

  void create();
  void close();

  bool write(const ModGMBusMessage& message);
  size_t read(ModGMBusMessage *message, size_t max);

signals:
  void started();
  void finished();
  void readyRead();
  void error(const QString& errorMessage);

private slots:
  void run();

  public slots:
  void start();
  void quit();

private:
  QString          m_name;
  Type             m_type;

  QSystemSemaphore *m_sem_readbuf_empty;
  QSystemSemaphore *m_sem_readbuf_full;
  QSharedMemory    m_shm_readbuf;

  QSystemSemaphore *m_sem_writebuf_empty;
  QSystemSemaphore *m_sem_writebuf_full;
  QSharedMemory    m_shm_writebuf;

  QThread          m_thread;
  bool             m_quit;
  QMutex m_mutex;
};



// Client connector
class MODGMBUS_EXPORT ModGMLocalBusClient : public QObject
{
  Q_OBJECT

public:
  ModGMLocalBusClient(const QString& base, const QString& name, QObject *parent = nullptr);
  virtual ~ModGMLocalBusClient();

  void connectToListener();

public slots:
  void setup();
  void quit();

signals:
  void finished();
  void receivedMessage(const ModGMBusMessage&);
  void handleMessage(const ModGMBusMessage&);
  void shmError(const QString& error);

private slots:
  void handleConnectedToListener();

  void read();
  void send(const ModGMBusMessage& message);

protected:
  QString m_base;
  QString m_name;
  ModGMConnector m_connector;
  ModGMSharedMemorySocket m_shm_socket;
};



// Server connector
class MODGMBUS_EXPORT ModGMLocalBusServer : public QObject
{
  Q_OBJECT

public:
  ModGMLocalBusServer(const QString & base, const QString & name, QObject *parent = nullptr);
  ~ModGMLocalBusServer();


signals:
  void quit();
  void receivedMessage(const ModGMBusMessage&);

public slots:
  void handleMessage(const ModGMBusMessage& message);

  private slots:
  void read();
  //void error(QLocalSocket::LocalSocketError socketError);

private:
  QString       m_base;
  QString       m_name;

  ModGMSharedMemorySocket m_shm_socket;
};



// The bus manager has the listener, adds processes, and sets up the server and client channel
class MODGMBUS_EXPORT ModGMLocalBusManager : public QObject
{
  Q_OBJECT

public:
  ModGMLocalBusManager(const QString& base, IConnectionManager *connectionManager, QObject *parent = nullptr);
  ~ModGMLocalBusManager();

signals:
  void quit();
  void receivedMessage(const ModGMBusMessage&);

public slots:
  void handleMessage(const ModGMBusMessage& message);

private slots:
  void handleRegisterChannel(const QString& service);
  void handleUnregisterChannel(const QString& service);

private:
  QString       m_base;
  ModGMListener m_listener;
  IConnectionManager *m_connectionManager;
  QHash<QString, ModGMLocalBusServer *> m_bus;
};

