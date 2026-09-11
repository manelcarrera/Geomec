#include "StdAfx.h"

#include "OIVSceneBase_Delegate.h"
#include "OIVSceneBase.h"

COIVSceneBase_Delegate::COIVSceneBase_Delegate(COIVSceneBase* node)
: CStorageNode_Delegate(node),
  m_Node(node)
{
}
