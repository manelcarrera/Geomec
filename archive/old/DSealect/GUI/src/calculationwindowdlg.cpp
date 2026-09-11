// calculationwindowdlg.cpp : implementation file
//

#include "stdafx.h"
#include "dsealect.h"
#include "DSealectDoc.h"
#include "CBatchTable.h"
#include "DoubleEdit.h"
#include "calculationwindowdlg.h"
#include "DSealectGlobal.h"
#include "Global.h"
#include "MultiViewSplitter.h"
#include "MainFrm.h"
#include "CDataModel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[] = __FILE__;#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CCalculationWindowDlg dialog

//##ModelId=3DDA0C5E02D6
CCalculationWindowDlg::CCalculationWindowDlg(CBatchTable* pBatchTable, CWnd* pParent /*=NULL*/)
	: CDialog(CCalculationWindowDlg::IDD, pParent)
{
	m_pBatchTable = pBatchTable;
	//{{AFX_DATA_INIT(CCalculationWindowDlg)
	m_iMaxNumberOfIterations = 0;
	m_iNumberOfTimeSteps = 0;
	m_intDefaultSteppingScheme = -1;
	//}}AFX_DATA_INIT
}


//##ModelId=3DDA0C5E036E
void CCalculationWindowDlg::DoDataExchange(CDataExchange* pDX)
{
	
	if(!pDX->m_bSaveAndValidate)
	{
		if(m_pCurrentBatchTableRow)
		{
			((CButton*)GetDlgItem(IDC_optDefaultSteppingScheme))->SetCheck(m_pCurrentBatchTableRow->DefaultSteppingScheme());
			m_iNumberOfTimeSteps = m_pCurrentBatchTableRow->TimeStepNumber();
			m_iMaxNumberOfIterations = m_pCurrentBatchTableRow->MaxNumberOfIterations();
			m_intDefaultSteppingScheme = m_pCurrentBatchTableRow->DefaultSteppingScheme() ? 0:1;
			
			((CButton*)GetDlgItem(IDC_chkStopBatchRunIfDivergence))->SetCheck(m_pCurrentBatchTableRow->StopBatchRun() ? 1:0);
			((CButton*)GetDlgItem(IDC_chkAllowFurtherPostProcessing))->SetCheck(m_pCurrentBatchTableRow->FurtherPostProcessing() ? 1:0);
			//((CButton*)GetDlgItem(IDC_chkNoCalculationMonitoring))->SetCheck(m_pCurrentBatchTableRow->NoMonitoring() ? 1:0);
			((CButton*)GetDlgItem(IDC_chkSaveDiagnostic))->SetCheck(m_pCurrentBatchTableRow->SaveDiagnostic() ? 1:0);
		}
	}

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCalculationWindowDlg)
	DDX_Control(pDX, IDC_bttnStart, m_btnStart);
	DDX_Control(pDX, IDC_lstAnalysis, m_lstAnalysis);
	DDX_Control(pDX, IDC_chkStopBatchRunIfDivergence, m_chkStopBatchRunIfDiv);
	DDX_Control(pDX, IDC_chkSaveDiagnostic, m_chkSaveDiagnostic);
	DDX_Control(pDX, IDC_chkAllowFurtherPostProcessing, m_chkAllowFurtherPostProc);
	DDX_Control(pDX, IDC_cmbIterationScheme, m_cmbIterationScheme);
	DDX_Control(pDX, IDC_cmbConvergenceNorm, m_cmbConvergenceNorm);
	DDX_Text(pDX, IDC_ebMaxNumberOfIterations, m_iMaxNumberOfIterations);
	DDV_MinMaxInt(pDX,m_iMaxNumberOfIterations,2,100);
	DDX_Text(pDX, IDC_ebNrOfTimeSteps, m_iNumberOfTimeSteps);
	DDV_MinMaxInt(pDX,m_iNumberOfTimeSteps,3,100);
	DDX_Radio(pDX, IDC_optDefaultSteppingScheme, m_intDefaultSteppingScheme);
	//}}AFX_DATA_MAP

	if(m_pCurrentBatchTableRow)
	{
		if(pDX->m_bSaveAndValidate)
		{
			m_pCurrentBatchTableRow->StopBatchRun(m_chkStopBatchRunIfDiv.GetCheck() == 1);
			m_pCurrentBatchTableRow->FurtherPostProcessing(m_chkAllowFurtherPostProc.GetCheck() == 1);
			//m_pCurrentBatchTableRow->NoMonitoring(m_chkNoCalculationMonitoring.GetCheck() == 1);
			m_pCurrentBatchTableRow->SaveDiagnostic(m_chkSaveDiagnostic.GetCheck() == 1);

			//from ctrl to data
			m_pCurrentBatchTableRow->DefaultSteppingScheme(m_intDefaultSteppingScheme == 0);
			if(m_pCurrentBatchTableRow->DefaultSteppingScheme())
			{
				// Set defaults
				m_pCurrentBatchTableRow->IterationScheme(CBatchTableRow::eIterationScheme(1+IDS_CONSTANT)); //IDS_CONSTANT=2000,IDS_REGULARNR=2001,IDS_MODIFIEDNR=2002
				m_pCurrentBatchTableRow->ConvergenceNorm(CBatchTableRow::eConvergenceNorm(2+IDS_FORCE)); //IDS_FORCE=3000,IDS_DISPLACEMENT=3001,IDS_ENERGY=3002
				m_pCurrentBatchTableRow->TimeStepNumber(6);
				m_pCurrentBatchTableRow->MaxNumberOfIterations(10);
			}
			else
			{
				m_pCurrentBatchTableRow->IterationScheme(CBatchTableRow::eIterationScheme(m_cmbIterationScheme.GetCurSel()+IDS_CONSTANT));
				m_pCurrentBatchTableRow->ConvergenceNorm(CBatchTableRow::eConvergenceNorm(m_cmbConvergenceNorm.GetCurSel()+IDS_FORCE));
				m_pCurrentBatchTableRow->TimeStepNumber(m_iNumberOfTimeSteps);
				m_pCurrentBatchTableRow->MaxNumberOfIterations(m_iMaxNumberOfIterations);
			}
		}
		else
		{
			//update controls (from data to ctrls)
			m_cmbIterationScheme.SetCurSel((UINT)m_pCurrentBatchTableRow->IterationScheme()-IDS_CONSTANT);
			m_cmbConvergenceNorm.SetCurSel((UINT)m_pCurrentBatchTableRow->ConvergenceNorm()-IDS_FORCE);
		}
	}
	
	for(int i=0 ;i< GetDoc()->DSealectModel()->BatchTable()->RowCount();i++)
	{
		GetDoc()->DSealectModel()->BatchTable()->GetRow(i)->UseForCalculation(m_lstAnalysis.GetCheck(i));
	}
	
	m_optDefaultSteppingScheme.Enable(m_intDefaultSteppingScheme == 1); //dis-/enable frame
	m_btnStart.EnableWindow(m_pBatchTable->UseForCalculation());
}


