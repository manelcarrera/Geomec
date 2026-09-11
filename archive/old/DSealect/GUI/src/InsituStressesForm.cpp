// InsituStressesForm.cpp : implementation file
//

#include "stdafx.h"
#include "dsealect.h"
#include "InsituStressesForm.h"
#include "treeobject.h"
#include <sstream>
#include "DoubleFormat.h"
#include "DSealectGlobal.h"
#include "CDataModel.h"
#include "CFormationTopography.h"
#include "PorePressureDataReader.h"
#include "DensityDataReader.h"
#include "TemperatureDataReader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[] = __FILE__;#endif  // _MSC_VER
#endif



/////////////////////////////////////////////////////////////////////////////
// CInsituStressesForm

IMPLEMENT_DYNCREATE(CInsituStressesForm, CBaseFormView)

//##ModelId=3C0DC15902EE
CInsituStressesForm::CInsituStressesForm()
	: CBaseFormView(CInsituStressesForm::IDD)
{
	//{{AFX_DATA_INIT(CInsituStressesForm)
	//m_optFullyUndrained = -1;
	//}}AFX_DATA_INIT
	m_pCurrentFormation=0;
}

//##ModelId=3C0DC15A005D
CInsituStressesForm::~CInsituStressesForm()
{
	if(m_toolTip)
		delete m_toolTip;
}

