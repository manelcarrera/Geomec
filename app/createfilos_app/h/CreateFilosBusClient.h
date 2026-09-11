#pragma once

#include "ModGMBus.h"

class CreateFilosBusClient : public ModGMLocalBusClient
{
  Q_OBJECT

public:
  CreateFilosBusClient(const QString& base, const QString& name, QObject *parent = nullptr);
  virtual ~CreateFilosBusClient();

  static void create_filos();

private:
//public slots:
  void send_response_to_geomec();

//private slots:

  void onReceivedMessage(const ModGMBusMessage& message);
  
private:
	void get_shared_memory();
	void get_shared_memory_01();

	void run_diana_02();
	void run_diana_03();

private:
  ModGMBusMessage m_msg;
  QSharedMemory m_shm;

  QString m_path;
};