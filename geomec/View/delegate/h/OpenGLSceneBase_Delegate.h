#ifndef _OpenGLSceneBase_Delegate_h_
#define _OpenGLSceneBase_Delegate_h_

class COpenGLSceneBase;

#include "OpenGLValueScene_Delegate.h"

class COpenGLSceneBase_Delegate : public COpenGLValueScene_Delegate {
public:
  COpenGLSceneBase_Delegate(COpenGLSceneBase *openGLSceneBase);

  virtual bool Attributes();

  virtual void AppendToMenu(CMenu &menu);

private:
  COpenGLSceneBase_Delegate(const COpenGLSceneBase_Delegate &rhs);
  COpenGLSceneBase_Delegate &operator=(const COpenGLSceneBase_Delegate &rhs);

  COpenGLSceneBase *m_openGLSceneBase;

  REGISTER_DELEGATE(COpenGLSceneBase, COpenGLSceneBase_Delegate);
};

#endif // _OpenGLSceneBase_Delegate_h_
