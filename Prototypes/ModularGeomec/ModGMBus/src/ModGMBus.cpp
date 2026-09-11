#include "ModGMBus.h"

// Linux C
// #define __STDC_WANT_LIB_EXT1__ 1
// #include <string.h>

#include <cstring> // memcpy

#include <algorithm> // std::min

#include <exception>
#include <iostream>

#include <QtCore/QDataStream>
#include <QtCore/QRandomGenerator>
#include <QtCore/QTime>

using namespace std;

// make protected method public
class MyQLocalSocket : public QLocalSocket {
public:
  using QLocalSocket::readData;
};

/////////////////////////////// ModGMBusMessage

ModGMBusMessage::ModGMBusMessage() { memset(package, 0, TOTAL_SIZE); }

ModGMBusMessage::ModGMBusMessage(Type type, const char *from, const char *to) : ModGMBusMessage() {
  fields.type = type;

  // memcpy_s(fields.from, NAME_SIZE, from, strlen(from));
  // memcpy_s(fields.to, NAME_SIZE, to, strlen(to));

  // from just windows to multiplatform
  // from: errno_t memcpy_s( void *restrict dest, rsize_t destsz, const void *restrict src, rsize_t count );
  // to: memcpy(dest, src, std::min(destsize,count))

  // std::memcpy( fields.from, from, std::min( NAME_SIZE, strlen( from ) ) );
  std::memcpy(fields.from, from, strlen(from) < NAME_SIZE ? strlen(from) : NAME_SIZE);
  // std::memcpy( fields.to, to, std::min( NAME_SIZE, strlen( to ) ) );
  std::memcpy(fields.to, to, strlen(to) < NAME_SIZE ? strlen(to) : NAME_SIZE);
}

ModGMBusMessage::ModGMBusMessage(const ModGMBusMessage &rhs) {
  // memcpy_s(package, TOTAL_SIZE, rhs.package, TOTAL_SIZE);
  std::memcpy(package, rhs.package, std::min(TOTAL_SIZE, TOTAL_SIZE));
}

void ModGMBusMessage::setType(Type type) { fields.type = type; }

void ModGMBusMessage::setFrom(const char *from) {
  int sz = (int)strlen(from);
  // memcpy_s(fields.from, NAME_SIZE, from, sz);
  // std::memcpy( fields.from, from, std::min( NAME_SIZE, sz ) );
  std::memcpy(fields.from, from, sz < NAME_SIZE ? sz : NAME_SIZE);
  if (sz < NAME_SIZE)
    memset(fields.from + sz, 0, NAME_SIZE - sz);
}

void ModGMBusMessage::setTo(const char *to) {
  int sz = (int)strlen(to);
  // memcpy_s(fields.to, NAME_SIZE, to, sz);
  // std::memcpy(fields.to, to, std::min( NAME_SIZE, sz ) );
  std::memcpy(fields.to, to, sz < NAME_SIZE ? sz : NAME_SIZE);
  if (sz < NAME_SIZE)
    memset(fields.to + sz, 0, NAME_SIZE - sz);
}

void ModGMBusMessage::setPayload(const char *data) {
  int sz = (int)strlen(data);
  // quint8 sz = (quint8)strlen(data);

  // memcpy_s(fields.payload, PAYLOAD_SIZE, data, sz);
  // std::memcpy( fields.payload, data, std::min( PAYLOAD_SIZE, sz ) );
  std::memcpy(fields.payload, data, sz < PAYLOAD_SIZE ? sz : PAYLOAD_SIZE);
  if (sz < PAYLOAD_SIZE)
    memset(fields.payload + sz, 0, PAYLOAD_SIZE - sz);
}

void ModGMBusMessage::setPayload(int data) { *(int *)fields.payload = data; }

/////////////////////////////// ModGMQueue

void ModGMQueue::init() { memset(this, 0, sizeof(ModGMQueue)); }

