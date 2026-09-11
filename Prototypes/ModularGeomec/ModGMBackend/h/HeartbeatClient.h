#pragma once

#include "ModGMBus.h"

// Client connector
// It contains a heartbeat function that sends packages every so often to simulate work
class HeartbeatClient : public ModGMLocalBusClient {
  Q_OBJECT

public:
  HeartbeatClient(const QString &base, const QString &name, QObject *parent = nullptr);
  virtual ~HeartbeatClient();

public slots:
  void heartbeat_setup();

private slots:
  void heartbeat();

private:
  QTimer *m_timer;
  ModGMBusMessage m_hb_message;

  int m_dropped;
  int m_hb_id;
};