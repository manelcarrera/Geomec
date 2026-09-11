// CementPropertiesForm.cpp : implementation file
//

#include "stdafx.h"
#include "dsealect.h"
#include "DoubleFormat.h"
#include "treeobject.h"
#include "CementPropertiesForm.h"
#include "CementMaterial.h"
#include "CementMaterialDlg.h"
#include "DSealectGlobal.h"
#include "CementMaterialLibDlg.h"
#include "CDataModel.h"
#include "CFormationTopography.h"
#include "CAnalysis.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[] = __FILE__;#endif  // _MSC_VER
#endif

#define LEAD_LIST_NAME "lead"
#define TAIL_LIST_NAME "tail"

/////////////////////////////////////////////////////////////////////////////
// CCementPropertiesForm

IMPLEMENT_DYNCREATE(CCementPropertiesForm, CBaseFormView)

//##ModelId=3C0DC15E02FD
CCementPropertiesForm::CCementPropertiesForm()
	: CBaseFormView(CCementPropertiesForm::IDD)
{
	m_pCurrentScenario = 0;
	m_iControlToValidate = 0;
	bIsInKillFocusLoop = FALSE;
	//{{AFX_DATA_INIT(CCementPropertiesForm)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

//##ModelId=3C0DC15E0312
CCementPropertiesForm::~CCementPropertiesForm()
{
	if(m_toolTip)
		delete m_toolTip;
}

//##ModelId=3C0DC15E030F
void CCementPropertiesForm::DoDataExchange(CDataExchange* pDX)
{	
	CFormView::DoDataExchange(pDX);
		
	if(!pDX->m_bSaveAndValidate)
	{
		m_iNumberOfCementScenarios=GetModel()->DrillingScheme()->GetDrillingOperation(0)->CementScenariosTable()->NumberOfCementScenarios();
	}

	//{{AFX_DATA_MAP(CCementPropertiesForm)
	DDX_Text(pDX, IDC_ebNumberOfCementScenarios, m_iNumberOfCementScenarios);
	DDV_MinMaxInt(pDX,m_iNumberOfCementScenarios,1,10);
	DDX_Control(pDX, IDC_lstCementPropertiesParams, m_lstCementPropertiesParams);
	DDX_Control(pDX, IDC_lstCementProperties, m_lstCementProperties);
	//}}AFX_DATA_MAP

	if(!CheckEntry())
		return;
	
	if(pDX->m_bSaveAndValidate)
	{
		GetModel()->DrillingScheme()->GetDrillingOperation(0)->CementScenariosTable()->NumberOfCementScenarios(m_iNumberOfCementScenarios);
	}

	CString MatName = GetSelectedMatName() ;

	CCementMaterial *pCementMaterial=GetModel()->CementMatList()->GetMaterial(MatName);

	if(pCementMaterial)
	{
		m_lstCementPropertiesParams.ShowMaterial(*pCementMaterial);
		GetDlgItem(IDC_Warning)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_btnEdit)->EnableWindow(TRUE);
		GetDlgItem(IDC_btnToLibrary)->EnableWindow(TRUE);
	}
	else
	{
		m_lstCementPropertiesParams.DeleteAllItems();
		GetDlgItem(IDC_Warning)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_btnEdit)->EnableWindow(FALSE);
		GetDlgItem(IDC_btnToLibrary)->EnableWindow(FALSE);
	}

	GetDlgItem(IDC_btnNew)->EnableWindow(TRUE);

	GetDlgItem(IDC_btnFromLibrary)->EnableWindow(TRUE); //TODO check data base
	
	BuildCementPropertieList();
	GetModel()->RemoveUnusedMaterial();

	if(m_iControlToValidate == IDC_ebNumberOfCementScenarios && pDX->m_bSaveAndValidate)
	{
		if(GetModel()->BatchTable()->CanBuildBatchTable())
			GetModel()->BatchTable()->BuildBatchTable();
	}

	DrawView()->Invalidate();
	TreeEntry()->UpdateEnable();
    UpdateButtons();
	
}


