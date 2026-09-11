#ifndef _FaultParametersNode_Delegate_h_
#define _FaultParametersNode_Delegate_h_

class CFaultParametersNode;

#include "StorageNode_Delegate.h"

class CFaultParametersNode_Delegate : public CStorageNode_Delegate {
public:
  CFaultParametersNode_Delegate(CFaultParametersNode *faultParametersNode);

  virtual bool Attributes();

private:
  CFaultParametersNode_Delegate(const CFaultParametersNode_Delegate &rhs);
  CFaultParametersNode_Delegate &operator=(const CFaultParametersNode_Delegate &rhs);

  CFaultParametersNode *m_faultParametersNode;

  REGISTER_DELEGATE(CFaultParametersNode, CFaultParametersNode_Delegate);
};

#endif // _FaultParametersNode_Delegate_h_
