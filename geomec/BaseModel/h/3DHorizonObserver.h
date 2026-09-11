// 3DHorizonObserver.h: interface for the C3DHorizon class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_3DHORIZONOBSERVER_H__8766F7A6_0ADE_4556_8EEF_26DBE96605AC__INCLUDED_)
#define AFX_3DHORIZONOBSERVER_H__8766F7A6_0ADE_4556_8EEF_26DBE96605AC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HorizonBase.h"
#include "MeasuredTopDisplacementsNode.h"
#include "HorizonObserver.h"
#include "MeasuredTopDisplacementsNodeObserver.h"

template <bool bFixedSurface>
class C3DHorizonObserver : public CHorizonObserver<bFixedSurface>
{
public:
  C3DHorizonObserver(C3DHorizon& node,
           CTreeNode &parent,
           const BOOL rename,
           const enum REMOVE_TYPE remove,
           HTREEITEM hInsertAfter = TVI_LAST,
           const BOOL bInitialUpdate = TRUE)
           : CHorizonObserver<bFixedSurface>(node, parent, rename, remove, hInsertAfter, bInitialUpdate)
  {
  }

  C3DHorizonObserver(C3DHorizon& node,
           CTreeCtrl &ctrl,
           const BOOL rename,
           const enum REMOVE_TYPE remove,
           HTREEITEM hParent = TVI_ROOT,
           HTREEITEM hInsertAfter = TVI_LAST,
           const BOOL bInitialUpdate = TRUE)
           : CHorizonObserver<bFixedSurface>(ctrl, rename, remove, hParent, hInsertAfter, bInitialUpdate)
  {
  }

  virtual BOOL OnFilter(const CGraphNode& node) const
  {
  if(dynamic_cast<const CMeasuredTopDisplacementsNode*>(&node) != 0)
      return TRUE;

  return CHorizonObserver<bFixedSurface>::OnFilter(node);
  }

  virtual CTreeNode* InsertChild(CGraphNode& t)
  {
  CMeasuredTopDisplacementsNode* pMTDN = dynamic_cast<CMeasuredTopDisplacementsNode*>(&t);
  if(pMTDN)
  {
      return new TMeasuredTopDisplacementsNodeObserver(*pMTDN, *this, FALSE, FIXED_ITEM);
  }

  return CHorizonObserver<bFixedSurface>::InsertChild(t);
  }
};

template <bool bFixedSurface>
class C3DHorizonEntryObserver : public CStateBranch_Delegate<CGraphEntry, CGraphEntry_Delegate, C3DHorizon, C3DHorizonObserver<bFixedSurface>, CUndefinedIconProvider, TRUE, DELETE_ITEM>
{
  typedef CStateBranch_Delegate<CGraphEntry, CGraphEntry_Delegate, C3DHorizon, C3DHorizonObserver<bFixedSurface>, CUndefinedIconProvider, TRUE, DELETE_ITEM> TBase;
public:
  C3DHorizonEntryObserver(CGraphEntry& entry, CTreeCtrl& ctrl, const BOOL rename, const enum REMOVE_TYPE remove, HTREEITEM hParent = TVI_ROOT, HTREEITEM hInsertAfter = TVI_LAST)
  : TBase(entry, ctrl, rename, remove, hParent, hInsertAfter, FALSE)
  {
  Update();
  }

  virtual CTreeNode* InsertChild(C3DHorizon& t)
  {
  CTreeNode* pTreeNode = TBase::InsertChild(t);

    if(t.Slip())
  {
      new TFaultPressureEnumerator(*pTreeNode, _T("Pressures"), IDI_PRESSURES);
      new TFaultParametersEnumerationBranch(*pTreeNode, _T("Fault Parameters"),
    IDI_FAULTPARAMETERS);
  }

  return pTreeNode;
  }

  virtual void OnChildModified(CTreeNode &child)
  {
    CHorizonBase& hor = dynamic_cast<CHorizonBase&>(child.ObservedItem());

  if(!hor.IsCopy() && !hor.IsCopied())
  {
    if(hor.Slip())
    {
      bool bPressuresFound = false;
    bool bParametersFound = false;
      CTreeNode::TChildren vcChildren = child.Children();
      for(size_t i = 0; i < vcChildren.size(); ++i)
      {
        if(dynamic_cast<TFaultPressureEnumerator*>(vcChildren[i]))
          bPressuresFound = true;
          else if(dynamic_cast<TFaultParametersEnumerationBranch*>(
      vcChildren[i]))
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
          TFaultParametersEnumerationBranch* pe =
      dynamic_cast<TFaultParametersEnumerationBranch*>(vcChildren[i]);
        if(fpe)
          delete fpe;
          else if(pe)
      delete pe;
      }
    }
  }

  TBase::OnChildModified(child);
  }
};

#endif // !defined(AFX_3DHORIZONOBSERVER_H__8766F7A6_0ADE_4556_8EEF_26DBE96605AC__INCLUDED_)
