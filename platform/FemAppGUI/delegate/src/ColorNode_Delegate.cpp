#include "stdafx.h"

#include "ColorNode_Delegate.h"
#include "colornode.h"

CColorNode_Delegate::CColorNode_Delegate(CColorNode *colorNode)
    : COpenGLNode_Delegate(colorNode), m_colorNode(colorNode) {}