BEGIN_MESSAGE_MAP(CCalculationWindowDlg, CDialog)
	//{{AFX_MSG_MAP(CCalculationWindowDlg)
	ON_BN_CLICKED(IDC_bttnCancel, OnbttnCancel)
	ON_BN_CLICKED(IDC_bttnStart, OnbttnStart)
	ON_BN_CLICKED(IDC_chkAllowFurtherPostProcessing, OnchkAllowFurtherPostProcessing)
	ON_BN_CLICKED(IDC_chkSaveDiagnostic, OnchkSaveDiagnostic)
	ON_BN_CLICKED(IDC_chkStopBatchRunIfDivergence, OnchkStopBatchRunIfDivergence)
	ON_CBN_CLOSEUP(IDC_cmbConvergenceNorm, OnCloseupcmbConvergenceNorm)
	ON_CBN_CLOSEUP(IDC_cmbIterationScheme, OnCloseupcmbIterationScheme)
	ON_BN_CLICKED(IDC_optAdvancedOptionsForSteppingScheme, OnoptAdvancedOptionsForSteppingScheme)
	ON_BN_CLICKED(IDC_optDefaultSteppingScheme, OnoptDefaultSteppingScheme)
	ON_NOTIFY(NM_CLICK, IDC_lstAnalysis, OnClicklstAnalysis)
	ON_EN_KILLFOCUS(IDC_ebNrOfTimeSteps, OnKillfocusebNrOfTimeSteps)
	ON_EN_KILLFOCUS(IDC_ebMaxNumberOfIterations, OnKillfocusebMaxNumberOfIterations)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_lstAnalysis, OnItemchangedlstAnalysis)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCalculationWindowDlg message handlers

