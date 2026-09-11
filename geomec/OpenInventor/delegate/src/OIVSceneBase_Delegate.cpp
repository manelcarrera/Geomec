#include "StdAfx.h"

#include "OIVSceneBase.h"
#include "OIVSceneBase_Delegate.h"

COIVSceneBase_Delegate::COIVSceneBase_Delegate(COIVSceneBase *node) : CStorageNode_Delegate(node), m_Node(node) {}
