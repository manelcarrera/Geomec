#ifndef _IDualProgressBase_h_
#define _IDualProgressBase_h_

#include "ProgressBase.h"

class IDualProgressBase : public CProgressBase
{
public:
  IDualProgressBase();
  virtual ~IDualProgressBase();

  // The interface for the bottom progress bar.

  virtual void AddSteps(int steps);
  virtual void Step(int steps = 1);
  virtual void StatusMessage(const QString& message);

  virtual void NextJob(const QString& secondaryTitle);

private:
  IDualProgressBase(const IDualProgressBase& rhs);
  IDualProgressBase& operator = (const IDualProgressBase& rhs);
};

#endif  // _IDualProgressBase_h_