//##ModelId=3DDA0C5E0371
void CCalculationWindowDlg::OnbttnCancel() 
{
	CDialog::OnCancel();
	
}

//##ModelId=3DDA0C5E037E
void CCalculationWindowDlg::OnbttnStart() 
{
	if(GetDoc()->GetPathName().IsEmpty())
	{
		AfxMessageBox("First save this project");
		return;
	}

	CString sFileName;
	CString sPath;

	//use the Pathname of the document as workingdir and basename
	SplitPathAndFileName(GetDoc()->GetPathName(),sPath,sFileName);

	CStartAnalysisData  sad;

	sad.DatabaseName = GetDoc()->GetPathName();
	sad.DianaPath    = GetDianaPath(); //"D:\\users\\all\\DSealect\\Diana 8"
  sad.DianaSharePath = GetDianaSharePath();
	sad.Basename     = RemoveExtension(sFileName);
	sad.Workingdir   = sPath;
	sad.TempDir		 = GetDoc()->TempDir();

	GetDoc()->DSealectModel()->StartAnalysis(sad);
	GetDoc()->SetModifiedFlag(FALSE);
	GetDoc()->UpdateAllViews(0);
	BuildAnalysisList();
	UpdateData();
}

//##ModelId=3DDA0C5E0380
void CCalculationWindowDlg::OnchkAllowFurtherPostProcessing() 
{
	UpdateData();
	
}

//void CCalculationWindowDlg::OnchkNoCalculationMonitoring() 
//{
//	UpdateData();
	
//}

//##ModelId=3DDA0C5E0382
void CCalculationWindowDlg::OnchkSaveDiagnostic() 
{
	UpdateData();
	
}

//##ModelId=3DDA0C5E0384
void CCalculationWindowDlg::OnchkStopBatchRunIfDivergence() 
{
	UpdateData();
	
}

//##ModelId=3DDA0C5E038D
void CCalculationWindowDlg::OnCloseupcmbConvergenceNorm() 
{
	UpdateData();	
	UpdateData(FALSE);
	//GetDocument()->SetModifiedFlag(TRUE);
	
}

//##ModelId=3DDA0C5E038F
void CCalculationWindowDlg::OnCloseupcmbIterationScheme() 
{
	UpdateData();	
	UpdateData(FALSE);
	//GetDocument()->SetModifiedFlag(TRUE);
	
}

//##ModelId=3DDA0C5E0391
void CCalculationWindowDlg::OnoptAdvancedOptionsForSteppingScheme() 
{
	UpdateData();	
	UpdateData(FALSE);
	//GetDocument()->SetModifiedFlag(TRUE);
	
}

//##ModelId=3DDA0C5E0393
void CCalculationWindowDlg::OnoptDefaultSteppingScheme() 
{
	UpdateData();	
	UpdateData(FALSE);
	//GetDocument()->SetModifiedFlag(TRUE);
	
}