BEGIN_MESSAGE_MAP(CCementPropertiesForm, CBaseFormView)
	//{{AFX_MSG_MAP(CCementPropertiesForm)
	ON_NOTIFY(NM_CLICK, IDC_lstCementProperties, OnClicklstCementProperties)
	ON_NOTIFY(LVN_KEYDOWN, IDC_lstCementProperties, OnKeydownlstCementProperties)
	ON_EN_KILLFOCUS(IDC_ebNumberOfCementScenarios, OnKillfocusebNumberOfCementScenarios)
	ON_BN_CLICKED(IDC_btnEdit, OnEdit)
	ON_BN_CLICKED(IDC_btnFromLibrary, OnFromLibrary)
	ON_BN_CLICKED(IDC_btnNew, OnNew)
	ON_BN_CLICKED(IDC_btnToLibrary, OnToLibrary)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_lstCementPropertiesParams, OnItemchanginglstCementPropertiesParams)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCementPropertiesForm diagnostics

#ifdef _DEBUG
//##ModelId=3C0DC15E031D
void CCementPropertiesForm::AssertValid() const
{
	CFormView::AssertValid();
}

//##ModelId=3C0DC15E031F
void CCementPropertiesForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCementPropertiesForm message handlers

//##ModelId=3C3040BB0175
void CCementPropertiesForm::OnClicklstCementProperties(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CListCtrl *pList=&m_lstCementProperties;

	if(pList->GetItemCount()<1)
		return ;

	CCementScenariosTable *pCS = GetModel()->DrillingScheme()->GetDrillingOperation(0)->CementScenariosTable();

	POSITION pos = pList->GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		pList->SetItem(0, 0, LVIF_STATE, NULL, 0, LVIS_SELECTED, LVIS_SELECTED, 0);
		pos = pList->GetFirstSelectedItemPosition();
	}

	ASSERT(pos);
	
	int nIndex = pList->GetNextSelectedItem(pos);
	
	if(GetModel()->DrillingScheme()->GetDrillingOperation(0)->CementingOperation()->LeadTailCements())
	{
		nIndex = int(nIndex / 2);
	}

	m_pCurrentScenario=pCS->CementScenarioAt(nIndex);
	UpdateData();
	*pResult = 0;
}

//##ModelId=3C3040BB026F
void CCementPropertiesForm::OnKeydownlstCementProperties(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	
	*pResult = 1; //set from 0 to 1 to lock the use of arrow keys
}

//##ModelId=3C3040BB0117
void CCementPropertiesForm::OnInitialUpdate() 
{
	CBaseFormView::OnInitialUpdate();
	
	static short bInit = 0;
	if(!bInit)
	{
		//One time initialization!!
		bInit = 1;
		//Create Columns
		LV_COLUMN lvColumn;
		lvColumn.mask = LVCF_FMT|LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM;
		lvColumn.fmt = LVCFMT_LEFT;
		lvColumn.cx = 90;

		CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_lstCementProperties);

		ASSERT(pList);

		pList->SetExtendedStyle(LVS_EX_FULLROWSELECT);

		lvColumn.iSubItem = 0;
		lvColumn.pszText = "Scenario nb.";
		pList->InsertColumn(0, &lvColumn);
		lvColumn.iSubItem = 1;
		lvColumn.pszText = "Cement Section";
		pList->InsertColumn(1, &lvColumn);
		lvColumn.iSubItem = 2;
		lvColumn.pszText = "Material Name";
		pList->InsertColumn(2, &lvColumn);

		//One time initialization!!
				
		m_toolTip.Create(this);
		CRect rect;
		CString sTmp;

		sTmp.LoadString(IDTT_CEP_ebNumberOfCementScenarios);
		m_toolTip.AddTool( GetDlgItem(IDC_ebNumberOfCementScenarios), sTmp);
		GetDlgItem(IDC_ebNumberOfCementScenarios)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		//IDC_ebNumberOfCementScenarios
		//IDTT_CEP_ebNumberOfCementScenarios
		
		CRect rectMargin(TOOL_TIP_MARGE,TOOL_TIP_MARGE,TOOL_TIP_MARGE,TOOL_TIP_MARGE); //left, top, right, bottom
		m_toolTip.SetMargin(rectMargin);
		m_toolTip.Activate(TRUE);
	}
	
	
}

