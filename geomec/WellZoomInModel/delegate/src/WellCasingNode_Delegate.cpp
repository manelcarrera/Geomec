#include "stdafx.h"

#include "WellCasingNode_Delegate.h"
#include "WellCasingNode.h"

CWellCasingNode_Delegate::CWellCasingNode_Delegate(
  CWellCasingNode* wellCasingNode)
: COpenGLNode_Delegate(wellCasingNode)
, m_wellCasingNode(wellCasingNode)
{
}
