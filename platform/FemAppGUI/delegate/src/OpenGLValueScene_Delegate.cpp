#include "stdafx.h"

#include "OpenGLValueScene_Delegate.h"
#include "OpenGLValueScene.h"

COpenGLValueScene_Delegate::COpenGLValueScene_Delegate(
  COpenGLValueScene* openGLValueScene)
: COpenGLSceneNode_Delegate(openGLValueScene)
, m_openGLValueScene(openGLValueScene)
{
}
