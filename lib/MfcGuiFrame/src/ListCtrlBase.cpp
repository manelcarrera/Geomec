// ListCtrlBase.cpp : implementation file
//

#include "stdafx.h"
#include "ListCtrlBase.h"
#include "InplaceEdit.h"
#include "ISubListObject.h"
#include "ContextMenuInvoker.h"

#define RECT_BORDER	10

#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CListCtrlBase
IMPLEMENT_DYNCREATE(CListCtrlBase, CListCtrl)


CListCtrlBase::CListCtrlBase()
{
	m_ilNormal.Create(32, 32, ILC_COLOR16, 1, 1);
	m_ilState.Create(16, 16, ILC_COLOR16, 1, 1);
	m_ilSmall.Create(16, 16, ILC_COLOR16, 1, 1);
}

CListCtrlBase::~CListCtrlBase()
{

}


BEGIN_MESSAGE_MAP(CListCtrlBase, CListCtrl)
	//{{AFX_MSG_MAP(CListCtrlBase)
	ON_NOTIFY_REFLECT(LVN_BEGINLABELEDIT, OnBeginLabelEdit)
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndLabelEdit)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnClick)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetDisplayInfo)
	ON_WM_CONTEXTMENU()
	ON_NOTIFY_REFLECT(LVN_BEGINDRAG, OnBeginDrag)
	ON_NOTIFY_REFLECT(LVN_DELETEITEM, OnDeleteItem)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGING, OnItemChanging)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDoubleClick)
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
	ON_NOTIFY_EX(HDN_ITEMCHANGEDW, 0, OnHeaderTrack)
	ON_NOTIFY_EX(HDN_ITEMCHANGEDA, 0, OnHeaderTrack)
	ON_WM_SIZE()
	ON_WM_WINDOWPOSCHANGED()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListCtrlBase message handlers

void CListCtrlBase::OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_ITEM& item = ((LV_DISPINFO*)pNMHDR)->item;

	*pResult = TRUE;

	IListObject *pObject = (IListObject*)(GetItemData(item.iItem));
	assert(item.iSubItem == 0);
	if (pObject->CanEditText())
		*pResult = FALSE;
}

void CListCtrlBase::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult) 
{
  *pResult = FALSE;

  LV_ITEM& item = ((LV_DISPINFO*)pNMHDR)->item;
  IListObject *pObject = (IListObject*)(GetItemData(item.iItem));

  if(item.pszText != NULL)
  {
    if(item.iSubItem == 0)
      *pResult = pObject->EditText(item.pszText);
    else
    {
      if(pObject->SubObject(item.iSubItem))
        *pResult = pObject->SubObject(item.iSubItem)->EditText(item.pszText);
    }
  }
}

void CListCtrlBase::OnHScroll(unsigned int nSBCode, unsigned int nPos, CScrollBar* pScrollBar) 
{
	if(GetFocus() != this)
		SetFocus();	

	ResizeCustomControl();
	CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CListCtrlBase::OnVScroll(unsigned int nSBCode, unsigned int nPos, CScrollBar* pScrollBar) 
{
	if(GetFocus() != this)
		SetFocus();	

	ResizeCustomControl();
	CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CListCtrlBase::OnLButtonDown(unsigned int nFlags, CPoint point) 
{
	CListCtrl::OnLButtonDown(nFlags, point);
	unsigned int uFlags;
	std::pair<int, int> location = HitTestEx( point, uFlags );	// Row / Colomn

	if((location.first != -1) && ( uFlags & LVHT_ONITEMSTATEICON ))
	{
		IListObject* pObject = (IListObject*) (GetItemData(location.first));
		if(pObject) {
			// Is the item in a state?
			if(pObject->StateIcon() != 0)
			{
				pObject->ToggleState();
			}
		}
	}	
	else if( location.first !=  -1 )
	{
		unsigned int flag = LVIS_FOCUSED;
		if( (GetItemState( location.first, flag ) & flag) == flag && location.second > 0)
		{
			// Check for LVS_EDITLABELS
			if( GetWindowLong(m_hWnd, GWL_STYLE) & LVS_EDITLABELS )
			{
				//Check for edit ability of item
				IListObject *pObject = (IListObject*)(GetItemData(location.first));
				if(pObject->SubObject(location.second))
					if (pObject->SubObject(location.second)->CanEditText())
						EditSubLabel( location );
			}
		}
		else
			SetItemState( location.first, LVIS_SELECTED | LVIS_FOCUSED , LVIS_SELECTED | LVIS_FOCUSED); 
	}
}

void CListCtrlBase::OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	// Sort the column
	std::pair<int, CListCtrl*> lData(pNMListView->iSubItem, this);
	SortItems(ListCompare, (DWORD_PTR)(&lData));
	
	*pResult = 0;
}

void CListCtrlBase::Sort(int nColumn)
{
	std::pair<int, CListCtrl*> lData(nColumn, this);
	SortItems(ListCompare, (DWORD_PTR)(&lData));
}

int CListCtrlBase::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_target.Register(this);
	
	return 0;
}

