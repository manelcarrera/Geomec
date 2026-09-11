#if !defined(POINTSET_ENTRY_OBSERVER_INCLUDED_)
#define POINTSET_ENTRY_OBSERVER_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class IPointSet_Delegate;

#include "PointSet.h"
#include "ValueCompositeObserver.h"
#include "StateEnumerationBranch.h"
#include "UndefinedIconProvider.h"

typedef CGraphEntryTemp_Delegate <IPointSet> TPointSetEntry_Delegate;
typedef CNodeObserver_Delegate<TPointSetEntry, TPointSetEntry_Delegate, CDummyNode, CDummyObserver, FALSE, FIXED_ITEM> TPointSetEntryObserver;

// TODO AppendContextMenu
// CPointSetEntryObserver is not derived from CGraphNode, hence
// the function AppendContextMenu does not ask for a delegate. However one
// could verify that this function is never used!

class CPointSetEntryObserver : public TPointSetEntryObserver
{
	typedef CNodeObserver_Delegate<IPointSet, IPointSet_Delegate, CDummyNode, CDummyObserver, TRUE, DELETE_ITEM> TPointSetObserver;
	typedef CEnumerationBranch<IValueComposite, CValueCompositeObserver, TRUE, FIXED_ITEM> TPropertyEnumerator;
	class CPropertyEnumerator : public TPropertyEnumerator
	{
	public:
		CPropertyEnumerator(TPointSetObserver &parent_observer, 
							const CString& strName,
							const unsigned int uIcon,
							HTREEITEM hInsertAfter = TVI_LAST);
		virtual BOOL OnFilter(const child_type& t) const;
	};

	typedef CNodeObserver_Delegate<IPointSet, IPointSet_Delegate, CDummyNode, CDummyObserver, TRUE, DELETE_ITEM> TPointSetObserver;
  typedef CStateEnumerationBranch <IPointSet, TPointSetObserver, TRUE, DELETE_ITEM, CUndefinedIconProvider> TStateEnumerationBranch;
  class CPointSetSubBranch : public TStateEnumerationBranch
	{
		const CPointSet::DIMENSION m_dim;
	public:
		CPointSetSubBranch(CPointSetEntryObserver &parent_observer, 
							const CPointSet::DIMENSION dim,
							const CString& strName,
							const unsigned int uIcon,
							HTREEITEM hInsertAfter = TVI_LAST);
			virtual BOOL OnFilter(const child_type& t) const;
			virtual CTreeNode* InsertChild(child_type& child);
			virtual void OnChildModified(CTreeNode &child);
	};

public:
	CPointSetEntryObserver(TPointSetEntry& node,
                           CModelBase& model,
						   CTreeCtrl& view,
						   HTREEITEM hParent = TVI_ROOT,
						   HTREEITEM hInsertAfter = TVI_LAST);

//	virtual void OnAppendContextMenu( const CTreeNode& child ) const;
	virtual BOOL OnFilterPointSet( const IPointSet& point_set ) const;
	virtual CTreeNode* OnInsertPointSetObserver(CPointSetSubBranch& branch,
		                                        IPointSet& point_set);

	virtual void AppendContextMenu(CContextMenuInvoker &invoker);
	void CreatePointSet();
}; 

#endif // !defined(POINTSET_ENTRY_OBSERVER_INCLUDED_)