//##ModelId=3C3040BB00E8
void CCementPropertiesForm::BuildCementPropertieList()
{
	//get the current unit system:
	CDSealectDoc* pDoc = (CDSealectDoc*)GetDocument();
	QU::UNIT us = pDoc->UnitSystem();
		
	CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_lstCementProperties);


	ASSERT(pList);
	BOOL bTailSelected = TailSelected();

	pList->DeleteAllItems();


	CCementScenariosTable *pCS = GetModel()->DrillingScheme()->GetDrillingOperation(0)->CementScenariosTable();
	long iNrScenarios = pCS->NumberOfCementScenarios(); 

	if(iNrScenarios < 1)
	{
		m_pCurrentScenario = 0;
		return;
	}

	//Create items
	LV_ITEM lvItem;
	
	lvItem.mask = LVIF_TEXT|LVIF_STATE;
	lvItem.state = 0;
	lvItem.stateMask = 0;
	
	CString sTmp;
	lvItem.iItem = 0;
	lvItem.iSubItem = 0;
	
	int iMultiplier = 1;
	if (GetModel()->DrillingScheme()->GetDrillingOperation(0)->CementingOperation()->LeadTailCements())
	{
		iMultiplier = 2; //for lead and for tail
		
	}
	else
	{
		bTailSelected = FALSE;
	}

	for(int j = 0 ;j < iNrScenarios ;j++)
	{
		int i = j * iMultiplier;
		
		CString sTmp;
		lvItem.iItem=(i);
		
		lvItem.iSubItem = 0;
		sTmp.Format("%i", j + 1); 
		lvItem.pszText= sTmp.GetBufferSetLength(sTmp.GetLength());
		pList->InsertItem(&lvItem); //Formation Name

		sTmp=LEAD_LIST_NAME;
		pList->SetItemText(i,1,sTmp.GetBufferSetLength(sTmp.GetLength())); //Material Name
		
		sTmp=pCS->CementScenarioAt(j)->HeadMaterialName();
		pList->SetItemText(i,2,sTmp.GetBufferSetLength(sTmp.GetLength()));
		

		if (GetModel()->DrillingScheme()->GetDrillingOperation(0)->CementingOperation()->LeadTailCements())
		{
			i = j * iMultiplier + 1;

			//CString sTmp;
			lvItem.iItem = (i);
			
			lvItem.iSubItem = 0;
			sTmp.Format("%i", j + 1); 
			lvItem.pszText= sTmp.GetBufferSetLength(sTmp.GetLength());
			pList->InsertItem(&lvItem); //Formation Name

			sTmp = TAIL_LIST_NAME;
			pList->SetItemText(i,1,sTmp.GetBufferSetLength(sTmp.GetLength())); //Material Name
			
			sTmp=pCS->CementScenarioAt(j)->TailMaterialName();
			pList->SetItemText(i,2,sTmp.GetBufferSetLength(sTmp.GetLength()));
		}
	}

	long Index = 0;
	if(m_pCurrentScenario)
	{
		Index = pCS->GetScenarioIndex(m_pCurrentScenario);
		if(Index < 0)
		{
			Index = 0;
			m_pCurrentScenario = pCS->CementScenarioAt(0);
		}
	}
	else
	{
		m_pCurrentScenario = pCS->CementScenarioAt(0);
	}

	if(GetModel()->DrillingScheme()->GetDrillingOperation(0)->CementingOperation()->LeadTailCements())
	{
		Index = 2 * Index;
		if(bTailSelected)
		{
			Index++;
		}
	}

	pList->SetItem(Index, 0, LVIF_STATE, NULL, 0, LVIS_SELECTED, LVIS_SELECTED, 0);
	pList->EnsureVisible(Index,true);

}

//##ModelId=3C3040BB0398
void CCementPropertiesForm::OnKillfocusebNumberOfCementScenarios() 
{
	if(bIsInKillFocusLoop) //because the messagebox also gives an OnKillFocus, but we need it only once
		return;

	bIsInKillFocusLoop = TRUE;
	CEdit *pEdit = (CEdit*)GetDlgItem(IDC_ebNumberOfCementScenarios);

	if(pEdit->GetModify())
	{
		CString sFromEditBox;
		GetDlgItem(IDC_ebNumberOfCementScenarios)->GetWindowText(sFromEditBox);
		int iFromEditBox = atoi(sFromEditBox);//convert string to int
		int iFromModel = GetModel()->DrillingScheme()->GetDrillingOperation(0)->CementScenariosTable()->NumberOfCementScenarios();
		if(iFromEditBox != iFromModel) 
		{
			if(GetDoc()->ContinueAndLoseResultsWarning())
			{
				//continue and lose results
				GetDoc()->SetModifiedFlag(TRUE);
				GetModel()->BatchTable()->Invalidate();
				UpdateData();
			}
			else
			{
				//restore value and keep model unchanged
				UpdateData(FALSE);
			}
		}
		
		pEdit->SetModify(FALSE);
		m_iControlToValidate = 0;
	}
	bIsInKillFocusLoop = FALSE;
}

