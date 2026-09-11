// FormationDefinitionForm.cpp : implementation file
//

#include "stdafx.h"
#include "dsealect.h"
#include "DoubleEdit.h"
#include "FormationDefinitionForm.h"
#include "treeobject.h"
#include "AddFormationDlg.h"
#include "DoubleFormat.h"
#include "DSealectGlobal.h"
#include "CDataModel.h"
#include "CFormationTopography.h"
#include "CAnalysis.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[] = __FILE__;#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CFormationDefinitionForm

IMPLEMENT_DYNCREATE(CFormationDefinitionForm, CBaseFormView)

//##ModelId=3C0DC15A0341
CFormationDefinitionForm::CFormationDefinitionForm()
	: CBaseFormView(CFormationDefinitionForm::IDD)
{
	//{{AFX_DATA_INIT(CFormationDefinitionForm)
	//}}AFX_DATA_INIT
}

//##ModelId=3C0DC15B0011
CFormationDefinitionForm::~CFormationDefinitionForm()
{
	if(m_toolTip)
		delete m_toolTip;
}


//##ModelId=3C0DC15B0002
void CFormationDefinitionForm::DoDataExchange(CDataExchange* pDX)
{

	CFormView::DoDataExchange(pDX);

	if(!pDX->m_bSaveAndValidate)
	{
		//update ctrls
		m_intOnShoreDefinition=GetModel()->FormationTopography()->Offshore();
		SetUnitlabel(IDC_lbUOverburdenBottomTVD,	GetModel()->FormationTopography()->Overburden()->OverburdenBottomTVD());
		SetUnitlabel(IDC_lbUModelTopTVD,			GetModel()->FormationTopography()->Overburden()->OverburdenBottomTVD());
		SetUnitlabel(IDC_lbUModelBottomTVD,			GetModel()->FormationTopography()->Seawater()->WaterDepth());
		SetUnitlabel(IDC_lbUMeanGroundLevel,		GetModel()->FormationTopography()->ElevationTVD());
		SetUnitlabel(IDC_lbUWaterDepth,				GetModel()->FormationTopography()->Seawater()->WaterDepth());
		SetUnitlabel(IDC_lbUMeanSeaLevel,			GetModel()->FormationTopography()->ElevationTVD());
		
	}

	//{{AFX_DATA_MAP(CFormationDefinitionForm)
	DDX_Control(pDX, IDC_lstFormationDefinition, m_lstFormationName);
	DDX_Radio(pDX, IDC_optOnshoreDefinition,	m_intOnShoreDefinition);
	DDX_Control(pDX, IDC_ebOverburdenBottomTVD,	m_debOverburdenBottomTVD);
	DDX_Control(pDX, IDC_ebModelTopTVD,			m_debModelTopTVD);
	DDX_Control(pDX, IDC_ebModelBottomTVD,		m_debModelBottomTVD);
	DDX_Control(pDX, IDC_ebMeanGroundLevel,		m_debMeanGroundLevel);
	DDX_Control(pDX, IDC_ebMeanSeaLevel,		m_deSeaLevel);
	DDX_Control(pDX, IDC_ebWaterDepth,			m_deWaterDepth);
	//}}AFX_DATA_MAP


	if(pDX->m_bSaveAndValidate)
	{
			//update data
		GetModel()->FormationTopography()->Offshore(m_intOnShoreDefinition==TRUE);
	}
	else
	{
		
		
	}
	ValidateQuantity(pDX,&m_debMeanGroundLevel,		GetModel()->FormationTopography()->ElevationTVD(),	CString("MeanGroundLevel"));
	ValidateQuantity(pDX,&m_debOverburdenBottomTVD,	GetModel()->FormationTopography()->Overburden()->OverburdenBottomTVD(),	CString("OverburdenBottomTVD"));
	
	ShowQuantity(&m_debModelTopTVD,		&GetModel()->GetModelTopTVD());
	ShowQuantity(&m_debModelBottomTVD,	&GetModel()->GetModelBottomTVD());
		
	
	if(GetModel()->FormationTopography()->Offshore())
	{
		ValidateQuantity(pDX,&m_deWaterDepth,GetModel()->FormationTopography()->Seawater()->WaterDepth(),CString("WaterDepth"));
	}
	else
	{
		m_deWaterDepth.MakeEmpty();
		GetModel()->FormationTopography()->Seawater()->WaterDepth()->Invalidate();
	}

	if(m_debMeanGroundLevel.IsEmpty())
		m_deSeaLevel.MakeEmpty();
	else
		m_deSeaLevel.ShowValue(m_debMeanGroundLevel.GetValue());
	
	m_optOnShore.Enable(!GetModel()->FormationTopography()->Offshore()); //dis-/enable onshore frame
	m_optOffShore.Enable(GetModel()->FormationTopography()->Offshore()); //dis-/enable offshore frame


	BuildFormationDefinitionList();

	if(GetModel()->FormationTopography()->NrOfFormations() && m_pCurrentFormation)
	{
		((CButton*)GetDlgItem(IDC_btnRemoveFormation))->EnableWindow(TRUE);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_btnRemoveFormation))->EnableWindow(FALSE);
	}

	

	((CButton*)GetDlgItem(IDC_btnAddFormation))->EnableWindow(GetModel()->FormationTopography()->CanAddFormations());

	DrawView()->Invalidate();
	TreeEntry()->UpdateEnable();
    UpdateButtons();
}


