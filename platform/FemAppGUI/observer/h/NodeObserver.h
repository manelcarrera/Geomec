// NodeObserver.h: interface for the CNodeObserver class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NODEOBSERVER_H__F559CC3C_1A5F_4724_AAC7_303FA0E0B233__INCLUDED_)
#define AFX_NODEOBSERVER_H__F559CC3C_1A5F_4724_AAC7_303FA0E0B233__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GraphNode_Delegate.h"
#include "TreeNode.h"
#include "ChildEnumerator.h"
#include "ivaluecomposite.h"
#include "treectrlbase.h"

class CDummyNode : public CGraphNode 
{
public:
  CDummyNode(const CString strName)
    : CGraphNode((LPCSTR) strName){}
};

enum REMOVE_TYPE { UNLINK_ITEM, DELETE_ITEM, FIXED_ITEM, TREE_DELETE_ITEM };	// Remove types
class CDummyObserver : public CTreeNode 
{
public:
  CDummyNode *m_pDummy;
  CDummyObserver(CGraphNode& node,
          CTreeNode &parent,
          const BOOL rename,
          const enum REMOVE_TYPE remove,
          HTREEITEM hInsertAfter = TVI_LAST) 
          : CTreeNode(parent.Ctrl()) {}

  // Interface supplied by the client class ...
  virtual void Delete() { assert(FALSE); }
  virtual BOOL CanDelete() const { assert(FALSE); return false; }
  virtual BOOL CanEditTest() const { assert(FALSE); return false; }
  virtual const CGraphNode& ObservedItem() const { assert(FALSE); return *m_pDummy;}
  virtual CGraphNode& ObservedItem() { assert(FALSE); return *m_pDummy;}
  virtual const CGraphNode_Delegate& Delegate() const { assert(FALSE); CGraphNode_Delegate* pBogus = 0; return *pBogus;}
  virtual CGraphNode_Delegate& Delegate() { assert(FALSE); CGraphNode_Delegate* pBogus = 0; return *pBogus;}
};


// OBSERVED_TYPE	Type of the graphnode observed by this observer
// CHILD_TYPE		Type of the childs enumerated by this node
// CHILD_OBS_TYPE	Type of the observer type
// RENAME			Rename or not
// REMOVE			Remove

template<class OBSERVED_TYPE, class CHILD_TYPE, class CHILD_OBS_TYPE, BOOL RENAME, enum REMOVE_TYPE REMOVE>
class CNodeObserver : public CChildEnumerator<CHILD_TYPE>
{
protected:
  OBSERVED_TYPE&	m_node;		// Reference to observed item ...
private:
  const enum REMOVE_TYPE m_remove;
  const BOOL m_rename;
public:
  typedef OBSERVED_TYPE node_type;							// Type of the node
  typedef CHILD_TYPE child_type;
  typedef CHILD_OBS_TYPE child_observer_type;

  CNodeObserver(OBSERVED_TYPE& node,
          CTreeNode &parent,
          const BOOL rename,
          const enum REMOVE_TYPE remove,
          HTREEITEM hInsertAfter = TVI_LAST,
          BOOL bInitialUpdate = TRUE,
          BOOL bSortAfterInsertion = TRUE )
  : CChildEnumerator<CHILD_TYPE>(parent.Ctrl(), parent.Handle(), hInsertAfter), m_node(node), m_remove(remove), m_rename(rename)
  {
    Observer().reParent(&node);

    // Set the pointer ...
    Ctrl().SetItemData(Handle(), (DWORD_PTR) this);

    if(bInitialUpdate)
      Update();

    if(bSortAfterInsertion)
      parent.SortChildren();

    assert(Handle());
  CGraphNode & observedItem = parent.ObservedItem();

#if 0
  // fails in OpenInventor version when opening a zoom-in model
    assert(node.IsLinkedTo(observedItem));
#endif
  }

