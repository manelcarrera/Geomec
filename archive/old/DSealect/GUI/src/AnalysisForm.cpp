// AnalysisForm.cpp : implementation file
//
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif  // _MSC_VER
#include "stdafx.h"
#include "dsealect.h"
#include "AnalysisForm.h"
#include "treeobject.h"
#include "DSealectGlobal.h"
#include "DoubleFormat.h"
#include "CalculationWindowDlg.h"
#include "CDataModel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif


/////////////////////////////////////////////////////////////////////////////
// CAnalysisForm

IMPLEMENT_DYNCREATE(CAnalysisForm, CBaseFormView)

//##ModelId=3C0DC161008E
CAnalysisForm::CAnalysisForm()
	: CBaseFormView(CAnalysisForm::IDD)
{
	//{{AFX_DATA_INIT(CAnalysisForm)
	//}}AFX_DATA_INIT
}

//##ModelId=3C0DC16100AC
CAnalysisForm::~CAnalysisForm()
{
	if(m_toolTip)
		delete m_toolTip;
}


//##ModelId=3C0DC161009F
void CAnalysisForm::DoDataExchange(CDataExchange* pDX)
{	
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAnalysisForm)
	DDX_Control(pDX, IDC_lstAnalysis, m_lstAnalysis);
	//}}AFX_DATA_MAP

	if(!CheckEntry())
		return;

	if(!GetModel()->BatchTable()->IsBuild())
	{
		ASSERT(GetModel()->BatchTable()->CanBuildBatchTable());
		GetModel()->BatchTable()->BuildBatchTable();
	}
	GetModel()->BatchTable()->UpdateChanges();


	if(pDX->m_bSaveAndValidate)
	{
		//for(int i=0 ;i<GetModel()->BatchTable()->RowCount();i++)
		//{
		//	GetModel()->BatchTable()->GetRow(i)->UseForCalculation( m_lstAnalysis.GetCheck(i));
		//}
	}
	else
	{
		BuildAnalysisList();
	}

	if(GetDoc()->GetPathName().IsEmpty() || GetDoc()->IsModified())
	{
		GetDlgItem(IDC_btnRunAnalysis)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_btnRunAnalysis)->EnableWindow(TRUE);
	}

	DrawView()->Invalidate();
	TreeEntry()->UpdateEnable();
	UpdateButtons();

}


BEGIN_MESSAGE_MAP(CAnalysisForm, CBaseFormView)
	//{{AFX_MSG_MAP(CAnalysisForm)
	ON_BN_CLICKED(IDC_btnSaveProject, OnbtnSaveProject)
	ON_BN_CLICKED(IDC_btnRunAnalysis, OnbtnRunAnalysis)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_lstAnalysis, OnItemchangedlstAnalysis)
	ON_NOTIFY(NM_CLICK, IDC_lstAnalysis, OnClicklstAnalysis)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAnalysisForm diagnostics

#ifdef _DEBUG
//##ModelId=3C0DC16100AE
void CAnalysisForm::AssertValid() const
{
	CFormView::AssertValid();
}

//##ModelId=3C0DC16100B0
void CAnalysisForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAnalysisForm message handlers

//##ModelId=3C3040BE03D6
void CAnalysisForm::OnInitialUpdate() 
{
	CBaseFormView::OnInitialUpdate();
	

	static short bInit=0;
	if(!bInit)
	{
		//One time initialization!!
		bInit=1;
		//Create Columns
		LV_COLUMN lvColumn;
		lvColumn.mask = LVCF_FMT|LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM;
		lvColumn.fmt= LVCFMT_LEFT;
		lvColumn.cx=60;
		
		CListCtrl* pList=(CListCtrl*)GetDlgItem(IDC_lstAnalysis);
		
		ASSERT(pList);

		//pList->SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES);	
		pList->SetExtendedStyle(LVS_EX_FULLROWSELECT);	
		
		lvColumn.iSubItem=0;
		lvColumn.cx=62;
		lvColumn.pszText=("Is Calculated");
		pList->InsertColumn(0, &lvColumn);

		lvColumn.iSubItem=1;
		lvColumn.cx=56;
		lvColumn.pszText="Scenario";
		pList->InsertColumn(1, &lvColumn);
		lvColumn.iSubItem=2;
		lvColumn.cx=55;
		lvColumn.pszText="TMD";
		pList->InsertColumn(2, &lvColumn);
		lvColumn.iSubItem=3;
		lvColumn.cx=50;
		lvColumn.pszText="Section";
		pList->InsertColumn(3, &lvColumn);
		lvColumn.iSubItem=4;
		lvColumn.cx=72;
		lvColumn.pszText="Cement Mat.";
		pList->InsertColumn(4, &lvColumn);
		
		m_toolTip.Create(this);
		CRect rect;
		CString sTmp;

		sTmp.LoadString(IDTT_MORE_FiniteElementAnalysis);
		m_toolTip.AddTool( GetDlgItem(IDC_btnRunAnalysis), sTmp);
		GetDlgItem(IDC_btnRunAnalysis)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_ANA_lstAnalysis);
		m_toolTip.AddTool( GetDlgItem(IDC_lstAnalysis), sTmp);
		GetDlgItem(IDC_lstAnalysis)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		//IDC_btnRunAnalysis
		//IDC_lstAnalysis

		//IDTT_ANA_btnRunAnalysis
		//IDTT_ANA_lstAnalysis

		
		CRect rectMargin(TOOL_TIP_MARGE,TOOL_TIP_MARGE,TOOL_TIP_MARGE,TOOL_TIP_MARGE); //left, top, right, bottom
		m_toolTip.SetMargin(rectMargin);
		m_toolTip.Activate(TRUE);
	}
	
	
}