//##ModelId=3C0DC15A0050
void CInsituStressesForm::DoDataExchange(CDataExchange* pDX)
{

	if(!CheckEntry())
	{
		m_pCurrentFormation=0;
		return;
	}

	if(!pDX->m_bSaveAndValidate)
	{
		//update gui
		if(m_pCurrentFormation)
		{
			if(GetModel()->FormationTopography()->GetFormationIndex(m_pCurrentFormation)<0)
				m_pCurrentFormation=0;
			/*
			else
				if(m_pCurrentFormation->Porous())
				{
					m_optFullyUndrained = 1;
				}
				else
				{
					m_optFullyUndrained = 0;
				}
			*/
		}

    CString strFileName(GetModel()->PorePressureProfileFileName());
    DDX_Text(pDX, IDC_EditPressureFileName, strFileName);

    GetDlgItem(IDC_ButtonImportPressure)->EnableWindow(GetModel()->PorePressureProfile().empty());
    GetDlgItem(IDC_ButtonRemovePressure)->EnableWindow(!GetModel()->PorePressureProfile().empty());

    strFileName = GetModel()->DensityProfileFileName();
    DDX_Text(pDX, IDC_EditDensityFileName, strFileName);

    GetDlgItem(IDC_ButtonImportDensity)->EnableWindow(GetModel()->DensityProfile().empty());
    GetDlgItem(IDC_ButtonRemoveDensity)->EnableWindow(!GetModel()->DensityProfile().empty());

    strFileName = GetModel()->TemperatureProfileFileName();
    DDX_Text(pDX, IDC_EditTemperatureFileName, strFileName);

    GetDlgItem(IDC_ButtonImportTemperature)->EnableWindow(GetModel()->TemperatureProfile().empty());
    GetDlgItem(IDC_ButtonRemoveTemperature)->EnableWindow(!GetModel()->TemperatureProfile().empty());
  }

	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInsituStressesForm)
	DDX_Control(pDX, IDC_lstFormationName,				m_lstFormationName);
	DDX_Control(pDX, IDC_ebPorePressFormationBottom,	m_debPorePressFormationBottom);
	DDX_Control(pDX, IDC_ebPorePressAtFormationTop,		m_debPorePressAtFormationTop);
	DDX_Control(pDX, IDC_ebAzimuthOfSHWithNorth,		m_debAzimuthOfSHWithNorth);
	DDX_Control(pDX, IDC_ebMinHorizontalStressRatio,	m_debMinHorizontalStressRatio);
	DDX_Control(pDX, IDC_ebMaxHorizontalStressRatio,	m_debMaxHorizontalStressRatio);
	DDX_Control(pDX, IDC_ebAverageDensity,		m_debAverageDensity);
	DDX_Control(pDX, IDC_ebGeothermalGradientF,			m_debGeothermalGradientF);
	DDX_Control(pDX, IDC_ebAverageDensity,	m_debAverageDensity);
	DDX_Control(pDX, IDC_ebGeothermalGradient,			m_debGeothermalGradient);
	DDX_Control(pDX, IDC_ebSurfaceUndisturbedTemp,		m_debSurfaceUndisturbedTemp);
	//DDX_Radio(pDX, IDC_optFullyUndrained, m_optFullyUndrained);
	//}}AFX_DATA_MAP


	CDSealectDoc* pDoc=(CDSealectDoc*)GetDoc();
	QU::UNIT us = pDoc->UnitSystem();

	if(m_pCurrentFormation)
	{
		if(GetModel()->FormationTopography()->GetFormationIndex(m_pCurrentFormation)<0)
		{
			m_pCurrentFormation=GetModel()->FormationTopography()->GetFormationAt(0);
		}

    CSinglePressure BottomPressureFromProfile;
    CSinglePressure TopPressureFromProfile;

    CSinglePressure* pBottomPressure = m_pCurrentFormation->BottomPorePressure();
    CSinglePressure* pTopPressure = m_pCurrentFormation->TopPorePressure();

    if(!GetModel()->PorePressureProfile().empty())
    {
      BottomPressureFromProfile.SetValue(GetModel()->PorePressureProfile().ValueAt(m_pCurrentFormation->BottomTVD()->Value()));
      TopPressureFromProfile.SetValue(GetModel()->PorePressureProfile().ValueAt(m_pCurrentFormation->GetTopTVD()));
      pBottomPressure = &BottomPressureFromProfile;
      pTopPressure = &TopPressureFromProfile;
    }

		ValidateQuantity(pDX, &m_debPorePressFormationBottom,	pBottomPressure, CString("PorePressFormationBottom"));
		ValidateQuantity(pDX, &m_debPorePressAtFormationTop,	pTopPressure, CString("PorePressAtFormationTop"));
		ValidateQuantity(pDX, &m_debAzimuthOfSHWithNorth,		m_pCurrentFormation->AzimuthOfSHWithNorth(),	CString("AzimuthOfSHWithNorth"));
		ValidateQuantity(pDX, &m_debMinHorizontalStressRatio,	m_pCurrentFormation->MinHorizontalStressRatio(),CString("MinHorizontalStressRatio"));
		ValidateQuantity(pDX, &m_debMaxHorizontalStressRatio,	m_pCurrentFormation->MaxHorizontalStressRatio(),CString("MaxHorizontalStressRatio"));

    if(GetModel()->TemperatureProfile().empty())
		  ValidateQuantity(pDX, &m_debGeothermalGradientF,		m_pCurrentFormation->GeothermalGradient(),		CString("GeothermalGradient"));
    else
      m_debGeothermalGradientF.MakeEmpty();

		
		//if(pDX->m_bSaveAndValidate)
		
		if(m_pCurrentFormation->Porous()==FALSE)
		{
			m_pCurrentFormation->Porous(TRUE); //set to drained means porous = true //(m_optFullyUndrained == 1);
			GetDocument()->SetModifiedFlag(TRUE);
			
		}

		m_lstItemOverburden.Enable(false);					//disable overburden frame
		m_lstItemFormation.Enable(true);					//enable formation frame

    m_debPorePressFormationBottom.EnableWindow(GetModel()->PorePressureProfile().empty());
    m_debPorePressAtFormationTop.EnableWindow(GetModel()->PorePressureProfile().empty());
    GetDlgItem(IDC_ebGeothermalGradientF)->EnableWindow(GetModel()->TemperatureProfile().empty());
	}
	else
	{
		// if overburden selected, the formation data will be removed from screen:
		m_debPorePressFormationBottom.MakeEmpty();
		m_debPorePressAtFormationTop.MakeEmpty();
		m_debAzimuthOfSHWithNorth.MakeEmpty();
		m_debMinHorizontalStressRatio.MakeEmpty();
		m_debMaxHorizontalStressRatio.MakeEmpty();
		m_debGeothermalGradientF.MakeEmpty();

		m_lstItemOverburden.Enable(true);	//enable overburden frame
		m_lstItemFormation.Enable(false);	//disable formation frame

    GetDlgItem(IDC_ebAverageDensity)->EnableWindow(GetModel()->DensityProfile().empty());
    GetDlgItem(IDC_ebGeothermalGradient)->EnableWindow(GetModel()->TemperatureProfile().empty());
    GetDlgItem(IDC_ebSurfaceUndisturbedTemp)->EnableWindow(GetModel()->TemperatureProfile().empty());
	}

  if(GetModel()->DensityProfile().empty())
	  ValidateQuantity(pDX, &m_debAverageDensity,	GetModel()->FormationTopography()->Overburden()->OBAverageDensity(),			CString("AverageVerticalStressGrad"));
  else
    m_debAverageDensity.MakeEmpty();

  if(GetModel()->TemperatureProfile().empty())
  {
	  ValidateQuantity(pDX, &m_debGeothermalGradient,			GetModel()->FormationTopography()->Overburden()->OBGeothermalGradient(),			CString("GeothermalGradient"));
	  ValidateQuantity(pDX, &m_debSurfaceUndisturbedTemp,		GetModel()->FormationTopography()->Overburden()->SurfaceTemperature(),				CString("SurfaceUndisturbedTemp"));	
  }
  else
  {
    m_debGeothermalGradient.MakeEmpty();
    //m_debSurfaceUndisturbedTemp.MakeEmpty();
    CTemperatureQuantity qTemp(GetModel()->TemperatureProfile().ValueAt(GetModel()->FormationTopography()->Overburden()->TopTVD()));
    ValidateQuantity(pDX, &m_debSurfaceUndisturbedTemp, &qTemp);
  }

	BuildFormationList();

	if (!pDX->m_bSaveAndValidate)
	{
		//update gui
		//temporary quantities, only used to set right unit-string-label:
		CTemperatureGradientQuantity	tempTGQ;
		CStressGradientQuantity			tempSGQ;
		CNoneQuantity					tempNQ;
		CAngleQuantity					tempAQ;
		CSinglePressure					tempSPQ;
			
		SetUnitlabel(IDC_lbUPorePressFormationBottom,	&tempSPQ);
		SetUnitlabel(IDC_lbUPorePressAtFormationTop,	&tempSPQ);
		SetUnitlabel(IDC_lbUAzimuthOfSHWithNorth,		&tempAQ);
		SetUnitlabel(IDC_lbUMinHorizontalStressRatio,	&tempNQ);
		SetUnitlabel(IDC_lbUMaxHorizontalStressRatio,	&tempNQ);
		SetUnitlabel(IDC_lbUGeothermalGradientF,		&tempTGQ);
		
		SetUnitlabel(IDC_lbUAverageDensity ,	GetModel()->FormationTopography()->Overburden()->OBAverageDensity());
		SetUnitlabel(IDC_lbUGeothermalGradient ,		GetModel()->FormationTopography()->Overburden()->OBGeothermalGradient());
		SetUnitlabel(IDC_lbUSurfaceUndisturbedTemp ,	GetModel()->FormationTopography()->Overburden()->SurfaceTemperature());
		
	}

	if(pDX->m_bSaveAndValidate)
	{
		DrawView()->Invalidate();
		TreeEntry()->UpdateEnable();
		UpdateButtons();
	}

}