BEGIN_MESSAGE_MAP(CFormationDefinitionForm, CBaseFormView)
	//{{AFX_MSG_MAP(CFormationDefinitionForm)
	ON_EN_KILLFOCUS(IDC_ebMeanSeaLevel,			OnKillfocusMeanSeaLevel)
	ON_EN_KILLFOCUS(IDC_ebWaterDepth,			OnKillfocusWaterDepth)
	ON_EN_KILLFOCUS(IDC_ebMeanGroundLevel,		OnKillfocusMeanGroundLevel)
	ON_EN_KILLFOCUS(IDC_ebModelBottomTVD,		OnKillfocusModelBottomTVD)
	ON_EN_KILLFOCUS(IDC_ebModelTopTVD,			OnKillfocusModelTopTVD)
	ON_EN_KILLFOCUS(IDC_ebOverburdenBottomTVD,	OnKillfocusOverburdenBottomTVD)
	ON_BN_CLICKED(IDC_optOffshoreDefinition,	OnoptOffshoreDefinition)
	ON_BN_CLICKED(IDC_optOnshoreDefinition,		OnoptOnshoreDefinition)
	ON_BN_CLICKED(IDC_btnAddFormation, OnAddFormation)
	ON_BN_CLICKED(IDC_btnRemoveFormation, OnRemoveFormation)
	ON_NOTIFY(NM_CLICK, IDC_lstFormationDefinition, OnClicklstFormationDefinition)
	ON_NOTIFY(LVN_KEYDOWN, IDC_lstFormationDefinition, OnKeydownlstFormationDefinition)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFormationDefinitionForm diagnostics

#ifdef _DEBUG
//##ModelId=3C0DC15B001F
void CFormationDefinitionForm::AssertValid() const
{
	CFormView::AssertValid();
}

//##ModelId=3C0DC15B0021
void CFormationDefinitionForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFormationDefinitionForm message handlers

//##ModelId=3C0DC15B002E
void CFormationDefinitionForm::OnKillfocusMeanSeaLevel() 
{
	if(m_deSeaLevel.IsEmpty())
		m_debMeanGroundLevel.MakeEmpty();
	else
		m_debMeanGroundLevel.ShowValue(m_deSeaLevel.GetValue());
	
	AskForRemovingAllFormations(GetModel()->FormationTopography()->ElevationTVD(),&m_debMeanGroundLevel);
	UpdateData();
	UpdateData(FALSE); // extra update needed to get old value back when copied value out of range
	
}

//##ModelId=3C0DC15B0032
void CFormationDefinitionForm::OnKillfocusMeanGroundLevel() 
{
	AskForRemovingAllFormations(GetModel()->FormationTopography()->ElevationTVD(),&m_debMeanGroundLevel);
	UpdateData();
}