//##ModelId=3C60D57B0010
void CCementPropertiesForm::OnEdit() 
{

	CString MatName = GetSelectedMatName();
	CCementMaterialDlg dlg(*GetModel()->CementMatList()->GetMaterial(GetSelectedMatName()));

	if(dlg.DoModal() == IDOK)
	{
		CCementMaterial cem;
		cem = dlg.Material();
		if(GetModel()->CementMatList()->Exists(cem.Name()))
		{
			if(MessageBox("Material already exist in the model, replace?",NULL,MB_YESNO) == IDNO)
				return;
			else
			{
				GetModel()->CementMatList()->Remove(cem.Name());
			}
		}

		if(TailSelected())
		{
			GetSelectedScenario()->TailMaterialName(cem.Name());
		}
		else
		{
			GetSelectedScenario()->HeadMaterialName(cem.Name());
		}
		GetDocument()->SetModifiedFlag(TRUE);
		GetModel()->CementMatList()->Add(cem);
		UpdateData();
	}
	
}

//##ModelId=3C60D57B0020
void CCementPropertiesForm::OnFromLibrary() 
{
	CCementMaterialDatabase db;
	db.Database().Open(GetDoc()->CementMatDatabasePath(),TRUE,FALSE);
	if(!db.HasMaterials())
	{
		CString msg;
		msg.LoadString(IDS_MATLIB_NO_MATERIAL);
		db.Database().Close();
		MessageBox(msg);
		return;
	}
	db.Database().Close();

	CDSealectDoc* pDoc = (CDSealectDoc*)GetDocument();
	CCementMaterialLibDlg cesld(GetDoc()->CementMatDatabasePath());

	if(cesld.DoModal() == IDOK)
	{
		if(cesld.SelectedMaterial())
		{
			CCementMaterial ces =*cesld.SelectedMaterial();
			if(GetModel()->CementMatList()->Exists(ces.Name()))
			{
				if(MessageBox("Material already exist in the model, replace?",NULL,MB_YESNO) == IDNO)
					return;
				else
				{
					GetModel()->CementMatList()->Remove(ces.Name());
				}
			}
			
			if(TailSelected())
			{
				GetSelectedScenario()->TailMaterialName(ces.Name());
			}
			else
			{
				GetSelectedScenario()->HeadMaterialName(ces.Name());
			}
			
			GetModel()->CementMatList()->Add(ces);
			GetDocument()->SetModifiedFlag(TRUE);
			MessageBox("Material imported from database"); 
			UpdateData();
		}
	}
}

//##ModelId=3C60D57B0022
void CCementPropertiesForm::OnNew() 
{
	CCementMaterial cem;

	CCementMaterialDlg dlg(cem);
	if(dlg.DoModal() == IDOK)
	{
		cem=dlg.Material();
		if(GetModel()->CementMatList()->Exists(cem.Name()))
		{
			if(MessageBox("Material already exist in the model, replace?",NULL,MB_YESNO) == IDNO)
				return;
			else
			{
				GetModel()->CementMatList()->Remove(cem.Name());
			}
		}

		if(TailSelected())
		{
			GetSelectedScenario()->TailMaterialName(cem.Name());
		}
		else
		{
			GetSelectedScenario()->HeadMaterialName(cem.Name());
		}
		GetModel()->CementMatList()->Add(cem);
		GetDocument()->SetModifiedFlag(TRUE);
		UpdateData();
	}
	
}

