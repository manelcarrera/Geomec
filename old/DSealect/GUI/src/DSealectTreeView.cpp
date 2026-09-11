// DSealectTreeView.cpp : implementation file
//

#include "stdafx.h"
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif  // _MSC_VER

#include "DSealect.h"
#include "DSealectTreeView.h"
#include "FormationDefinitionForm.h"
#include <vector>
#include <iostream>
#include <list>
#include <string>

//using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[] = __FILE__;#endif  // _MSC_VER
#endif

	
	
	
//typedef list<string> LISTSTRING;
/////////////////////////////////////////////////////////////////////////////
// CDSealectTreeView

IMPLEMENT_DYNCREATE(CDSealectTreeView, CTreeView)


//##ModelId=3C0DC15B03A9
CDSealectTreeView::CDSealectTreeView()
{
}


//##ModelId=3C0DC15C002F
CDSealectTreeView::~CDSealectTreeView()
{
	
}

//##ModelId=3C0DC15C0012
BOOL CDSealectTreeView::CreateImageList()
{
	//IDI_ICON_DISABLE

	m_ilTreeImages.Create(16,16,ILC_COLOR,1,26);
	m_ilTreeImagesState.Create(16,16,ILC_COLOR,1,26);
	
	CTreeCtrl & Tree = GetTreeCtrl();
	Tree.SetImageList(&m_ilTreeImages,TVSIL_NORMAL);
	Tree.SetImageList(&m_ilTreeImagesState,TVSIL_STATE);

	
	int nOverlayIndex;
	HICON hIcon;

	
	//IDI_EXCLAMATION   
	
	hIcon = AfxGetApp()->LoadStandardIcon(IDI_QUESTION); //IDI_ICON_DISABLE
	nOverlayIndex = m_ilTreeImagesState.Add(hIcon);
	//m_ilTreeImagesState.SetOverlayImage(nOverlayIndex,1);

	hIcon = AfxGetApp()->LoadStandardIcon(IDI_EXCLAMATION);
	nOverlayIndex = m_ilTreeImagesState.Add(hIcon);
	//m_ilTreeImagesState.SetOverlayImage(nOverlayIndex,2);


	//Tree.ModifyStyle(NULL,!TVS_HASBUTTONS|TVS_HASLINES|TVS_LINESATROOT);

	return TRUE;
}

BEGIN_MESSAGE_MAP(CDSealectTreeView, CTreeView)
	//{{AFX_MSG_MAP(CDSealectTreeView)
	ON_NOTIFY_REFLECT(TVN_GETDISPINFO, OnGetdispinfo)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, OnItemexpanding)
	ON_WM_PAINT()
	ON_NOTIFY_REFLECT(TVN_SELCHANGING, OnSelchanging)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDSealectTreeView drawing

//##ModelId=3C0DC15C0013
void CDSealectTreeView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	
}

/////////////////////////////////////////////////////////////////////////////
// CDSealectTreeView diagnostics

#ifdef _DEBUG
//##ModelId=3C0DC15C003E
void CDSealectTreeView::AssertValid() const
{
	CTreeView::AssertValid();
}

//##ModelId=3C0DC15C0040
void CDSealectTreeView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDSealectTreeView message handlers


