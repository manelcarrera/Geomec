#ifndef _MatParamDlgDianaSignals_h_
#define _MatParamDlgDianaSignals_h_

class CMatParamDlg;

#include "DianaSignals.h"

class CMatParamDlgDianaSignals : public CDianaSignals {
public:
  CMatParamDlgDianaSignals(const QObject *sender, CMatParamDlg *matParamDlg);
  ~CMatParamDlgDianaSignals();

  virtual void onMessage(QString message);
  virtual void onAppSpcMessage(QString appSpcMessage);
  virtual void onError(QString error);
  virtual void onWarning(QString warning);
  virtual void onStopExecuting(bool &stop);
  virtual void onFinished();

private:
  CMatParamDlgDianaSignals(const CMatParamDlgDianaSignals &rhs);
  CMatParamDlgDianaSignals &operator=(const CMatParamDlgDianaSignals &rhs);

  CMatParamDlg *m_matParamDlg;
};

#endif // _MatParamDlgDianaSignals_h_
