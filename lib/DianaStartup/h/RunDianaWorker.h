#pragma once

#include <QObject>
#include <QString>

class GeomecBusClient;
class CDianaStartUp;

namespace
{
  int pid();
  void delete_( GeomecBusClient* client );
  void heartbeat_( GeomecBusClient* client );
  bool launch_diana_task( CDianaStartUp* this_, int pid_ );
  bool run_diana_task_01( CDianaStartUp* this_, GeomecBusClient* client );
  void process_events();
  bool run_diana_task( CDianaStartUp* this_, const QString& path, int pid_ );
}

class RunDianaWorker : public QObject 
{
  Q_OBJECT

    QString m_path;
    int m_pid;
 
public:
  RunDianaWorker( const QString& path, int pid );
  ~RunDianaWorker();
 
public slots:
  void process();
 
signals:
  void finished();
  void error(QString err);

public:

  bool m_res;
};