  CNodeObserver(OBSERVED_TYPE& node,
          CTreeCtrl &ctrl,
          const BOOL rename,
          const enum REMOVE_TYPE remove,
          HTREEITEM hParent = TVI_ROOT,
          HTREEITEM hInsertAfter = TVI_LAST,
          BOOL bInitialUpdate = TRUE,
          BOOL bSortAfterInsertion = TRUE)
  : CChildEnumerator<CHILD_TYPE>(ctrl, hParent, hInsertAfter), m_node(node), m_remove(remove), m_rename(rename)
  {
    // Link observer node to the observer ...
    Observer().reParent(&node);

    // Set the pointer ...
    Ctrl().SetItemData(Handle(), (DWORD_PTR) this);

    if(bInitialUpdate)
      Update();

    if(Parent() && bSortAfterInsertion)
      Parent()->SortChildren();

    assert(Handle());
    assert(remove != UNLINK_ITEM);
  }

  virtual CTreeNode* InsertChild(CHILD_TYPE& t)
  {
    return new CHILD_OBS_TYPE(t,
                  *this,
                  RENAME,
                  REMOVE );
  }

  virtual void Delete()
  {
    assert(CanDelete());
    switch(m_remove)
    {
    case DELETE_ITEM:
      static_cast <CGraphNode_Delegate *>(m_node.getDelegate())->Destroy();
      break;

    case UNLINK_ITEM:
      assert(Parent());
      m_node.UnLink(((CTreeNode*)Parent())->ObservedItem());
      break;

  case TREE_DELETE_ITEM:
  {
      auto p = Parent();
      static_cast <CGraphNode_Delegate *>(m_node.getDelegate())->Destroy();
      ((CTreeNode*)p)->Update();
      break;
  }

  default:
      assert(FALSE);	// Bad definition ...
      break;
    }
  }

  virtual BOOL CanDelete() const
  {
    switch(m_remove) 
    {
    case FIXED_ITEM:
      return FALSE;
      break;
    case DELETE_ITEM:
      return m_node.CanDestroy();
      break;
    case UNLINK_ITEM:
      assert(Parent());
      return (m_node.CanDisconnectItem(((CTreeNode*)Parent())->ObservedItem())) &&
           (((CTreeNode*)Parent())->ObservedItem().CanDisconnectItem(m_node));
      break;
  case TREE_DELETE_ITEM:
      return m_node.CanDestroy();
    default:
      assert(FALSE);	// Bad definition ...
    }

  assert(FALSE);
  return FALSE;
  }

  virtual BOOL CanModify() const
  {
    switch(m_remove)
    {
    case FIXED_ITEM:
      return FALSE;
      break;
    }

  // TODO DELEGATE

  CGraphNode_Delegate* graphNode_Delegate =
      static_cast <CGraphNode_Delegate*> (m_node.getDelegate());

  return graphNode_Delegate->CanEdit();
  }

  virtual void Modify()
  {
    assert(CanModify());
    switch(m_remove)
    {
    case DELETE_ITEM:
    case UNLINK_ITEM:
      {
    // TODO DELEGATE

    CGraphNode_Delegate* graphNode_Delegate =
          static_cast <CGraphNode_Delegate*> (m_node.getDelegate());

    graphNode_Delegate->Edit();
      }
      break;
    default:
      assert(FALSE);	// Bad definition ...
      break;
    }
  }

  virtual BOOL CanEditText() const
  {
    return m_rename == TRUE ? true: false ;
  }

  virtual const CGraphNode& ObservedItem() const
  {
    return m_node;
  }
  virtual CGraphNode& ObservedItem()
  {
    return m_node;
  }

  virtual const CGraphNode_Delegate& Delegate() const
  {
    assert(FALSE);
  CGraphNode_Delegate* pBogus = 0;
  return *pBogus;
  }
  virtual CGraphNode_Delegate& Delegate()
  {
    assert(FALSE);
  CGraphNode_Delegate* pBogus = 0;
  return *pBogus;
  }
};

#endif // !defined(AFX_NODEOBSERVER_H__F559CC3C_1A5F_4724_AAC7_303FA0E0B233__INCLUDED_)
