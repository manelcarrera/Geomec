#ifndef _OpenGLSceneNode_Delegate_h_
#define _OpenGLSceneNode_Delegate_h_

class COpenGLSceneNode;

#include "StorageNode_Delegate.h"

class COpenGLSceneNode_Delegate : public CStorageNode_Delegate {
public:
  COpenGLSceneNode_Delegate(COpenGLSceneNode *openGLSceneNode);

private:
  COpenGLSceneNode_Delegate(const COpenGLSceneNode_Delegate &rhs);
  COpenGLSceneNode_Delegate &operator=(const COpenGLSceneNode_Delegate &rhs);

  COpenGLSceneNode *m_openGLSceneNode;

  REGISTER_DELEGATE(COpenGLSceneNode, COpenGLSceneNode_Delegate);
};

#endif // _OpenGLSceneNode_Delegate_h_