void CListCtrlBase::OnGetDisplayInfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_ITEM& item = ((LV_DISPINFO*)pNMHDR)->item;

	IListObject *pObject = (IListObject*) (item.lParam);
	if (pObject)
	{
		// Are displaylists initialized?
		if(GetImageList(LVSIL_NORMAL) != &m_ilNormal)
			SetImageList(&m_ilNormal, LVSIL_NORMAL);

		if(GetImageList(LVSIL_SMALL) != &m_ilSmall)
			SetImageList(&m_ilSmall, LVSIL_SMALL);

		ICtrlObjectBase *pBaseObject = 0;
		switch(item.iSubItem)
		{
		case 0:
			pBaseObject = pObject;

			// Do the icon ...
			if((item.mask & TVIF_IMAGE)==TVIF_IMAGE)
			{
				if((GetStyle() & LVS_TYPEMASK) == LVS_ICON)
					item.iImage = m_ilNormal.IconToImage(pObject->Icon());
				else
					item.iImage = m_ilSmall.IconToImage(pObject->Icon());
			}

			break;
		default:
			assert(pObject->SubObject(item.iSubItem));
			pBaseObject = pObject->SubObject(item.iSubItem);
		};

		// Do the text ...
		if((item.mask&LVIF_TEXT) == LVIF_TEXT)
		{
			if(strcmp(item.pszText, "")  != 0)
				free(item.pszText);
			item.pszText = _tcsdup(pBaseObject->Text().toStdString().c_str());
		}
	}
	*pResult = 0;
}

TListObjectVec CListCtrlBase::GetSelection() const
{
	// Return selected objects ....
	TListObjectVec vcSelected;
	POSITION pos = GetFirstSelectedItemPosition();
	while(pos)
	{
		IListObject *pObject = (IListObject*)( GetItemData(GetNextSelectedItem(pos)));
		if(pObject)
			vcSelected.push_back(pObject);
	}

	return vcSelected;
}


// EditSubLabel		- Start edit of a sub item label
// Returns		- Temporary pointer to the new edit control
// nItem		- The row index of the item to edit
// nCol			- The column of the sub item.
CEdit* CListCtrlBase::EditSubLabel(std::pair<int, int> location)
{
	// The returned pointer should not be saved

	// Make sure that the item is visible
	if( !EnsureVisible( location.first, TRUE ) ) return NULL;

	// Make sure that nCol is valid
	CHeaderCtrl* pHeader = (CHeaderCtrl*) GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();
	if( location.second >= nColumnCount || GetColumnWidth(location.second) < 5 )
		return NULL;

	// Get the column offset
	int offset = 0;
	for( int i = 0; i < location.second; i++ )
		offset += GetColumnWidth( i );

	CRect rect;
	GetItemRect( location.first, &rect, LVIR_BOUNDS );

	// Now scroll if we need to expose the column
	CRect rcClient;
	GetClientRect( &rcClient );
	if( offset + rect.left < 0 || offset + rect.left > rcClient.right )
	{
		CSize size;
		size.cx = offset + rect.left;
		size.cy = 0;
		Scroll( size );
		rect.left -= size.cx;
	}

	// Get Column alignment
	LV_COLUMN lvcol;
	lvcol.mask = LVCF_FMT;
	GetColumn( location.second, &lvcol );
	DWORD dwStyle ;
	if((lvcol.fmt&LVCFMT_JUSTIFYMASK) == LVCFMT_LEFT)
		dwStyle = ES_LEFT;
	else if((lvcol.fmt&LVCFMT_JUSTIFYMASK) == LVCFMT_RIGHT)
		dwStyle = ES_RIGHT;
	else dwStyle = ES_CENTER;

	rect.left += offset+4;
	rect.right = rect.left + GetColumnWidth( location.second ) - 3 ;
	if( rect.right > rcClient.right) rect.right = rcClient.right;

	dwStyle |= WS_BORDER|WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL;
	CEdit *pEdit = new CInPlaceEdit(location.first, location.second, GetItemText( location.first, location.second ));
	pEdit->Create( dwStyle, rect, this, 0);


	return pEdit;
}