void ModGMQueue::push(const ModGMBusMessage &msg) {
  // memcpy_s(&queue[tail++], ModGMBusMessage::TOTAL_SIZE, msg.data(), ModGMBusMessage::TOTAL_SIZE);
  std::memcpy(&queue[tail++], msg.data(), std::min(ModGMBusMessage::TOTAL_SIZE, ModGMBusMessage::TOTAL_SIZE));
  tail %= 20;
  ++size;
}

void ModGMQueue::pop(ModGMBusMessage *msg) {
  // memcpy_s((char *)msg, ModGMBusMessage::TOTAL_SIZE, &queue[head++], ModGMBusMessage::TOTAL_SIZE);
  std::memcpy((char *)msg, &queue[head++], std::min(ModGMBusMessage::TOTAL_SIZE, ModGMBusMessage::TOTAL_SIZE));
  head %= 20;
  --size;
}

ModGMBusMessage ModGMQueue::pop() {
  ModGMBusMessage msg;
  pop(&msg);
  return std::move(msg);
}

/////////////////////////////// ModGMLocalBusHelper

quint64 ModGMLocalBusHelper::getPid(int argc, char *argv[]) {
  quint64 pid = 0;

  for (int i = 1; i < argc; ++i) {
    QStringList l = QString(argv[i]).split('=');
    if (l.size() > 1 && l[0] == "pid") {
      pid = l[1].toULongLong();
    }
  }

  return pid;
}

QString ModGMLocalBusHelper::getBase(quint64 pid) { return QString("\\\\.\\pipe\\ModGM\\%1\\").arg(pid); }

QString ModGMLocalBusHelper::createListenException(const QString &service) {
  return QString("Failed to listen to ") + service;
}

QString ModGMLocalBusHelper::readFrom(QLocalSocket *socket) {
  quint32 size = 0;
  QString msg;
  QDataStream in;

  in.setDevice(socket);
#if QT_VERSION == QT_VERSION_CHECK(5, 10, 1)
  in.setVersion(QDataStream::Qt_5_10);
#elif QT_VERSION == QT_VERSION_CHECK(5, 12, 0)
  in.setVersion(QDataStream::Qt_5_12);
#endif

  if (socket->bytesAvailable() > (int)sizeof(quint32))
    in >> size;

  if (socket->bytesAvailable() >= size && !in.atEnd())
    in >> msg;

  return msg;
}

void ModGMLocalBusHelper::writeTo(QLocalSocket *socket, const QString &message) {
  QByteArray block;
  QDataStream out(&block, QIODevice::WriteOnly);
#if QT_VERSION == QT_VERSION_CHECK(5, 10, 1)
  out.setVersion(QDataStream::Qt_5_10);
#elif QT_VERSION == QT_VERSION_CHECK(5, 12, 0)
  out.setVersion(QDataStream::Qt_5_12);
#endif
  quint32 size = 0;

  out << quint32(message.size());
  out << message;

  socket->write(block);
  socket->flush();
}

ModGMBusMessage ModGMLocalBusHelper::readMessageFrom(QLocalSocket *socket) {
  ModGMBusMessage msg;

  if (socket->bytesAvailable() >= (int)sizeof(ModGMBusMessage)) {
    qint64 len = 32;
    while (len > 0)
      len -= ((MyQLocalSocket *)socket)->readData((char *)msg.data(), len);
  }

  return std::move(msg);
}

void ModGMLocalBusHelper::writeMessageTo(QLocalSocket *socket, const ModGMBusMessage &message) {
  socket->write(message.data(), (int)sizeof(ModGMBusMessage));
  socket->flush();
}

QString ModGMLocalBusHelper::error(QLocalSocket::LocalSocketError socketError, QLocalSocket *socket) {
  QString msg = "error: ";
  switch (socketError) {
  case QLocalSocket::ServerNotFoundError:
    msg += "server not found";
    break;
  case QLocalSocket::ConnectionRefusedError:
    msg += "connection refused";
    break;
  case QLocalSocket::PeerClosedError:
    msg += "peer closed";
    break;
  default:
    if (socket)
      msg += socket->errorString();
  }

  return std::move(msg);
}

