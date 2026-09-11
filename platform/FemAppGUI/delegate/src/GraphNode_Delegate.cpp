#include "stdafx.h"

#include "FemAppMainWindow.h"
#include "GraphMainFrm.h"
#include "GraphNode_Delegate.h"

CGraphNode_Delegate::CGraphNode_Delegate(CGraphNode *graphNode) : m_graphNode(graphNode), m_treeNode(nullptr) {}

CGraphNode_Delegate::CGraphNode_Delegate(const QString &strInstanceName) : m_graphNode(0), m_treeNode(nullptr) {}

bool CGraphNode_Delegate::Attributes() { return true; }

CTreeNode *CGraphNode_Delegate::TreeNode() { return m_treeNode; }

void CGraphNode_Delegate::TreeNode(CTreeNode *treeNode) { m_treeNode = treeNode; }

void CGraphNode_Delegate::AppendToMenu(CMenu &menu) {}

void CGraphNode_Delegate::AppendContextMenu(CContextMenuInvoker &menu) {}

bool CGraphNode_Delegate::CanEdit() const { return true; }

bool CGraphNode_Delegate::Edit() { return true; }

bool CGraphNode_Delegate::OnCanPaste() const {
#ifndef _TEST_DELEGATE
  CGraphMainFrm *pMainFrame = (CGraphMainFrm *)FemAppGetMainWnd();

  if (pMainFrame->Paste()) {
    if (m_graphNode->CanConnectItem(*pMainFrame->Paste())) {
      return true;
    }
  }
#endif // _TEST_DELEGATE

  return false;
}

void CGraphNode_Delegate::OnPaste() {
#ifndef _TEST_DELEGATE
  CGraphMainFrm *pMainFrame = (CGraphMainFrm *)FemAppGetMainWnd();

  assert(pMainFrame->Paste());
  assert(m_graphNode->CanConnectItem(*pMainFrame->Paste()));
  m_graphNode->ConnectItem(*pMainFrame->Paste());
#endif // _TEST_DELEGATE
}

void CGraphNode_Delegate::OnCopy() {
#ifndef _TEST_DELEGATE
  CGraphMainFrm *pMainFrame = (CGraphMainFrm *)FemAppGetMainWnd();

  pMainFrame->Copy(*m_graphNode);
#endif // _TEST_DELEGATE
}

bool CGraphNode_Delegate::CanDestroy() const { return m_graphNode->CanDestroy(); }

bool CGraphNode_Delegate::Destroy() { return m_graphNode->Destroy(); }

void CGraphNode_Delegate::OnSelect(CGraphNode *previousSelected) {
  // Default no action
}
