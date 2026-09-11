#pragma once

#include "ModGMBus.h"

// #include <qobject>

// Client connector
// It contains a heartbeat function that sends packages every so often to simulate work
class DianaClient : public ModGMLocalBusClient {
  Q_OBJECT

public:
  DianaClient(const QString &base, const QString &name, QObject *parent = nullptr);
  virtual ~DianaClient();

public slots:
  void diana_setup();

private slots:
  // void heartbeat();
  void run_diana();

private:
  QTimer *m_timer;
  ModGMBusMessage m_hb_message;

  int m_dropped;
  int m_hb_id;
};