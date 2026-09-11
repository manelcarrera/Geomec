// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "DSealect.h"
#include "MultiViewSplitter.h"
#include "MainFrm.h"
#include "DSealectDoc.h"
#include "DSealectDrawView.h"
#include "DSealectTreeView.h"
#include "InfoForm.h"
#include "FormationDefinitionForm.h"
#include "FormationDefinitionDraw.h"
#include "InsituStressesForm.h"
#include "FormationPropertiesForm.h"
#include "FormationPropertiesDraw.h"
#include "DrillingOperationForm.h"
#include "CasingMaterialLibDlg.h"
#include "CementMaterialLibDlg.h"
#include "FormationMatrialLibDlg.h"
#include "CasingOperationForm.h"
#include "CasingOperationDraw.h"
#include "CasingStressForm.h"
#include "CasingStressDraw.h"
#include "CementOperationForm.h"
#include "CementOperationDraw.h"
#include "CompletionAndTestingForm.h"
#include "CompletionAndTestingDraw.h"
#include "ProductionAndInjectionForm.h"
#include "ProductionAndInjectionDraw.h"
#include "ExtremeOperationsForm.h"
#include "ExtremeOperationsDraw.h"
#include "DepthsForAnalysisForm.h"
#include "DepthsForAnalysisDraw.h"
#include "DetailedResultsForm.h"
#include "DetailedResultsDraw.h"
#include "CementPropertiesForm.h"
#include "CementPropertiesDraw.h"
#include "MeshRefinementForm.h"
#include "MeshRefinementDraw.h"
#include "SteppingSchemeForm.h"
#include "SteppingSchemeDraw.h"
#include "OptionsDlg.h"
#include "OutputSelectionForm.h"
#include "OutputSelectionDraw.h"
#include "ResultOverviewForm.h"
#include "ResultOverviewDraw.h"
#include "AnalysisForm.h"
#include "DSealectGlobal.h"
#include "Global.h"
#include "CDataModel.h"

#define NR_INFO_FORM       6

//info forms
#define INFO_FORMATIONS    0
#define INFO_DRILLING      1
#define INFO_DEPTH         2
#define INFO_OPERATIONS    3
#define INFO_ANALYSIS      4
#define INFO_RESULT        5

#if defined _WELLLIFE
	#define APP_TITLE "WellLife"
#else
	#define APP_TITLE "Wellbore Cement Integrity Model"
#endif



//filenames to get infotext
//for the info-forms
const char* sInfoNames[] =
{
	"formations_info",
	"drilling_info",
	"depth_info",
	"operations_info",
	"analysis_info",
	"results_info"
};


#ifdef _DEBUG
#define new DEBUG_NEW
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[] = __FILE__;#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_COMMAND(ID_ACTIONS_CASINGLIBRARY, OnActionsCasinglibrary)
	ON_COMMAND(ID_ACTIONS_CEMENTLIBRARY, OnActionsCementlibrary)
	ON_COMMAND(ID_ACTIONS_FORMATIONLIBRARY, OnActionsFormationlibrary)
	ON_UPDATE_COMMAND_UI(ID_APP_RUN, OnUpdateAppRun)
	ON_COMMAND(ID_ACTIONS_OPTIONS, OnActionsOptions)
	ON_COMMAND(ID_APP_TOOLTIP, OnAppTooltip)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_HELP_INDEX, CFrameWnd::OnHelpIndex)
	ON_COMMAND(ID_HELP_FINDER, CFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_CONTEXT_HELP, CFrameWnd::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, CFrameWnd::OnHelpFinder)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

//##ModelId=3C0DC1590157
CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	
}

//##ModelId=3C0DC1590197
CMainFrame::~CMainFrame()
{
}

//##ModelId=3C0DC15901D4
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	//m_strTitle = APP_TITLE;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	
	if (!m_wndUnitBar.Create(this, CBRS_TOP |CBRS_TOOLTIPS|CBRS_FLYBY))
	{
		TRACE0("Failed to create DlgBar\n");
		return -1;      // Fail to create.
	}


	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}


	//set tooltip button in toolbar as checkbox style
	m_wndToolBar.SetButtonStyle(m_wndToolBar.CommandToIndex(ID_APP_TOOLTIP),TBBS_CHECKBOX);
	


	// TODO: Delete these three lines if you don't want the toolbar to
	// be dockable

	m_wndToolBar.EnableDocking(CBRS_ALIGN_TOP);
	m_wndUnitBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
	DockControlBar(&m_wndUnitBar);

	return 0;
}

//##ModelId=3C0DC159017C
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
//##ModelId=3C0DC1590199
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

