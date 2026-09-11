// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Geomec.h"
#include "TreeSplitView.h"
#include "OpenGLSplitView.h"
#include "GeomecDoc.h"
#include "ModelBase.h"
#include "MainFrm.h"
#include "ResultRegister.h"
#include "ProgressDlg_MFC.h"
#include "MeshBase.h"
#include "3dmodel.h"

#include <afxpriv.h>

#include "HtmlHelp.h"
#include "GlobalMessage.h"
#include "resourceIDP.h"
#include "IProgressBase.h"
#include "Global.h"
#include "IProgressFactory.h"
//
// FIXME: move to gm
//
#include "ProgressFactory_MFC.h"
#include "ProgressFactory.h"

// -> WORKING ON: Context menu on right button
#include "Item.h"
#include "util_.h"
#include "ISettings.h"

#include "Printer.h"

#include "GmHelper.h" // minimize, keystroke simulator


#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif

namespace mainframe
{
	CGeomecDoc* doc()
	{
		//
		// way before
		//
		/*POSITION pos = AfxGetApp()->GetFirstDocTemplatePosition();
		CDocTemplate *pDocTemplate = AfxGetApp()->GetNextDocTemplate(pos);
		POSITION docpos = pDocTemplate->GetFirstDocPosition();
		CGeomecDoc* pDoc = (CGeomecDoc*) (pDocTemplate->GetNextDoc(docpos));*/

		CGeomecDoc* pDoc = GetGeomecDoc();
		return pDoc;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame

const CRect CMainFrame::s_rectDefault( 10, 10, 1000, 750 );
// section: Windows size
const char CMainFrame::s_profileHeading[] = "Window size";
const char CMainFrame::s_profileRect[] = "Rect";
const char CMainFrame::s_profileIcon[] = "icon";
const char CMainFrame::s_profileMax[] = "max";
const char CMainFrame::s_profileMainFrameSplitter[] = "MainFrameSplitter";
const char CMainFrame::s_profileOpenGLSplitViewSplitter[] = "OpenGLSplitViewSplitter";
const char CMainFrame::s_profileTreeSplitViewSplitter[] = "TreeSplitViewSplitter";

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_UNIT, OnUpdateUnit)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_LIN, OnUpdateLin)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_MESH, OnUpdateMesh)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_NON_LIN, OnUpdateNonLin)
  ON_UPDATE_COMMAND_UI(ID_INDICATOR_HEAT, OnUpdateHeat)
  ON_UPDATE_COMMAND_UI(ID_INDICATOR_MIXTURE, OnUpdateMixture)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_SCRL, OnUpdate_ElapsedTime)
	ON_COMMAND(ID_VIEW_TOOLBAR_GENERAL, OnViewToolbarGeneral)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLBAR_GENERAL, OnUpdateViewToolbarGeneral)
	ON_COMMAND(ID_VIEW_TOOLBAR_VIEW, OnViewToolbarView)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLBAR_VIEW, OnUpdateViewToolbarView)
	ON_WM_DROPFILES()
	ON_WM_DESTROY()
	// ON_COMMAND(ID_VIEW_TOOLBAR_TOPVIEW, OnViewToolbarTopview)
	// ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLBAR_TOPVIEW, OnUpdateViewToolbarTopview)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_VIEW_TOOLBAR_CALC, OnViewToolbarCalc)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLBAR_CALC, OnUpdateViewToolbarCalc)
    ON_COMMAND(ID_TOOLBAR_TENSOR, OnViewTensorToolBar)
    ON_UPDATE_COMMAND_UI(ID_TOOLBAR_TENSOR, OnUpdateViewTensorToolBar)
	ON_WM_CLOSE()
	ON_COMMAND(ID_HELP_FINDER, OnHelpFinder)
	ON_COMMAND_RANGE(WM_USER+200, WM_USER+220, &CMainFrame::OnHelpFinder_Custom) //FIXME: HC
	//
	// pdf docs
	//
  ON_COMMAND(ID_MR_MANUAL, &CMainFrame::OnMeshRefinementManual)
  ON_COMMAND(ID_OIV_TUTORIAL, &CMainFrame::OnOIVTutorial)
  ON_COMMAND(ID_LICENSE_HELP, &CMainFrame::OnLicenseBorrowingHelp)
  ON_COMMAND(ID_WELL_ZOOMIN_HELP, &CMainFrame::OnWellZoominHelp)
  ON_COMMAND(ID_GVT_QUICK_TOUR_HELP, &CMainFrame::OnGVTQuickTourHelp)
	//
	//
	//
  ON_COMMAND(ID_VIEW_TOOLBAR_VECTORDISPLAY, &CMainFrame::OnViewToolbarVectorDisplay)
  ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLBAR_VECTORDISPLAY, &CMainFrame::OnUpdateViewToolbarVectorDisplay)
	ON_MESSAGE( WM_USER + 200, &on_status )

	ON_MESSAGE(ID_ON_EVENT, &CMainFrame::on_event) 

	//ON_WM_NCRBUTTONUP()
	//ON_WM_CONTEXTMENU()	// Uncomment to enable it; process needs to be implemented but at least event is handled
