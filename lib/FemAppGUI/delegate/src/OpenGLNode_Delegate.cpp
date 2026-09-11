#include "stdafx.h"

#include "OpenGLNode_Delegate.h"
#include "openglnode.h"

COpenGLNode_Delegate::COpenGLNode_Delegate(COpenGLNode* openGLNode)
: COpenGLNodeBase_Delegate(openGLNode)
, m_openGLNode(openGLNode)
{
}

bool COpenGLNode_Delegate::MouseDblClk(TFrame& frame, TKeyboardModifiers state,
  TMouseButton button, const TScreenPoint& point, const TObjectVec& vcHit)
{
  return false; // Default not handled
}

bool COpenGLNode_Delegate::MouseMove(TFrame& frame, TKeyboardModifiers state,
  TMouseButton button, const TScreenPoint& point, const TObjectVec& vcHit)
{
  return false; // Default not handled
}

bool COpenGLNode_Delegate::MousePress(TFrame& frame, TKeyboardModifiers state,
  TMouseButton button, const TScreenPoint& point, const TObjectVec& vcHit)
{
  return false; // Default not handled
}

bool COpenGLNode_Delegate::MouseRelease(TFrame& frame,
  TKeyboardModifiers state, TMouseButton button, const TScreenPoint& point,
  const TObjectVec& vcHit)
{
  return false; // Default not handled
}
