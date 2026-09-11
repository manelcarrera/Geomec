#ifndef _WellCasingNode_Delegate_h_
#define _WellCasingNode_Delegate_h_

class CWellCasingNode;

#include "OpenGLNode_Delegate.h"

class CWellCasingNode_Delegate : public COpenGLNode_Delegate {
public:
  CWellCasingNode_Delegate(CWellCasingNode *wellCasingNode);

private:
  CWellCasingNode_Delegate(const CWellCasingNode_Delegate &rhs);
  CWellCasingNode_Delegate &operator=(const CWellCasingNode_Delegate &rhs);

  CWellCasingNode *m_wellCasingNode;

  REGISTER_DELEGATE(CWellCasingNode, CWellCasingNode_Delegate);
};

#endif // _WellCasingNode_Delegate_h_
