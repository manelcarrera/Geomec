#include "stdafx.h"

#include "OpenGLValueScene.h"
#include "OpenGLValueScene_Delegate.h"

COpenGLValueScene_Delegate::COpenGLValueScene_Delegate(COpenGLValueScene *openGLValueScene)
    : COpenGLSceneNode_Delegate(openGLValueScene), m_openGLValueScene(openGLValueScene) {}
