#ifndef _clDualProgress_h_
#define _clDualProgress_h_

#include <qstring.h>

#include "ProgressBase.h"

class CDualProgress : public CProgressBase
{
public:
  CDualProgress(int jobs, const QString& primaryTitle, bool cancel = true);
  virtual ~CDualProgress();

  virtual void AddSteps(int steps);
  virtual void Step(int steps = 1);
  virtual void StatusMessage(const QString& message);

  virtual void NextJob(const QString& secondaryTitle);

private:
  CDualProgress(const CDualProgress& rhs);
  CDualProgress& operator = (const CDualProgress& rhs);

  QString m_secondaryTitle;
  int m_totalSteps;
  int m_currentSteps;
};

#endif  // _DualProgress_h_
