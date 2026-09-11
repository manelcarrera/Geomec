#pragma once

#include "ProgressBase.h"

class CWaitProgress : public CProgressBase {
public:
  CWaitProgress();
  virtual ~CWaitProgress();

private:
  CWaitProgress(const CWaitProgress &rhs);
  CWaitProgress &operator=(const CWaitProgress &rhs);

  CWaitCursor m_waitCursor;
};