/////////////////////////////// ModGMListener

ModGMListener::ModGMListener(const QString &base, IConnectionManager *connectionManager, QObject *parent)
    : QObject(parent), m_connectionManager(connectionManager) {
  m_server = new QLocalServer(this);

  if (!m_server->listen(base + BUS_LISTENER))
    throw std::runtime_error(ModGMLocalBusHelper::createListenException(base + BUS_LISTENER).toStdString());

  connect(m_server, &QLocalServer::newConnection, this, &ModGMListener::handleConnect);
}

ModGMListener::~ModGMListener() {
  disconnect(m_server, &QLocalServer::newConnection, this, &ModGMListener::handleConnect);
  m_server->close();
}

void ModGMListener::handleConnect() {
  QLocalSocket *client = m_server->nextPendingConnection();

  connect(client, &QLocalSocket::disconnected, client, &QLocalSocket::deleteLater);

  client->waitForReadyRead();

  QString msg = ModGMLocalBusHelper::readFrom(client);

  m_connectionManager->handleCommand(msg);

  client->disconnectFromServer();
}

/////////////////////////////// ModGMConnector

ModGMConnector::ModGMConnector(const QString &base, QObject *parent)
    : QObject(parent), m_base(base), m_socket(new QLocalSocket(this)) {
  connect(m_socket, &QLocalSocket::connected, this, &ModGMConnector::handleConnect);
  connect(m_socket, &QLocalSocket::disconnected, this, &ModGMConnector::handleDisconnect);
}

ModGMConnector::~ModGMConnector() { delete m_socket; }

void ModGMConnector::connectToListener(const QString &command) {
  m_command = command; // only one at the time; if more are needed, caller needs to check signal commandDelivered before
                       // sending again
  m_socket->connectToServer(m_base + BUS_LISTENER);
}

void ModGMConnector::handleConnect() { ModGMLocalBusHelper::writeTo(m_socket, m_command); }

void ModGMConnector::handleDisconnect() {
  // we should add some robustness, but for now we assume that sending the command always succeeds
  emit commandDelivered();
}

/////////////////////////////// ModGMSharedMemorySocket

ModGMSharedMemorySocket::ModGMSharedMemorySocket(Type type)
    : QObject(), m_type(type), m_sem_readbuf_empty(nullptr), m_sem_readbuf_full(nullptr), m_sem_writebuf_empty(nullptr),
      m_sem_writebuf_full(nullptr), m_quit(false) {}

ModGMSharedMemorySocket::ModGMSharedMemorySocket(const QString &name, Type type)
    : QObject(), m_name(name), m_type(type), m_sem_readbuf_empty(nullptr), m_sem_readbuf_full(nullptr),
      m_sem_writebuf_empty(nullptr), m_sem_writebuf_full(nullptr), m_quit(false) {}

