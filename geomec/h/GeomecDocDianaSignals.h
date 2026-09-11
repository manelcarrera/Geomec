#ifndef _GeomecDocDianaSignals_h_
#define _GeomecDocDianaSignals_h_

class CGeomecDoc;

#include "DianaSignals.h"

class CGeomecDocDianaSignals : public CDianaSignals {
public:
  CGeomecDocDianaSignals(const QObject *sender, CGeomecDoc *geomecDoc);
  ~CGeomecDocDianaSignals();

  virtual void onMessage(QString message);
  virtual void onAppSpcMessage(QString appSpcMessage);
  virtual void onError(QString error);
  virtual void onWarning(QString warning);
  virtual void onStopExecuting(bool &stop);
  virtual void onFinished();

private:
  CGeomecDocDianaSignals(const CGeomecDocDianaSignals &rhs);
  CGeomecDocDianaSignals &operator=(const CGeomecDocDianaSignals &rhs);

  CGeomecDoc *m_geomecDoc;
};

#endif // _GeomecDocDianaSignals_h_