// HitTestEx	- Determine the row index and column index for a point
// Returns	- the row index or -1 if point is not over a row
// point	- point to be tested.
// col		- to hold the column index
std::pair<int, int> CListCtrlBase::HitTestEx(CPoint &point, unsigned int& uFlags) const
{
	std::pair<int, int> ret;	// Row / Column


	ret.second = 0;	// Column = 0;
	ret.first = HitTest( point, &uFlags );	// Row
	
	// Make sure that the ListView is in LVS_REPORT
	if( (GetWindowLong(m_hWnd, GWL_STYLE) & LVS_TYPEMASK) != LVS_REPORT )
		return ret;

	// Get the top and bottom row visible
	ret.first = GetTopIndex();
	int bottom = ret.first + GetCountPerPage();
	if( bottom > GetItemCount() )
		bottom = GetItemCount();
	
	// Get the number of columns
	CHeaderCtrl* pHeader = (CHeaderCtrl*) GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();

	// Loop through the visible rows
	for( ;ret.first <= bottom; ret.first++)
	{
		// Get bounding rect of item and check whether point falls in it.
		CRect rect;
		GetItemRect( ret.first, &rect, LVIR_BOUNDS );
		if( rect.PtInRect(point) )
		{
			// Now find the column
			for( ret.second = 0; ret.second < nColumnCount; ret.second++ )
			{
				int colwidth = GetColumnWidth(ret.second);
				if( point.x >= rect.left 
					&& point.x <= (rect.left + colwidth ) )
				{
					return ret;
				}
				rect.left += colwidth;
			}
		}
	}
	return std::pair<int, int>(-1, -1);
}

int CALLBACK ListCompare(LPARAM iItem1, LPARAM iItem2, LPARAM lData)
{
	std::pair<int, CListCtrl*>* pData = (std::pair<int, CListCtrl*>*) lData;
	IListObject* pObject1 = (IListObject*) iItem1;
	IListObject* pObject2 = (IListObject*) iItem2;

	if(pData->first == 0)
	{
		if((*pObject1) < (*pObject2))
			return -1;

		if((*pObject2) < (*pObject1))
			return 1;

		return 0;
	}

	// Sub list object should exist before comparing them ...
	assert(pObject1->SubObject(pData->first) && pObject2->SubObject(pData->first));

	ISubListObject *pSubObject1 = pObject1->SubObject(pData->first);
	ISubListObject *pSubObject2 = pObject2->SubObject(pData->first);

		if((*pSubObject1) < (*pSubObject2))
			return -1;

		if((*pSubObject2) < (*pSubObject1))
			return 1;

		return 0;
}

void CListCtrlBase::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CPoint ptClient = point;
	ScreenToClient(&ptClient);

	// Do an hit test
	if(HitTest(ptClient) != -1)
	{
		IListObject* pObject = (IListObject*) (GetItemData(HitTest(ptClient)));

		if(pObject) 
		{
			// Do the selection ...
			if(GetItemState(pObject->Index(), LVIS_SELECTED) == 0)
			{
				UnSelectAll();
	
				SetItemState(pObject->Index(), LVIS_SELECTED, LVIS_SELECTED);
			}

			// Set the context ...
			OnContext(GetSelection());

			// Create pop menu ...
			CMenu menu;
			menu.CreatePopupMenu();
			pObject->AppendToMenu(menu);
			if(menu.GetMenuItemCount() != 0)
			{
				CPoint ptContext(point);
				GetContextWnd().ScreenToClient(&ptContext);
				menu.TrackPopupMenu(TPM_LEFTALIGN , point.x, point.y, &GetContextWnd());
			}
			else
			{
				// Only use the new implementation when the old one isn't used.
				CContextMenuInvoker menuInvoker;
				pObject->AppendContextMenu(menuInvoker);
				menuInvoker.TrackMenu(point.x,point.y,GetContextWnd());
			}
		}
	}
	
}