//##ModelId=3DDA0C5E0395
void CCalculationWindowDlg::OnClicklstAnalysis(NMHDR* pNMHDR, LRESULT* pResult) 
{
	//CListCtrl::OnClicklstAnalysis(pNMHDR,pResult);


	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	TRACE("SubItem=%d\n",pNMListView->iSubItem);
	CListCtrl* pList=0;
	pList=(CListCtrl*)GetDlgItem(IDC_lstAnalysis);
		
	if (pNMListView->iItem<0)
	{
		m_pCurrentBatchTableRow	=0; 
		UpdateData(FALSE);
		UpdateData();
		return ;
	}
	else
	{
		if(pNMListView->iSubItem !=0)
		{
			int iFItem=((pNMListView->iItem)); 
			m_pCurrentBatchTableRow = m_pBatchTable->GetRow(iFItem);
		}
		//pList->SetItem(iFItem, 0, LVIF_STATE, NULL, 0, LVIS_SELECTED, LVIS_SELECTED, 0);
		//pList->SetItem(iFItem, 0, LVIF_STATE, NULL, 0, LVIS_FOCUSED, LVIS_FOCUSED, 0);
		
	}
	
	*pResult = 0;


	UpdateData(FALSE);
	UpdateData();

	
}

//##ModelId=3DDA0C5E039F
void CCalculationWindowDlg::OnKillfocusebNrOfTimeSteps() 
{
	UpdateData();
	
}

//##ModelId=3DDA0C5E03AD
void CCalculationWindowDlg::OnKillfocusebMaxNumberOfIterations() 
{
	UpdateData();
	
}

