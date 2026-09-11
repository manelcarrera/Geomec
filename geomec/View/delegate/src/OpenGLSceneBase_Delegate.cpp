#include "stdafx.h"

#include "OpenGLSceneBase_Delegate.h"
#include "OpenGLSceneBase.h"
#include "OpenInventorSceneNode.h"
#include "AttriScene.h" 

COpenGLSceneBase_Delegate::COpenGLSceneBase_Delegate(
  COpenGLSceneBase* openGLSceneBase)
: COpenGLValueScene_Delegate(openGLSceneBase)
, m_openGLSceneBase(openGLSceneBase)
{
}

bool COpenGLSceneBase_Delegate::Attributes()
{
    return false;
}

void COpenGLSceneBase_Delegate::AppendToMenu(CMenu& menu)
{
}
