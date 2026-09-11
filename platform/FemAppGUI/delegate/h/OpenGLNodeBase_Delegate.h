#ifndef _OpenGLNodeBase_Delegate_h_
#define _OpenGLNodeBase_Delegate_h_

class COpenGLNodeBase;

#include "StorageNode_Delegate.h"

class COpenGLNodeBase_Delegate : public CStorageNode_Delegate {
public:
  COpenGLNodeBase_Delegate(COpenGLNodeBase *openGLNodeBase);

private:
  COpenGLNodeBase_Delegate(const COpenGLNodeBase_Delegate &rhs);
  COpenGLNodeBase_Delegate &operator=(const COpenGLNodeBase_Delegate &rhs);

  COpenGLNodeBase *m_openGLNodeBase;

  REGISTER_DELEGATE(COpenGLNodeBase, COpenGLNodeBase_Delegate);
};

#endif // _OpenGLNodeBase_Delegate_h_
