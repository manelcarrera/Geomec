// ITreeObject.cpp: implementation of the ITreeObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ITreeObject.h"
#include "IconImageList.h"
#include "treectrlbase.h"

#define ITEM_DISABLE_COLOR RGB(125,125,125)
#define ITEM_ENABLE_COLOR RGB(0,0,255)


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ITreeObject::ITreeObject(CTreeCtrl &ctrl, HTREEITEM hParent, HTREEITEM hInsertAfter)
: m_ctrl(ctrl)
{
	m_hTreeHandle = Ctrl().InsertItem(TVIF_TEXT | TVIF_IMAGE | TVIF_PARAM | TVIF_SELECTEDIMAGE,
									  LPSTR_TEXTCALLBACK,
									  I_IMAGECALLBACK,
									  I_IMAGECALLBACK,
									  TVIS_STATEIMAGEMASK,
									  TVIS_STATEIMAGEMASK,
									  (LPARAM) 0,
									  hParent,
									  hInsertAfter);
	InitSpecialProps();
	assert(m_hTreeHandle);
}

ITreeObject::ITreeObject(ITreeObject &parent, HTREEITEM hInsertAfter)
: m_ctrl(parent.Ctrl())
{
	m_hTreeHandle = Ctrl().InsertItem(TVIF_TEXT | TVIF_IMAGE | TVIF_PARAM | TVIF_SELECTEDIMAGE,
									  LPSTR_TEXTCALLBACK,
									  I_IMAGECALLBACK,
									  I_IMAGECALLBACK,
									  TVIS_STATEIMAGEMASK,
									  TVIS_STATEIMAGEMASK,
									  (LPARAM) 0,
									  parent.Handle(),
									  hInsertAfter);
	InitSpecialProps();
	assert(m_hTreeHandle);
}

ITreeObject::ITreeObject(CTreeCtrl &ctrl)
: m_ctrl(ctrl), m_hTreeHandle(0)
{
	InitSpecialProps();
}

void ITreeObject::InitSpecialProps()
{
	m_logfont.lfFaceName[0] = '\0';
}

ITreeObject::~ITreeObject()
{
	// Set the data pointer to zero and delete sub tree
	TChildren children;
	SubTree(children, false);

	// Walk over sub tree and set data ptr's to zero so that the message on children has no effect
	for(size_t i = 0; i < children.size(); i++)
		Ctrl().SetItemData(children[i]->Handle(), 0);

	// Delete the sub tree
	for(size_t i = 0; i < children.size(); i++)
		delete children[i];

	// If the lParam of TV_ITEM is zero the destructor is called from the TVN_DELETEITEM
	// handler. In that case the item is deleted from the tree ....
	if(Ctrl().GetItemData(Handle()))
	{
		// Set the item data to zero to prevent another call of the destructor we're in 
		// from TVN_DELETEITEM
		Ctrl().SetItemData(Handle(), 0);
		Ctrl().DeleteItem(Handle());
	}
}

const CTreeCtrl& ITreeObject::Ctrl() const
{
	return m_ctrl;
}

CTreeCtrl& ITreeObject::Ctrl()
{
	return m_ctrl;
}

HTREEITEM ITreeObject::Handle() const
{
	assert(m_hTreeHandle);
	return m_hTreeHandle;
}

BOOL ITreeObject::IsExpanded() const
{
	return Ctrl().GetItemState(Handle(), TVIS_EXPANDED) & TVIS_EXPANDED;
}

void ITreeObject::OnCollapse()
{
}

void ITreeObject::OnExpand()
{
}

void ITreeObject::Update()
{
	Ctrl().SetItemText(Handle(), LPSTR_TEXTCALLBACK);
	Ctrl().SetItemImage(Handle(), I_IMAGECALLBACK, I_IMAGECALLBACK);

	CTreeCtrlBase* ctrl = dynamic_cast<CTreeCtrlBase*>(&Ctrl());
	assert(ctrl);
	ctrl->SetItemState(Handle(), 
			INDEXTOSTATEIMAGEMASK(ctrl->GetIconImageList(TVSIL_STATE)->IconToImage(StateIcon())),
			TVIS_STATEIMAGEMASK);

	// Update children also ....
	TChildren children = Children();
	for(TChildren::iterator it = children.begin(); it != children.end(); it++)
		(*it)->Update();
}

ITreeObject::TChildren ITreeObject::Children() const
{
	TChildren ret;
	SubTree(ret, true);
	return ret;
}

void ITreeObject::SubTree(TChildren& children, bool bFirstLevelOnly) const
{
	if(Ctrl().ItemHasChildren(Handle()))
	{
		HTREEITEM hChildItem = Ctrl().GetChildItem(Handle());

		while (hChildItem)
		{
			ITreeObject* pObject = (ITreeObject*)Ctrl().GetItemData(hChildItem);
			if(pObject)	// Data must be available
			{
				children.push_back(pObject);
				// Recursive when we need to collect the whole subtree
				if(!bFirstLevelOnly)
					pObject->SubTree(children, bFirstLevelOnly);
			}

			hChildItem = Ctrl().GetNextItem(hChildItem, TVGN_NEXT);
		}
	}
}

const ITreeObject* ITreeObject::Parent() const
{
	if(!Ctrl().GetParentItem(Handle()))
		return 0;

	return (const ITreeObject*) Ctrl().GetItemData(Ctrl().GetParentItem(Handle()));
}

ITreeObject* ITreeObject::Parent()
{
	if(!Ctrl().GetParentItem(Handle()))
		return 0;

	return (ITreeObject*) Ctrl().GetItemData(Ctrl().GetParentItem(Handle()));
}

void ITreeObject::SortChildren()
{
	TVSORTCB tvs;
	tvs.hParent = Handle();
	tvs.lpfnCompare = TreeCompare;
	tvs.lParam = (LPARAM) &Ctrl();
	Ctrl().SortChildrenCB(&tvs);
}

static int CALLBACK TreeCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
   // lParamSort contains a pointer to the tree control.
   // The lParam of an item is just its handle.
	CTreeCtrl* pTreeCtrl = (CTreeCtrl*) lParamSort;
	assert(pTreeCtrl);

	ITreeObject *pObject1 = ((ITreeObject*)  lParam1);
	ITreeObject *pObject2 = ((ITreeObject*)  lParam2);


	if((*pObject1) < (*pObject2))
		return -1;

	if((*pObject2) < (*pObject1))
		return 1;

	return 0;
}

BOOL ITreeObject::IsSelected() const
{
	return Ctrl().GetSelectedItem() == Handle();
}

COLORREF ITreeObject::Color()
{
	if(Enable())
		return (COLORREF)-1; //ITEM_ENABLE_COLOR;

	return ITEM_DISABLE_COLOR;
}

BOOL ITreeObject::Bold()
{
	return FALSE;
}

BOOL ITreeObject::Enable()
{
	return TRUE;
}

LOGFONT ITreeObject::Font()
{
	return m_logfont;
}

BOOL ITreeObject::HasSpecialProps()
{
	if(Bold())
		return TRUE;

	if(!(m_logfont.lfFaceName[0] == '\0'))
		return TRUE;

	if(!(Color() == (COLORREF)-1))
		return TRUE;

	return FALSE;
}
