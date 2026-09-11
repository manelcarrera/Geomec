#pragma once

#include "ProgressBase.h"

class CDualSilentProgress : public CProgressBase {
public:
  CDualSilentProgress() {}
  virtual ~CDualSilentProgress() {}

  virtual void AddSteps(int steps) {}
  virtual void Step(int steps = 1) {}
  virtual void StatusMessage(const QString &message) {}

  virtual void NextJob(const QString &secondaryTitle) {}

private:
  CDualSilentProgress(const CDualSilentProgress &rhs);
  CDualSilentProgress &operator=(const CDualSilentProgress &rhs);
};