//##ModelId=3DDA0C5E03AF
BOOL CCalculationWindowDlg::OnInitDialog() 
{
	bInitState = TRUE;
	//m_cImageList.Create(16,16,ILC_COLOR32,3,0);
	//m_cImageList.Add(AfxGetApp()->LoadIcon(IDI_DIANA));
	//m_cImageList.Add(AfxGetApp()->LoadIcon(IDI_DIANA));
	//m_cImageList.Add(AfxGetApp()->LoadIcon(IDI_DIANA));	

	m_pCurrentBatchTableRow = 0;//GetDoc()->DSealectModel()->BatchTable()->GetRow(0);
	
	CDialog::OnInitDialog();
	
	m_optDefaultSteppingScheme.Set(this, IDC_fraAdvancedOptionsForSteppingScheme);
	for(UINT i=IDS_CONSTANT;i<=IDS_MODIFIEDNR;i++)
	{
		CString strIter;
		strIter.LoadString(i);
		m_cmbIterationScheme.AddString(strIter);
	}

	for(UINT j=IDS_FORCE;j<=IDS_ENERGY;j++)
	{
		CString strConv;
		strConv.LoadString(j);
		m_cmbConvergenceNorm.AddString(strConv);
	}
	

	CLengthQuantity temp; // temporary quantity only needed to get the unit-string 
	//Create Columns
	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_FMT|LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM;
	lvColumn.fmt= LVCFMT_LEFT;
	lvColumn.cx=60;
	
	CListCtrl* pList=(CListCtrl*)GetDlgItem(IDC_lstAnalysis);
	
	ASSERT(pList);

	pList->SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES|LVS_EX_SUBITEMIMAGES);	

	lvColumn.iSubItem = 0;
	lvColumn.cx = 42;
	lvColumn.pszText = "Select";
	pList->InsertColumn(0, &lvColumn);

	lvColumn.iSubItem = 1;
	lvColumn.cx = 55;
	lvColumn.pszText = "Run";
	pList->InsertColumn(1, &lvColumn);

	lvColumn.iSubItem = 2;
	lvColumn.cx = 56;
	lvColumn.pszText = "Success";
	pList->InsertColumn(2, &lvColumn);

	lvColumn.iSubItem = 3;
	lvColumn.cx = 56;
	lvColumn.pszText = "Result";
	pList->InsertColumn(3, &lvColumn);

	lvColumn.iSubItem = 4;
	lvColumn.cx = 56;
	lvColumn.pszText = "Scenario";
	pList->InsertColumn(4, &lvColumn);
	
	lvColumn.iSubItem = 5;
	lvColumn.cx = 55;
	CString strUnit = ("TMD ");
	strUnit += GetUnitString(&temp);
	lvColumn.pszText = strUnit.GetBufferSetLength(strUnit.GetLength());
	pList->InsertColumn(5, &lvColumn);
	
	lvColumn.iSubItem = 6;
	lvColumn.cx = 50;
	lvColumn.pszText = "Section";
	pList->InsertColumn(6, &lvColumn);
	
	lvColumn.iSubItem = 7;
	lvColumn.cx = 72;
	lvColumn.pszText = "Cement Mat.";
	pList->InsertColumn(7, &lvColumn);

	BuildAnalysisList();
	
	UpdateData(FALSE);

	// TOOLTIP
	static short bInit=0;
	if(!bInit)
	{
		//One time initialization!!
		bInit=1;
		
		m_toolTip.Create(this);
		CRect rect;
		CString sTmp;

		sTmp.LoadString(IDTT_MORE_FiniteElementAnalysis);
		m_toolTip.AddTool( GetDlgItem(IDC_bttnStart), sTmp);
		GetDlgItem(IDC_bttnStart)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_CAW_chkAllowFurtherPostProcessing);
		m_toolTip.AddTool( GetDlgItem(IDC_chkAllowFurtherPostProcessing), sTmp);
		GetDlgItem(IDC_chkAllowFurtherPostProcessing)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_CAW_chkSaveDiagnostic);
		m_toolTip.AddTool( GetDlgItem(IDC_chkSaveDiagnostic), sTmp);
		GetDlgItem(IDC_chkSaveDiagnostic)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_CAW_fraAdvancedOptionsForSteppingScheme);
		m_toolTip.AddTool( GetDlgItem(IDC_fraAdvancedOptionsForSteppingScheme), sTmp);
		GetDlgItem(IDC_fraAdvancedOptionsForSteppingScheme)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		//IDTT_CAW_bttnStart
		//IDTT_CAW_chkAllowFurtherPostProcessing
		//IDTT_CAW_chkSaveDiagnostic
		//IDTT_CAW_fraAdvancedOptionsForSteppingScheme
		

		CRect rectMargin(TOOL_TIP_MARGE,TOOL_TIP_MARGE,TOOL_TIP_MARGE,TOOL_TIP_MARGE); //left, top, right, bottom
		m_toolTip.SetMargin(rectMargin);
		m_toolTip.Activate(TRUE);
	}
	
	//GetDlgItem(IDC_optDefaultSteppingScheme)->SetFont(GetFont(TRUE));	
	//GetDlgItem(IDC_optAdvancedOptionsForSteppingScheme)->SetFont(GetFont(TRUE));	
	bInitState = FALSE;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//##ModelId=3DDA0C5E035E
BOOL CCalculationWindowDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN)
	{
		CEdit* pMNI = (CEdit*)GetDlgItem(IDC_ebMaxNumberOfIterations);
		CEdit* pNOTS = (CEdit*)GetDlgItem(IDC_ebNrOfTimeSteps);
		
		if(GetFocus() == pMNI || GetFocus() == pNOTS) // if edit-box has focus
		{
			if(pMsg->wParam == VK_RETURN) // UpdateData when hit Return-key (doesn't work initially)
				UpdateData();
		}
	}

	if(GetDoc()->m_bShowToolTip)
		m_toolTip.RelayEvent(pMsg);

	return CWnd::PreTranslateMessage(pMsg);

}


