#pragma once

#include <QtCore/QProcess>
#include <QtCore/QHash>

#include "ModGMBus.h"


class ProcessManager : public IConnectionManager
{
  Q_OBJECT

public:
  ProcessManager(quint64 pid, QObject *parent = nullptr);
  ~ProcessManager();

  void RegisterProcess(const QString& program, bool connectFinished);

  void handleCommand(const QString& message) override;

public slots:

  void start();
  void stop();

signals:

  void quit();

private slots:

  void processFinished(int retval);

private:
  quint64 m_pid;
  QHash<QString, QProcess *> m_processes;

};