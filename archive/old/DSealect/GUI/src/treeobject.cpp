// treeobject.cpp: implementation of the ITreeObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dsealect.h"
#include "treeobject.h"
#include "DSealectTreeView.h"
#include "DSealectDrawView.h"
#include "BaseFormView.h"
#include "MultiViewSplitter.h"


#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif

#define ITEM_DISABLE_COLOR RGB(125,125,125)
#define ITEM_ENABLE_COLOR RGB(0,0,255)


//##ModelId=3C0DC156032C
ITreeObject::~ITreeObject()
{
	/*
	if(m_pControl)
	{
		m_pControl->DeleteItem(TreeItemHandle());
	}
	*/
}


//##ModelId=3C0DC156033C
ITreeObject::ITreeObject()
:m_hTreeItemHandle(0),
 m_pTreeView(0)
{
	
}

//##ModelId=3C0DC156034C
CTreeCtrl* ITreeObject::TreeCtr()
{
	return &m_pTreeView->GetTreeCtrl();
}


//##ModelId=3C0DC156034D
HTREEITEM ITreeObject::TreeItemHandle()
{
	return m_hTreeItemHandle;
}


//##ModelId=3C0DC156034E
void ITreeObject::InsertInTree(CDSealectTreeView* pTreeView, HTREEITEM hParent,HTREEITEM hInsertAfter)
{
	
	m_pTreeView=pTreeView;
	CImageList* pImageList = TreeCtr()->GetImageList(TVSIL_NORMAL);
	int nImage = pImageList->Add(AfxGetApp()->LoadIcon(IconID()));
	
	
	m_hTreeItemHandle = TreeCtr()->InsertItem(Name(),
											  nImage,
											  nImage,
											  hParent,
											  hInsertAfter);

    

	/*
	m_hTreeItemHandle = TreeCtr()->InsertItem(TVIF_TEXT,
											  Name(),
											  nImage,
											  nImage,
											  INDEXTOSTATEIMAGEMASK(0),
											  TVIS_STATEIMAGEMASK,
											  NULL,
											  hParent,
											  hInsertAfter);
	
	*/

	TreeCtr()->SetItemData(m_hTreeItemHandle,(DWORD)this);
	TreeCtr()->EnsureVisible(m_hTreeItemHandle);
	TreeCtr()->Expand(m_hTreeItemHandle,TVE_EXPAND); 
}


//##ModelId=3C0DC156034B
CDSealectTreeView* ITreeObject::TreeView()
{
	return m_pTreeView;
}

//##ModelId=3C0DC1570010
CBaseFormView* ITreeEntryBase::FormView()
{
	return m_pFormView;
}

//##ModelId=3C0DC1570011
CMultiViewSplitter* ITreeEntryBase::Splitter()
{
	return m_pSplitter;
}

//##ModelId=3C0DC157001F
CString ITreeEntryBase::Name() const
{
	return m_strName;
}

//##ModelId=3C0DC1570021
UINT ITreeEntryBase::IconID() const
{
	return m_IconID;
}

//##ModelId=3C0DC157002E
UINT ITreeEntryBase::ViewID() const
{
	return m_ViewID;
}

//##ModelId=3C0DC1570040
ITreeEntryBase* ITreeEntryBase::Next()
{
	return m_pNext;
}


//##ModelId=3C0DC1570041
ITreeEntryBase* ITreeEntryBase::Previous()
{
	return m_pPrevious;
}


//##ModelId=3C0DC1570012
void ITreeEntryBase::Activate(ITreeEntryBase* prev)
{
	Splitter()->ShowView(m_ViewID);
	Splitter()->ShowView(FormView()->DrawView()->ViewID());
	

	TreeCtr()->SelectItem(TreeItemHandle());
	TreeCtr()->SetFocus();
	TreeView()->SetItemBold(TreeItemHandle(),TRUE);
	if(prev)
		TreeView()->SetItemBold(prev->TreeItemHandle(),FALSE);
	
	FormView()->DrawView();
	TreeView()->Invalidate();
	FormView()->UpdateData(FALSE);
}

//##ModelId=3C0DC1570030
BOOL ITreeEntryBase::Enable() const
{
	return m_bEnable;
}

//##ModelId=3C0DC1570032
void ITreeEntryBase::Enable(BOOL newval,BOOL bUpdateWindow)
{
	m_bEnable=newval;
	if(m_bEnable)
	{
		//TreeCtr()->SetItemState(TreeItemHandle(),INDEXTOSTATEIMAGEMASK(0),TVIS_STATEIMAGEMASK );
		TreeView()->SetItemColor(TreeItemHandle(),ITEM_ENABLE_COLOR);
	}
	else
	{
		//TreeCtr()->SetItemState(TreeItemHandle(),INDEXTOSTATEIMAGEMASK(1),TVIS_STATEIMAGEMASK );
		TreeView()->SetItemColor(TreeItemHandle(), ITEM_DISABLE_COLOR);
	}

	if(bUpdateWindow)
		TreeView()->Invalidate();
}








//##ModelId=3C0DC15700DC
CTreeEntryBranch::CTreeEntryBranch(
					 CBaseFormView* FormView,
					 CMultiViewSplitter* SplitterView,
					 CString Name ,
					 int ViewID,
					 CString Text,
					 UINT IconID,
					 ITreeEntryBase* pPrevious)
{
	m_pFormView=FormView;
	m_pSplitter=SplitterView;
	m_strName=Name;
	m_IconID=IconID;
	m_ViewID=ViewID;
	m_pPrevious=pPrevious;
	m_pNext=NULL;
	if(m_pPrevious)
	{
		m_pPrevious->m_pNext=this;
	}
	m_pFormView->m_pTreeEntry=this;
	m_strText=Text;
}
	

//##ModelId=3C0DC15700ED
CString CTreeEntryBranch::Text()
{
	return m_strText;
}


//##ModelId=3C0DC157010A
CTreeEntryLeaf::CTreeEntryLeaf(CBaseFormView* FormView,
					 CMultiViewSplitter* SplitterView,
					 CString Name,
					 int ViewID,
					 UINT IconID,
					 ITreeEntryBase* pPrevious)
{
	m_pFormView=FormView;
	m_pSplitter=SplitterView;
	m_strName=Name;
	m_IconID=IconID;
	m_ViewID=ViewID;
	m_pPrevious=pPrevious;
	m_pNext=NULL;
	if(m_pPrevious)
	{
		m_pPrevious->m_pNext=this;
	}
	m_pFormView->m_pTreeEntry=this;
}



//##ModelId=3C0DC157000F
ITreeEntryBase* ITreeEntryBase::GetFirstEntry()
{
	
	ITreeEntryBase* pEntry = this;
	ITreeEntryBase* pEntryPrev = Previous();

	while(pEntryPrev)
	{
		pEntry=pEntryPrev;
		pEntryPrev=pEntryPrev->Previous();
	}

	return pEntry;
}

//##ModelId=3C0DC1570003
void ITreeEntryBase::UpdateEnable()
{
	ITreeEntryBase* pEntry = GetFirstEntry();

	while(pEntry)
	{
		if(pEntry->FormView()->CheckEntry())
		{
			pEntry->Enable(TRUE,FALSE);
		}
		else
		{
			pEntry->Enable(FALSE,FALSE);
		}

		pEntry=pEntry->Next();
	}

	TreeView()->Invalidate();
}
