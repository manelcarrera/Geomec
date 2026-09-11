#ifndef _OpenGLNode_Delegate_h_
#define _OpenGLNode_Delegate_h_

class COpenGLNode;

#include "OpenGLNodeBase_Delegate.h"
#include "MouseListener.h"
#include "IOpenGLFrame.h"

class COpenGLNode_Delegate : public COpenGLNodeBase_Delegate
{
public:
  COpenGLNode_Delegate(COpenGLNode* openGLNode);

  typedef CMouseListener::TFrame TFrame;
  typedef IOpenGLFrame::TKeyboardModifiers TKeyboardModifiers;
  typedef IOpenGLFrame::TMouseButton TMouseButton;
  typedef QPoint TScreenPoint;
  typedef IOpenGLFrame::TObjectVec TObjectVec;

  virtual bool MouseDblClk(TFrame& frame, TKeyboardModifiers state,
  TMouseButton button, const TScreenPoint& point,const TObjectVec& vcHit);
  virtual bool MouseMove(TFrame& frame, TKeyboardModifiers state,
  TMouseButton button, const TScreenPoint& point, const TObjectVec& vcHit);
  virtual bool MousePress(TFrame& frame, TKeyboardModifiers state,
  TMouseButton button, const TScreenPoint& point, const TObjectVec& vcHit);
  virtual bool MouseRelease(TFrame& frame, TKeyboardModifiers state,
  TMouseButton button, const TScreenPoint& point, const TObjectVec& vcHit);

private:
  COpenGLNode_Delegate(const COpenGLNode_Delegate& rhs);
  COpenGLNode_Delegate& operator = (const COpenGLNode_Delegate& rhs);

  COpenGLNode* m_openGLNode;

  REGISTER_DELEGATE(COpenGLNode, COpenGLNode_Delegate);
};

#endif  // _OpenGLNode_Delegate_h_
