// HorizonObserver.h: interface for the CHorizonBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HORIZONOBSERVER_H__1518D152_E0E9_40c3_99A2_C8F4A58A081B__INCLUDED_)
#define AFX_HORIZONOBSERVER_H__1518D152_E0E9_40c3_99A2_C8F4A58A081B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GeoSurface.h"
#include "openglnodeobserver.h"
#include "Valuetype.h"
#include "ValueCompositeObserver.h"
#include "FaultParametersNode.h"
#include "OpenGLNodeObserver_Delegate.h"
#include "HorizonBase_Delegate.h"

class CFaultPressure;

#include "FaultParametersNode_Delegate.h"
#include "StateBranch_Delegate.h"
#include "EnumerationBranch.h"

class CSurfaceBase_Delegate;

#include "FaultParametersEnumerationBranch.h"
#include "UndefinedIconProvider.h"

// Observer shows surfaces and distributed fault properties
template<bool bFixedSurface>
class CHorizonObserver : public COpenGLNodeObserver_Delegate<CHorizonBase, CHorizonBase_Delegate, CGraphNode, CDummyObserver, TRUE, UNLINK_ITEM>
{
  typedef COpenGLNodeObserver_Delegate<CSurfaceBase, CSurfaceBase_Delegate, CDummyNode, CDummyObserver, FALSE, FIXED_ITEM> TSurfaceObserver;
public:
  CHorizonObserver(CHorizonBase& node,
           CTreeNode &parent,
           const BOOL rename,
           const enum REMOVE_TYPE remove,
           HTREEITEM hInsertAfter = TVI_LAST,
           const BOOL bInitialUpdate = TRUE)
  : COpenGLNodeObserver_Delegate<CHorizonBase, CHorizonBase_Delegate, CGraphNode, CDummyObserver, TRUE, UNLINK_ITEM> (node, parent, rename, remove, hInsertAfter, false)
  {
    Update();
  }
  CHorizonObserver(CHorizonBase& node,
           CTreeCtrl &ctrl,
           const BOOL rename,
           const enum REMOVE_TYPE remove,
           HTREEITEM hParent = TVI_ROOT,
           HTREEITEM hInsertAfter = TVI_LAST,
           const BOOL bInitialUpdate = TRUE)
  : COpenGLNodeObserver_Delegate<CHorizonBase, CHorizonBase_Delegate, CGraphNode, CDummyObserver, TRUE, UNLINK_ITEM> (node, ctrl, rename, remove, hParent, hInsertAfter)
  {
    Update();
  }

  virtual CTreeNode* InsertChild(CGraphNode& t)
  {
    CSurfaceBase* pSurface = dynamic_cast<CSurfaceBase*>(&t);

    if(pSurface)
    {
      if(bFixedSurface)
        return new TSurfaceObserver(*pSurface, *this, FALSE, FIXED_ITEM, TVI_LAST, TRUE);
      else
        return new TSurfaceObserver(*pSurface, *this, FALSE, UNLINK_ITEM, TVI_LAST, TRUE);
    }
    return 0;
  }

  virtual BOOL OnFilter(const CGraphNode& t) const
  {
    const CSurfaceBase* pSurface = dynamic_cast<const CSurfaceBase*>(&t);
  return pSurface != 0;
  }
};

typedef CEnumerationBranch<CFaultPressure, CValueCompositeObserver, FALSE, FIXED_ITEM> TFaultPressureEnumerator;

typedef CNodeObserver_Delegate <CFaultParametersNode,
  CFaultParametersNode_Delegate, CValueType, CValueCompositeObserver, FALSE,
  UNLINK_ITEM> TFaultParametersNodeObserver;
typedef CFaultParametersEnumerationBranch <CFaultParametersNode,
  TFaultParametersNodeObserver, FALSE, FIXED_ITEM>
  TFaultParametersEnumerationBranch;

template <bool bFixedSurface>
class CHorizonEntryObserver : public CStateBranch_Delegate<CGraphEntry, CGraphEntry_Delegate, CHorizonBase, CHorizonObserver<bFixedSurface>, CUndefinedIconProvider, TRUE, DELETE_ITEM>
{
  typedef CStateBranch_Delegate<CGraphEntry, CGraphEntry_Delegate, CHorizonBase, CHorizonObserver<bFixedSurface>, CUndefinedIconProvider, TRUE, DELETE_ITEM> TBase;
public:
  CHorizonEntryObserver(CGraphEntry& entry, CTreeCtrl& ctrl, const BOOL rename, const enum REMOVE_TYPE remove, HTREEITEM hParent = TVI_ROOT, HTREEITEM hInsertAfter = TVI_LAST);
  virtual CTreeNode* InsertChild(CHorizonBase& t);
  virtual void OnChildModified(CTreeNode &child);			// Called when child is modified
};

template <bool bFixedSurface>
CHorizonEntryObserver<bFixedSurface>::CHorizonEntryObserver(CGraphEntry& entry, CTreeCtrl& ctrl, const BOOL rename, const enum REMOVE_TYPE remove, HTREEITEM hParent, HTREEITEM hInsertAfter)
: TBase(entry, ctrl, rename, remove, hParent, hInsertAfter, TRUE, FALSE)
{
  Update();
}

template <bool bFixedSurface>
CTreeNode* CHorizonEntryObserver<bFixedSurface>::InsertChild(CHorizonBase& t)
{
  CTreeNode* pTreeObserver = TBase::InsertChild(t);

  if(t.Slip())
  {
    new TFaultPressureEnumerator(*pTreeObserver, _T("Pressures"), IDI_PRESSURES);
  new TFaultParametersEnumerationBranch(*pTreeObserver,
      _T("Fault Parameters"), IDI_FAULTPARAMETERS);
  }

  return pTreeObserver;
}

template <bool bFixedSurface>
void CHorizonEntryObserver<bFixedSurface>::OnChildModified(CTreeNode &child)
{
  CHorizonBase& hor = dynamic_cast<CHorizonBase&>(child.ObservedItem());

  if(hor.Slip())
  {
    bool bPressuresFound = false;
  bool bParametersFound = false;
    CTreeNode::TChildren vcChildren = child.Children();
    for(size_t i = 0; i < vcChildren.size(); ++i)
    {
      if(dynamic_cast<TFaultPressureEnumerator*>(vcChildren[i]))
        bPressuresFound = true;
      else if(dynamic_cast<TFaultParametersEnumerationBranch*>(vcChildren[i]))
    bParametersFound = true;
    }

    if(!bPressuresFound)
      new TFaultPressureEnumerator(child, _T("Pressures"), IDI_PRESSURES);
  if(!bParametersFound)
      new TFaultParametersEnumerationBranch(child, _T("Fault Parameters"),
    IDI_FAULTPARAMETERS);
  }
  else
  {
    CTreeNode::TChildren vcChildren = child.Children();
    for(size_t i = 0; i < vcChildren.size(); ++i)
    {
      TFaultPressureEnumerator* fpe = dynamic_cast<TFaultPressureEnumerator*>(vcChildren[i]);
      if(fpe)
        delete fpe;

      TFaultParametersEnumerationBranch* pe =
    dynamic_cast<TFaultParametersEnumerationBranch*>(vcChildren[i]);
      if(pe)
    delete pe;
    }
  }

  TBase::OnChildModified(child);
}

#endif // !defined(AFX_HORIZONOBSERVER_H__1518D152_E0E9_40c3_99A2_C8F4A58A081B__INCLUDED_)
