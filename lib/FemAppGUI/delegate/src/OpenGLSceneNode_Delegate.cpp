#include "stdafx.h"

#include "OpenGLSceneNode_Delegate.h"
#include "OpenGLSceneNode.h"

COpenGLSceneNode_Delegate::COpenGLSceneNode_Delegate(
  COpenGLSceneNode* openGLSceneNode)
: CStorageNode_Delegate(openGLSceneNode)
, m_openGLSceneNode(openGLSceneNode)
{
}
