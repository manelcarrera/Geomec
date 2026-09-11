// TreeNode.h: interface for the CTreeNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TREENODE_H__A1BDBD48_2A37_4582_82BD_7B5C111FE669__INCLUDED_)
#define AFX_TREENODE_H__A1BDBD48_2A37_4582_82BD_7B5C111FE669__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CGraphNode_Delegate;

#include "GraphNode.h"
#include "ITreeObject.h"

class CGraphTreeView;
// Using the graph it is nescarry to derive general non-linked objects from IGraphTreeNode, because
// it support the framework selection. Using the ITreeNode object results in crashes
class IGraphTreeObject : public ITreeObject {
protected:
  IGraphTreeObject(CTreeCtrl &ctrl);
  IGraphTreeObject(CTreeCtrl &ctrl, HTREEITEM hParent, HTREEITEM hInsertAfter);
  IGraphTreeObject(IGraphTreeObject &parent, HTREEITEM hInsertAfter);

public:
  virtual ~IGraphTreeObject();
};

class CTreeNode : public IGraphTreeObject {
  class CObserverNode : public CGraphNode {
    CTreeNode *m_pTreeNode;
    friend class CTreeNode;

  public:
    CObserverNode(CTreeNode &tree_node);
    virtual ~CObserverNode();
    unsigned int IconId() const { return 0; }
    unsigned int TypeId() const { return 0; }
    virtual void OnNewNeighbour(const CGraphNode &node);
    virtual void OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint);
    virtual void OnNeighbourDeleted(const CGraphNode &node);
  };

  friend class CObserverNode;
  CObserverNode *m_pObserver;

protected:
  CTreeNode(CTreeCtrl &ctrl);
  CTreeNode(CTreeCtrl &ctrl, HTREEITEM hParent, HTREEITEM hInsertAfter);
  CTreeNode(IGraphTreeObject &parent, HTREEITEM hInsertAfter);

public:
  virtual ~CTreeNode();
  CGraphNode &Observer() const;

public:
  // ITreeObject overrides
  virtual QString Text() const;
  virtual BOOL CanEditText() const;
  virtual BOOL EditText(const QString &strText);
  virtual BOOL operator<(const ICtrlObjectBase &object) const;

  // Icon's
  virtual unsigned int Icon() const;

  // Drag and drop
  virtual DROPEFFECT CanDrop(TCtrlObjectVec &vcDragged, BOOL bMove) const;
  virtual void OnDragLeave(TCtrlObjectVec &vcDragged) const;
  virtual void Drop(TCtrlObjectVec &vcDragged, BOOL bMove);

  // Delete actions
  virtual void Delete();
  virtual BOOL CanDelete() const;

  // Context menu
  virtual void AppendToMenu(CMenu &menu);
  virtual void AppendContextMenu(CContextMenuInvoker &invoker);

  // These functions only called by the node observer node to handle events of
  // the observed node ...
  virtual void OnNewNeighbour(const CGraphNode &node);
  virtual void OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint);
  virtual void OnNeighbourDeleted(const CGraphNode &node);

  // Observed children
  TGraphNodeSet ObservedChildren() const;

  // Events
  virtual void OnChildModified(CTreeNode &child);
  virtual void OnChildDeleted(CTreeNode &child);

  // Interface to the observed item ...
  virtual const CGraphNode &ObservedItem() const = 0;
  virtual CGraphNode &ObservedItem() = 0;

  virtual const CGraphNode_Delegate &Delegate() const = 0;
  virtual CGraphNode_Delegate &Delegate() = 0;
};

#endif // !defined(AFX_TREENODE_H__A1BDBD48_2A37_4582_82BD_7B5C111FE669__INCLUDED_)