END_MESSAGE_MAP()

static unsigned int indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_COOR,
	ID_INDICATOR_UNIT,
	ID_INDICATOR_MESH,
	ID_INDICATOR_LIN,
	ID_INDICATOR_NON_LIN,
  ID_INDICATOR_HEAT,
  ID_INDICATOR_MIXTURE,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

namespace key{

	/*void a()
	{
		int a=0;
	}

	void F6()
	{
		int a=0;
	}*/

namespace ctrl{

//
// read geomec.ini and update values
//
void F5()
{
	gm::settings_::init();
	gm::global::read();
	//
	// FIXME: move to gm
	//
	IProgressFactory* f = 
	_g->mfc(CGlobal::Progress) ? 
		new CProgressFactory_MFC : 
		new CProgressFactory;
	_g->prog(f);

	gm::doc_::gui::popup("Settings","Settings updated values read",1000);
}

//
// Launch functional test
//
void F6()
{
	if(_g->status()==CGlobal::Idle)
		_e->notify(eCmd::Test,CEvents::GuiApp_EH);
}
void F7()
{
	//gm::send::minimize();
	gm::send::key_stroke_ctrl(VK_F6); // -> it also works !!!
	//gm::send::key_stroke(VK_F6);
	//gm::send::key_stroke_ctrl(0x41); // 'a' -> this works
}

/*void a()
{
	int a=0;
}*/

//
// save
//
void s()
{
	//
	// VK_F10: same as Alt -> but this one works
	//

	//
	// it works -> sequence for save as
	//

	auto k=[](WORD key,int times=1){ return gm::send::key_ext{key,times }; };

	//gm::send::key_stroke( {VK_F10,VK_DOWN,VK_DOWN,VK_DOWN,VK_DOWN,VK_RETURN} );  // -> Ok
	//gm::send::key_stroke( { k(VK_F10), k(VK_DOWN,4), k(VK_RETURN) } ); // -> Ok

	gm::send::key_stroke( { k(VK_F10), k(VK_DOWN,13), k(VK_RETURN) } ); // open first file in recent files - Ok

	//gm::send::command(ID_FILE_SAVE_AS); // it works !!!

	//
	// it doesn't work
	//
	//gm::send::key_stroke(VK_MENU); // Alt

	//
	// it does nothing
	//
	//keybd_event(0x12, MapVirtualKey(0x12, 0), 0, 0); //Alt press
	//keybd_event(0x12, MapVirtualKey(0x12, 0), KEYEVENTF_KEYUP, 0); // Alt relese

	//
	// nok
	//
	//keybd_event(VK_MENU,0xb8,0 , 0); //Alt Press
	//keybd_event(VK_TAB,0x8f,0 , 0); // Tab Press
	//keybd_event(VK_TAB,0x8f, KEYEVENTF_KEYUP,0); // Tab Release
	//keybd_event(VK_MENU,0xb8,KEYEVENTF_KEYUP,0); // Alt Release
}


} //namespace ctrl
} //namespace key


//
// it works!
//
// Ctrl+F5: read config file + assign values to global
// Ctrl+F6: launch functional test
//
BOOL CMainFrame::PreTranslateMessage(MSG* pMsg) 
{
	//
	// 1) keyboard events
	//

	//if(pMsg->message==WM_KEYDOWN)
	if(pMsg->message==WM_KEYUP)
    {
        //if(pMsg->wParam==VK_RETURN)
        //    pMsg->wParam=VK_TAB;

		auto ctrl=[](){ return ( GetKeyState(VK_CONTROL) & 0x8000 ); };
		auto key=[=](WPARAM  key_){ return pMsg->wParam == key_; };
		auto ctrl_key=[=](WPARAM  key_){ return key(key_) && ctrl(); };
		
		if(ctrl_key(VK_F5))
			key::ctrl::F5();
		else if(ctrl_key(VK_F6))
			key::ctrl::F6();
		else if(ctrl_key(VK_F7))
			key::ctrl::F7();
		//
		// save_as
		//
		//else if(ctrl_key('s')) // doesn't enter here
		//	key::ctrl::s();
		//
		// FIXME: ctrl_key shown in menu is fake, it does nothing
		//
		// next works:
		//
		//else if(ctrl_key('S')) // enters here, the one in CAPITAL
		//	key::ctrl::s();
	}

	//
	// 2) mouse events
	//

	/*else if(pMsg->message==WM_LBUTTONUP)
	{
		//
		// it's catched everywhere but in the tree items and menu items... catched in the scenes
		//
		int c=0;
	}
	//
	// not captured !!! -> Look at LegendView on how is it implemented
	//
	else if(pMsg->message==WM_RBUTTONUP)
	{
		int c=0;
	}*/

	//
	//
	//
	return CWnd::PreTranslateMessage(pMsg);
}