//##ModelId=3C0DC15C0011
void CDSealectTreeView::OnInitialUpdate()
{

	CTreeCtrl & Tree = GetTreeCtrl();
	ITreeEntryBase* pEntry=NULL;
	HTREEITEM hItem=Tree.GetRootItem();
	pEntry = (ITreeEntryBase*)Tree.GetItemData(hItem);
	if(pEntry)
		pEntry->Next()->Activate();

	//using namspace std;
	//typedef list<string> LISTSTRING;


	//list<string> ls;
	//ls.push_back("Een");
	//ls.push_back("Twee");
	//ls.push_back("Drie");

	//LISTSTRING listString;
	//LISTSTRING::iterator i;

	//listString.insert (listString.begin(), "Een");
	//listString.insert (listString.end(), "Twee");
	//listString.insert (listString.end(), "Drie");
	//listString.insert (listString.end(), "Vier");
	//listString.insert (listString.end(), "Vijf");

	//level1= Tree.InsertItem("Formations",1,1);
	//i = listString.begin();
	//level1= Tree.InsertItem(listString.begin() ,1,1);	

	/*
	m_ilTreeImages.Create(16,16,ILC_COLOR,1,26);

	m_ilTreeImages.Add(AfxGetApp()->LoadIcon(IDI_ICAppsStop));			
	m_ilTreeImages.Add(AfxGetApp()->LoadIcon(IDI_ICStonedFolder));		
	m_ilTreeImages.Add(AfxGetApp()->LoadIcon(IDI_ICBeGenericFile));
	m_ilTreeImages.Add(AfxGetApp()->LoadIcon(IDI_ICWildFolder37));
	m_ilTreeImages.Add(AfxGetApp()->LoadIcon(IDI_ICSphere));
	m_ilTreeImages.Add(AfxGetApp()->LoadIcon(IDI_ICPurpleTabFolder));	
	m_ilTreeImages.Add(AfxGetApp()->LoadIcon(IDI_ICBeEdit));
	m_ilTreeImages.Add(AfxGetApp()->LoadIcon(IDI_ICBeKaleidoscope));
	m_ilTreeImages.Add(AfxGetApp()->LoadIcon(IDI_ICTubeEmptyM));
	m_ilTreeImages.Add(AfxGetApp()->LoadIcon(IDI_ICShieldM));
	m_ilTreeImages.Add(AfxGetApp()->LoadIcon(IDI_ICControlFolders));	
	m_ilTreeImages.Add(AfxGetApp()->LoadIcon(IDI_ICBeBounce));
	m_ilTreeImages.Add(AfxGetApp()->LoadIcon(IDI_ICExpand));
	m_ilTreeImages.Add(AfxGetApp()->LoadIcon(IDI_ICShadedUtilities));
	m_ilTreeImages.Add(AfxGetApp()->LoadIcon(IDI_ICTools));
	m_ilTreeImages.Add(AfxGetApp()->LoadIcon(IDI_ICRecycling));			
	m_ilTreeImages.Add(AfxGetApp()->LoadIcon(IDI_ICSunburst));
	m_ilTreeImages.Add(AfxGetApp()->LoadIcon(IDI_ICDropFolder));
	m_ilTreeImages.Add(AfxGetApp()->LoadIcon(IDI_ICGraphFolder));
	m_ilTreeImages.Add(AfxGetApp()->LoadIcon(IDI_ICTime));
	m_ilTreeImages.Add(AfxGetApp()->LoadIcon(IDI_ICBeCardStack));		
	m_ilTreeImages.Add(AfxGetApp()->LoadIcon(IDI_ICCalculator3D));
	m_ilTreeImages.Add(AfxGetApp()->LoadIcon(IDI_ICShadedReports));
	m_ilTreeImages.Add(AfxGetApp()->LoadIcon(IDI_ICBeScript));
	m_ilTreeImages.Add(AfxGetApp()->LoadIcon(IDI_ICBeMagnify));
	m_ilTreeImages.Add(AfxGetApp()->LoadIcon(IDI_ICInkJet));			

	int i=0;
	//level1= Tree.InsertItem( ls.begin() ,1,1);
	
	level1= Tree.InsertItem(CString( v[i].c_str()),1,1);  
	

	//level1= Tree.InsertItem("Formations",1,1);	
	level2= Tree.InsertItem("Formation Definition",2,2, level1);
	Tree.SetItemData(level2, 2);
	level2= Tree.InsertItem("Insitu Stresses",3,3, level1);
	Tree.SetItemData(level2, 3);
	level2= Tree.InsertItem("Formation Properties",4,4, level1);
	Tree.SetItemData(level2, 4);
	Tree.Expand(level1,TVE_EXPAND);
	level1= Tree.InsertItem("Drilling and Completion",5,5);
	level2= Tree.InsertItem("Drilling Operation", 6, 6, level1);
	level2= Tree.InsertItem("Casing Operation", 7, 7,level1);
	level2= Tree.InsertItem("Casing Stresses", 8, 8, level1);
	level2= Tree.InsertItem("Cementing Operation", 9, 9, level1);
	Tree.Expand(level1,TVE_EXPAND);
	level1= Tree.InsertItem("Depths/Cements for Analysis", 10, 10);
	level2= Tree.InsertItem("Depths for Analysis", 11, 11, level1);
	level2= Tree.InsertItem("Cement Properties", 12, 12, level1);
	Tree.Expand(level1,TVE_EXPAND);
	level1= Tree.InsertItem("Operational Regimes", 13, 13);
	level2= Tree.InsertItem("Completion and Testing", 14, 14, level1);
	level2= Tree.InsertItem("Production and Injection", 15, 15, level1);
	level2= Tree.InsertItem("Extreme Operations", 16, 16, level1);
	Tree.Expand(level1,TVE_EXPAND);
	level1= Tree.InsertItem("Analysis Control", 17, 17);
	level2= Tree.InsertItem("Mesh Refinement", 18, 18, level1);
	level2= Tree.InsertItem("Stepping Scheme", 19, 19, level1);
	level2= Tree.InsertItem("Output Selection", 20, 20, level1);
	level2= Tree.InsertItem("Analysis", 21, 21, level1);
	Tree.Expand(level1,TVE_EXPAND);
	level1= Tree.InsertItem("Results Report", 22, 22);
	level2= Tree.InsertItem("Overview Tables", 23, 23, level1);
	level2= Tree.InsertItem("Detailed Results", 24, 24, level1);
	level2= Tree.InsertItem("Generate Report", 25, 25, level1);
	Tree.Expand(level1,TVE_EXPAND);
	*/

}



