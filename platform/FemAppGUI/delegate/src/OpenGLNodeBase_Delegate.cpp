#include "stdafx.h"

#include "OpenGLNodeBase.h"
#include "OpenGLNodeBase_Delegate.h"

COpenGLNodeBase_Delegate::COpenGLNodeBase_Delegate(COpenGLNodeBase *openGLNodeBase)
    : CStorageNode_Delegate(openGLNodeBase), m_openGLNodeBase(openGLNodeBase) {}
