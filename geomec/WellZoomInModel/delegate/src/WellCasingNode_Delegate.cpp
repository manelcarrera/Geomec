#include "stdafx.h"

#include "WellCasingNode.h"
#include "WellCasingNode_Delegate.h"

CWellCasingNode_Delegate::CWellCasingNode_Delegate(CWellCasingNode *wellCasingNode)
    : COpenGLNode_Delegate(wellCasingNode), m_wellCasingNode(wellCasingNode) {}
