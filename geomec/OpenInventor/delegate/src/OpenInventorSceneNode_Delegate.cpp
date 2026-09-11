#include "stdafx.h"

#include "OpenInventorSceneNode.h"
#include "OpenInventorSceneNode_Delegate.h"
#include "attriscene.h"

COpenInventorSceneNode_Delegate::COpenInventorSceneNode_Delegate(COpenInventorSceneNode *node)
    : CStorageNode_Delegate(node), m_Node(node) {}

bool COpenInventorSceneNode_Delegate::Attributes() {
  CAttriScene dlg(*m_Node);

  return dlg.DoModal() == IDOK;
}
