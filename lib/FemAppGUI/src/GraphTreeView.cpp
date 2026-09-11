// GraphTreeView.cpp: implementation of the CGraphTreeView class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "GraphNode.h"
#include "GraphPtr.h"
#include "GraphEntry.h"
#include "FemAppModel.h"
#include "GraphTreeCtrl.h"
#include "FemAppDoc.h"
#include "TreeNode.h"
#include "GraphTreeView.h"
#include "GraphMainFrm.h"


#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CGraphTreeView

//IMPLEMENT_DYNCREATE(CGraphTreeView, CTreeView)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGraphTreeView::CRootObserver::CRootObserver()
: CGraphNode(_T(""))
{
}

void CGraphTreeView::CRootObserver::DeleteRoots()
{
  while( referenceSize() > 0 ) UnLink( referenceAt(0) );
}

CGraphTreeView::CRootObserver::~CRootObserver()
{
	DeleteRoots();
}

unsigned int CGraphTreeView::CRootObserver::IconId() const
{ return 0; } // Function not in use
unsigned int CGraphTreeView::CRootObserver::TypeId() const
{ return 0; } // Function not in use

//##ModelId=3B653D000287
CGraphTreeView::CGraphTreeView()
: m_pSelection(0)
{
}

//##ModelId=3B653D0002D3
CGraphTreeView::~CGraphTreeView()
{
}

BEGIN_MESSAGE_MAP(CGraphTreeView, CTreeViewBase)
	//{{AFX_MSG_MAP(CDataTreeView)
//	ON_WM_CREATE()
//	ON_NOTIFY_REFLECT(NM_RCLICK, OnRightClick)
//	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDoubleClick)
//	ON_NOTIFY_REFLECT(TVN_GETDISPINFO, OnGetDisplayInfo)
//	ON_COMMAND(ID_EDIT_DELETE, OnDeleteSelection)
//	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBeginDrag)
//	ON_WM_LBUTTONUP()
//	ON_WM_MOUSEMOVE()
//	ON_COMMAND(ID_EDIT_COPY, OnCopy)
//	ON_COMMAND(IDM_COPY, OnCopy)
//	ON_COMMAND(IDM_PASTE, OnPaste)
//	ON_COMMAND(ID_EDIT_PASTE, OnPaste)
//	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
//	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
//	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelectionChanged)
//	ON_NOTIFY_REFLECT(TVN_BEGINLABELEDIT, OnBeginLabelEdit)
//	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, OnEndlLabelEdit)
//	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGraphTreeView drawing

//##ModelId=3B653D0002C4
void CGraphTreeView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CGraphTreeView diagnostics

#ifdef _DEBUG
//##ModelId=3B653D0002E1
void CGraphTreeView::AssertValid() const
{
	CTreeViewBase::AssertValid();
}

