#ifndef _clGraphMainFrameProgress_h_
#define _clGraphMainFrameProgress_h_

#include <qstring.h>

#include "ProgressBase.h"

class CGraphMainFrameProgress : public CProgressBase {
public:
  CGraphMainFrameProgress();
  virtual ~CGraphMainFrameProgress();

  virtual void AddSteps(int steps);
  virtual void Step(int steps = 1);
  virtual void StatusMessage(const QString &message);

private:
  CGraphMainFrameProgress(const CGraphMainFrameProgress &rhs);
  CGraphMainFrameProgress &operator=(const CGraphMainFrameProgress &rhs);

  QString m_message;
  bool m_messageSet;
};

#endif // _GraphMainFrameProgress_h_