void CListCtrlBase::OnBeginDrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	// Do the selected item in a set
	IListObject *pObject = (IListObject*) GetItemData(pNMListView->iItem);
	if(pObject && pObject->OnBeginDrag())
	{
		// Do the selection 
		if(GetItemState(pObject->Index(), LVIS_SELECTED) == 0)
		{
			UnSelectAll();
			SetItemState(pObject->Index(), LVIS_SELECTED, LVIS_SELECTED);
		}

		SetItemState(pObject->Index(), LVIS_DROPHILITED, LVIS_DROPHILITED);

		// Put the selected object in our "cool" ole drop target .....
		TCtrlObjectVec vcDragged;
		TListObjectVec vcSelected = GetSelection();
		for(size_t i = 0; i < vcSelected.size(); i++)
			vcDragged.push_back(vcSelected[i]);
	
		COleDataSource *poleSourceObj = new COleDataSource ;
		CCtrlDropTarget::m_vcDraggedObject = vcDragged;
	
		// Begin Drag operation
		DROPEFFECT dropeffect = poleSourceObj->DoDragDrop();
		// Remove the highlighting
//		SetItemState(pObject->Index(), 0, LVIS_DROPHILITED);

		// If user is moving item by pressing Shift, delete selected item
	//	if ( dropeffect == DROPEFFECT_MOVE)
	//		DeleteItem(hTSelItem); 
	delete poleSourceObj;
	}
	
	*pResult = 0;
}

void CListCtrlBase::UnSelectAll()
{
	// De-Select all and select current item...
	TListObjectVec vcSelected = GetSelection();

	for(size_t i = 0; i < vcSelected.size(); i++)
		SetItemState(vcSelected[i]->Index(), 0, LVIS_SELECTED);

	assert(GetSelection().size() == 0);
}

void CListCtrlBase::OnDeleteItem(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	IListObject *pObject = (IListObject*)(GetItemData(((NM_LISTVIEW*)pNMHDR)->iItem));
	
	if(pObject)
	{
		SetItemData(((NM_LISTVIEW*)pNMHDR)->iItem, 0);
		delete pObject;
	}
	
	*pResult = 0;
}


DROPEFFECT CListCtrlBase::OnDragOver(TCtrlObjectVec vcDragged,
									  COleDataObject* pDataObject, 
									  DWORD dwKeyState, 
									  CPoint point)
{
	DROPEFFECT deRet = DROPEFFECT_NONE;

	int nItem = HitTest(point);
	if (nItem != -1)
	{
		IListObject* pObject = (IListObject*) GetItemData(nItem);

		if(pObject)
		{
			deRet = pObject->CanDrop(vcDragged, (dwKeyState & MK_SHIFT) == MK_SHIFT);
		}
	}		
	
	// Scroll Tree control depending on mouse position
	CRect rectClient;
	GetClientRect(&rectClient);
	ClientToScreen(rectClient);
	ClientToScreen(&point);
	int nScrollDir = -1;
	if ( point.y >= rectClient.bottom - RECT_BORDER)
		nScrollDir = SB_LINEDOWN;
	else
	if ( (point.y <= rectClient.top + RECT_BORDER) )
		nScrollDir = SB_LINEUP;

	
	if ( nScrollDir != -1 ) 
	{
		int nScrollPos = GetScrollPos(SB_VERT);
		WPARAM wParam = MAKELONG(nScrollDir, nScrollPos);
		SendMessage(WM_VSCROLL, wParam);
	}
	
	nScrollDir = -1;
	if ( point.x <= rectClient.left + RECT_BORDER )
		nScrollDir = SB_LINELEFT;
	else
	if ( point.x >= rectClient.right - RECT_BORDER)
		nScrollDir = SB_LINERIGHT;
	
	if ( nScrollDir != -1 ) 
	{
		int nScrollPos = GetScrollPos(SB_VERT);
		WPARAM wParam = MAKELONG(nScrollDir, nScrollPos);
		SendMessage(WM_HSCROLL, wParam);
	}
	return deRet;
}


BOOL CListCtrlBase::OnDrop(TCtrlObjectVec vcDragged, 
						    COleDataObject* pDataObject,
							DROPEFFECT dropEffect, 
							CPoint point)
{
	BOOL bRet = FALSE;

	int nItem = HitTest(point);
	if (nItem != -1)
	{
		IListObject* pObject = (IListObject*) GetItemData(nItem);

		if(pObject)
		{
			if(pObject->CanDrop(vcDragged, dropEffect == DROPEFFECT_MOVE))
			{
				bRet = TRUE;
					pObject->Drop(vcDragged, dropEffect == DROPEFFECT_MOVE);
			}
		}

		//Remove highlighting
	//	SendMessage(TVM_SELECTITEM, TVGN_DROPHILITE,0);
	}		
	


	return bRet;
}

