#ifndef _ColorNode_Delegate_h_
#define _ColorNode_Delegate_h_

class CColorNode;

#include "OpenGLNode_Delegate.h"

class CColorNode_Delegate : public COpenGLNode_Delegate {
public:
  CColorNode_Delegate(CColorNode *colorNode);

private:
  CColorNode_Delegate(const CColorNode_Delegate &rhs);
  CColorNode_Delegate &operator=(const CColorNode_Delegate &rhs);

  CColorNode *m_colorNode;

  REGISTER_DELEGATE(CColorNode, CColorNode_Delegate);
};

#endif // _ColorNode_Delegate_h_