//##ModelId=3B653D0002E3
void CGraphTreeView::Dump(CDumpContext& dc) const
{
	CTreeViewBase::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGraphTreeView message handlers

//##ModelId=3B653D0002D0
BOOL CGraphTreeView::PreCreateWindow(CREATESTRUCT& cs) 
{
	// Configure TreeCtrl
	cs.style |= TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_EDITLABELS | TVS_SHOWSELALWAYS;
	return CTreeViewBase::PreCreateWindow(cs);
}

CTreeCtrlBase* CGraphTreeView::OnCreateCtrl()
{
	CTreeCtrlBase *pCtrl = new CGraphTreeCtrl(*this);
	pCtrl->Create(WS_CHILD|
				  WS_VISIBLE|
				  WS_VSCROLL|
				  WS_HSCROLL|
				  TVS_HASLINES |
				  TVS_LINESATROOT | 
				  TVS_HASBUTTONS |
				  TVS_EDITLABELS | 
				  TVS_SHOWSELALWAYS,
				  CRect(0,0,0,0), 
				  this, 1); 
	return pCtrl;
}
//##ModelId=3B653D00031E
void CGraphTreeView::OnDeleteSelection() 
{

	HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();

	if (hItem)
	{
		ITreeObject *pItem = (ITreeObject*) GetTreeCtrl().GetItemData(hItem);

		ITreeObject *pObject = dynamic_cast<ITreeObject*> (pItem);

		if(pObject)
		{
			if(pObject->CanDelete())
				pObject->Delete();
		}
	}

}



// PRE	: TreeCtrl exists
// POST	: NULL if no item is selected or pointer is NULL otherwise the treeobject
//##ModelId=3B653D000294
CTreeNode* CGraphTreeView::SelectedItem()
{
	if(GetTreeCtrl().GetSelectedItem())
		if(GetTreeCtrl().GetItemData(GetTreeCtrl().GetSelectedItem()))
			return (CTreeNode*) GetTreeCtrl().GetItemData(GetTreeCtrl().GetSelectedItem());
	
	return 0;
}

//##ModelId=3B653D000295
void CGraphTreeView::OnEditCopy() 
{
/*	// Get MainWnd
	CGraphMainFrm* pMainWnd = dynamic_cast<CGraphMainFrm*> (AfxGetMainWnd());

	CTreeNode* pObserver = dynamic_cast<CTreeNode*> (SelectedItem());

	if(pObserver) {

		TGraphNodeSet set;

		VERIFY(set.insert(&pObserver->ObservedItem()).second);

		pMainWnd->Copy(set);
	}*/
}

//##ModelId=3B653D000296
void CGraphTreeView::OnEditPaste() 
{
/*	// Get MainWnd
	CGraphMainFrm* pMainWnd = dynamic_cast<CGraphMainFrm*> (AfxGetMainWnd());

	assert(pMainWnd);

	for(TGraphNodeSetIt iter = pMainWnd->CopiedItems().begin(); iter != pMainWnd->CopiedItems().end(); iter++)
	{
		if(SelectedItem())
			SelectedItem()->ConnectItem(**iter);

	} */
}			



//##ModelId=3B653D0002A2
void CGraphTreeView::OnUpdateEditPaste(CCmdUI* pCmdUI) 
{
/*	CGraphMainFrm* pMainWnd = dynamic_cast<CGraphMainFrm*> (AfxGetMainWnd());

	if((pMainWnd->CopiedItems().size() != 0) && SelectedItem())
	{
		for(TGraphNodeSetIt iter = pMainWnd->CopiedItems().begin(); iter != pMainWnd->CopiedItems().end(); iter++)
			if(SelectedItem()->CanConnectItem(**iter))
			{
				pCmdUI->Enable();
				return;
			}
	}

	pCmdUI->Enable(false);
*/
}

void CGraphTreeView::OnEditDelete() 
{
	if(SelectedCtrlObject())
	{
		if(SelectedCtrlObject()->CanDelete())
			SelectedCtrlObject()->Delete();
	}
}
/*
void CGeomecDoc::OnUpdateEditDelete(CCmdUI* pCmdUI) 
{
	BOOL bEnable = FALSE;
	if(SelectedCtrlObject())
	{
		if(SelectedCtrlObject()->CanDelete())
			bEnable = TRUE;
	}

	pCmdUI->Enable(bEnable);
}
*/
//##ModelId=3B653D000340
void CGraphTreeView::OnSelectionChanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	ITreeObject *pItem = (ITreeObject*) (pNMTreeView->itemNew.lParam);
	if(pItem)
	{ 
		if(Document()->Model() && Document()->Model()->isOpen())
			OnSelect(*pItem);
	}
	
	*pResult = 0;
}



//##ModelId=3B653D000292
CFemAppDoc* CGraphTreeView::Document() const
{
	return (CFemAppDoc*)(GetDocument());
}

CBitmap* CGraphTreeView::GetEmptyBitmap(int Size, int bpp)
{
	CBitmap *pRet= new CBitmap();
	void *bits;
	int bitmapsize = bpp * Size * Size / 8; // size in bytes

	bits = malloc(bitmapsize);

	memset(bits, 0, bitmapsize);

	pRet->CreateBitmap(Size, Size, 1, bpp, bits);

	free(bits);

	return pRet;
}

void CGraphTreeView::AddRoot(CTreeNode &obs)
{
	m_root.LinkTo(obs.Observer());
}

void CGraphTreeView::DeleteTree()
{
	m_root.DeleteRoots();
	m_pSelection = 0;

	GetTreeCtrl().DeleteTree();
}

void CGraphTreeView::OnSelect(ITreeObject &tree_object)
{
	CGraphNode* pPreviousNode = SelectedNode();

	m_pSelection = &tree_object;

    CGraphNode* selectedNode = SelectedNode();
	//inform prev selected node of deselecting
	if(pPreviousNode != selectedNode)
	{
		// Update doc ...
		Document()->OnSelect(selectedNode);
	}
}



const CGraphNode* CGraphTreeView::SelectedNode() const
{
	const CTreeNode *pNode = dynamic_cast<const CTreeNode*> (m_pSelection);
	if(pNode)
		return &pNode->ObservedItem();

	return 0;
}

ICtrlObject* CGraphTreeView::SelectedCtrlObject()
{
	return m_pSelection;
}

CGraphNode* CGraphTreeView::SelectedNode()
{
	CTreeNode *pNode = dynamic_cast<CTreeNode*> (m_pSelection);
	if(pNode)
		return &pNode->ObservedItem();

	return 0;
}