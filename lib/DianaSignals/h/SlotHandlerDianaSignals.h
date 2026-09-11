#ifndef _SlotHandlerDianaSignals_h_
#define _SlotHandlerDianaSignals_h_

class CSlotHandler;

#include "DianaSignals.h"

class CSlotHandlerDianaSignals : public CDianaSignals
{
public:
  CSlotHandlerDianaSignals(const QObject* sender, CSlotHandler* slotHandler);
  ~CSlotHandlerDianaSignals();

  virtual void onMessage(QString message);
  virtual void onAppSpcMessage(QString appSpcMessage);
  virtual void onError(QString error);
  virtual void onWarning(QString warning);
  virtual void onStopExecuting(bool &stop);
  virtual void onFinished();

private:
  CSlotHandlerDianaSignals(const CSlotHandlerDianaSignals& rhs);
  CSlotHandlerDianaSignals& operator = (const CSlotHandlerDianaSignals& rhs);

  CSlotHandler* m_slotHandler;
};

#endif  // _SlotHandlerDianaSignals_h_