//##ModelId=3DDA0C5E0284
void CCalculationWindowDlg::BuildAnalysisList()
{
	//get the current unit system.
	CDSealectDoc* pDoc = (CDSealectDoc*)GetDoc();
	QU::UNIT us = pDoc->UnitSystem();

	CListCtrl* pList = 0;
	pList=(CListCtrl*)GetDlgItem(IDC_lstAnalysis);
	
	if(!pList)
		return;

	pList->DeleteAllItems();
	pList->SetImageList(&m_cImageList, LVSIL_NORMAL);
	
	//Create items
	LV_ITEM lvItem;
	
	lvItem.mask = LVIF_TEXT|LVIF_STATE|LVIF_IMAGE;
	lvItem.state = 0;
	lvItem.stateMask = 0;

	int iCurrentIndex;
	for(int j = 0 ; j < GetDoc()->DSealectModel()->BatchTable()->RowCount(); j++)
	{
		CBatchTableRow *pBTR = GetDoc()->DSealectModel()->BatchTable()->GetRow(j);
		CString sTmp;
		lvItem.iItem = j;

		lvItem.iSubItem = 0;
		lvItem.pszText = "";
		pList->InsertItem(&lvItem);
		pList->SetCheck(j,pBTR->UseForCalculation());
		
		lvItem.iSubItem = 1;
		//lvItem.pszText = "Test";
		//lvItem.iImage = 1;
		//pList->SetItem(&lvItem);
		if(pBTR->IsCalculated())
			sTmp = "YES";
		else
			sTmp = "NO";
		pList->SetItemText(j,1,sTmp.GetBufferSetLength(sTmp.GetLength()));

		if(pBTR->CalculationSucceeded())
			sTmp = "YES";
		else
			sTmp = "NO";
		pList->SetItemText(j,2,sTmp.GetBufferSetLength(sTmp.GetLength()));

		if(pBTR->HasResults())
			sTmp = "YES";
		else
			sTmp = "NO";

		pList->SetItemText(j,3,sTmp.GetBufferSetLength(sTmp.GetLength()));

		sTmp.Format("%i",pBTR->ScenarioNr() + 1);
		pList->SetItemText(j,4,sTmp.GetBufferSetLength(sTmp.GetLength()));
		sTmp=CDoubleEdit::Format(pBTR->AnalysisPoint()->TMD()->Value(us));
		pList->SetItemText(j,5,sTmp.GetBufferSetLength(sTmp.GetLength()));

		if(pBTR->IsTail())
			sTmp = "Tail";
		else
			sTmp = "Lead";

		pList->SetItemText(j,6,sTmp.GetBufferSetLength(sTmp.GetLength()));
		
		sTmp= pBTR->CementMaterial()->Name();
		pList->SetItemText(j,7,sTmp.GetBufferSetLength(sTmp.GetLength()));

		if(m_pBatchTable->GetRow(j) == m_pCurrentBatchTableRow)
			iCurrentIndex = j;
	}
	
	if(!m_pCurrentBatchTableRow)
		iCurrentIndex = 0;
	
	m_pCurrentBatchTableRow = m_pBatchTable->GetRow(iCurrentIndex);
	pList->SetItem(iCurrentIndex, 0, LVIF_STATE, NULL, 0, LVIS_SELECTED, LVIS_SELECTED, 0);
	pList->EnsureVisible(iCurrentIndex + 1,true);

}

//##ModelId=3DDA0C5E03B1
void CCalculationWindowDlg::OnItemchangedlstAnalysis(NMHDR* pNMHDR, LRESULT* pResult) 
{

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	//TRACE("pNMListView->iItem = %d\n",pNMListView->iItem); 
	//TRACE("pNMListView->uChanged = %d\n",pNMListView->uChanged); 
	//TRACE("pNMListView->uNewState = %d\n",pNMListView->uNewState); 
	//TRACE("pNMListView->uOldState = %d\n",pNMListView->uOldState); 

	if(!bInitState)
	{
		UpdateData(FALSE);
	}
	*pResult = 0;
}