BEGIN_MESSAGE_MAP(CInsituStressesForm, CBaseFormView)
	//{{AFX_MSG_MAP(CInsituStressesForm)
	ON_NOTIFY(NM_CLICK, IDC_lstFormationName, OnClicklstFormationName)
	ON_WM_SHOWWINDOW()
	ON_NOTIFY(LVN_KEYDOWN, IDC_lstFormationName, OnKeydownlstFormationName)
	//ON_BN_CLICKED(IDC_optFullyDrained, OnoptFullyDrained)
	//ON_BN_CLICKED(IDC_optFullyUndrained, OnoptFullyUndrained)
	//ON_BN_CLICKED(IDC_BUTTON_HELP, OnHelp)
  ON_BN_CLICKED(IDC_ButtonImportPressure, OnImportPressure)
  ON_BN_CLICKED(IDC_ButtonRemovePressure, OnRemovePressure)
  ON_BN_CLICKED(IDC_ButtonImportDensity, OnImportDensity)
  ON_BN_CLICKED(IDC_ButtonRemoveDensity, OnRemoveDensity)
  ON_BN_CLICKED(IDC_ButtonImportTemperature, OnImportTemperature)
  ON_BN_CLICKED(IDC_ButtonRemoveTemperature, OnRemoveTemperature)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CInsituStressesForm diagnostics

