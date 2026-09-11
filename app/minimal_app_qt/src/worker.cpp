#include "worker.h"

#include <QAbstractEventDispatcher>

Worker::Worker() {
  // you could copy data from constructor arguments to internal variables here.
}

Worker::~Worker() {
  // free resources
}

void Worker::process() {
  // allocate resources using new here
  qDebug("process : 1");
  m_quit = false;
  while (!m_quit) {
    QThread::sleep(1);
    m_thread.eventDispatcher()->processEvents(QEventLoop::AllEvents);
  }
  qDebug("process : 2");

  qDebug("process : emit finished");

  emit finished();
}

void Worker::stop() {
  qDebug("stop");
  qDebug("wait : before");
  m_quit = true;
  m_thread.wait();
  qDebug("wait : after");
}