#include "stdafx.h"

#include "AttriGlobalStressDlg.h"
#include "Geomec.h"
#include "GlobalInitialStressNode.h"
#include "GlobalInitialStressNode_Delegate.h"

CGlobalInitialStressNode_Delegate::CGlobalInitialStressNode_Delegate(CGlobalInitialStressNode *globalInitialStressNode)
    : CStorageNode_Delegate(globalInitialStressNode), m_globalInitialStressNode(globalInitialStressNode) {}

bool CGlobalInitialStressNode_Delegate::Attributes() {
  CAttriGlobalStressDlg dlg(*m_globalInitialStressNode);

  return dlg.DoModal() == IDOK;
}
