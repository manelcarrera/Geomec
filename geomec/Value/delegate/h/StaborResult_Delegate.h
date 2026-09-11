#ifndef _StaborResult_Delegate_h_
#define _StaborResult_Delegate_h_

class CStaborResult;

#include "Result_Delegate.h"

class CStaborResult_Delegate : public IResult_Delegate {
public:
  CStaborResult_Delegate(CStaborResult *staborResult);

private:
  CStaborResult_Delegate(const CStaborResult_Delegate &rhs);
  CStaborResult_Delegate &operator=(const CStaborResult_Delegate &rhs);

  CStaborResult *m_staborResult;

  REGISTER_DELEGATE(CStaborResult, CStaborResult_Delegate);
};

#endif // _StaborResult_Delegate_h_