void CListCtrlBase::OnItemChanging(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// Detect selection change ...
	if((pNMListView->uChanged & LVIF_STATE)==LVIF_STATE)
	{
    bool bSelected = ((pNMListView->uNewState & LVIS_SELECTED)==LVIS_SELECTED);
    IListObject* pObject = (IListObject*)GetItemData(pNMListView->iItem);
    if(pObject)
      pObject->SelectionStateChanged(bSelected);

		if(bSelected)
		{
			// Selection
			OnSelect();
		}
	}
	
	*pResult = 0;
}

 void CListCtrlBase::OnKeyDown(unsigned int nChar, unsigned int nRepCnt, unsigned int nFlags) 
{
	if(nChar == VK_DELETE)
		OnDeleteSelection();

	CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CListCtrlBase::OnDeleteSelection()
{
	// Scan for deletion ....
	TListObjectVec vcDelete = GetSelection();

	BOOL bCanDelete = vcDelete.size() > 0;

	for(size_t i = 0; i < vcDelete.size(); i++)
		if(!vcDelete[i]->CanDelete())
			bCanDelete = FALSE;

	// Start the deletion ...
	if(bCanDelete)
		for(size_t i = 0; i < vcDelete.size(); i++)
			vcDelete[i]->Delete();
}

void CListCtrlBase::OnCopy()
{
	// Copy the selection
	TListObjectVec vcSel = GetSelection();

	m_vcClipBoard.clear();

	for(size_t i = 0; i < vcSel.size(); i++)
		m_vcClipBoard.push_back(vcSel[i]);
	
	assert(m_vcClipBoard.size() == vcSel.size());
}

BOOL CListCtrlBase::CanPaste()
{
	// Scan for paste ...
	TListObjectVec vcSelection = GetSelection();
	
	BOOL bCanPaste = vcSelection.size() > 0;

	for(size_t i = 0; i < vcSelection.size(); i++)
		if(vcSelection[i]->CanDrop(m_vcClipBoard, FALSE) == DROPEFFECT_NONE)
			bCanPaste = FALSE;

	return bCanPaste;
}

void CListCtrlBase::OnPaste()
{
	// Scan for paste ...
	TListObjectVec vcSelection = GetSelection();
	
	for(size_t i = 0; i < vcSelection.size(); i++)
		if(vcSelection[i]->CanDrop(m_vcClipBoard, FALSE))
			vcSelection[i]->Drop(m_vcClipBoard, FALSE);
}

void CListCtrlBase::OnSelect()
{
	// Selection handler ...
}

void CListCtrlBase::OnContext(const TListObjectVec &vec)
{
	// Set the context for context menu ...
}

CWnd& CListCtrlBase::GetContextWnd() const
{
	return (CWnd&)(*this); 
}

void CListCtrlBase::RegisterDropTarget()
{
	m_target.Register(this);
}
	
void CListCtrlBase::OnDoubleClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	DWORD dwPos = ::GetMessagePos();
  POINTS points = MAKEPOINTS(dwPos);
  CPoint point(points.x, points.y);
	CPoint abspoint = point;
	ScreenToClient(&point);
	
	unsigned int uFlags;
	std::pair<int, int> location = HitTestEx(point, uFlags);
	if(location.first != -1)
	{
		IListObject *pObject = (IListObject*) GetItemData(location.first);
		if(pObject != 0)
		{
			// always send to the IListObject
      pObject->OnDoubleClick(location.second); // send with sub object index (0 = main)

			// see if a subobject was clicked
			if(location.second > 0) pObject->SubObject(location.second)->OnDoubleClick(point);
		}
	}
	
	*pResult = 0;
}

void CListCtrlBase::ResizeCustomControl()
{
	for(int i = 0; i < GetItemCount(); i++)
	{
		IListObject *pObject = (IListObject*) GetItemData(i);
		if(pObject)
		{
			for(int j = 1; j <= pObject->SubObjectSize(); j++)
			{
				ISubListObject *pSubObject = pObject->SubObject(j);
				if(pSubObject)
				{
					CWnd *pCtrl = pSubObject->Control();
					if(pCtrl)
					{
						CRect rect;
						GetSubItemRect(i, j, LVIR_BOUNDS, rect);
						pCtrl->MoveWindow(&rect);
						Invalidate();
					}
				}
			}
		}
	}
}

void CListCtrlBase::OnSize(unsigned int nType, int cx, int cy)
{
	CListCtrl::OnSize(nType, cx, cy);
	ResizeCustomControl();
}

void CListCtrlBase::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos)
{
	CListCtrl::OnWindowPosChanged(lpwndpos);
	ResizeCustomControl();
}

BOOL CListCtrlBase::OnHeaderTrack(unsigned int id, NMHDR* pNMHDR, LRESULT* pResult)
{
	ResizeCustomControl();
	*pResult = 0;

	return FALSE;
}

IListObject* CListCtrlBase::GetObject(int iRow)
{
	return (IListObject*) GetItemData(iRow);
}
