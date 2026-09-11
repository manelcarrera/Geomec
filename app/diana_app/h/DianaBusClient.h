#pragma once

#include "ModGMBus.h"

#include <QProcess>

#include "Interprocess.h"

// #include <mutex>
// #include <condition_variable>

class DianaBusClient : public ModGMLocalBusClient {
  Q_OBJECT

  // std::condition_variable m_cond_var;
  // std::mutex m_mutex;

public:
  DianaBusClient(const QString &base, const QString &name, QObject *parent = nullptr);
  virtual ~DianaBusClient();

  void run_diana(const QString &path);
  // void notify_diana_end();

  void send_keep_alive();

public slots:
  void read_output();

  // public:
private:
  // public slots:
  void send(const QString &to, BusClientCmd id);

  // private slots:

  void onReceivedMessage(const ModGMBusMessage &message);

private:
  QString data();

private:
  ModGMBusMessage m_msg;
  QSharedMemory m_shm;

  bool m_diana_running;

  QProcess m_process;

  QString m_url;

public:
  void url(const QString &val) { m_url = val; }
  const QString &url() { return m_url; }

#ifdef kk
  // draft
private:
  void get_shared_memory_01();

  void run_diana_02();
  void run_diana_03();
#endif
};