LRESULT CMainFrame::on_event( WPARAM p1, LPARAM p2 )
{	
	//Printer::instance()->debug("main frame : on_event");

	CGeomecDoc* pDoc = GetGeomecDoc();

	return pDoc->on_event( p1, p2 );
}

LRESULT CMainFrame::on_status( WPARAM p1, LPARAM p2 )
{	
	char* cp = (char*)p2;
	//
	// FIXME: HC
	//
	if(p1==0) // gm::send::eStatusType::Text
	{
		//StatusBar().SetWindowText( cp );
		m_wndStatusBar.SetWindowText( cp );
	}
	else // p1==1 --> gm::send::eStatusType::ElapsedTime
	{
		m_wndStatusBar.SetPaneText( 10, cp );
	}

	delete []cp;
	return (LRESULT)0;
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

void CMainFrame::OnNcRButtonUp(UINT nHitTest, CPoint point) {

    /*if (nHitTest == HTMENU)
	{
        //PopupMenu(point);
		int a=0;
	}*/

    CFrameWnd::OnNcRButtonUp(nHitTest, point);
}


//##ModelId=3BC55D6102E4
CMainFrame::CMainFrame() : m_bDoSaveParameters(FALSE), m_bFirstTime(TRUE)
{
}

//##ModelId=3BC55D6102F6
CMainFrame::~CMainFrame()
{
}

void CMainFrame::SetIcon(CToolBar& toolbar, int icon, int resource)
{
  CToolBarCtrl& ctrl = toolbar.GetToolBarCtrl();

  CImageList *pList = ctrl.GetImageList();
  
  if (icon < pList->GetImageCount())
  {
    HICON hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(resource));

    pList->Replace(icon, hIcon);
    
    ctrl.SetImageList(pList);

    toolbar.Invalidate();
  }
}

CToolBar& CMainFrame::GetConsistencyToolbar()
{
  return m_wndCalcBar;
}

//##ModelId=3BC55D610327
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// Create Toolbar
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME) || 
		!m_wndViewBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC, CRect(0, 0, 0, 0), ID_VIEW_TOOLBAR_VIEW) ||
		!m_wndViewBar.LoadToolBar(IDR_VIEW_BAR) ||
		!m_wndTopViewBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC, CRect(0, 0, 0, 0), ID_VIEW_TOOLBAR_TOPVIEW) ||
		// !m_wndTopViewBar.LoadToolBar(IDR_TOPVIEW_BAR) ||
		!m_wndCalcBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC, CRect(0, 0, 0, 0), ID_VIEW_TOOLBAR_CALC) ||
		!m_wndCalcBar.LoadToolBar(IDR_CALC_BAR) ||
		!m_wndTensorBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC, CRect(0, 0, 0, 0), ID_TOOLBAR_TENSOR) ||
        !m_wndTensorBar.LoadToolBar(IDR_TENSOR_BAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	// Create Status bar
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(unsigned int)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

  if (!m_vectorDialogBar.Create(this, IDD_VECTOR_DIALOGBAR, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC, IDD_VECTOR_DIALOGBAR))
  {
    TRACE0("Failed to create dialog bar\n");
    return -1;      // fail to create
  }

  m_vectorDialogBar.SetBarStyle(m_wndToolBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
  m_vectorDialogBar.EnableDocking(CBRS_ALIGN_ANY);

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.SetWindowText(_T("General"));
	m_wndViewBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndViewBar.SetWindowText(_T("View"));
	m_wndTopViewBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndTopViewBar.SetWindowText(_T("Topview"));
    m_wndTensorBar.EnableDocking(CBRS_ALIGN_ANY);
    m_wndTensorBar.SetWindowText(_T("Tensor"));
	m_wndCalcBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndCalcBar.SetWindowText(_T("Analysis"));

	EnableDocking(CBRS_ALIGN_ANY);

	DockControlBar(&m_wndToolBar, AFX_IDW_DOCKBAR_TOP);
	DockControlBarLeftOf(&m_wndViewBar, &m_wndToolBar);
	DockControlBarLeftOf(&m_wndCalcBar, &m_wndViewBar);
	// DockControlBarLeftOf(&m_wndTopViewBar, &m_wndCalcBar);
    DockControlBar(&m_wndTensorBar);
    DockControlBar(&m_vectorDialogBar);
    DockControlBar(&m_wndTopViewBar);

	try
	{
		std::string profile_name = AfxGetApp()->m_pszProfileName;
		LoadBarState(AfxGetApp()->m_pszProfileName);
	}
	catch(...)
	{
	}

	OnModelChanged(0);
	
	DragAcceptFiles(TRUE);

	return 0;
}

//void CMainFrame::DockControlBarLeftOf(CToolBar* Bar,CToolBar* LeftOf)
void CMainFrame::DockControlBarLeftOf(CControlBar* Bar,CControlBar* LeftOf)
{
	CRect rect;
	DWORD dw;
	unsigned int n;

	// get MFC to adjust the dimensions of all docked ToolBars
	// so that GetWindowRect will be accurate
	RecalcLayout();
	LeftOf->GetWindowRect(&rect);
	rect.OffsetRect(1,0);
	dw=LeftOf->GetBarStyle();
	n = 0;
	n = (dw&CBRS_ALIGN_TOP) ? AFX_IDW_DOCKBAR_TOP : n;
	n = (dw&CBRS_ALIGN_BOTTOM && n==0) ? AFX_IDW_DOCKBAR_BOTTOM : n;
	n = (dw&CBRS_ALIGN_LEFT && n==0) ? AFX_IDW_DOCKBAR_LEFT : n;
	n = (dw&CBRS_ALIGN_RIGHT && n==0) ? AFX_IDW_DOCKBAR_RIGHT : n;

	// When we take the default parameters on rect, DockControlBar will dock
	// each Toolbar on a seperate line.  By calculating a rectangle, we in effect
	// are simulating a Toolbar being dragged to that location and docked.
	DockControlBar(Bar,n,&rect);
}

void CMainFrame::OnClose()
{
  CTnoWinApp* app = (CTnoWinApp*)AfxGetApp();
  app->SetIsExiting();
	SaveBarState(AfxGetApp()->m_pszProfileName);
	CFrameWnd::OnClose();
}

//##ModelId=3BC55D6102E5
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
//##ModelId=3BC55D6102F8
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

//##ModelId=3BC55D6102FA
void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

static const int SPLITTER_BAR_X = 192;

//##ModelId=3BC55D6102E8
BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
    int iMainFrameSplitter = ISettings::instance()->getProfileInt(s_profileHeading, s_profileMainFrameSplitter, SPLITTER_BAR_X);

    if(!m_wndSplitter.CreateStatic(this, 1, 2) ||
		  !m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(COpenGLSplitView),
        CSize(0, 0), pContext) ||
        !m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CTreeSplitView),
        CSize(iMainFrameSplitter, 0), pContext))
    {
        return FALSE;
    }
    else
        return TRUE;
}