//##ModelId=3C3040BE03B7
void CAnalysisForm::BuildAnalysisList()
{
	//get the current unit system.
	CDSealectDoc* pDoc=(CDSealectDoc*)GetDocument();
	QU::UNIT us = pDoc->UnitSystem();

	CListCtrl* pList=0;
	pList=(CListCtrl*)GetDlgItem(IDC_lstAnalysis);
	
	if(!pList)
		return;

	pList->DeleteAllItems();

	LV_COLUMN col;
	col.mask=LVCF_TEXT;

	CLengthQuantity temp; // temporary quantity only needed for getting the unit-string 
	CString strUnit=("TMD ");
	strUnit+= GetUnitString(&temp);
	col.pszText=strUnit.GetBufferSetLength(strUnit.GetLength());
	m_lstAnalysis.SetColumn(2, &col);

	//Create items
	LV_ITEM lvItem;
	
	lvItem.mask= LVIF_TEXT|LVIF_STATE;
	lvItem.state=0;
	lvItem.stateMask=0;

	int iCurrentIndex=0;

	for(int j =0 ;j<GetModel()->BatchTable()->RowCount();j++)
	{
		CBatchTableRow *pBTR = GetModel()->BatchTable()->GetRow(j);
		CString sTmp;
		lvItem.iItem=j;

		lvItem.iSubItem=0;
		lvItem.pszText="";
		pList->InsertItem(&lvItem);
		//pList->SetCheck(j,pBTR->UseForCalculation());
		if(pBTR->IsCalculated())
		{
			sTmp = "YES";
		}
		else
		{
			sTmp = "NO";
		}
		pList->SetItemText(j,0,sTmp.GetBufferSetLength(sTmp.GetLength()));

		sTmp.Format("%i",pBTR->ScenarioNr()+1);
		pList->SetItemText(j,1,sTmp.GetBufferSetLength(sTmp.GetLength()));
		sTmp=CDoubleEdit::Format(pBTR->AnalysisPoint()->TMD()->Value(us));
		pList->SetItemText(j,2,sTmp.GetBufferSetLength(sTmp.GetLength()));

		if(pBTR->IsTail())
			sTmp="Tail";
		else
			sTmp="Lead";

		pList->SetItemText(j,3,sTmp.GetBufferSetLength(sTmp.GetLength()));
		
		sTmp= pBTR->CementMaterial()->Name();
		pList->SetItemText(j,4,sTmp.GetBufferSetLength(sTmp.GetLength()));

		if(GetModel()->BatchTable()->GetRow(j) == GetModel()->BatchTableSelection().SelectedRow())
			iCurrentIndex = j;
	}


	if(!GetModel()->BatchTableSelection().SelectedRow())
		iCurrentIndex = 0;
	
	ASSERT(iCurrentIndex != -1);
	GetModel()->BatchTableSelection().SelectRow(GetModel()->BatchTable()->GetRow(iCurrentIndex));
	pList->SetItem(iCurrentIndex, 0, LVIF_STATE, NULL, 0, LVIS_SELECTED, LVIS_SELECTED, 0);
	pList->EnsureVisible(iCurrentIndex+1,true);

}


//##ModelId=3C0DC1610091
BOOL CAnalysisForm::CheckEntry() const
{
	if(!GetModel()->BatchTable()->CanBuildBatchTable())
	{
		GetModel()->BatchTable()->Invalidate();
		return FALSE;
	}
  return CBaseFormView::CheckEntry();
}

//##ModelId=3C0DC161008F
BOOL CAnalysisForm::CheckComplete() const
{
	if(!TreeEntry()->Previous()->FormView()->CheckComplete())
	{
		return FALSE;
	}

	return TRUE;
}

//##ModelId=3C8D9E2802EF
void CAnalysisForm::OnbtnSaveProject() 
{
	SendMessage(WM_COMMAND,ID_FILE_SAVE);
	UpdateData(FALSE);
}

//##ModelId=3C8D9E280310
void CAnalysisForm::OnbtnRunAnalysis() 
{
	GetDoc()->StartAnalysis();
	UpdateData();
	
}

//##ModelId=3C8D9E28031F
void CAnalysisForm::OnItemchangedlstAnalysis(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if((pNMListView->uChanged & LVIF_STATE) == LVIF_STATE)
	{
		
		UpdateData();
	}
	*pResult = 0;
}



//##ModelId=3DDA0C600003
void CAnalysisForm::OnClicklstAnalysis(NMHDR* pNMHDR, LRESULT* pResult) 
{
	
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	CListCtrl* pList=0;
	pList=(CListCtrl*)GetDlgItem(IDC_lstAnalysis);

	if (pNMListView->iItem<0)
	{
    GetModel()->BatchTableSelection().SelectRow(0);
		UpdateData(FALSE);
		return ;
	}
	else
	{
		int iFItem=((pNMListView->iItem)); 
		GetModel()->BatchTableSelection().SelectRow(GetModel()->BatchTable()->GetRow(iFItem));
		pList->SetItem(iFItem, 0, LVIF_STATE, NULL, 0, LVIS_SELECTED, LVIS_SELECTED, 0);
		
	}

	UpdateData(FALSE);
	
	*pResult = 0;
	
}


/*
void CAnalysisForm::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	CBaseFormView::OnUpdate(pSender,lHint,pHint);
	//CView::OnUpdate(pSender,lHint,pHint);

	if(lHint==UPDATE_DOC_SAVED)
		UpdateData(FALSE);
	
	
}
*/

