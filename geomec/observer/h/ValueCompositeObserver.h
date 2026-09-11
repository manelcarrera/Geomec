// ValueCompositeObserver.h: interface for the CValueCompositeObserver class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VALUECOMPOSITEOBSERVER_H__0EBF847A_1F90_4651_AC56_49D81A49E99E__INCLUDED_)
#define AFX_VALUECOMPOSITEOBSERVER_H__0EBF847A_1F90_4651_AC56_49D81A49E99E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DeletableNodeObserver_Delegate.h"
#include "IValueComposite_Delegate.h"
#include "IValueComponent_Delegate.h"

typedef CNodeObserver_Delegate<IValueComponentBase, IValueComponent_Delegate, CDummyNode, CDummyObserver, FALSE, FIXED_ITEM> TValueComponentObserver;
	
typedef CDeletableNodeObserver_Delegate<IValueComposite, IValueComposite_Delegate, CStorageNode, CDummyObserver, FALSE, FIXED_ITEM> TValueBaseObserver;

class CValueCompositeObserver : public TValueBaseObserver
{
	unsigned int m_uMode;
public:
	CValueCompositeObserver(IValueComposite& node,
							CTreeNode &parent,
							const BOOL rename,
							const enum REMOVE_TYPE remove,
							HTREEITEM hInsertAfter = TVI_LAST,
							BOOL bInitialUpdate = FALSE,
							BOOL bSortAfterInsertion = TRUE );

	CValueCompositeObserver(IValueComposite& node,
							CTreeCtrl &ctrl,
							const BOOL rename,
							const enum REMOVE_TYPE remove,
							HTREEITEM hParent = TVI_ROOT,
							HTREEITEM hInsertAfter = TVI_LAST,
							BOOL bInitialUpdate = FALSE,
							BOOL bSortAfterInsertion = TRUE);

// The filter
	virtual CTreeNode* InsertChild(CStorageNode& t);
	virtual BOOL OnFilter(const CStorageNode& t) const;
};

class CValueReferenceObserver : public CValueCompositeObserver
{
  unsigned int m_uMode;
public:
  CValueReferenceObserver(IValueComposite& node,
              CTreeNode& parent,
              const BOOL rename,
              const enum REMOVE_TYPE remove,
              HTREEITEM hInsertAfter = TVI_LAST,
              BOOL bInitialUpdate = FALSE,
              BOOL bSortAfterInsertion = TRUE );

  CValueReferenceObserver(IValueComposite& node,
              CTreeCtrl& ctrl,
              const BOOL rename,
              const enum REMOVE_TYPE remove,
              HTREEITEM hParent = TVI_ROOT,
              HTREEITEM hInsertAfter = TVI_LAST,
              BOOL bInitialUpdate = FALSE,
              BOOL bSortAfterInsertion = TRUE);

// The filter
  virtual BOOL OnFilter(const CStorageNode& t) const;
};

#endif // !defined(AFX_VALUECOMPOSITEOBSERVER_H__0EBF847A_1F90_4651_AC56_49D81A49E99E__INCLUDED_)