//////////////////////////////////////////////////////////////////////
//
//
//					status bar (ini)
//
//
//////////////////////////////////////////////////////////////////////

void CMainFrame::OnUpdateUnit(CCmdUI* pCmdUI) 
{
	m_wndStatusBar.SetPaneInfo( 2, ID_INDICATOR_UNIT, SBPS_NORMAL, 30 );

	CGeomecDoc* pDoc = mainframe::doc();

	pCmdUI->SetText(pDoc->UnitNode().UnitName(pDoc->UnitNode().Unit()).toStdString().c_str());
}

void CMainFrame::OnUpdateMesh(CCmdUI* pCmdUI) 
{
	m_wndStatusBar.SetPaneInfo( 3, ID_INDICATOR_MESH, SBPS_NORMAL, 90);

	CGeomecDoc* pDoc = mainframe::doc();

	CModelBase *pModel = dynamic_cast<CModelBase*>(pDoc->Model());
	pCmdUI->Enable(pModel != 0);
	if(pModel)
	{
		if(pModel->IsMesh())
		{
			CString sNrOfElements;
			sNrOfElements.Format("%d elements", pModel->Mesh().Mesh().ElementSize());
			pCmdUI->SetText(sNrOfElements);
		}
		else
			pCmdUI->SetText("NO MESH");
	}
}

void CMainFrame::OnUpdateLin(CCmdUI* pCmdUI) 
{
	m_wndStatusBar.SetPaneInfo( 4, ID_INDICATOR_LIN, SBPS_NORMAL, 30 );

	CGeomecDoc* pDoc = mainframe::doc();

	bool bEnable=false;
	pCmdUI->SetText("LIN");
	pCmdUI->Enable(FALSE);
	CModelBase *pModel = dynamic_cast<CModelBase*>(pDoc->Model());
	if(pModel)
	{
		if(pModel->ResultRegister().Linear())
		{
			bEnable = true;
			pCmdUI->Enable(TRUE);
		}
		if (!bEnable && pModel->CanCalculate())
		{
			pCmdUI->SetText("CALC");
			pCmdUI->Enable(TRUE);
		}
	}

}

void CMainFrame::OnUpdateNonLin(CCmdUI* pCmdUI) 
{
	m_wndStatusBar.SetPaneInfo( 5, ID_INDICATOR_NON_LIN, SBPS_NORMAL, 40 );

	CGeomecDoc* pDoc = mainframe::doc();

	pCmdUI->SetText("NONLIN");

	pCmdUI->Enable(FALSE);
	CModelBase *pModel = dynamic_cast<CModelBase*>(pDoc->Model());
	if(pModel)
	{
		pCmdUI->Enable(pModel->ResultRegister().NonLinear());
	}
}

