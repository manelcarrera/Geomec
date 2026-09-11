#pragma once

#include "IDianaXWrapper.h"

class CDianaExecuterSignalHandler;

class CDianaStartUp;

class CDianaExecuter : public IDianaXWrapper
{
  CDianaStartUp* m_dsu;
public:
  CDianaExecuter();
  virtual ~CDianaExecuter();

  virtual void SetDisplayStopMessage(bool);
  virtual void SetShowDefaultMessages(bool);
  virtual void SetAutoCloseDialog(bool);

  virtual void SetProgressIsLifesign(bool);

  virtual void SetRunner(dia::IDianaRunner *runner);
  virtual short ExecuteDiana();
  virtual void ExecuteDianaCleanup();

  virtual bool GetCalculationResult();

private:
  //
  // not used !!!
  //
  CDianaExecuterSignalHandler* m_handler;

  //
  // not used !!!
  //
  bool m_bShowDefaultMessages;
};