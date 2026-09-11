// IListObject.cpp: implementation of the IListObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IListObject.h"
#include "ISubListObject.h"
#include "ListCtrlBase.h"

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif  // _MSC_VER
//#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IListObject::IListObject(CListCtrl& ctrl, int nIndex, BOOL bSelected)
: m_ctrl(ctrl)
{
	if(nIndex == -1)
		nIndex = Ctrl().GetItemCount();

	VERIFY(Ctrl().InsertItem(nIndex, LPSTR_TEXTCALLBACK, I_IMAGECALLBACK) != -1);

	if(bSelected)
		VERIFY(Ctrl().SetItemState(nIndex, LVIS_SELECTED, LVIS_SELECTED));
	else
		VERIFY(Ctrl().SetItemState(nIndex, 0, LVIS_SELECTED));

}

IListObject::~IListObject()
{
	// Delete sub items
	while(m_mpSubObject.begin() != m_mpSubObject.end())
	{
		delete m_mpSubObject.begin()->second;
		m_mpSubObject.erase(m_mpSubObject.begin());
	}

	// Delete item from list if nescarry
	int nIndex = Index();
	if(nIndex != -1)
	{
		Ctrl().SetItemData(nIndex, 0);
		Ctrl().DeleteItem(nIndex);
	}
}

CListCtrl& IListObject::Ctrl() const
{
	return m_ctrl;
}

std::size_t IListObject::SubObjectSize() const
{
	return m_mpSubObject.size();
}

ISubListObject* IListObject::SubObject(const int nIndex)
{
	if(m_mpSubObject.find(nIndex) == m_mpSubObject.end())
		return 0;

	return m_mpSubObject.find(nIndex)->second;
}

const ISubListObject* IListObject::SubObject(const int nIndex) const
{
	if(m_mpSubObject.find(nIndex) == m_mpSubObject.end())
		return 0;

	return m_mpSubObject.find(nIndex)->second;

}

void IListObject::Update()
{
	CListCtrlBase* pCtrl = dynamic_cast<CListCtrlBase*>(&Ctrl());

	// Update the state icon
	if(StateIcon() != 0)
	{
		pCtrl->SetImageList( &pCtrl->StateIconImageList(), LVSIL_STATE );
		VERIFY(pCtrl->SetItemState(Index(),
								   INDEXTOSTATEIMAGEMASK(pCtrl->StateIconImageList().IconToImage(StateIcon()) + 1),
								   LVIS_STATEIMAGEMASK));
	}
	pCtrl->Update(Index());
}

// Return value`:
// -1	Item not in list
// Index of item
int IListObject::Index() const
{
	LV_FINDINFO info;
	info.flags = LVFI_PARAM;
	info.lParam = (LONG_PTR) this;

	int nItem = Ctrl().FindItem(&info);

	return nItem;
}

BOOL IListObject::IsSelected() const
{
	return Ctrl().GetItemState(Index(), LVIS_SELECTED) == LVIS_SELECTED;
}

void IListObject::SelectionStateChanged(bool bSelected)
{
}

void IListObject::OnDoubleClick()
{
  ICtrlObject::OnDoubleClick();
}

void IListObject::OnDoubleClick(int nItemIndex)
{
  OnDoubleClick();
}