//##ModelId=3CA011A40232
LRESULT CFormationDefinitionForm::OnValidateDoubleEdit(WPARAM id, LPARAM lparam)
{

		if(m_iControlToValidate != 0)
			return TRUE;

		m_iControlToValidate = id;
		CDoubleQuantity* pQuantity;

		if(IDC_ebOverburdenBottomTVD==id)
		{
			pQuantity=GetModel()->FormationTopography()->Overburden()->OverburdenBottomTVD();
			AskForRemovingAllFormations(pQuantity,(CDoubleEdit*)GetDlgItem(m_iControlToValidate));
			UpdateData();
		}
		else if(IDC_ebModelTopTVD==id)
		{
			UpdateData();
		}
		else if(IDC_ebModelBottomTVD==id)
		{
			UpdateData();
		}
		else if(IDC_ebMeanGroundLevel==id)
		{
			pQuantity = GetModel()->FormationTopography()->ElevationTVD();
			AskForRemovingAllFormations(pQuantity,(CDoubleEdit*)GetDlgItem(m_iControlToValidate));
			UpdateData();
		}
		else if(IDC_ebMeanSeaLevel==id)
		{
			if(m_deSeaLevel.IsEmpty())
				m_debMeanGroundLevel.MakeEmpty();
			else
				m_debMeanGroundLevel.ShowValue(m_deSeaLevel.GetValue());

			pQuantity = GetModel()->FormationTopography()->ElevationTVD();
			AskForRemovingAllFormations(pQuantity,(CDoubleEdit*)GetDlgItem(m_iControlToValidate));
			UpdateData();
			UpdateData(FALSE);

	}
	else if(IDC_ebWaterDepth==id)
	{
		pQuantity = GetModel()->FormationTopography()->Seawater()->WaterDepth();
		AskForRemovingAllFormations(pQuantity,(CDoubleEdit*)GetDlgItem(m_iControlToValidate));
		UpdateData();
	}

	m_iControlToValidate = 0;
	return TRUE;

}

//##ModelId=3C0DC15B0030
void CFormationDefinitionForm::OnKillfocusWaterDepth() 
{
	AskForRemovingAllFormations(GetModel()->FormationTopography()->Seawater()->WaterDepth(),&m_deWaterDepth);
	UpdateData();
}


//##ModelId=3C0DC15B003E
void CFormationDefinitionForm::OnKillfocusModelBottomTVD() 
{
	UpdateData();
}

//##ModelId=3C0DC15B0040
void CFormationDefinitionForm::OnKillfocusModelTopTVD() 
{
	UpdateData();
}

//##ModelId=3C0DC15B0042
void CFormationDefinitionForm::OnKillfocusOverburdenBottomTVD() 
{
	AskForRemovingAllFormations(GetModel()->FormationTopography()->Overburden()->OverburdenBottomTVD(),&m_debOverburdenBottomTVD);
	UpdateData();
}

//##ModelId=3C0DC15A0360
BOOL CFormationDefinitionForm::CheckComplete() const
{
	if(GetModel()->FormationTopography()->ElevationTVD()->Undefined())
		return FALSE;
		
	if (GetModel()->FormationTopography()->Offshore()) 
	{
		if(!GetModel()->FormationTopography()->Seawater()->CheckComplete())
			return FALSE;
	}

	if(GetModel()->FormationTopography()->NrOfFormations()<1)
		return FALSE;
		
	if(GetModel()->FormationTopography()->Overburden()->OverburdenBottomTVD()->Undefined())
		return FALSE;
	
	return TRUE;

}

//##ModelId=3C0DC15A0362
BOOL CFormationDefinitionForm::CheckEntry() const
{
	return TRUE;
}

