#ifndef _clWaitProgress_h_
#define _clWaitProgress_h_

#include "ProgressBase.h"

class CWaitProgress : public CProgressBase {
public:
  CWaitProgress();
  virtual ~CWaitProgress();

private:
  CWaitProgress(const CWaitProgress &rhs);
  CWaitProgress &operator=(const CWaitProgress &rhs);
};

#endif // _WaitProgress_h_
