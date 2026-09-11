// TreeNode.cpp: implementation of the CTreeNode class.
//
//////////////////////////////////////////////////////////////////////
#include "TreeNode.h"
#include "FemAppDoc.h"
#include "FemAppModel.h"
#include "GraphNode.h"
#include "GraphNode_Delegate.h"
#include "GraphTreeCtrl.h"
#include "GraphTreeView.h"
#include "graphptr.h"
#include "stdafx.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IGraphTreeObject::IGraphTreeObject(CTreeCtrl &ctrl) : ITreeObject(ctrl) {}

IGraphTreeObject::IGraphTreeObject(CTreeCtrl &ctrl, HTREEITEM hParent, HTREEITEM hInsertAfter)
    : ITreeObject(ctrl, hParent, hInsertAfter) {}

IGraphTreeObject::IGraphTreeObject(IGraphTreeObject &parent, HTREEITEM hInsertAfter)
    : ITreeObject(parent, hInsertAfter) {}

IGraphTreeObject::~IGraphTreeObject() {
  // Reset selection
  CGraphTreeCtrl *pCtrl = dynamic_cast<CGraphTreeCtrl *>(&Ctrl());
  if (pCtrl) {
    if (pCtrl->TreeView().m_pSelection == this)
      pCtrl->TreeView().m_pSelection = 0;
  }
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTreeNode::CObserverNode::CObserverNode(CTreeNode &tree_node) : CGraphNode(_T("")), m_pTreeNode(&tree_node) {};

CTreeNode::CObserverNode::~CObserverNode() {
  if (m_pTreeNode)
    m_pTreeNode->m_pObserver = 0;
  delete m_pTreeNode;
}

void CTreeNode::CObserverNode::OnNewNeighbour(const CGraphNode &node) {
  CGraphNode::OnNewNeighbour(node);
  // Redirected to the node observer ...
  if (m_pTreeNode)
    m_pTreeNode->OnNewNeighbour(node);
}

void CTreeNode::CObserverNode::OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint) {
  CGraphNode::OnNeighbourModified(node, uHint);
  // Redirected to the node observer ...
  if (m_pTreeNode)
    m_pTreeNode->OnNeighbourModified(node, uHint);
}

void CTreeNode::CObserverNode::OnNeighbourDeleted(const CGraphNode &node) {
  CGraphNode::OnNeighbourDeleted(node);
  // Redirected to the node observer ...
  if (m_pTreeNode)
    m_pTreeNode->OnNeighbourDeleted(node);
}

CTreeNode::CTreeNode(CTreeCtrl &ctrl) : IGraphTreeObject(ctrl), m_pObserver(0) {}

CTreeNode::CTreeNode(CTreeCtrl &ctrl, HTREEITEM hParent, HTREEITEM hInsertAfter)
    : IGraphTreeObject(ctrl, hParent, hInsertAfter), m_pObserver(0) {
  m_pObserver = new CObserverNode(*this);
}

CTreeNode::CTreeNode(IGraphTreeObject &parent, HTREEITEM hInsertAfter)
    : IGraphTreeObject(parent, hInsertAfter), m_pObserver(0) {
  m_pObserver = new CObserverNode(*this);
}

CTreeNode::~CTreeNode() {
  // Set the pointer to the tree node to zero
  if (m_pObserver)
    m_pObserver->m_pTreeNode = 0;

  delete m_pObserver;

  // Inform parent
  CTreeNode *pParent = dynamic_cast<CTreeNode *>(Parent());
  if (pParent)
    pParent->OnChildDeleted(*this);
}

QString CTreeNode::Text() const { return ObservedItem().Name(); }

BOOL CTreeNode::CanEditText() const { return FALSE; }

BOOL CTreeNode::EditText(const QString &strText) {
  assert(CanEditText());
  ObservedItem().Name(strText);
  return TRUE;
}

unsigned int CTreeNode::Icon() const { return ObservedItem().IconId(); }

DROPEFFECT CTreeNode::CanDrop(TCtrlObjectVec &vcDragged, BOOL bMove) const {
  if (vcDragged.size() == 1) {
    CTreeNode *pTreeNode = dynamic_cast<CTreeNode *>(vcDragged[0]);

    if ((pTreeNode) && (ObservedItem().CanConnectItem(pTreeNode->ObservedItem())))
      return DROPEFFECT_COPY;
  }

  return DROPEFFECT_NONE;
}

void CTreeNode::OnDragLeave(TCtrlObjectVec &vcDragged) const {
  if (vcDragged.size() == 1) {
    CTreeNode *pTreeNode = dynamic_cast<CTreeNode *>(vcDragged[0]);
    if (pTreeNode)
      ObservedItem().OnDragLeave(pTreeNode->ObservedItem());
  }
}

void CTreeNode::Drop(TCtrlObjectVec &vcDragged, BOOL bMove) {
  if (vcDragged.size() == 1) {
    CTreeNode *pTreeNode = dynamic_cast<CTreeNode *>(vcDragged[0]);

    if (pTreeNode)
      ObservedItem().ConnectItem(pTreeNode->ObservedItem());
  }
}

// Delete actions
void CTreeNode::Delete() {
  assert(CanDelete());
  ObservedItem().Destroy();
}

BOOL CTreeNode::CanDelete() const { return ObservedItem().CanDestroy(); }

/* NOT OBSOLETE */ void CTreeNode::AppendToMenu(CMenu &menu) {
  if (&Delegate())
    Delegate().AppendToMenu(menu);
}

/* NOT OBSOLETE */ void CTreeNode::AppendContextMenu(CContextMenuInvoker &invoker) {
  if (&Delegate()) {
    Delegate().TreeNode(this);
    Delegate().AppendContextMenu(invoker);
  }
}

void CTreeNode::OnNewNeighbour(const CGraphNode &node) {
  // Nothing happen ...
}

void CTreeNode::OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint) {
  // Update node and tell the parent, if any, that we are modified
  if (&ObservedItem() == &node) {
    // Update the current
    Update();
    CTreeNode *pTreeNode = dynamic_cast<CTreeNode *>(Parent());
    if (pTreeNode)
      pTreeNode->OnChildModified(*this);
  }
}

void CTreeNode::OnNeighbourDeleted(const CGraphNode &node) {
  if (&ObservedItem() == &node)
    delete this;
}

TGraphNodeSet CTreeNode::ObservedChildren() const {
  ITreeObject::TChildren vcChild = Children();

  TGraphNodeSet set;

  for (size_t i = 0; i < vcChild.size(); i++) {
    CTreeNode *pNode = dynamic_cast<CTreeNode *>(vcChild[i]);

    if (pNode)
      set.insert(&pNode->ObservedItem());
  }

  return set;
}

CGraphNode &CTreeNode::Observer() const {
  assert(m_pObserver);
  return (CGraphNode &)*m_pObserver;
}

void CTreeNode::OnChildModified(CTreeNode &child) {}

void CTreeNode::OnChildDeleted(CTreeNode &child) {}

BOOL CTreeNode::operator<(const ICtrlObjectBase &object) const {
  const CTreeNode *pNode = dynamic_cast<const CTreeNode *>(&object);

  if (pNode)
    return ObservedItem().Less(pNode->ObservedItem());

  return ITreeObject::operator<(object);
}
