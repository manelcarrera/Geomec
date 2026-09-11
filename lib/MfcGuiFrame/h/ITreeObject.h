// ITreeObject.h: interface for the ITreeObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITREEOBJECT_H__43AE08BB_C51A_4E21_912C_E3AFF15EB3E6__INCLUDED_)
#define AFX_ITREEOBJECT_H__43AE08BB_C51A_4E21_912C_E3AFF15EB3E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxcmn.h>
#include <vector>
#include "ICtrlObject.h"

class ITreeObject : public ICtrlObject
{
	HTREEITEM m_hTreeHandle;
	CTreeCtrl& m_ctrl;

	//special props
	LOGFONT  m_logfont;
protected:
	ITreeObject(CTreeCtrl &ctrl, HTREEITEM hParent, HTREEITEM hInsertAfter);
	ITreeObject(ITreeObject &parent, HTREEITEM m_hInsertAfter);
	ITreeObject(CTreeCtrl &ctrl);
	void InitSpecialProps();
public:

	virtual ~ITreeObject();
	virtual void SortChildren();
	// Access to tree ctrl
	const CTreeCtrl& Ctrl() const;
	CTreeCtrl& Ctrl();

	// Tree item NOTE: TreeItem is defined by client ...
	HTREEITEM Handle() const;

	virtual void Update();
	virtual BOOL IsSelected() const;

	// Children
	typedef std::vector<ITreeObject*> TChildren;
	TChildren Children() const;					// Returns the children of the current node
	// bFirstLevelOnly = FALSE	Adds the whole sub tree to the set (Children of the children's children)
	// bFirstLevelOnly = TRUE	Adds only the children to the set
	void SubTree(TChildren& children, bool bFirstLevelOnly) const;	

	BOOL IsExpanded() const;
	virtual void OnExpand();
	virtual void OnCollapse();

	// Parent
	const ITreeObject* Parent() const;
	ITreeObject* Parent();

	virtual COLORREF Color();
	virtual BOOL Bold();
	virtual BOOL Enable();
	virtual LOGFONT Font();
	virtual BOOL HasSpecialProps();
};

typedef std::vector<ITreeObject*> TTreeObjectVec;

// Function to compare treeobject in a branch
static int CALLBACK TreeCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

#endif // !defined(AFX_ITREEOBJECT_H__43AE08BB_C51A_4E21_912C_E3AFF15EB3E6__INCLUDED_)