#ifdef _DEBUG
//##ModelId=3C0DC15A006D
void CInsituStressesForm::AssertValid() const
{
	CFormView::AssertValid();
}

//##ModelId=3C0DC15A006F
void CInsituStressesForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CInsituStressesForm message handlers

//##ModelId=3C0DC15A004E
void CInsituStressesForm::OnInitialUpdate() 
{
	//m_optFullyUndrained = 1;

	CBaseFormView::OnInitialUpdate();

	GetDlgItem(IDC_fraOverburden)->SetFont(GetFont(TRUE));
	GetDlgItem(IDC_fraFormationInsituStresses)->SetFont(GetFont(TRUE));
	
	m_pCurrentFormation=0;

	static short bInit=0;
	if(!bInit)
	{
		//One time initialization!!
		bInit=1;

    GetDlgItem(IDC_ButtonImportPressure)->EnableWindow(GetModel()->PorePressureProfile().empty());
    GetDlgItem(IDC_ButtonRemovePressure)->EnableWindow(!GetModel()->PorePressureProfile().empty());

    GetDlgItem(IDC_ButtonImportDensity)->EnableWindow(GetModel()->DensityProfile().empty());
    GetDlgItem(IDC_ButtonRemoveDensity)->EnableWindow(!GetModel()->DensityProfile().empty());

    GetDlgItem(IDC_ButtonImportTemperature)->EnableWindow(GetModel()->TemperatureProfile().empty());
    GetDlgItem(IDC_ButtonRemoveTemperature)->EnableWindow(!GetModel()->TemperatureProfile().empty());

		m_lstItemOverburden.Set(this,IDC_fraOverburden);
		m_lstItemFormation.Set(this,IDC_fraFormationInsituStresses);

		//Create Columns
		LV_COLUMN lvColumn;
		lvColumn.mask = LVCF_FMT|LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM;
		lvColumn.fmt = LVCFMT_LEFT;
		lvColumn.cx = 71;
		
		CListCtrl* pList=(CListCtrl*)GetDlgItem(IDC_lstFormationName);

		ASSERT(pList);
		pList->DeleteAllItems();

		pList->SetExtendedStyle(LVS_EX_FULLROWSELECT);

		CLengthQuantity temp; // temporary quantity only needed for getting the unit-string 

		lvColumn.iSubItem=0;
		lvColumn.pszText="Formation";
		lvColumn.cx = 95;
		pList->InsertColumn(0, &lvColumn);

		lvColumn.iSubItem=1;
		lvColumn.pszText="Valid Input";
		lvColumn.cx = 65;
		pList->InsertColumn(1, &lvColumn);

		lvColumn.iSubItem=2;
		CString strUnit("Top ");
		strUnit+= GetUnitString(&temp);
		lvColumn.pszText = (LPSTR)(LPCSTR)strUnit;
		lvColumn.cx = 60;
		pList->InsertColumn(2, &lvColumn);

		lvColumn.iSubItem=3;
		strUnit=("Bottom ");
		strUnit+= GetUnitString(&temp);
		lvColumn.pszText=strUnit.GetBufferSetLength(strUnit.GetLength());
		lvColumn.cx = 70;
		pList->InsertColumn(3, &lvColumn);

		m_toolTip.Create(this);
		CRect rect;
		CString sTmp;

		sTmp.LoadString(IDTT_INS_ebAzimuthOfSHWithNorth);
		m_toolTip.AddTool( GetDlgItem(IDC_ebAzimuthOfSHWithNorth), sTmp);
		GetDlgItem(IDC_ebAzimuthOfSHWithNorth)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_INS_ebGeothermalGradient);
		m_toolTip.AddTool( GetDlgItem(IDC_ebGeothermalGradient), sTmp);
		GetDlgItem(IDC_ebGeothermalGradient)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_INS_ebGeothermalGradientF);
		m_toolTip.AddTool( GetDlgItem(IDC_ebGeothermalGradientF), sTmp);
		GetDlgItem(IDC_ebGeothermalGradientF)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_INS_ebSurfaceUndisturbedTemp);
		m_toolTip.AddTool( GetDlgItem(IDC_ebSurfaceUndisturbedTemp), sTmp);
		GetDlgItem(IDC_ebSurfaceUndisturbedTemp)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_INS_ebVerticalStressGradient);
		m_toolTip.AddTool( GetDlgItem(IDC_ebAverageDensity), sTmp);
		GetDlgItem(IDC_ebAverageDensity)->GetWindowRect(&rect);	
		ScreenToClient(rect);
		
		//IDTT_INS_ebAzimuthOfSHWithNorth
		//IDTT_INS_ebGeothermalGradient
		//IDTT_INS_ebGeothermalGradientF
		//IDTT_INS_ebSurfaceUndisturbedTemp
		//IDTT_INS_ebVerticalStressGradient

		//IDC_ebAzimuthOfSHWithNorth
		//IDC_ebGeothermalGradient
		//IDC_ebGeothermalGradientF
		//IDC_ebSurfaceUndisturbedTemp
		//IDC_ebVerticalStressGradient

		CRect rectMargin(TOOL_TIP_MARGE,TOOL_TIP_MARGE,TOOL_TIP_MARGE,TOOL_TIP_MARGE); //left, top, right, bottom
		m_toolTip.SetMargin(rectMargin);
		m_toolTip.Activate(TRUE);
	}

	
			
}