//##ModelId=3C0DC15B0000
void CFormationDefinitionForm::OnInitialUpdate() 
{
	
	GetDlgItem(IDC_optOnshoreDefinition)->SetFont(GetFont(TRUE));
	GetDlgItem(IDC_optOffshoreDefinition)->SetFont(GetFont(TRUE));
	GetDlgItem(IDC_fraModelRange)->SetFont(GetFont(TRUE));
	GetDlgItem(IDC_fraFormationDefinition)->SetFont(GetFont(TRUE));

	static short bInit = 0;
	if(!bInit)
	{
		//One time initialization!!
		bInit = 1;
		
		m_optOnShore.Set(this, IDC_fraOnshoreDefinition);
		m_optOffShore.Set(this, IDC_fraOffShoreDefinition);

		LV_COLUMN lvColumn;
		lvColumn.mask = LVCF_FMT|LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM;
		lvColumn.fmt = LVCFMT_LEFT;
		lvColumn.cx = 120;

		CListCtrl* pFDList=(CListCtrl*)GetDlgItem(IDC_lstFormationDefinition);
		
		ASSERT(pFDList);

		pFDList->SetExtendedStyle(LVS_EX_FULLROWSELECT);
		
		lvColumn.iSubItem = 0;
		lvColumn.pszText = "";//"Bottom TVD";
		pFDList->InsertColumn(0, &lvColumn);
		lvColumn.iSubItem = 1;
		lvColumn.pszText = "Formation Name";
		pFDList->InsertColumn(1, &lvColumn);

		m_toolTip.Create(this);
		CRect rect;
		CString sTmp;

		sTmp.LoadString(IDTT_MORE_Elevation);
		m_toolTip.AddTool( GetDlgItem(IDC_ebMeanGroundLevel), sTmp);
		GetDlgItem(IDC_ebMeanGroundLevel)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_MORE_Elevation);
		m_toolTip.AddTool( GetDlgItem(IDC_ebMeanSeaLevel), sTmp);
		GetDlgItem(IDC_ebMeanSeaLevel)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_MORE_ModelRange);
		m_toolTip.AddTool( GetDlgItem(IDC_ebModelBottomTVD), sTmp);
		GetDlgItem(IDC_ebModelBottomTVD)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_MORE_ModelRange);
		m_toolTip.AddTool( GetDlgItem(IDC_ebModelTopTVD), sTmp);
		GetDlgItem(IDC_ebModelTopTVD)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_FOD_ebOverburdenBottomTVD);
		m_toolTip.AddTool( GetDlgItem(IDC_ebOverburdenBottomTVD), sTmp);
		GetDlgItem(IDC_ebOverburdenBottomTVD)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_FOD_lstFormationDefinition);
		m_toolTip.AddTool( GetDlgItem(IDC_lstFormationDefinition), sTmp);
		GetDlgItem(IDC_lstFormationDefinition)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_FOD_optOffshoreDefinition);
		m_toolTip.AddTool( GetDlgItem(IDC_optOffshoreDefinition), sTmp);
		GetDlgItem(IDC_optOffshoreDefinition)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_FOD_optOnshoreDefinition);
		m_toolTip.AddTool( GetDlgItem(IDC_optOnshoreDefinition), sTmp);
		GetDlgItem(IDC_optOnshoreDefinition)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		//IDTT_FOD_ebMeanGroundLevel
		//IDTT_FOD_ebMeanSeaLevel
		//IDTT_FOD_ebModelBottomTVD
		//IDTT_FOD_ebModelTopTVD
		//IDTT_FOD_ebOverburdenBottomTVD
		//IDTT_FOD_lstFormationDefinition
		//IDTT_FOD_optOffshoreDefinition
		//IDTT_FOD_optOnshoreDefinition

		//IDC_ebMeanGroundLevel
		//IDC_ebMeanSeaLevel
		//IDC_ebModelBottomTVD
		//IDC_ebModelTopTVD
		//IDC_ebOverburdenBottomTVD
		//IDC_lstFormationDefinition
		//IDC_optOffshoreDefinition
		//IDC_optOnshoreDefinition

		
		CRect rectMargin(TOOL_TIP_MARGE,TOOL_TIP_MARGE,TOOL_TIP_MARGE,TOOL_TIP_MARGE); //left, top, right, bottom
		m_toolTip.SetMargin(rectMargin);
		m_toolTip.Activate(TRUE);
	}

	
	CBaseFormView::OnInitialUpdate();

}

