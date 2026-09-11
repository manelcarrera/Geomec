#pragma once

//#include <QtCore/QProcess>
//#include <QtCore/QHash>

#include <QProcess>
#include <QHash>

#include "ModGMBus.h"

class QProcessEnvironment;

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

  void start(	const QString& name, 
        const QStringList& args,
        const QProcessEnvironment* env = nullptr );

  void start(	const QString& name, 
        const QProcessEnvironment* env = nullptr );

  void stop_( const QString& name ); // FIXME: why can't be named just 'stop' 
                                     // Answer: https://stackoverflow.com/questions/16794695/connecting-overloaded-signals-and-slots-in-qt-5

signals:

  void quit();

private slots:

  void processFinished(int retval);

private:
  quint64 m_pid;
  QHash<QString, QProcess *> m_processes;

};