//##ModelId=3C0DC15C004E
void CDSealectTreeView::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	/*
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
	TV_ITEM pItem =  pTVDispInfo->item;
	ASSERT(pItem.mask |= TVIF_IMAGE);
	ASSERT(pItem.iImage > 0);
	ASSERT(FALSE);
	*/

	*pResult = 0;
}




//##ModelId=3C0DC15C0003
BOOL CDSealectTreeView::InsertItem(ITreeObject *Item ,ITreeObject* pParent,ITreeObject* pInsertAfter)	
{
	HTREEITEM hParent=TVI_ROOT;
	HTREEITEM hInsertAfter=TVI_LAST;

	if(NULL!=pParent)
		hParent=pParent->TreeItemHandle();

	if(NULL!=pInsertAfter)
		hInsertAfter=pInsertAfter->TreeItemHandle();

	CTreeCtrl & Tree = GetTreeCtrl();

	Item->InsertInTree(this,hParent,hInsertAfter);
	Item->Enable(FALSE);

	//SetItemColor(Item->TreeItemHandle(),RGB(rand()%255,rand()%255,rand()%255));
	

	return true;

}

//##ModelId=3C0DC15C007F
void CDSealectTreeView::OnSelchanging(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	
	ITreeEntryBase* pEntry=NULL;
	ITreeEntryBase* pEntryPrev=NULL;

	CTreeCtrl & Tree = GetTreeCtrl();
	
	HTREEITEM hPrev=0;
	hPrev=pNMTreeView->itemOld.hItem;
	if(hPrev)
	{
		pEntryPrev = (ITreeEntryBase*)Tree.GetItemData(hPrev);
	}
	

	HTREEITEM hCurrent = pNMTreeView->itemNew.hItem; //Tree.GetSelectedItem();
	if(hCurrent)
		pEntry = (ITreeEntryBase*)Tree.GetItemData(hCurrent);

	if(pEntry)
	{
		if(pEntry->Enable())
		{
			//pEntry->Activate(pEntryPrev);
			*pResult = 0;
		}
		else
		{
			*pResult = 1;
		}
	}
	
}


//##ModelId=3C0DC15C0052
void CDSealectTreeView::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	
	ITreeEntryBase* pEntry=NULL;
	ITreeEntryBase* pEntryPrev=NULL;

	CTreeCtrl & Tree = GetTreeCtrl();

	
	HTREEITEM hPrev=0;
	hPrev=pNMTreeView->itemOld.hItem;
	if(hPrev)
	{
		pEntryPrev = (ITreeEntryBase*)Tree.GetItemData(hPrev);
	}
	

	HTREEITEM hCurrent = Tree.GetSelectedItem();
	if(hCurrent)
		pEntry = (ITreeEntryBase*)Tree.GetItemData(hCurrent);

	if(pEntry)
	{
		if(pEntry->Enable())
		{
			pEntry->Activate(pEntryPrev);
			*pResult = 0;
		}
		else
		{
			*pResult = 1;
		}
	}
	
}

//##ModelId=3C0DC15C0021
BOOL CDSealectTreeView::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style = 0;
	cs.style = !TVS_HASBUTTONS | WS_VISIBLE | WS_TABSTOP | WS_CHILD | WS_BORDER
   |TVS_DISABLEDRAGDROP|TVS_SHOWSELALWAYS|TVS_LINESATROOT;
	return CTreeView::PreCreateWindow(cs);

}

//##ModelId=3C0DC15C005F
void CDSealectTreeView::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	//dont react on dubbel click
	*pResult = 1;
}

//##ModelId=3C0DC15C0001
void CDSealectTreeView::ReleaseTreeItem(const HTREEITEM hti)
{
	CTreeCtrl &Tree = GetTreeCtrl();

	if(hti == NULL)
		return;

	HTREEITEM hti_ch = Tree.GetChildItem( hti );

	while( hti_ch != NULL )
	{
		ITreeEntryBase* pEntry=NULL;


		pEntry = ( ITreeEntryBase* )Tree.GetItemData( hti_ch );
		if(pEntry)
		{
			TRACE("deleting: %s\n",pEntry->Name());
			delete pEntry;
			Tree.SetItemData(hti_ch,NULL);
		}


		ReleaseTreeItem( hti_ch );
		hti_ch = Tree.GetNextSiblingItem( hti_ch );

	} 

}; 


//##ModelId=3C0DC15C006D
void CDSealectTreeView::OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	
	*pResult = 0;
}

