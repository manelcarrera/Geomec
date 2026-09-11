
#include <cassert>

#include "DianaSignals.h"

CDianaSignals::CDianaSignals(const QObject *sender) : QObject(), m_sender(sender) {
  assert(m_sender != 0);

  connect(m_sender, SIGNAL(OnMessage(QString)), this, SLOT(slotMessage(QString)));
  connect(m_sender, SIGNAL(OnAppSpcMessage(QString)), this, SLOT(slotAppSpcMessage(QString)));
  connect(m_sender, SIGNAL(OnError(QString)), this, SLOT(slotError(QString)));
  connect(m_sender, SIGNAL(OnWarning(QString)), this, SLOT(slotWarning(QString)));
  connect(m_sender, SIGNAL(OnStopExecuting(bool &)), this, SLOT(slotStopExecuting(bool &)));
  connect(m_sender, SIGNAL(OnFinished()), this, SLOT(slotFinished()));
}

CDianaSignals::~CDianaSignals() {
  assert(m_sender != 0);

  disconnect(m_sender, SIGNAL(OnFinished()), this, SLOT(slotFinished()));
  disconnect(m_sender, SIGNAL(OnStopExecuting(bool &)), this, SLOT(slotStopExecuting(bool &)));
  disconnect(m_sender, SIGNAL(OnWarning(QString)), this, SLOT(slotWarning(QString)));
  disconnect(m_sender, SIGNAL(OnError(QString)), this, SLOT(slotError(QString)));
  disconnect(m_sender, SIGNAL(OnAppSpcMessage(QString)), this, SLOT(slotAppSpcMessage(QString)));
  disconnect(m_sender, SIGNAL(OnMessage(QString)), this, SLOT(slotMessage(QString)));
}

void CDianaSignals::slotMessage(QString message) { onMessage(message); }

void CDianaSignals::slotAppSpcMessage(QString appSpcMessage) { onAppSpcMessage(appSpcMessage); }

void CDianaSignals::slotError(QString error) { onError(error); }

void CDianaSignals::slotWarning(QString warning) { onWarning(warning); }

void CDianaSignals::slotStopExecuting(bool &stop) { onStopExecuting(stop); }

void CDianaSignals::slotFinished() { onFinished(); }