void ModGMSharedMemorySocket::create() {
  if (m_type == CLIENT) {
    qDebug() << "ModGMSharedMemorySocket::create(CLIENT)";
    m_sem_readbuf_empty = new QSystemSemaphore(m_name + "re");
    m_sem_readbuf_full = new QSystemSemaphore(m_name + "rf");
    m_sem_writebuf_empty = new QSystemSemaphore(m_name + "we");
    m_sem_writebuf_full = new QSystemSemaphore(m_name + "wf");

    m_shm_readbuf.setKey(m_name + "rb");
    m_shm_writebuf.setKey(m_name + "wb");

    if (!m_shm_readbuf.isAttached() && !m_shm_readbuf.attach()) {
      qDebug() << "CLIENT: m_shm_readbuf failed to attach " << m_shm_readbuf.errorString();
      emit error(m_shm_readbuf.error() ? m_shm_readbuf.errorString() : "CLIENT: Failed to attach to read queue");
    }
    if (!m_shm_writebuf.isAttached() && !m_shm_writebuf.attach()) {
      qDebug() << "CLIENT: m_shm_writebuf failed to attach " << m_shm_writebuf.errorString();
      emit error(m_shm_writebuf.error() ? m_shm_writebuf.errorString() : "CLIENT: Failed to attach to write queue");
    }
  } else {
    qDebug() << "ModGMSharedMemorySocket::create(SERVER)";
    m_sem_readbuf_empty = new QSystemSemaphore(m_name + "we", 20, QSystemSemaphore::Create);
    m_sem_readbuf_full = new QSystemSemaphore(m_name + "wf", 0, QSystemSemaphore::Create);
    m_sem_writebuf_empty = new QSystemSemaphore(m_name + "re", 20, QSystemSemaphore::Create);
    m_sem_writebuf_full = new QSystemSemaphore(m_name + "rf", 0, QSystemSemaphore::Create);

    m_shm_readbuf.setKey(m_name + "wb");
    m_shm_writebuf.setKey(m_name + "rb");

    const int size = int(sizeof(ModGMQueue));

    if (!m_shm_readbuf.isAttached() && !m_shm_readbuf.attach() && !m_shm_readbuf.create(size)) {
      qDebug() << "SERVER: m_shm_readbuf failed to create " << m_shm_readbuf.errorString();
      emit error(m_shm_readbuf.error() ? m_shm_readbuf.errorString() : "SERVER: Failed to attach to write queue");
    } else {
      reinterpret_cast<ModGMQueue *>(m_shm_readbuf.data())->init();
    }
    if (!m_shm_writebuf.isAttached() && !m_shm_writebuf.attach() && !m_shm_writebuf.create(size)) {
      qDebug() << "SERVER: m_shm_writebuf failed to create " << m_shm_writebuf.errorString();
      emit error(m_shm_writebuf.error() ? m_shm_writebuf.errorString() : "SERVER: Failed to attach to read queue");
    } else {
      reinterpret_cast<ModGMQueue *>(m_shm_writebuf.data())->init();
    }
  }

  start();
}

void ModGMSharedMemorySocket::close() {
  m_quit = true;

  m_thread.wait();

  m_shm_readbuf.detach();
  m_shm_writebuf.detach();

  delete m_sem_readbuf_empty;
  delete m_sem_readbuf_full;
  delete m_sem_writebuf_empty;
  delete m_sem_writebuf_full;
}

bool ModGMSharedMemorySocket::write(const ModGMBusMessage &message) {
  ModGMQueue *queue = reinterpret_cast<ModGMQueue *>(m_shm_writebuf.data());
  bool retval = false;

  qDebug() << "write: payload: " << message.payloadAsInt();

  qDebug() << "write: acquire m_sem_writebuf_empty";
  if (!m_sem_writebuf_empty->acquire()) {
    qDebug() << "write: error acquire m_sem_writebuf_empty: " << m_sem_writebuf_empty->errorString();
    emit error(m_sem_writebuf_empty->error() ? m_sem_writebuf_empty->errorString()
                                             : "Failed to acquire write semaphore");
  }

  qDebug() << "write: lock m_shm_writebuf";
  m_shm_writebuf.lock();

  if (!queue->full()) {
    qDebug() << "write: push";
    queue->push(message);
    retval = true;
  } else {
    qDebug() << "write: full";
  }

  qDebug() << "write: unlock m_shm_writebuf";
  m_shm_writebuf.unlock();

  qDebug() << "write: release m_sem_writebuf_full";
  m_sem_writebuf_full->release();

  return retval;
}

size_t ModGMSharedMemorySocket::read(ModGMBusMessage *message, size_t max) {
  ModGMQueue *queue = reinterpret_cast<ModGMQueue *>(m_shm_readbuf.data());
  int number = 0;

  qDebug() << "read: assume we're in critical section (from run)";

  while (!queue->empty() && number < max) {
    qDebug() << "read: pop";
    queue->pop(&message[number++]);
  }

  qDebug() << "read: unlock m_shm_readbuf";
  m_shm_readbuf.unlock();

  qDebug() << "read: leave critical section";
  m_mutex.unlock();

  qDebug() << "read: release m_sem_readbug_empty (" << number << ")";
  m_sem_readbuf_empty->release(number);

  return number;
}