//##ModelId=3C0DC15901A5
void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


//##ModelId=3C0DC1590188
BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	
	//Create the splitter
	if(!m_wndSplitter.CreateStatic(this, 1, 3))
		return FALSE;
	
	if(!CreateViews(pContext))
		return FALSE;
		


	return TRUE;
}



//##ModelId=3C0DC15901E6
void CMainFrame::OnDestroy() 
{
	//delete tree item data
	m_pTreeView->ReleaseTreeItem(TVI_ROOT);
	m_pTreeView->GetTreeCtrl().DeleteAllItems();
}



//##ModelId=3C0DC159015B
BOOL CMainFrame::CreateViews(CCreateContext* pContext)
{


	CString sInfo[NR_INFO_FORM];
	for(int i=0 ; i<NR_INFO_FORM ; i++)
	{
		GetInfoString(GetAppPath() + sInfoNames[i],sInfo[i]);
	}


	//Create the TreeView in pane 0,0
	int ViewID;
	CTreeEntryLeaf* pLeaf;
	CTreeEntryBranch* pBranch;

	ViewID = m_wndSplitter.AddView(0, 0, RUNTIME_CLASS(CDSealectTreeView), pContext);
	m_pTreeView = (CDSealectTreeView*) m_wndSplitter.GetView(ViewID);
	m_pTreeView->CreateImageList();
	

	//Build a linked list with the FormViews and DrawViews
	
	// 1 FORMATIONS /////////////////////////////////////////////////
	pBranch = InsertTreeEntryBranch(RUNTIME_CLASS(CInfoForm),
									RUNTIME_CLASS(CDSealectDrawView),
									"1 - Formations",
									sInfo[INFO_FORMATIONS],	
									IDI_ICStonedFolder,
									pContext,
									NULL);

	pLeaf = InsertTreeEntryLeaf(RUNTIME_CLASS(CFormationDefinitionForm),
		 						RUNTIME_CLASS(CFormationDefinitionDraw),
								"Formation Definition",
								IDI_ICBeGenericFile,
								pContext,
								pBranch,
								pBranch);

	pLeaf = InsertTreeEntryLeaf(RUNTIME_CLASS(CInsituStressesForm),
		 						RUNTIME_CLASS(CInsituStressesDraw),
								"Insitu Stresses",
								IDI_ICWildFolder37,
								pContext,
								pLeaf,
								pBranch);

	pLeaf = InsertTreeEntryLeaf(RUNTIME_CLASS(CFormationPropertiesForm),
		 						RUNTIME_CLASS(CFormationPropertiesDraw),
								"Formation Properties",
								IDI_ICSphere,
								pContext,
								pLeaf,
								pBranch);


	// 2 DRILLING /////////////////////////////////////////////////
	pBranch = InsertTreeEntryBranch(RUNTIME_CLASS(CInfoForm),
									RUNTIME_CLASS(CDSealectDrawView),
									"2 - Drilling And Completion",
									sInfo[INFO_DRILLING],	
									IDI_ICPurpleTabFolder,
									pContext,
									pLeaf);


	pLeaf = InsertTreeEntryLeaf(RUNTIME_CLASS(CDrillingOperationForm),
		 						RUNTIME_CLASS(CDrillingOperationDraw),
								"Drilling Operation",
								IDI_ICBeEdit,
								pContext,
								pBranch,
								pBranch);

	pLeaf = InsertTreeEntryLeaf(RUNTIME_CLASS(CCasingOperationForm),
		 						RUNTIME_CLASS(CCasingOperationDraw),
								"Casing Operation",
								IDI_ICBeKaleidoscope,
								pContext,
								pLeaf,
								pBranch);

	pLeaf = InsertTreeEntryLeaf(RUNTIME_CLASS(CCasingStressForm),
		 						RUNTIME_CLASS(CCasingStressDraw),
								"Casing Stresses",
								IDI_ICTubeEmptyM,
								pContext,
								pLeaf,
								pBranch);


	pLeaf = InsertTreeEntryLeaf(RUNTIME_CLASS(CCementOperationForm),
		 						RUNTIME_CLASS(CCementOperationDraw),
								"Cement Operation",
								IDI_ICShieldM,
								pContext,
								pLeaf,
								pBranch);



	// 3 DEPTH/CEMENT /////////////////////////////////////////////////
	pBranch = InsertTreeEntryBranch(RUNTIME_CLASS(CInfoForm),
									RUNTIME_CLASS(CDSealectDrawView),
									"3 - Depths/Cement for analysis",
									sInfo[INFO_DEPTH],	
									IDI_ICControlFolders,
									pContext,
									pLeaf);


	pLeaf = InsertTreeEntryLeaf(RUNTIME_CLASS(CDepthsForAnalysisForm),
		 						RUNTIME_CLASS(CDepthsForAnalysisDraw),
								"Depth for Analysis",
								IDI_ICBeBounce,
								pContext,
								pBranch,
								pBranch);

	pLeaf = InsertTreeEntryLeaf(RUNTIME_CLASS(CCementPropertiesForm),
		 						RUNTIME_CLASS(CCementPropertiesDraw),
								"Cement Properties",
								IDI_ICExpand,
								pContext,
								pLeaf,
								pBranch);


	// 4 OPERATION REGIME /////////////////////////////////////////////////

	pBranch = InsertTreeEntryBranch(RUNTIME_CLASS(CInfoForm),
									RUNTIME_CLASS(CDSealectDrawView),
									"4 - Operational Regimes",
									sInfo[INFO_OPERATIONS],	
									IDI_ICShadedUtilities,
									pContext,
									pLeaf);


	pLeaf = InsertTreeEntryLeaf(RUNTIME_CLASS(CCompletionAndTestingForm),
		 						RUNTIME_CLASS(CCompletionAndTestingDraw),
								"Completion & testing",
								IDI_ICTools,
								pContext,
								pBranch,
								pBranch);

	pLeaf = InsertTreeEntryLeaf(RUNTIME_CLASS(CProductionAndInjectionForm),
		 						RUNTIME_CLASS(CProductionAndInjectionDraw),
								"Production & Injection",
								IDI_ICRecycling,
								pContext,
								pLeaf,
								pBranch);

	pLeaf = InsertTreeEntryLeaf(RUNTIME_CLASS(CExtremeOperationsForm),
		 						RUNTIME_CLASS(CExtremeOperationsDraw),
								"Extreme Operations",
								IDI_ICSunburst,
								pContext,
								pLeaf,
								pBranch);


	// 5 ANALYSIS CONTROL /////////////////////////////////////////////////

	pBranch = InsertTreeEntryBranch(RUNTIME_CLASS(CInfoForm),
									RUNTIME_CLASS(CDSealectDrawView),
									"5 - Analysis Control",
									sInfo[INFO_ANALYSIS],	
									IDI_ICDropFolder,
									pContext,
									pLeaf);


	pLeaf = InsertTreeEntryLeaf(RUNTIME_CLASS(CMeshRefinementForm),
		 						RUNTIME_CLASS(CMeshRefinementDraw),
								"Mesh refinement",
								IDI_ICGraphFolder,
								pContext,
								pBranch,
								pBranch);

	/*pLeaf = InsertTreeEntryLeaf(RUNTIME_CLASS(CSteppingSchemeForm),
		 						RUNTIME_CLASS(CSteppingSchemeDraw),
								"Stepping Scheme",
								IDI_ICTime,
								pContext,
								pLeaf,
								pBranch);
*/
	pLeaf = InsertTreeEntryLeaf(RUNTIME_CLASS(COutputSelectionForm),
		 						RUNTIME_CLASS(COutputSelectionDraw),
								"Output Selection",
								IDI_ICBeCardStack,
								pContext,
								pLeaf,
								pBranch);


	pLeaf = InsertTreeEntryLeaf(RUNTIME_CLASS(CAnalysisForm),
		 						RUNTIME_CLASS(CAnalysisDraw),
								"Analysis",
								IDI_ICCalculator3D,
								pContext,
								pLeaf,
								pBranch);



	// 6 RESULT /////////////////////////////////////////////////

	pBranch = InsertTreeEntryBranch(RUNTIME_CLASS(CInfoForm),
									RUNTIME_CLASS(CDSealectDrawView),
									"6 - Results Report",
									sInfo[INFO_RESULT],	
									IDI_ICShadedReports,
									pContext,
									pLeaf);


	pLeaf = InsertTreeEntryLeaf(RUNTIME_CLASS(CResultOverviewForm),
		 						RUNTIME_CLASS(CResultOverviewDraw),
								"Overview Tables",
								IDI_ICBeScript,
								pContext,
								pBranch,
								pBranch);

	pLeaf = InsertTreeEntryLeaf(RUNTIME_CLASS(CDetailedResultsForm),
		 						RUNTIME_CLASS(CDetailedResultsDraw),
								"Detailed Results",
								IDI_ICBeMagnify,
								pContext,
								pLeaf,
								pBranch);

	m_wndSplitter.SetColumnInfo( 0, 180, 0 );

	return TRUE;
}