void CMainFrame::OnUpdateHeat(CCmdUI* pCmdUI) 
{
	m_wndStatusBar.SetPaneInfo( 6, ID_INDICATOR_HEAT, SBPS_NORMAL, 30 );

	CGeomecDoc* pDoc = mainframe::doc();

	pCmdUI->SetText("HEAT");

	pCmdUI->Enable(FALSE);
	CModelBase *pModel = dynamic_cast<CModelBase*>(pDoc->Model());
	if(pModel)
	{
		pCmdUI->Enable(pModel->ResultRegister().Heat());
	}
}

void CMainFrame::OnUpdateMixture(CCmdUI* pCmdUI) 
{
	m_wndStatusBar.SetPaneInfo( 7, ID_INDICATOR_MIXTURE, SBPS_NORMAL, 30 );

	CGeomecDoc* pDoc = mainframe::doc();

	pCmdUI->SetText("MIX");
	pCmdUI->Enable(FALSE);

	CModelBase *pModel = dynamic_cast<CModelBase*>(pDoc->Model());
	if(pModel)
	{
		pCmdUI->Enable(pModel->ResultRegister().Mixture());
	}
}

void CMainFrame::OnUpdate_ElapsedTime(CCmdUI* pCmdUI) 
{
	int idx=10;
	UINT id = ID_INDICATOR_SCRL;

	m_wndStatusBar.SetPaneInfo( idx, id, SBPS_NORMAL, 50 );

	m_wndStatusBar.SetPaneText( idx, m_wndStatusBar.GetPaneText(idx) );
}

//////////////////////////////////////////////////////////////////////
//
//
//					status bar (end)
//
//
//////////////////////////////////////////////////////////////////////


//##ModelId=3BC55D610333
void CMainFrame::OnUpdateEditPaste(CCmdUI* pCmdUI) 
{
//	pCmdUI->Enable(CanPaste());
}

void CMainFrame::OnViewToolbarGeneral() 
{
	ShowControlBar(&m_wndToolBar, !m_wndToolBar.IsWindowVisible(), FALSE);
}

void CMainFrame::OnUpdateViewToolbarGeneral(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_wndToolBar.IsWindowVisible());
}

void CMainFrame::OnViewToolbarView() 
{
	ShowControlBar(&m_wndViewBar, !m_wndViewBar.IsWindowVisible(), FALSE);
}

void CMainFrame::OnUpdateViewToolbarView(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_wndViewBar.IsWindowVisible());
}

void CMainFrame::OnViewToolbarCalc() 
{
	ShowControlBar(&m_wndCalcBar, !m_wndCalcBar.IsWindowVisible(), FALSE);
}

void CMainFrame::OnUpdateViewToolbarCalc(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_wndCalcBar.IsWindowVisible());
}

void CMainFrame::OnViewTensorToolBar()
{
    ShowControlBar(&m_wndTensorBar, !m_wndTensorBar.IsWindowVisible(), FALSE);
}

void CMainFrame::OnUpdateViewTensorToolBar(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_wndTensorBar.IsWindowVisible());
}

void CMainFrame::OnDropFiles(HDROP hDropInfo) 
{
	// Get the document ...
	CGeomecDoc *pDocument = &((CGeomecApp*) AfxGetApp())->GetDoc();

	// Get the number of files ....
	int nFileCount = ::DragQueryFile(hDropInfo, 0xFFFFFFFF, 0, 0);		

	if(nFileCount > 0)
	{
    // first try to open document (File -> Open)
    CDocument* pDoc = 0;
    if(nFileCount == 1)
    {
		  TCHAR szFileName[_MAX_PATH];
		  ::DragQueryFile(hDropInfo, 0, szFileName, _MAX_PATH);
    	CWinApp* pApp = AfxGetApp();
  		pDoc = pApp->OpenDocumentFile(szFileName);
    }

    if(!pDoc && pDocument->Model())
    {
      // import files

		  // Create a dialog ...	
		  std::vector<CGraphNode*> created_objects;

		  std::auto_ptr <IProgressBase> dlg( _g->prog()->create( eProgress::Dual, "Importing...", true, nFileCount ));

		  BOOL bFailure = FALSE;	// Failure flag ...
		  for(unsigned int uFile = 0; uFile < nFileCount; uFile++)
		  {
			  // Allocate file name and get it ...
			  size_t size = ::DragQueryFile(hDropInfo, uFile, 0, 0) + 1;
			  char* sFileName = (char*) malloc( size );
			  DragQueryFile(hDropInfo, uFile, sFileName, size);
			  dlg->NextJob(sFileName);
			  if(!pDocument->OnImportFile(sFileName, *dlg, created_objects))
			  bFailure = TRUE;
		  }
    }
	}

  ::DragFinish(hDropInfo);

}

