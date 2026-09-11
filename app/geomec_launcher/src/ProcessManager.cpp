#include "ProcessManager.h"

#include <iostream>

#include "Printer.h"

#include <QProcessEnvironment>

QStringList getArgs(quint64 pid) { return QStringList{QString("pid=%1").arg(pid)}; }

ProcessManager::ProcessManager(quint64 pid, QObject *parent) : IConnectionManager(parent), m_pid(pid) {}

ProcessManager::~ProcessManager() {}

void ProcessManager::RegisterProcess(const QString &program, bool connectFinished) {
  if (!m_processes.contains(program)) {
    QProcess *process = new QProcess(this);
    if (connectFinished)
      connect(process, QOverload<int>::of(&QProcess::finished), this, &ProcessManager::processFinished);
    m_processes.insert(program, process);
  }
}

void ProcessManager::handleCommand(const QString &message) {
  Printer::instance()->info("ProcessManager > handleCommand > msg: '%s'", message.toStdString().c_str());

  QStringList l = message.split(' ');
  if (l.size() == 1) {
    QString command = l[0];

    if (command == "quit")
      emit quit();
  } else if (l.size() > 1) {
    QString command = l[0];
    QString service = l[1];

    if (command == "register") {
      // We should check here if we have registered this as process already
      // Let's assume we didn't for now -- that means, we expect exactly one "register" per process
      // Later on we want more flexibility here, with processes attaching and detaching, exposing different
      // interfaces/services, etc

      emit registerChannel(service);
    } else if (command == "unregister") {
      // We should have similar checks as mentioned above

      emit unregisterChannel(service);
    }
  }
}

void ProcessManager::start() {
  for (QHash<QString, QProcess *>::iterator it = m_processes.begin(); it != m_processes.end(); ++it) {
    QStringList l = it.key().split(':');
    if (l.size() == 2) {
      QStringList args = getArgs(m_pid);
      args.append(QString("name=%1").arg(l[1]));

      it.value()->start(l[0], args);
    } else {
      Printer::instance()->info("ProcessManager > start > process: '%s' pid:'%d'", it.key().toStdString().c_str(),
                                m_pid);
      it.value()->start(it.key(), getArgs(m_pid));
    }
    // this was before, Geomec was taking pid from a file, not from the args
    /*if( it.key() == "Geomec.exe" ) //FIXME
      it.value()->start(it.key());
    else
      it.value()->startDetached(it.key(), getArgs(m_pid));*/
  }
}

void ProcessManager::start(const QString &name, const QStringList &args, const QProcessEnvironment *env) {
  // syntax 'name:id' won't work
  if (m_processes.contains(name)) {
    Printer::instance()->info("ProcessManager > start > process: '%s' pid:'%d'", name.toStdString().c_str(), m_pid);
    if (env)
      m_processes[name]->setProcessEnvironment(*env);
    m_processes[name]->start(name, args);
  }
}

void ProcessManager::start(const QString &name, const QProcessEnvironment *env) {
  // syntax 'name:id' won't work
  if (m_processes.contains(name)) {
    Printer::instance()->info("ProcessManager > start > process: '%s' pid:'%d'", name.toStdString().c_str(), m_pid);
    if (env)
      m_processes[name]->setProcessEnvironment(*env);
    m_processes[name]->start(name, getArgs(m_pid));
  }
}

void ProcessManager::stop_(const QString &name) {
  // syntax 'name:id' won't work
  if (m_processes.contains(name)) {
    Printer::instance()->info("ProcessManager > stop > process: '%s'", name.toStdString().c_str());
    m_processes[name]->kill();

    // FIXME: HC
    // This crashes, QThread
    Printer::instance()->info("ProcessManager > unregister > process: 'Diana'");
    emit unregisterChannel("Diana");
  }
}

void ProcessManager::stop() {
  for (QHash<QString, QProcess *>::iterator it = m_processes.begin(); it != m_processes.end(); ++it) {
    it.value()->kill();
  }

  emit quit();
}

void ProcessManager::processFinished(int retval) {
  std::cout << "processFinished: " << retval << std::endl;
  stop();
}