//##ModelId=3C0DC15A00BE
void CInsituStressesForm::OnClicklstFormationName(NMHDR* pNMHDR, LRESULT* pResult) 
{
	
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	if (pNMListView->iItem<=0)
	{
		m_pCurrentFormation	=0; //is overburden!
		UpdateData(FALSE);
		return ;
	}
	else
	{
		int iFItem=((pNMListView->iItem)-1); 
		m_pCurrentFormation=GetModel()->FormationTopography()->GetFormationAt(iFItem);
	}

	UpdateData(FALSE);
	
	*pResult = 0;
}


//##ModelId=3C0DC159031C
BOOL CInsituStressesForm::CheckComplete() const
{
	if(!TreeEntry()->Previous()->FormView()->CheckComplete())
	{
		return FALSE;
	}

	if(GetModel()->FormationTopography()->NrOfFormations()<1)
	{
		return FALSE;
	}

	for(int i =0 ;i<GetModel()->FormationTopography()->NrOfFormations();i++)
	{
		if(!GetModel()->FormationTopography()->GetFormationAt(i)->CheckInsituStressesComplete())
		{
			return FALSE;
		}
	}
	if(!GetModel()->FormationTopography()->Overburden()->CheckComplete())
	{
		return FALSE;
	}

	return TRUE;	
}


