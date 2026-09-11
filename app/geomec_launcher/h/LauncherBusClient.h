#pragma once

#include "ModGMBus.h"

#include "Interprocess_OLD.h" // cmd id


class ProcessManager; //FIXME

class LauncherBusClient : public ModGMLocalBusClient
{
  Q_OBJECT

public:
  LauncherBusClient(const QString& base, const QString& name, QObject *parent = nullptr);
  virtual ~LauncherBusClient();

private:
//public slots:
  void send( const QString& to, BusClientCmd id );

//private slots:

  void onReceivedMessage(const ModGMBusMessage& message);
  

private:
  ModGMBusMessage m_msg;
  QSharedMemory m_shm;

public:
  ProcessManager* m_pm;
  
};