void ModGMSharedMemorySocket::run() {
  ModGMQueue *queue = reinterpret_cast<ModGMQueue *>(m_shm_readbuf.data());

  while (!m_quit) {
    qDebug() << "run: acquire m_sem_readbuf_full";
    if (!m_sem_readbuf_full->acquire()) {
      qDebug() << "read: error acquire m_sem_readbuf_full: " << m_sem_readbuf_full->errorString();
      emit error(m_sem_readbuf_full->error() ? m_sem_readbuf_full->errorString() : "Failed to acquire read semaphore");
    }

    qDebug() << "run: enter critical section";
    m_mutex.lock();

    qDebug() << "run: lock m_shm_readbuf";
    m_shm_readbuf.lock();

    emit readyRead();
  }
}

void ModGMSharedMemorySocket::start() {
  moveToThread(&m_thread);
  connect(&m_thread, &QThread::started, this, &ModGMSharedMemorySocket::run);
  m_thread.start();
  emit started();
}

void ModGMSharedMemorySocket::quit() {
  m_sem_readbuf_full->release();
  close();

  emit finished();
}

/////////////////////////////// ModGMLocalBusClient

ModGMLocalBusClient::ModGMLocalBusClient(const QString &base, const QString &name, QObject *parent)
    : QObject(parent), m_base(base), m_name(name), m_connector(base, parent), m_shm_socket(name) {
  connect(&m_connector, &ModGMConnector::commandDelivered, this, &ModGMLocalBusClient::handleConnectedToListener);
  connect(this, &ModGMLocalBusClient::handleMessage, this, &ModGMLocalBusClient::send);
}

ModGMLocalBusClient::~ModGMLocalBusClient() { m_connector.blockSignals(true); }

void ModGMLocalBusClient::connectToListener() {
  QString msg = QString("register %1").arg(m_name);
  m_connector.connectToListener(msg);
}

void ModGMLocalBusClient::setup() { connectToListener(); }

void ModGMLocalBusClient::quit() {}

void ModGMLocalBusClient::handleConnectedToListener() {
  connect(&m_shm_socket, &ModGMSharedMemorySocket::readyRead, this, &ModGMLocalBusClient::read, Qt::QueuedConnection);
  connect(&m_shm_socket, &ModGMSharedMemorySocket::error, this, &ModGMLocalBusClient::shmError);
  m_shm_socket.create();
}

void ModGMLocalBusClient::read() {
  ModGMBusMessage msg[MODGMBUS_QUEUE_SIZE];

  qDebug() << "Client read...";

  int number = (int)m_shm_socket.read(msg, MODGMBUS_QUEUE_SIZE);

  qDebug() << "We read " << number << " messages";

  for (int i = 0; i < number; ++i)
    emit receivedMessage(msg[i]);
}

void ModGMLocalBusClient::send(const ModGMBusMessage &message) { m_shm_socket.write(message); }

/////////////////////////////// ModGMLocalBusServer

ModGMLocalBusServer::ModGMLocalBusServer(const QString &base, const QString &name, QObject *parent)
    : QObject(), m_base(base), m_name(name), m_shm_socket(name, ModGMSharedMemorySocket::SERVER) {
  connect(&m_shm_socket, &ModGMSharedMemorySocket::readyRead, this, &ModGMLocalBusServer::read);

  m_shm_socket.create();
}

ModGMLocalBusServer::~ModGMLocalBusServer() {}

void ModGMLocalBusServer::handleMessage(const ModGMBusMessage &message) {
  qDebug() << "Server write...";
  if (!m_shm_socket.write(message)) {
    qDebug() << "Write was dropped";
  }
}