//##ModelId=3C0DC15B004E
void CFormationDefinitionForm::OnoptOffshoreDefinition() 
{
	GetDocument()->SetModifiedFlag(TRUE);
	UpdateData();
}

//##ModelId=3C0DC15B0050
void CFormationDefinitionForm::OnoptOnshoreDefinition() 
{
	GetDocument()->SetModifiedFlag(TRUE);
	UpdateData();
}

//##ModelId=3C0DC15B0052
void CFormationDefinitionForm::OnAddFormation() 
{

	CAddFormationDlg dlgAddFormation(GetModel()->FormationTopography());
	dlgAddFormation.DoModal();
	if(dlgAddFormation.IsFormationAdded())
	{
		GetDocument()->SetModifiedFlag(TRUE);
		UpdateData(FALSE);
	}
}

//##ModelId=3C0DC15B005D
void CFormationDefinitionForm::OnRemoveFormation() 
{
	
	std::vector <CAnalysisPoint*> vecPointsToRemove; 

	if((m_pCurrentFormation == GetModel()->FormationTopography()->GetLastFormation()) && GetModel()->IsPolyLineCreated())
	{
		for(int i = 0 ; i < GetModel()->Analysis()->NrOfAnalysisPoints() ; i++)
		{
			double dTMD = GetModel()->Analysis()->GetAnalysisPointAt(i)->TMD()->Value();
      std::pair<double, bool> prTVD = GetModel()->TMDtoTVD(dTMD);

			if(prTVD.second && GetModel()->FormationTopography()->GetFormationAtDepth(prTVD.first) == m_pCurrentFormation)
			{
				vecPointsToRemove.push_back(GetModel()->Analysis()->GetAnalysisPointAt(i));
			}
		}
		if(vecPointsToRemove.size()>0)
		{
			if(MessageBox("Delete all Analysis Points in Formation ?",NULL,MB_YESNO) == IDYES)
			{
				for(int i = 0 ; i < vecPointsToRemove.size() ; i++)
				{
					GetModel()->Analysis()->RemoveAnalysisPoint(vecPointsToRemove[i]);
				}
			}
			else
			{
				return;
			}	
		}
	}
	

	if(m_pCurrentFormation)
	{
		GetModel()->FormationTopography()->RemoveFormation(m_pCurrentFormation);
		GetDocument()->SetModifiedFlag(TRUE);
		UpdateData(FALSE);
	}
	
}


//##ModelId=3C0DC15B005F
void CFormationDefinitionForm::OnClicklstFormationDefinition(NMHDR* pNMHDR, LRESULT* pResult) 
{
	
	int iNrOfFirstItems;

	if (GetModel()->FormationTopography()->Offshore())
	{
		iNrOfFirstItems = 3;
	} 
	else //onshore
	{
		iNrOfFirstItems = 2;
	}


	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	TRACE( "OnClicklstFormationName\n");
	TRACE( "item:      %i \n" ,pNMListView->iItem);
	TRACE( "old state: %i \n" ,pNMListView->uOldState);
	TRACE( "new state: %i \n" ,pNMListView->uNewState);
	TRACE( "changed  : %i \n" ,pNMListView->uChanged);
	TRACE( "LVIS_SELECTED: %i \n" ,LVIS_SELECTED);
	TRACE("\n\n");

	
	int nItem = pNMListView->iItem;

	if (nItem<0)
	{
		m_pCurrentFormation	= 0;
		UpdateData(FALSE);
	    return ;
	}
	else
	{
		
		nItem = nItem-(2*iNrOfFirstItems+1);
		if(nItem%2)
		{
			nItem--;
		}

		nItem = nItem/2;
		if (nItem<0)
		{
			m_pCurrentFormation	= 0;
			UpdateData(FALSE);
			return ;
		}
		//return GetModel()->FormationTopography()->GetFormationAt(nItem);
	}

	m_pCurrentFormation = GetModel()->FormationTopography()->GetFormationAt(nItem);
	UpdateData(FALSE);
	*pResult = 0;
}



//##ModelId=3C204400034C