//##ModelId=3CA011A00261
CTreeEntryLeaf* CMainFrame::InsertTreeEntryLeaf(CRuntimeClass* FormView,CRuntimeClass* DrawView,CString text,UINT IconID,CCreateContext* pContext, ITreeEntryBase* pParentEntry,CTreeEntryBranch* pBranch)
{
	CBaseFormView     *pFormView;
	CDSealectDrawView *pDrawView;
	CTreeEntryLeaf    *pLeaf;
	int ViewID;

	ViewID = m_wndSplitter.AddView(0, 2, DrawView, pContext);
	pDrawView = (CDSealectDrawView*) m_wndSplitter.GetView(ViewID);
	pDrawView->m_ViewID=ViewID;

	ViewID = m_wndSplitter.AddView(0, 1, FormView, pContext);
	pFormView = (CBaseFormView*) m_wndSplitter.GetView(ViewID);
	

	pFormView->DrawView(pDrawView);

	pLeaf = new CTreeEntryLeaf(pFormView,
							   &m_wndSplitter,
							   text,
							   ViewID,
							   IconID,
							   pParentEntry);

	m_pTreeView->InsertItem(pLeaf,pBranch,NULL);
	return pLeaf;
}

//##ModelId=3CA011A002CE
CTreeEntryBranch* CMainFrame::InsertTreeEntryBranch(CRuntimeClass* FormView,CRuntimeClass* DrawView,CString LabelText,CString InfoText,UINT IconID,CCreateContext* pContext, ITreeEntryBase* pParentEntry)
{
	CBaseFormView     *pFormView;
	CDSealectDrawView *pDrawView;
	CTreeEntryBranch  *pBranch;
	int ViewID;

	ViewID = m_wndSplitter.AddView(0, 2, DrawView, pContext);
	pDrawView = (CDSealectDrawView*) m_wndSplitter.GetView(ViewID);
	pDrawView->m_ViewID=ViewID;

	ViewID = m_wndSplitter.AddView(0, 1, FormView, pContext);
	pFormView = (CBaseFormView*) m_wndSplitter.GetView(ViewID);

	pFormView->DrawView(pDrawView);

	pBranch = new CTreeEntryBranch(pFormView,
								   &m_wndSplitter,
								   LabelText,
								   ViewID,
								   InfoText,
								   IconID,
								   pParentEntry);

	m_pTreeView->InsertItem(pBranch,NULL,NULL);
	return pBranch;
}

