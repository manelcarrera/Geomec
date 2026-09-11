#ifndef _OpenGLValueScene_Delegate_h_
#define _OpenGLValueScene_Delegate_h_

class COpenGLValueScene;

#include "OpenGLSceneNode_Delegate.h"

class COpenGLValueScene_Delegate : public COpenGLSceneNode_Delegate
{
public:
  COpenGLValueScene_Delegate(COpenGLValueScene* openGLValueScene);

private:
  COpenGLValueScene_Delegate(const COpenGLValueScene_Delegate& rhs);
  COpenGLValueScene_Delegate& operator = (const COpenGLValueScene_Delegate& rhs);

  COpenGLValueScene* m_openGLValueScene;

  REGISTER_DELEGATE(COpenGLValueScene, COpenGLValueScene_Delegate);
};

#endif  // _OpenGLValueScene_Delegate_h_