void CMainFrame::ActivateFrame(int nCmdShow) 
{
	CString strText;
	BOOL bIconic, bMaximized;
	unsigned int flags;
	WINDOWPLACEMENT wndpl;
	CRect rect;

	ISettings* settings = ISettings::instance();

	if(m_bFirstTime)
	{
		m_bFirstTime = FALSE;
		strText = settings->getProfileString(s_profileHeading, s_profileRect).toStdString().c_str();

		if( !strText.IsEmpty() )
		{
			rect.left = atoi((const char *) strText);
			rect.top = atoi((const char *) strText + 5);
			rect.right = atoi((const char *) strText + 10);
			rect.bottom = atoi((const char *) strText + 15);
		}
		else
		{
			rect = s_rectDefault;
		}
		bIconic = settings->getProfileInt(s_profileHeading, s_profileIcon, 0);
		bMaximized = settings->getProfileInt(s_profileHeading, s_profileMax, 0);

		if(bIconic)
		{
			nCmdShow = SW_SHOWMINNOACTIVE;
			if(bMaximized) flags = WPF_RESTORETOMAXIMIZED;
			else           flags = WPF_SETMINPOSITION;
		}
		else
		{
			if(bMaximized)
			{
				nCmdShow = SW_SHOWMAXIMIZED;
				flags = WPF_RESTORETOMAXIMIZED;
			}
			else
			{
				nCmdShow = SW_NORMAL;
				flags = WPF_SETMINPOSITION;
			}
		}
		wndpl.length = sizeof(WINDOWPLACEMENT);
		wndpl.showCmd = nCmdShow;
		wndpl.flags = flags;
		wndpl.ptMinPosition = CPoint(0, 0);
		wndpl.ptMaxPosition = CPoint(-::GetSystemMetrics(SM_CXBORDER), -::GetSystemMetrics(SM_CYBORDER));
		wndpl.rcNormalPosition = rect;

		BOOL bRet = SetWindowPlacement(&wndpl);
	}

	CFrameWnd::ActivateFrame(nCmdShow);
}

void CMainFrame::GetMessageString(UINT nID, CString& rMessage) const
{
  rMessage.Empty();
}

void CMainFrame::OnDestroy() 
{
	if(!m_bDoSaveParameters) return;
	CString strText;
	BOOL bIconic, bMaximized;

	WINDOWPLACEMENT wndpl;
	wndpl.length = sizeof(WINDOWPLACEMENT);

	BOOL bRet = GetWindowPlacement(&wndpl);
	if(wndpl.showCmd == SW_SHOWNORMAL)
	{
		bIconic = FALSE;
		bMaximized = FALSE;
	}
	else if(wndpl.showCmd == SW_SHOWMAXIMIZED)
	{
		bIconic = FALSE;
		bMaximized = TRUE;
	}
	else if(wndpl.showCmd == SW_SHOWMINIMIZED)
	{
		bIconic = TRUE;
		if(wndpl.flags) bMaximized = TRUE;
		else            bMaximized = FALSE;
	}
	strText.Format("%04d %04d %04d %04d",
					wndpl.rcNormalPosition.left,
					wndpl.rcNormalPosition.top,
					wndpl.rcNormalPosition.right,
					wndpl.rcNormalPosition.bottom);

	ISettings* settings = ISettings::instance();
	settings->writeProfileString(s_profileHeading, s_profileRect, (LPCTSTR)strText );
	settings->writeProfileInt(s_profileHeading, s_profileIcon, bIconic);
	settings->writeProfileInt(s_profileHeading, s_profileMax, bMaximized);

	int cxCur = SPLITTER_BAR_X;
	int cxMin = 0;

	m_wndSplitter.GetColumnInfo(0, cxCur, cxMin);

	settings->writeProfileInt(s_profileHeading, s_profileMainFrameSplitter, cxCur);

	cxCur = 0;

	dynamic_cast <COpenGLSplitView*> (m_wndSplitter.GetPane(0, 1))->GetColumnInfo(0, cxCur, cxMin, true);

  settings->writeProfileInt(s_profileHeading, s_profileOpenGLSplitViewSplitter, cxCur);

	int cyCur = 0;
	int cyMin = 0;

	dynamic_cast <CTreeSplitView*> (m_wndSplitter.GetPane(0, 0))->GetRowInfo(0, cyCur, cyMin);

	settings->writeProfileInt(s_profileHeading, s_profileTreeSplitViewSplitter, cyCur);

	CFrameWnd::OnDestroy();
}

void CMainFrame::OnViewToolbarTopview() 
{
	// ShowControlBar(&m_wndTopViewBar, !m_wndTopViewBar.IsWindowVisible(), FALSE);
}

void CMainFrame::OnUpdateViewToolbarTopview(CCmdUI* pCmdUI) 
{
	// pCmdUI->SetCheck(m_wndTopViewBar.IsWindowVisible());	
}

void CMainFrame::OnHelpFinder()
{
  HWND hWnd = ::HtmlHelp(m_hWnd, AfxGetApp()->m_pszHelpFilePath, HH_DISPLAY_TOC, 0);
	if(!hWnd) _m()->msg(IDP_NOHELPAVAILABLE);
}