//##ModelId=3C60D57B002F
void CCementPropertiesForm::OnToLibrary() 
{

	CString MatName = GetSelectedMatName();

	CCementMaterialDatabase db;
	db.Database().Open(GetDoc()->CementMatDatabasePath(),TRUE,FALSE);
	
	ASSERT(GetModel()->CementMatList()->Exists(MatName));

	CCementMaterial *ces = GetModel()->CementMatList()->GetMaterial(MatName);

	if(db.MaterialExists(ces->Name()))
	{
		if(MessageBox("Material already exists in the database, replace?",NULL,MB_YESNO) == IDNO)
		{
			db.Database().Close();
			return;
		}
		db.RemoveMaterial(ces->Name());
	}

	db.AddMaterial(*ces);
	db.Database().Close();

	MessageBox("Material added to database"); 
	
}



//##ModelId=3C6A14C901F2
CString CCementPropertiesForm::GetSelectedMatName()
{
	if(TailSelected())
	{
		return GetSelectedScenario()->TailMaterialName();
	}
	else
	{
		return GetSelectedScenario()->HeadMaterialName();
	}
	
}

//##ModelId=3C6A14C901E4
BOOL CCementPropertiesForm::TailSelected()
{
	CListCtrl *pList = &m_lstCementProperties;

	if(pList->GetItemCount() < 1)
		return FALSE;

	POSITION pos = pList->GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		pList->SetItem(0, 0, LVIF_STATE, NULL, 0, LVIS_SELECTED, LVIS_SELECTED, 0);
		pos = pList->GetFirstSelectedItemPosition();
	}

	ASSERT(pos);
	
	int nIndex = pList->GetNextSelectedItem(pos);
	return (pList->GetItemText(nIndex,1) == TAIL_LIST_NAME);
}


//##ModelId=3C6A14C90194
CCementScenario* CCementPropertiesForm::GetSelectedScenario()
{
	long Index = 0;
	CCementScenariosTable *pCS = GetModel()->DrillingScheme()->GetDrillingOperation(0)->CementScenariosTable();

	if(m_pCurrentScenario)
	{
		Index = pCS->GetScenarioIndex(m_pCurrentScenario);
		if(Index < 0)
		{
			m_pCurrentScenario = pCS->CementScenarioAt(0);
		}
	}
	else
	{
		m_pCurrentScenario = pCS->CementScenarioAt(0);
	}
	return m_pCurrentScenario;
}

//##ModelId=3C6A14C90211
void CCementPropertiesForm::OnItemchanginglstCementPropertiesParams(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	*pResult = 1;//0;
}

//##ModelId=3C0DC15E02FE
BOOL CCementPropertiesForm::CheckComplete() const
{
	if(!TreeEntry()->Previous()->FormView()->CheckComplete())
	{
		return FALSE;
	}

	if(GetModel()->FormationTopography()->NrOfFormations()<1)
		return FALSE;

	if(GetModel()->Analysis()->NrOfAnalysisPoints()<1)
		return FALSE;

	CCementScenariosTable* pCST = GetModel()->DrillingScheme()->GetDrillingOperation(0)->CementScenariosTable();
	
	for(int i = 0 ;i < pCST->NumberOfCementScenarios();i++)
	{
		if(pCST->CementScenarioAt(i)->HeadMaterialName().IsEmpty())
			return FALSE;

		if (GetModel()->DrillingScheme()->GetDrillingOperation(0)->CementingOperation()->LeadTailCements())
		{
			if(pCST->CementScenarioAt(i)->TailMaterialName().IsEmpty())
			return FALSE;
		}
	}
	
	return TRUE;
}

//##ModelId=3C6A14C901B3
BOOL CCementPropertiesForm::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN)
	{
		CEdit* pNOCS = (CEdit*)GetDlgItem(IDC_ebNumberOfCementScenarios);
		
		if(GetFocus() == pNOCS) // if edit-box-Nr-of-cement-scenarios has focus
		{
			if(pMsg->wParam == VK_RETURN) // UpdateData when hit Return-key (doesn't work initially)
			{
				OnKillfocusebNumberOfCementScenarios();
				//GetDocument()->SetModifiedFlag(TRUE);
				//UpdateData();
			}
		}
	}

	if(GetDoc()->m_bShowToolTip)
		m_toolTip.RelayEvent(pMsg);
		
	return CWnd::PreTranslateMessage(pMsg);

}