//##ModelId=3C0DC1590158
void CMainFrame::GetInfoString(const CString &sFrom, CString &sInfo)
{
	CFile f;
	CFileException e;
	if( !f.Open( sFrom, CFile::modeRead, &e ) )
	{
		sInfo = "Couldn't open file: ";
		sInfo =sInfo + sFrom;
		return;
	}
	
	LPTSTR pbuf = sInfo.GetBufferSetLength(f.GetLength()); 
	UINT nBytesRead = f.Read( pbuf, f.GetLength());
	f.Close();

}


//##ModelId=3C2043FD0129
BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if(m_wndUnitBar.OnCmdMsg(nID,nCode,pExtra,pHandlerInfo))
		return TRUE;

	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

//##ModelId=3C8D9E15037C
void CMainFrame::OnActionsCasinglibrary() 
{
	CCasingMaterialLibDlg casld(GetDoc()->CasingMatDatabasePath(),GetDoc()->DSealectModel());

	casld.DoModal();
	
}

//##ModelId=3C8D9E15038D
void CMainFrame::OnActionsCementlibrary() 
{
	CCementMaterialLibDlg cesld(GetDoc()->CementMatDatabasePath());

	cesld.DoModal();
	
}

//##ModelId=3C8D9E15039B
void CMainFrame::OnActionsFormationlibrary() 
{
	CFormationMatrialLibDlg fmld(GetDoc()->FormationMatDatabasePath());

	fmld.DoModal();
	
}

//##ModelId=3DDA0C510293
void CMainFrame::OnUpdateAppRun(CCmdUI* pCmdUI) 
{	
	//TRACE("Test");
	if(GetDoc()->GetPathName().IsEmpty() || GetDoc()->IsModified()|| !(GetDoc()->DSealectModel()->CanStartAnalysis()))
	{
		pCmdUI->Enable(FALSE);
	}
	else
	{
		pCmdUI->Enable(TRUE);
	}	
}

//##ModelId=3DDA0C5102A3
void CMainFrame::OnActionsOptions() 
{
	COptionsDlg odlg;

	odlg.DoModal();
	
}

//##ModelId=3DDA0C5102A5
void CMainFrame::OnAppTooltip() 
{
	// TODO: Add your command handler code here
	GetDoc()->m_bShowToolTip =!GetDoc()->m_bShowToolTip;
	
}