void ModGMLocalBusServer::read() {
  ModGMBusMessage msg[MODGMBUS_QUEUE_SIZE];

  qDebug() << "Server read...";

  int number = (int)m_shm_socket.read(msg, MODGMBUS_QUEUE_SIZE);

  qDebug() << "We read " << number << " messages";

  for (int i = 0; i < number; ++i) {
    qDebug() << "read: " << msg[i].payloadAsInt();
    emit receivedMessage(msg[i]);
  }
}

/////////////////////////////// ModGMLocalBusManager

ModGMLocalBusManager::ModGMLocalBusManager(const QString &base, IConnectionManager *connectionManager, QObject *parent)
    : QObject(parent), m_base(base), m_listener(base, connectionManager, this), m_connectionManager(connectionManager) {
  qDebug() << "";
  connect(m_connectionManager, &IConnectionManager::registerChannel, this,
          &ModGMLocalBusManager::handleRegisterChannel);
  connect(m_connectionManager, &IConnectionManager::unregisterChannel, this,
          &ModGMLocalBusManager::handleUnregisterChannel);
}

ModGMLocalBusManager::~ModGMLocalBusManager() {
  m_listener.blockSignals(true);

  disconnect(m_connectionManager, &IConnectionManager::registerChannel, this,
             &ModGMLocalBusManager::handleRegisterChannel);
  disconnect(m_connectionManager, &IConnectionManager::unregisterChannel, this,
             &ModGMLocalBusManager::handleUnregisterChannel);

  for (QHash<QString, ModGMLocalBusServer *>::iterator it = m_bus.begin(); it != m_bus.end(); ++it)
    delete it.value();

  m_bus.clear();
}

void ModGMLocalBusManager::handleMessage(const ModGMBusMessage &message) {
  qDebug() << "BM : handleMessage";

  if (strncmp(message.payload(), "quit", 4) == 0) {
    qDebug() << "BM : handleMessage : 1";
    emit quit();
  } else {
    if (*message.to() == '*') {
      qDebug() << "BM : handleMessage : 2";
      emit receivedMessage(message);
    } else if (m_bus.contains(message.to())) {

      qDebug() << "BM : handleMessage : 3";

      QTime tm = QTime::fromString(message.payload(), "hh:mm:ss.zzz");
      int x = tm.msecsTo(QTime::currentTime());

      if (x > 50)
        return;

      qDebug() << "BM : handleMessage : Ok";

      // ModGMBusMessage msg(message);

      // msg.setData(QString::number(x).toStdString().c_str());
      // std::cout << message.to() << std::endl;

      QMetaObject::invokeMethod(m_bus.value(message.to()), "handleMessage", Qt::AutoConnection,
                                Q_ARG(const ModGMBusMessage &, message));
    } else {
      qDebug() << "BM : handleMessage : 4 ... to:" << message.to();

      for (auto service : m_bus.keys()) {
        qDebug() << "BM : handleMessage : service: " << service;
      }
    }
  }
  // m_bus.value(message.fields.to)->handleMessage(message);
}

void ModGMLocalBusManager::handleRegisterChannel(const QString &service) {
  // qDebug() << "BusManager > handleRegisterChannel";

  if (!m_bus.contains(service)) {
    ModGMLocalBusServer *server = new ModGMLocalBusServer(m_base, service, this);
    connect(server, &ModGMLocalBusServer::quit, this, &ModGMLocalBusManager::quit);
    connect(server, &ModGMLocalBusServer::receivedMessage, this, &ModGMLocalBusManager::handleMessage);
    connect(this, &ModGMLocalBusManager::receivedMessage, server, &ModGMLocalBusServer::handleMessage);
    m_bus.insert(service, server);

    qDebug() << "BusManager : +" << service;
    QHashIterator<QString, ModGMLocalBusServer *> i(m_bus);
    while (i.hasNext()) {
      i.next();
      qDebug() << "BusManager : Registered: " << i.key();
    }
  }
}

void ModGMLocalBusManager::handleUnregisterChannel(const QString &service) {
  if (m_bus.contains(service))
    m_bus.remove(service);
}