#ifndef _DCasingDianaSignals_h_
#define _DCasingDianaSignals_h_

class CDCasing;

#include "DianaSignals.h"

class CDCasingDianaSignals : public CDianaSignals {
public:
  CDCasingDianaSignals(const QObject *sender, CDCasing *dCasing);
  ~CDCasingDianaSignals();

  virtual void onMessage(QString message);
  virtual void onAppSpcMessage(QString appSpcMessage);
  virtual void onError(QString error);
  virtual void onWarning(QString warning);
  virtual void onStopExecuting(bool &stop);
  virtual void onFinished();

private:
  CDCasingDianaSignals(const CDCasingDianaSignals &rhs);
  CDCasingDianaSignals &operator=(const CDCasingDianaSignals &rhs);

  CDCasing *m_dCasing;
};

#endif // _DCasingDianaSignals_h_
