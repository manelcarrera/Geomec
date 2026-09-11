#pragma once

//class CExecuteDianaDialog;
class IExecuteDianaDialog;

#include "DianaSignals.h"

class CExecuteDianaSignals : public CDianaSignals
{
  IExecuteDianaDialog* m_exe_diana_dlg;

public:
  CExecuteDianaSignals(	const QObject* sender,
            IExecuteDianaDialog* exe_diana_dlg );

  ~CExecuteDianaSignals();

  virtual void onMessage(QString message);
  virtual void onAppSpcMessage(QString appSpcMessage);
  virtual void onError(QString error);
  virtual void onWarning(QString warning);
  virtual void onStopExecuting(bool &stop);
  virtual void onFinished();
};