//##ModelId=3C0DC15B03DA
void CDSealectTreeView::SetItemFont(HTREEITEM hItem, LOGFONT& logfont)
{
	Color_Font cf;
	if( !m_mapColorFont.Lookup( hItem, cf ) )
		cf.color = (COLORREF)-1;
	cf.logfont = logfont;
	m_mapColorFont[hItem] = cf;
}

//##ModelId=3C0DC15B03CA
void CDSealectTreeView::SetItemBold(HTREEITEM hItem, BOOL bBold)
{
	GetTreeCtrl().SetItemState( hItem, bBold ? TVIS_BOLD: 0, TVIS_BOLD );
}

//##ModelId=3C0DC15B03BD
void CDSealectTreeView::SetItemColor(HTREEITEM hItem, COLORREF color)
{
	Color_Font cf;
	if( !m_mapColorFont.Lookup( hItem, cf ) )
		cf.logfont.lfFaceName[0] = '\0';
	cf.color = color;
	m_mapColorFont[hItem] = cf;
}

//##ModelId=3C0DC15B03BA
BOOL CDSealectTreeView::GetItemFont(HTREEITEM hItem, LOGFONT * plogfont)
{
	Color_Font cf;
	if( !m_mapColorFont.Lookup( hItem, cf ) )
		return FALSE;
	if( cf.logfont.lfFaceName[0] == '\0' ) 
		return FALSE;
	*plogfont = cf.logfont;
	return TRUE;

}


//##ModelId=3C0DC15B03AC
BOOL CDSealectTreeView::GetItemBold(HTREEITEM hItem)
{
	return GetTreeCtrl().GetItemState( hItem, TVIS_BOLD ) & TVIS_BOLD;
}

//##ModelId=3C0DC15B03AA
COLORREF CDSealectTreeView::GetItemColor(HTREEITEM hItem)
{
	// Returns (COLORREF)-1 if color was not set
	Color_Font cf;
	if( !m_mapColorFont.Lookup( hItem, cf ) )
		return (COLORREF)-1;
	return cf.color;

}


//##ModelId=3C0DC15C007D
void CDSealectTreeView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// Create a memory DC compatible with the paint DC
	CDC memDC;
	memDC.CreateCompatibleDC( &dc );

	CRect rcClip, rcClient;
	dc.GetClipBox( &rcClip );
	GetClientRect(&rcClient);

	// Select a compatible bitmap into the memory DC
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap( &dc, rcClient.Width(), rcClient.Height() );
	memDC.SelectObject( &bitmap );
	
	// Set clip region to be same as that in paint DC
	CRgn rgn;
	rgn.CreateRectRgnIndirect( &rcClip );
	memDC.SelectClipRgn(&rgn);
	rgn.DeleteObject();
	


	// First let the control do its default drawing.
	CWnd::DefWindowProc( WM_PAINT, (WPARAM)memDC.m_hDC, 0 );


	HTREEITEM hItem = GetTreeCtrl().GetFirstVisibleItem();

	int n = GetTreeCtrl().GetVisibleCount()+1;
	while( hItem && n--)
	{
		CRect rect;

		// Do not meddle with selected items or drop highlighted items
		UINT selflag = TVIS_DROPHILITED | TVIS_SELECTED;
		Color_Font cf;
	
		if ( !(GetTreeCtrl().GetItemState( hItem, selflag ) & selflag ) 
			&& m_mapColorFont.Lookup( hItem, cf ))
		{
			CFont *pFontDC;
			CFont fontDC;
			LOGFONT logfont;

			if( cf.logfont.lfFaceName[0] != '\0' ) 
			{
				logfont = cf.logfont;
			}
			else
			{
				// No font specified, so use window font
				CFont *pFont = GetFont();
				pFont->GetLogFont( &logfont );
			}

			if( GetItemBold( hItem ) )
				logfont.lfWeight = 700;
			fontDC.CreateFontIndirect( &logfont );
			pFontDC = memDC.SelectObject( &fontDC );

			if( cf.color != (COLORREF)-1 )
				memDC.SetTextColor( cf.color );

			CString sItem = GetTreeCtrl().GetItemText( hItem );

			GetTreeCtrl().GetItemRect( hItem, &rect, TRUE );
			memDC.SetBkColor( GetSysColor( COLOR_WINDOW ) );
			memDC.TextOut( rect.left+2, rect.top+1, sItem );
			
			memDC.SelectObject( pFontDC );
		}
		hItem = GetTreeCtrl().GetNextVisibleItem( hItem );
	}


	dc.BitBlt( rcClip.left, rcClip.top, rcClip.Width(), rcClip.Height(), &memDC, 
				rcClip.left, rcClip.top, SRCCOPY );	

	// Do not call CTreeView::OnPaint() for painting messages
}