//https://docs.microsoft.com/en-us/previous-versions/windows/desktop/htmlhelp/hh-display-topic-command
//https://www.drexplain.com/software-documentation-tool/opening_help_files_by_topic_name_or_by_help_id.php
void CMainFrame::OnHelpFinder_Custom( UINT nID )
{
	std::string path_to_file; 

	if( nID == WM_USER+200 )
		path_to_file = "::/context/dialogs/upgrade_mesher.htm#position"; 

	QString path_to_content = QString::asprintf("%s%s", AfxGetApp()->m_pszHelpFilePath, path_to_file.c_str() );
	HWND hWnd = ::HtmlHelp(m_hWnd, path_to_content.toStdString().c_str(), HH_DISPLAY_TOPIC, 0);

	if(!hWnd) 
		_m()->msg(IDP_NOHELPAVAILABLE);
}

std::string ExePath()
{
    char buffer[MAX_PATH];
    GetModuleFileName( NULL, buffer, MAX_PATH );
    std::string::size_type pos = std::string( buffer ).find_last_of( "\\/" );
    return std::string( buffer ).substr( 0, pos);
}

void CMainFrame::OpenPdfDocument(const char *fileName, const char *path)
{
    HINSTANCE result = ShellExecute(m_hWnd, "open", fileName, "", path ? path : "", SW_SHOWNORMAL);
    if ( (long long)result <= 32 ) {
        DWORD dw= GetLastError(); 
        char szMsg[250];
        FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM,	0, dw, 0, szMsg, sizeof(szMsg), NULL );

        char szTitle[250];
        snprintf(szTitle, 250, "Error opening '%s'", fileName);
        MessageBox( szMsg, szTitle );
    }
}

void CMainFrame::OnOIVTutorial()
{
  OpenPdfDocument("Geomec OIV-5.0.pdf", ExePath().c_str());
}

void CMainFrame::OnMeshRefinementManual()
{
  OpenPdfDocument("Mesh refinement guide.pdf", ExePath().c_str());
}

void CMainFrame::OnLicenseBorrowingHelp()
{
  OpenPdfDocument("License borrowing and troubleshooting.pdf", ExePath().c_str());
}

void CMainFrame::OnWellZoominHelp()
{
  OpenPdfDocument("zoominwell.pdf", ExePath().c_str());
}

void CMainFrame::OnGVTQuickTourHelp()
{
  OpenPdfDocument("GVT Quick Tour.pdf", ExePath().c_str());
}

extern HWND WINAPI AfxHtmlHelp(HWND hWnd, LPCTSTR szHelpFilePath, unsigned int nCmd, DWORD_PTR dwData);

void CMainFrame::HtmlHelp(DWORD_PTR dwData, unsigned int nCmd)
{
	CWinApp* pApp = AfxGetApp();
	ASSERT_VALID(pApp);
	assert(pApp->m_pszHelpFilePath != NULL);
	// to call HtmlHelp the m_fUseHtmlHelp must be set in
	// the application's constructor
	assert(pApp->m_eHelpType == afxHTMLHelp);

	CWaitCursor wait;

	PrepareForHelp();

	// need to use top level parent (for the case where m_hWnd is in DLL)
	CWnd* pWnd = EnsureTopLevelParent();

	TRACE(traceAppMsg, 0, _T("HtmlHelp: pszHelpFile = '%s', dwData: $%lx, fuCommand: %d.\n"), pApp->m_pszHelpFilePath, dwData, nCmd);

	// run the HTML Help engine
	if (!AfxHtmlHelp(pWnd->m_hWnd, pApp->m_pszHelpFilePath, nCmd, dwData))
	{
    if (!AfxHtmlHelp(pWnd->m_hWnd, pApp->m_pszHelpFilePath, HH_DISPLAY_TOC, 0))
		  _m()->msg(IDP_NOHELPAVAILABLE);
	}
}

void CMainFrame::OnModelChanged(const CFemAppModel* pModel)
{
	// show/hide model type dependent toolbars
	// show them all if no model loaded (pModel == 0)
	// ShowControlBar(&m_wndTopViewBar, pModel == 0 || (dynamic_cast<const C3DModel*>(pModel) != 0), FALSE);
	// ShowControlBar(&m_wnd2DViewBar,  pModel == 0 || (dynamic_cast<const C2DModel*>(pModel) != 0), FALSE);
}


void CMainFrame::OnViewToolbarVectorDisplay()
{
  ShowControlBar(&m_vectorDialogBar, !m_vectorDialogBar.IsWindowVisible(), FALSE);
}


void CMainFrame::OnUpdateViewToolbarVectorDisplay(CCmdUI *pCmdUI)
{
  pCmdUI->SetCheck(m_vectorDialogBar.IsWindowVisible());	
}


