
#include "SlotHandlerDianaSignals.h"
#include "DCasintSlotHandler.h"

CSlotHandlerDianaSignals::CSlotHandlerDianaSignals(const QObject* sender,
  CSlotHandler* slotHandler)
: CDianaSignals(sender)
, m_slotHandler(slotHandler)
{
}

CSlotHandlerDianaSignals::~CSlotHandlerDianaSignals()
{
}

void CSlotHandlerDianaSignals::onMessage(QString message)
{
  m_slotHandler->slotOnMessage(message);
}

void CSlotHandlerDianaSignals::onAppSpcMessage(QString /*appSpcMessage*/)
{
}

void CSlotHandlerDianaSignals::onError(QString error)
{
  m_slotHandler->slotOnError(error);
}

void CSlotHandlerDianaSignals::onWarning(QString warning)
{
  m_slotHandler->slotOnWarning(warning);
}

void CSlotHandlerDianaSignals::onStopExecuting(bool &/*stop*/)
{
}

void CSlotHandlerDianaSignals::onFinished()
{
  m_slotHandler->slotOnFinished();
}
