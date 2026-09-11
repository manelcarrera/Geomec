#ifndef _SILENT_PROGRESS_H_
#define _SILENT_PROGRESS_H_

#include "ProgressBase.h"

class CSilentProgress : public CProgressBase
{
public:
  CSilentProgress() {}
  virtual ~CSilentProgress() {}

  virtual void AddSteps(int steps) {}
  virtual void Step(int steps = 1) {}
  virtual void StatusMessage(const QString& message) {}

private:
  CSilentProgress(const CSilentProgress& rhs);
  CSilentProgress& operator = (const CSilentProgress& rhs);
};

#endif