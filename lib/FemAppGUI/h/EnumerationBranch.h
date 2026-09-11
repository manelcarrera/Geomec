#if !defined(AFX_ENUMERATIONBRANCH_H__INCLUDED_)
#define AFX_ENUMERATIONBRANCH_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ChildEnumerator.h"

template<class CHILD_TYPE, class CHILD_OBS_TYPE, BOOL RENAME, enum REMOVE_TYPE REMOVE>
class CEnumerationBranch : public CChildEnumerator<CHILD_TYPE> 
{
public:
	// Important typedefs ...
//	typedef C child_type;
//	typedef CNodeObserverTemp<T> child_obs_type;
//	typedef child_obs_type::remove_type remove_type;
private:
	const unsigned int m_uIcon;
	CTreeNode& m_parent_node;
public:
	CEnumerationBranch(CTreeNode &parent_node, 
					   const CString& strName,
					   const unsigned int uIcon,
					  HTREEITEM hInsertAfter = TVI_LAST,
					  const BOOL bInitialUpdate = TRUE)
	: CChildEnumerator<CHILD_TYPE>(parent_node, hInsertAfter), m_parent_node(parent_node), m_uIcon(uIcon)
	{

		Observer().reParent(&parent_node.ObservedItem());
		Observer().Name((LPCSTR) strName);
		Ctrl().SetItemData(Handle(), (DWORD_PTR)(this));

		if(bInitialUpdate)
			Update();

		parent_node.SortChildren();

	}
	// Insertion
	virtual CTreeNode* InsertChild(CHILD_TYPE& t)
	{
		return new CHILD_OBS_TYPE(t,
								  *this,
								  RENAME,
								  REMOVE );
	}

	virtual QString Text() const
	{	return Observer().Name(); }
	virtual unsigned int Icon() const
	{	return m_uIcon; }
	virtual void Delete()
	{	assert(FALSE); }
	virtual BOOL CanDelete() const
	{	return FALSE; }
	virtual BOOL CanEditText() const
	{	return FALSE; }
	virtual const CGraphNode& ObservedItem() const
	{	return m_parent_node.ObservedItem(); }
	virtual CGraphNode& ObservedItem()
	{	return m_parent_node.ObservedItem(); }
	virtual const CGraphNode_Delegate& Delegate() const
	{	return m_parent_node.Delegate(); }
	virtual CGraphNode_Delegate& Delegate()
	{	return m_parent_node.Delegate(); }
	virtual void AppendToMenu(CMenu &menu)
	{	 }
};


#endif // !defined(AFX_ENUMERATIONBRANCH_H__INCLUDED_)