//##ModelId=3C2043FE011A
void CInsituStressesForm::BuildFormationList()
{
	//get the current unit system.
	CDSealectDoc* pDoc=(CDSealectDoc*)GetDocument();
	QU::UNIT us = pDoc->UnitSystem();

	CListCtrl* pList=0;
	pList=(CListCtrl*)GetDlgItem(IDC_lstFormationName);
	
	if(!pList)
		return;

	pList->DeleteAllItems();

	/*
	LV_COLUMN col;

	col.mask = LVCF_WIDTH; //set the width of the columns
	col.cx = 95;
	m_lstFormationName.SetColumn(0, &col);
	col.cx = 65;
	m_lstFormationName.SetColumn(1, &col);

	col.mask = LVCF_TEXT|LVCF_WIDTH;

	CLengthQuantity temp; // temporary quantity only needed for getting the unit-string 
	CString strUnit("Top ");
	strUnit+= GetUnitString(&temp);
	col.pszText = (LPSTR)(LPCSTR)strUnit;
	col.cx = 60;
	m_lstFormationName.SetColumn(2, &col);

	strUnit=("Bottom ");
	strUnit+= GetUnitString(&temp);
	col.pszText=strUnit.GetBufferSetLength(strUnit.GetLength());
	col.cx = 70;
	m_lstFormationName.SetColumn(3, &col);

	*/

	if(GetModel()->FormationTopography()->NrOfFormations()<1)
	{
		m_pCurrentFormation=0; //is overburden!
		return;
	}

	//Create items
	LV_ITEM lvItem;
	
	lvItem.mask= LVIF_TEXT|LVIF_STATE;
	lvItem.state=0;
	lvItem.stateMask=0;

	int iNrOfFirstItems=0; 
	
	CString sTmp;
	lvItem.iItem=0;
	lvItem.iSubItem=0;
	
	sTmp="Overburden";
	lvItem.pszText= sTmp.GetBufferSetLength(sTmp.GetLength());
	pList->InsertItem(&lvItem);

	if(GetModel()->FormationTopography()->Overburden()->CheckComplete())
	{
		sTmp="OK";
	}
	else
	{
		sTmp="Not OK";
	}
	pList->SetItemText(0,1,sTmp.GetBufferSetLength(sTmp.GetLength()));

	if (GetModel()->FormationTopography()->Offshore())
	{	
		sTmp=CDoubleEdit::Format(GetModel()->FormationTopography()->Seawater()->SeaBedTVD().Value(us));
	}
	else
	{
		sTmp=CDoubleEdit::Format(GetModel()->FormationTopography()->ElevationTVD()->Value(us));	
	}
	pList->SetItemText(0,2,sTmp.GetBufferSetLength(sTmp.GetLength()));
	
	sTmp=CDoubleEdit::Format(GetModel()->FormationTopography()->Overburden()->OverburdenBottomTVD()->Value(us));
	pList->SetItemText(0,3,sTmp.GetBufferSetLength(sTmp.GetLength()));
	
	for(int i =0 ;i<GetModel()->FormationTopography()->NrOfFormations();i++)
	{
		CString sTmp;
		lvItem.iItem=(i+1);

		lvItem.iSubItem=0;
		
		sTmp=GetModel()->FormationTopography()->GetFormationAt(i)->FormationName();
		lvItem.pszText= sTmp.GetBufferSetLength(sTmp.GetLength());
		pList->InsertItem(&lvItem);

		if(GetModel()->FormationTopography()->GetFormationAt(i)->CheckInsituStressesComplete())
		{
			sTmp="OK";
		}
		else
		{
			sTmp="Not OK";
		}
		pList->SetItemText((i+1),1,sTmp.GetBufferSetLength(sTmp.GetLength()));
	
		//(Formation topTVD) in col3 is equal to (previous formation bottomTVD) in col4:
		pList->SetItemText((i+1),2,m_lstFormationName.GetItemText(((i+1)-1),3));
		
		sTmp=CDoubleEdit::Format(GetModel()->FormationTopography()->GetFormationAt(i)->BottomTVD()->Value(us));
		pList->SetItemText((i+1),3,sTmp.GetBufferSetLength(sTmp.GetLength()));
	}
	
	if(m_pCurrentFormation)
	{
		long Index=GetModel()->FormationTopography()->GetFormationIndex(m_pCurrentFormation);
		pList->SetItem((Index+1), 0, LVIF_STATE, NULL, 0, LVIS_SELECTED, LVIS_SELECTED, 0);
		pList->EnsureVisible(Index+1,true);
	}
	else
	{
		//if there is no CurrentFormation the Overburden is selected(ListItem 0)
		pList->SetItem((0), 0, LVIF_STATE, NULL, 0, LVIS_SELECTED, LVIS_SELECTED, 0);
		pList->EnsureVisible(0+1,true);
	}
	
}

//##ModelId=3C2043FE01A6
void CInsituStressesForm::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CBaseFormView::OnShowWindow(bShow, nStatus);
	
}

//##ModelId=3C2043FE02CF
void CInsituStressesForm::OnKeydownlstFormationName(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	
	// set from 0 to 1 to lock the use of arrow-keys 	
	*pResult = 1;
}

