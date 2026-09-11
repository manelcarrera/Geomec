#pragma once

#include "ModGMBus.h"

#include "Interprocess_OLD.h" // cmd id

#include <QDateTime>
#include <QTimer>

class GeomecBusClient : public ModGMLocalBusClient {
  Q_OBJECT

public:
  enum eStatus { Stopped, Running, Succeded, Error };

  static const std::vector<std::string> m_status_s_v;

public:
  GeomecBusClient(const QString &base, const QString &name, QObject *parent = nullptr);
  virtual ~GeomecBusClient();

public slots:
  // void send_run_diana_request( const QString& path );
  void send_run_diana_request();

  void send(const QString &to, BusClientCmd cmd);

private slots:

  virtual void onReceivedMessage(const ModGMBusMessage &message);
  void onReceivedError(const QString &error);

public:
  void data(const QString &path);

  bool result_available() { return m_result_available; };
  void result_available(bool val) { m_result_available = val; };

  // void set_shared_memory_01();
  // void set_shared_memory_02();

public:
  void path(const QString &path) { m_path = path; };

  void status(eStatus var) { m_status = var; }
  eStatus status() { return m_status; }

private:
  ModGMBusMessage m_msg;
  QSharedMemory m_shm;

  QString m_path;

  eStatus m_status;

public:
  bool m_result_available;
  QDateTime m_heartbeat_timestamp;
  bool m_heartbeat_error;
  bool m_diana_running;
};