void CMainFrame::OnContextMenu(CWnd* pWnd, CPoint point)
{
	return;
	//int a=0;
	// TODO: Add your message handler code here

	CWnd* wnd = AfxGetApp()->m_pMainWnd;
	HWND hWnd = wnd->m_hWnd;
	CMenu* menu = wnd->GetMenu();

	UINT pos = 3;

	//LPRECT lprcItem;
	//BOOL res = GetMenuItemRect( hWnd, *menu, 3, lprcItem );
	/*GetMenuItemRect(
		hWnd,					// _In_opt_ HWND   hWnd,
		*menu,					// _In_     HMENU  hMenu,
		3,						// _In_     UINT   uItem,
		lprcItem				// _Out_    LPRECT lprcItem
	);*/

	RECT ri;
	BOOL res1 = GetMenuItemRect( hWnd, *menu, 3, &ri );

	CPoint p1_ = CPoint( ri.left, ri.top );
	CPoint p2_ = CPoint( ri.right, ri.bottom );

	wnd->ScreenToClient(&p1_);
	wnd->ScreenToClient(&p2_);

	CItem item( menu, pos );
	std::string name = item.name();


	/*RECT rc;                    // client area of window 
    AfxGetApp()->m_pMainWnd->GetClientRect( &rc );

	RECT rc3 = {ri.left-rc.left,
				ri.top-rc.top,
				ri.right-rc.right,
				ri.bottom-rc.bottom};*/

	CPoint p0(point);
	wnd->ScreenToClient(&p0);
	CPoint p3(p0);
	wnd->ScreenToClient(&p3);

	//CPoint p2(point);
	//AfxGetApp()->m_pMainWnd->ClientToScreen(&p2);
	

	//---------------------
#ifdef kk
	CMenu* pPopup = AfxGetApp()->m_pMainWnd->GetMenu()->GetSubMenu( 0/*pMenuID*/); // -> File submenu
	POINT pp;
	GetCursorPos(&pp);
	//DWORD SelectionMade = pPopup->TrackPopupMenu( TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD, p0.x,p0.y,this);
	DWORD SelectionMade = pPopup->TrackPopupMenu( TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD, p0.x,p0.y,wnd);
#endif
	//---------------------

	/*
	CMenu menu2;
	menu2.CreatePopupMenu();
	menu2.AppendMenu(MF_STRING, 10000, "Text");
	//menu.LoadMenu(IDR_POPUP_MY_MENU);
	CMenu* pPopup = menu2.GetSubMenu( 0 );
	//BOOL res1 = pPopup->TrackPopupMenu(TPM_LEFTALIGN , 0, 0, wnd );
	//BOOL res2 = pPopup->TrackPopupMenu(TPM_LEFTALIGN , p0.x, p0.y, wnd );
	BOOL res3 = pPopup->TrackPopupMenu(TPM_LEFTALIGN , p3.x, p3.y, wnd );
	//BOOL res4 = pPopup->TrackPopupMenu(TPM_LEFTALIGN , point.x, point.y, wnd );
	*/

	CMenu m_RightClickMenu;
	VERIFY(m_RightClickMenu.CreatePopupMenu());
	//m_RightClickMenu.AppendMenu(MF_STRING, 10000, "Text");
    //MENUINFO MenuInfo;
    //m_RightClickMenu.GetMenuInfo(&MenuInfo);
    //MenuInfo.dwStyle = MNS_NOTIFYBYPOS;
    //m_RightClickMenu.SetMenuInfo(&MenuInfo);
    HMENU hMenu = m_RightClickMenu.GetSafeHmenu();
    if (NULL != hMenu)
    {
        CString tempStr;
        // Add menu Items
		CPoint p4(point);
        ClientToScreen(&p4);
        //BOOL res = m_RightClickMenu.TrackPopupMenu( TPM_LEFTALIGN, p3.x, p3.y, wnd);
		//BOOL res = m_RightClickMenu.TrackPopupMenu( TPM_LEFTALIGN, p4.x, p4.y, wnd);
		BOOL res2 = m_RightClickMenu.TrackPopupMenu( TPM_RECURSE | TPM_LEFTALIGN, 0, 0, wnd);

		if( !res2 )
		{
			std::string error_s = util::GetLastErrorAsString();
                //DWORD dwErr = GetLastError();
                //MessageBox("Error doing trackpopup!");
				//CString strMsg;
                //strMsg.Format("getlasterror: 0x%08x", dwErr);
                //MessageBox(strMsg);
				MessageBox( error_s.c_str() );
        }
		//BOOL res = m_RightClickMenu.TrackPopupMenu( TPM_LEFTALIGN, point.x, point.y, wnd);
        //m_RightClickMenu.DestroyMenu();
    }


	// Get a pointer to the button
	//CButton *pButton;
	//pButton = reinterpret_cast<CButton *>(GetDlgItem(IDC_SUBMIT_BTN));

	// Find the rectangle around the button
	//CRect rectSubmitButton;
	//pButton->GetWindowRect(&rectSubmitButton);

}
