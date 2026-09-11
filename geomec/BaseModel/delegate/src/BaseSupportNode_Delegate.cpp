#include "stdafx.h"

#include "BaseSupportNode_Delegate.h"
#include "BaseSupportNode.h"

CSupportDepletion_Delegate::CSupportDepletion_Delegate(CSupportDepletion* supportDepletion)
: CStorageNode_Delegate(supportDepletion)
, m_supportDepletion(supportDepletion)
{
}

CBaseSupportNode_Delegate::CBaseSupportNode_Delegate(CBaseSupportNode* baseSupportNode)
: COpenGLNode_Delegate(baseSupportNode)
, m_baseSupportNode(baseSupportNode)
{
}
