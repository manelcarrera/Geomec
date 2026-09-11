#ifndef _FractTensor_Delegate_h_
#define _FractTensor_Delegate_h_

class CFractTensor;

#include "MatParamsTensor_Delegate.h"

class CFractTensor_Delegate : public CMatParamsTensor_Delegate {
public:
  CFractTensor_Delegate(CFractTensor *fractTensor);

private:
  CFractTensor_Delegate(const CFractTensor_Delegate &rhs);
  CFractTensor_Delegate &operator=(const CFractTensor_Delegate &rhs);

  CFractTensor *m_fractTensor;

  REGISTER_DELEGATE(CFractTensor, CFractTensor_Delegate);
};

#endif // _FractTensor_Delegate_h_