//##ModelId=3C2044000204
void CFormationDefinitionForm::BuildFormationDefinitionList()
{
	//get the current unit system.
	CDSealectDoc* pDoc=(CDSealectDoc*)GetDocument();
	QU::UNIT us = pDoc->UnitSystem();
		
	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT|LVIF_STATE;
	lvItem.state = 0;
	lvItem.stateMask = 0;
	
	m_lstFormationName.DeleteAllItems();

	LV_COLUMN col;
	col.mask = LVCF_TEXT;
	
	CLengthQuantity temp;
	CString strUnit("Bottom TVD ");
	strUnit+= GetUnitString(&temp);


	col.pszText = strUnit.GetBufferSetLength(strUnit.GetLength());
	//col.pszText = (LPSTR)(LPCSTR)strUnit;
	m_lstFormationName.SetColumn(0, &col);



	if(!GetModel()->FormationTopography()->CanAddFormations())
	{
		m_pCurrentFormation = 0;
		return;
	}

	int iNumberOfFirstItems = 0;
	
	CString sTmp;
	CString sDotLine = "------------------------------";
	
	lvItem.iItem = ((0)); //0 depth
	lvItem.iSubItem = 0;
	sTmp = "0";
	lvItem.pszText = sTmp.GetBufferSetLength(sTmp.GetLength());
	m_lstFormationName.InsertItem(&lvItem);
	sTmp = sDotLine;
	m_lstFormationName.SetItemText(0,1,sTmp.GetBufferSetLength(sTmp.GetLength()));
	
	
	lvItem.iItem = (1);	// Elevation (name)
	lvItem.iSubItem = 0;
	sTmp = " ";
	lvItem.pszText = sTmp.GetBufferSetLength(sTmp.GetLength());
	m_lstFormationName.InsertItem(&lvItem);
  if(GetModel()->FormationTopography()->Offshore())
	  sTmp = "Air gap";
  else
    sTmp = "Substructure height";
	m_lstFormationName.SetItemText(1,1,sTmp.GetBufferSetLength(sTmp.GetLength()));
	 
	lvItem.iItem = (2);	//Elevation (depth)
	lvItem.iSubItem = 0;
	sTmp = CDoubleEdit::Format(GetModel()->FormationTopography()->ElevationTVD()->Value(us));

	lvItem.pszText = sTmp.GetBufferSetLength(sTmp.GetLength());
	m_lstFormationName.InsertItem(&lvItem);
	sTmp = sDotLine;
	m_lstFormationName.SetItemText(2,1,sTmp.GetBufferSetLength(sTmp.GetLength()));
	
	if (GetModel()->FormationTopography()->Offshore())
	{
		lvItem.iItem = (3);	// Seawater (name)
		lvItem.iSubItem = 0;
		sTmp = " ";
		lvItem.pszText = sTmp.GetBufferSetLength(sTmp.GetLength());
		m_lstFormationName.InsertItem(&lvItem);
		sTmp = "Water depth";
		m_lstFormationName.SetItemText(3,1,sTmp.GetBufferSetLength(sTmp.GetLength()));
		 
		lvItem.iItem = (4);	//Water depth
		lvItem.iSubItem = 0;
		sTmp = CDoubleEdit::Format(GetModel()->FormationTopography()->Seawater()->SeaBedTVD().Value(us));
		lvItem.pszText = sTmp.GetBufferSetLength(sTmp.GetLength());
		m_lstFormationName.InsertItem(&lvItem);
		sTmp = sDotLine;
		m_lstFormationName.SetItemText(4,1,sTmp.GetBufferSetLength(sTmp.GetLength()));

		iNumberOfFirstItems = 3;
	}
	else //onshore
	{
		iNumberOfFirstItems = 2;
	}
	
	lvItem.iItem = (2*iNumberOfFirstItems -1);	// Overburden (name)
	lvItem.iSubItem = 0;
	sTmp = " ";
	lvItem.pszText = sTmp.GetBufferSetLength(sTmp.GetLength());
	m_lstFormationName.InsertItem(&lvItem);
	sTmp = "Overburden";
	m_lstFormationName.SetItemText((2*iNumberOfFirstItems -1),1,sTmp.GetBufferSetLength(sTmp.GetLength()));
	 
	lvItem.iItem = (2*iNumberOfFirstItems);	//Overburden (depth)
	lvItem.iSubItem = 0;
	sTmp = CDoubleEdit::Format(GetModel()->FormationTopography()->Overburden()->OverburdenBottomTVD()->Value(us));
	lvItem.pszText = sTmp.GetBufferSetLength(sTmp.GetLength());
	m_lstFormationName.InsertItem(&lvItem);
	sTmp = sDotLine;
	m_lstFormationName.SetItemText((2*iNumberOfFirstItems),1,sTmp.GetBufferSetLength(sTmp.GetLength()));

	
	if(GetModel()->FormationTopography()->NrOfFormations()<1)
	{
		m_pCurrentFormation = 0;
		return;
	}

	for(int i = 0 ;i<GetModel()->FormationTopography()->NrOfFormations();i++)
	{
		int j = ((2*iNumberOfFirstItems +1)+(2*i));
		CString sTmp;
		lvItem.iItem = j; 

		lvItem.iSubItem = 0;	//Formation names
		sTmp = " ";
		lvItem.pszText = sTmp.GetBufferSetLength(sTmp.GetLength());
		m_lstFormationName.InsertItem(&lvItem);
		sTmp = GetModel()->FormationTopography()->GetFormationAt(i)->FormationName();
		m_lstFormationName.SetItemText(j,1,sTmp.GetBufferSetLength(sTmp.GetLength()));
		 
		lvItem.iItem = j+1;	//Formation bottoms

		lvItem.iSubItem = 0;
		sTmp = CDoubleEdit::Format(GetModel()->FormationTopography()->GetFormationAt(i)->BottomTVD()->Value(us));
		lvItem.pszText = sTmp.GetBufferSetLength(sTmp.GetLength());
		m_lstFormationName.InsertItem(&lvItem);
		sTmp = sDotLine;
		m_lstFormationName.SetItemText((j+1),1,sTmp.GetBufferSetLength(sTmp.GetLength()));
			
	}

	if(!m_pCurrentFormation || GetModel()->FormationTopography()->GetFormationIndex(m_pCurrentFormation)<0)
		m_pCurrentFormation = GetModel()->FormationTopography()->GetFormationAt(0);
	

	long Index = GetModel()->FormationTopography()->GetFormationIndex(m_pCurrentFormation);	
	long ListIndex = (((2*iNumberOfFirstItems)+1)+(2*Index));

	m_lstFormationName.SetItem(ListIndex, 0, LVIF_STATE, NULL, 0, LVIS_SELECTED, LVIS_SELECTED, 0);
	m_lstFormationName.SetItem(ListIndex+1 , 0, LVIF_STATE, NULL, 0, LVIS_SELECTED, LVIS_SELECTED, 0);

	m_lstFormationName.EnsureVisible(ListIndex+1,true);

}

//##ModelId=3C2044000243
void CFormationDefinitionForm::AskForRemovingAllFormations(CDoubleQuantity* pQuantity,CDoubleEdit* pEdit)
{
	static short bExit = 0;

	if(bExit)
		return;

	if(pEdit->IsEmpty() && !pQuantity->Undefined())
	{
		double val = pQuantity->Value();

		if(GetModel()->FormationTopography()->NrOfFormations())
		{
			bExit = 1;
			if(MessageBox("Delete All Formations ?",NULL,MB_YESNO) == IDYES)
			{
				
				GetModel()->FormationTopography()->RemoveAllFormations();
				GetDocument()->SetModifiedFlag(TRUE);
				UpdateData();
				bExit = 0;
				return;
			}
			else
			{
				pQuantity->Value(val,CDoubleQuantity::SI_UNIT);
				UpdateData(FALSE);
				bExit = 0;
				return;
			}
		}
	}

	bExit = 0;
}



//##ModelId=3C8D9E1A038C
void CFormationDefinitionForm::OnKeydownlstFormationDefinition(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	*pResult = 1; // 1 ipv 0 to lock use of arrow keys
}