/*
void CInsituStressesForm::OnoptFullyDrained() 
{
	GetDocument()->SetModifiedFlag(TRUE);
	UpdateData();
	
}

void CInsituStressesForm::OnoptFullyUndrained() 
{
	
	MessageBox("Please, enter undrained elastic properties (E, nu) for this formation material.");

	GetDocument()->SetModifiedFlag(TRUE);
	UpdateData();
	
}
*/

void CInsituStressesForm::OnImportPressure()
{
  CFileDialog dlg(TRUE, 0, 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "Data Files (*.dat;*.txt)|*.dat;*.txt|All Files (*.*)|*.*||");
  if(dlg.DoModal() == IDOK)
  {
    CString strFilePath = dlg.GetPathName();
    CDataModel& model = *GetModel();
    CValueProfile tempProfile;
    CPorePressureDataReader reader(model, tempProfile);
    if(reader.Import(strFilePath))
    {
      model.PorePressureProfile() = tempProfile;
      model.PorePressureProfileFileName(strFilePath);
      UpdateData(FALSE);
      GetDoc()->SetModifiedFlag();
      model.InvalidateResults();
      TreeEntry()->UpdateEnable();
  		DrawView()->Invalidate();
    }
  }
}

void CInsituStressesForm::OnRemovePressure()
{
  if(AfxMessageBox("Are you sure you want to remove the pressure profile?", MB_YESNO) == IDYES)
  {
    GetModel()->PorePressureProfile().clear();
    GetModel()->PorePressureProfileFileName("");
    UpdateData(FALSE);
    GetDoc()->SetModifiedFlag();
    GetModel()->InvalidateResults();
    TreeEntry()->UpdateEnable();
		DrawView()->Invalidate();
  }
}

void CInsituStressesForm::OnImportDensity()
{
  CFileDialog dlg(TRUE, 0, 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "Data Files (*.dat;*.txt)|*.dat;*.txt|All Files (*.*)|*.*||");
  if(dlg.DoModal() == IDOK)
  {
    CString strFilePath = dlg.GetPathName();
    CDataModel& model = *GetModel();
    CValueProfile tempProfile;
    CDensityDataReader reader(model, tempProfile);
    if(reader.Import(strFilePath))
    {
      model.DensityProfile() = tempProfile;
      model.DensityProfileFileName(strFilePath);
      UpdateData(FALSE);
      GetDoc()->SetModifiedFlag();
      model.InvalidateResults();
      TreeEntry()->UpdateEnable();
  		DrawView()->Invalidate();
    }
  }
}

void CInsituStressesForm::OnRemoveDensity()
{
  if(AfxMessageBox("Are you sure you want to remove the density profile?", MB_YESNO) == IDYES)
  {
    GetModel()->DensityProfile().clear();
    GetModel()->DensityProfileFileName("");
    UpdateData(FALSE);
    GetDoc()->SetModifiedFlag();
    GetModel()->InvalidateResults();
    TreeEntry()->UpdateEnable();
 		DrawView()->Invalidate();
  }
}

void CInsituStressesForm::OnImportTemperature()
{
  CFileDialog dlg(TRUE, 0, 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "Data Files (*.dat;*.txt)|*.dat;*.txt|All Files (*.*)|*.*||");
  if(dlg.DoModal() == IDOK)
  {
    CString strFilePath = dlg.GetPathName();
    CDataModel& model = *GetModel();
    CValueProfile tempProfile;
    CTemperatureDataReader reader(model, tempProfile);
    if(reader.Import(strFilePath))
    {
      model.TemperatureProfile() = tempProfile;
      model.TemperatureProfileFileName(strFilePath);
      UpdateData(FALSE);
      GetDoc()->SetModifiedFlag();
      model.InvalidateResults();
      TreeEntry()->UpdateEnable();
  		DrawView()->Invalidate();
    }
  }
}

void CInsituStressesForm::OnRemoveTemperature()
{
  if(AfxMessageBox("Are you sure you want to remove the temperature profile?", MB_YESNO) == IDYES)
  {
    GetModel()->TemperatureProfile().clear();
    GetModel()->TemperatureProfileFileName("");
    UpdateData(FALSE);
    GetDoc()->SetModifiedFlag();
    GetModel()->InvalidateResults();
    TreeEntry()->UpdateEnable();
 		DrawView()->Invalidate();
  }
}
