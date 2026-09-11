#pragma once

class QString;

namespace dia {
class IDianaRunner;
}

class IDianaXWrapper {
public:
  IDianaXWrapper() {};
  virtual ~IDianaXWrapper() {};

  virtual void SetDisplayStopMessage(bool) = 0;
  virtual void SetShowDefaultMessages(bool) = 0;
  virtual void SetAutoCloseDialog(bool) = 0;

  virtual void SetProgressIsLifesign(bool) = 0;

  virtual void SetRunner(dia::IDianaRunner *runner) = 0;

  virtual short ExecuteDiana() = 0;
  virtual void ExecuteDianaCleanup() = 0;

  virtual bool GetCalculationResult() = 0;
};