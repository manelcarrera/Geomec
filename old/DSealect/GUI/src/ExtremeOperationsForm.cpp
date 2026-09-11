// ExtremeOperationsForm.cpp : implementation file
//

#include "stdafx.h"
#include "dsealect.h"
#include "ExtremeOperationsForm.h"
#include "treeobject.h"
#include "DSealectGlobal.h"
#include "CDataModel.h"
#include "CAnalysis.h"
#include "CFormationTopography.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[] = __FILE__;#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CExtremeOperationsForm

static BOOL AnalysisPointOk(CAnalysisPoint* point)
{		
		return point->CheckCompleteExtremeOper();

		/*
		bool FCheck;
		if(GetDoc()->DSealectModel()->Control()->Fracturing())
		{
			FCheck=point->FracturingLoads()->CheckComplete();
		}
		else
		{
			FCheck=true;
		}	
	
		bool ECheck;
		if(GetDoc()->DSealectModel()->Control()->Evacuation())
		{
			ECheck=point->EvacuationLoads()->CheckComplete();
		}
		else
		{
			ECheck=true;
		}
	
		if(FCheck && ECheck)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
		*/
					
}

IMPLEMENT_DYNCREATE(CExtremeOperationsForm, CBaseFormView)

//##ModelId=3C0DC15B0148
CExtremeOperationsForm::CExtremeOperationsForm()
	: CBaseFormView(CExtremeOperationsForm::IDD)
	,m_lstAnalysisPoints(AnalysisPointOk)
{
	//{{AFX_DATA_INIT(CExtremeOperationsForm)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

//##ModelId=3C0DC15B01D4
CExtremeOperationsForm::~CExtremeOperationsForm()
{
	if(m_toolTip)
		delete m_toolTip;
}

//##ModelId=3C0DC15B01C5
void CExtremeOperationsForm::DoDataExchange(CDataExchange* pDX)
{
	if(!pDX->m_bSaveAndValidate)
	{
		//update gui
		m_bFracturing = GetModel()->Control()->Fracturing()? TRUE:FALSE;
		m_bEvacuation = GetModel()->Control()->Evacuation()? TRUE:FALSE;
	}

	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExtremeOperationsForm)
	DDX_Control(pDX, IDC_fraAnalysisPoint, m_fraAnalysisPoint);
	DDX_Control(pDX, IDC_ebChosenWellType, m_ebChosenWellType);
	DDX_Check(pDX, IDC_chkFracturingHighRateKilling, m_bFracturing);
	DDX_Check(pDX, IDC_chkEvacuationUnloading, m_bEvacuation);
	DDX_Control(pDX, IDC_lstAnalysisPoint, m_lstAnalysisPoints);
	DDX_Control(pDX, IDC_ebTreatingFluidTemperatureEU, m_debTreatingFluidTemperatureEU);
	DDX_Control(pDX, IDC_ebTreatingFluidTemperature, m_debTreatingFluidTemperature);
	DDX_Control(pDX, IDC_ebTimeOfExposToTreatFluidEU, m_debTimeOfExposToTreatFluidEU);
	DDX_Control(pDX, IDC_ebTimeOfExposToTreatFluid, m_debTimeOfExposToTreatFluid);
	DDX_Control(pDX, IDC_ebMaximumInjectionPressure, m_debMaximumInjectionPressure);
	DDX_Control(pDX, IDC_ebBottomHoleTreatingPres, m_debBottomHoleTreatingPres);
	//}}AFX_DATA_MAP

	if(!CheckEntry())
		return;

	CString strWellName;
	strWellName.LoadString((UINT)GetModel()->Control()->WellType());
	m_ebChosenWellType.SetWindowText(strWellName);
	
	CButton* pFracturingButton = (CButton*)GetDlgItem(IDC_chkFracturingHighRateKilling);
	CButton* pEvacuationButton = (CButton*)GetDlgItem(IDC_chkEvacuationUnloading);

	switch(GetModel()->Control()->WellType())
	{

		case CControl::OilProducer:
		case CControl::GasProducer:
		case CControl::ThermalWell:
			m_fraFracturing.Enable(pFracturingButton->GetCheck()); //dis-/enable fracturing frame
			m_fraEvacuation.Enable(pEvacuationButton->GetCheck()); //dis-/enable evacuation frame
			pFracturingButton->EnableWindow(TRUE);
			pEvacuationButton->EnableWindow(TRUE);
			break;

		
		case CControl::WaterInjector:
			m_fraFracturing.Enable(pFracturingButton->GetCheck()); //dis-/enable fracturing frame
			m_fraEvacuation.Enable(FALSE); //disable evacuation frame
			pFracturingButton->EnableWindow(TRUE);
			pEvacuationButton->SetCheck(0);
			pEvacuationButton->EnableWindow(FALSE);
			break;
		
		case CControl::WaterSource:
			m_fraFracturing.Enable(FALSE); //disable fracturing frame
			m_fraEvacuation.Enable(FALSE); //disable evacuation frame
			pFracturingButton->SetCheck(0);
			pFracturingButton->EnableWindow(FALSE);
			pEvacuationButton->SetCheck(0);
			pEvacuationButton->EnableWindow(FALSE);
			break;
		
		case CControl::GasStorage:
		case CControl:: SteamInjector:
			m_fraFracturing.Enable(FALSE); //disable fracturing frame
			m_fraEvacuation.Enable(pEvacuationButton->GetCheck()); //dis-/enable evacuation frame
			pFracturingButton->SetCheck(0);
			pFracturingButton->EnableWindow(FALSE);
			pEvacuationButton->EnableWindow(TRUE);
			break;
	}

	CAnalysisPoint* pAnalysisPoint=m_lstAnalysisPoints.GetSelectedAnalysisPoint();

	if(pDX->m_bSaveAndValidate)
	{
		GetModel()->Control()->Fracturing(m_bFracturing == TRUE);
		GetModel()->Control()->Evacuation(m_bEvacuation == TRUE);
	}

	if(pAnalysisPoint)
	{
		ValidateQuantity(pDX, &m_debMaximumInjectionPressure, pAnalysisPoint->FracturingLoads()->MaxInjectingFracPressure(), CString("MaxInjectingFracPressure"));
		ValidateQuantity(pDX, &m_debTreatingFluidTemperature, pAnalysisPoint->FracturingLoads()->TreatingFluidTemp(), CString("TreatingFluidTemp"));
		ValidateQuantity(pDX, &m_debTimeOfExposToTreatFluid, pAnalysisPoint->FracturingLoads()->TimeOfExposureToTreatingFluid(), CString("TimeOfExposureToTreatingFluid"));

		ValidateQuantity(pDX, &m_debBottomHoleTreatingPres, pAnalysisPoint->EvacuationLoads()->BottomHoleTreatingPressure(), CString("BottomHoleTreatingPressure"));
		ValidateQuantity(pDX, &m_debTreatingFluidTemperatureEU, pAnalysisPoint->EvacuationLoads()->TreatingFluidTempEvac(), CString("TreatingFluidTempEvac"));
		ValidateQuantity(pDX, &m_debTimeOfExposToTreatFluidEU, pAnalysisPoint->EvacuationLoads()->TimeOfExposureToTreatingFluidEvac(), CString("TimeOfExposureToTreatingFluidEvac"));
	}
	
	if (!pDX->m_bSaveAndValidate)
	{
		//temporary quantities, only used to set right unit-string-label
		CSinglePressure			tempSP;
		CTemperatureQuantity	tempTQ;
		CTimeHQuantity			tempTHQ;

		SetUnitlabel(IDC_lbUMaximumInjectionPressure, &tempSP);
		SetUnitlabel(IDC_lbUTreatingFluidTemperature, &tempTQ);
		SetUnitlabel(IDC_lbUTimeOfExposToTreatFluid, &tempTHQ);

		SetUnitlabel(IDC_lbUBottomHoleTreatingPres, &tempSP);
		SetUnitlabel(IDC_lbUTreatingFluidTemperatureEU, &tempTQ);
		SetUnitlabel(IDC_lbUTimeOfExposToTreatFluidEU, &tempTHQ);
	}

	
	m_lstAnalysisPoints.Update();
	CString strNr; //caption of frame depends on selected analysis point
	long Index=GetDoc()->DSealectModel()->Analysis()->GetAnalysisPointIndex(pAnalysisPoint);
	strNr.Format("%i", Index+1);
	CString strTMD;
	strTMD=m_lstAnalysisPoints.GetItemText(Index,1);
	m_fraAnalysisPoint.SetWindowText(CString("Analysis Point " + strNr + " at TMD= " + strTMD));

	m_lstAnalysisPoints.EnsureVisible(Index,true);

	DrawView()->Invalidate();
	TreeEntry()->UpdateEnable();
	UpdateButtons();
}


BEGIN_MESSAGE_MAP(CExtremeOperationsForm, CBaseFormView)
	//{{AFX_MSG_MAP(CExtremeOperationsForm)
	ON_BN_CLICKED(IDC_chkEvacuationUnloading, OnchkEvacuationUnloading)
	ON_BN_CLICKED(IDC_chkFracturingHighRateKilling, OnchkFracturingHighRateKilling)
	ON_NOTIFY(LVN_KEYDOWN, IDC_lstAnalysisPoint, OnKeydownlstAnalysisPoint)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExtremeOperationsForm diagnostics

#ifdef _DEBUG
//##ModelId=3C0DC15B01D6
void CExtremeOperationsForm::AssertValid() const
{
	CFormView::AssertValid();
}

//##ModelId=3C0DC15B01D8
void CExtremeOperationsForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CExtremeOperationsForm message handlers



//##ModelId=3C3040B601F2
void CExtremeOperationsForm::OnInitialUpdate() 
{	
	m_bFracturing=TRUE;
	m_bEvacuation=TRUE;

	m_fraEvacuation.Set(this,IDC_fraEvacuationUnloading);
	m_fraFracturing.Set(this,IDC_fraFracturingHighRateKilling);

	GetDlgItem(IDC_fraAnalysisPoint)->SetFont(GetFont(TRUE));

	CBaseFormView::OnInitialUpdate();

	GetDlgItem(IDC_fraFracturingHighRateKilling)->SetFont(GetFont(TRUE));
	GetDlgItem(IDC_fraEvacuationUnloading)->SetFont(GetFont(TRUE));

	// TOOLTIP
	static short bInit=0;
	if(!bInit)
	{
		//One time initialization!!
		bInit=1;
		
		m_toolTip.Create(this);
		CRect rect;
		CString sTmp;

		sTmp.LoadString(IDTT_EXO_chkEvacuationUnloading);
		m_toolTip.AddTool( GetDlgItem(IDC_chkEvacuationUnloading), sTmp);
		GetDlgItem(IDC_chkEvacuationUnloading)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_EXO_chkFracturingHighRateKilling);
		m_toolTip.AddTool( GetDlgItem(IDC_chkFracturingHighRateKilling), sTmp);
		GetDlgItem(IDC_chkFracturingHighRateKilling)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_MORE_BottomHolePressure);
		m_toolTip.AddTool( GetDlgItem(IDC_ebBottomHoleTreatingPres), sTmp);
		GetDlgItem(IDC_ebBottomHoleTreatingPres)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_MORE_BottomHolePressure);
		m_toolTip.AddTool( GetDlgItem(IDC_ebMaximumInjectionPressure), sTmp);
		GetDlgItem(IDC_ebMaximumInjectionPressure)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_MORE_ExposureTimeToFluid);
		m_toolTip.AddTool( GetDlgItem(IDC_ebTimeOfExposToTreatFluid), sTmp);
		GetDlgItem(IDC_ebTimeOfExposToTreatFluid)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_MORE_ExposureTimeToFluid);
		m_toolTip.AddTool( GetDlgItem(IDC_ebTimeOfExposToTreatFluidEU), sTmp);
		GetDlgItem(IDC_ebTimeOfExposToTreatFluidEU)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_MORE_BottomHoleTemperature);
		m_toolTip.AddTool( GetDlgItem(IDC_ebTreatingFluidTemperature), sTmp);
		GetDlgItem(IDC_ebTreatingFluidTemperature)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_MORE_BottomHoleTemperature);
		m_toolTip.AddTool( GetDlgItem(IDC_ebTreatingFluidTemperatureEU), sTmp);
		GetDlgItem(IDC_ebTreatingFluidTemperatureEU)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_MORE_AnalysisPoint);
		m_toolTip.AddTool( GetDlgItem(IDC_lstAnalysisPoint), sTmp);
		GetDlgItem(IDC_lstAnalysisPoint)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		//IDTT_EXO_chkEvacuationUnloading
		//IDTT_EXO_chkFracturingHighRateKilling
		//IDTT_EXO_ebBottomHoleTreatingPres
		//IDTT_EXO_ebMaximumInjectionPressure
		//IDTT_EXO_ebTimeOfExposToTreatFluid
		//IDTT_EXO_ebTimeOfExposToTreatFluidEU
		//IDTT_EXO_ebTreatingFluidTemperature
		//IDTT_EXO_ebTreatingFluidTemperatureEU
		//IDTT_EXO_lstAnalysisPoint

		//IDC_chkEvacuationUnloading
		//IDC_chkFracturingHighRateKilling
		//IDC_ebBottomHoleTreatingPres
		//IDC_ebMaximumInjectionPressure
		//IDC_ebTimeOfExposToTreatFluid
		//IDC_ebTimeOfExposToTreatFluidEU
		//IDC_ebTreatingFluidTemperature
		//IDC_ebTreatingFluidTemperatureEU
		//IDC_lstAnalysisPoint

		
		CRect rectMargin(TOOL_TIP_MARGE,TOOL_TIP_MARGE,TOOL_TIP_MARGE,TOOL_TIP_MARGE); //left, top, right, bottom
		m_toolTip.SetMargin(rectMargin);
		m_toolTip.Activate(TRUE);
	}
	
}


//##ModelId=3C3040B7009A
void CExtremeOperationsForm::OnchkEvacuationUnloading() 
{
	UpdateData();	
	UpdateData(FALSE);
	GetDocument()->SetModifiedFlag(TRUE);

	
}

//##ModelId=3C3040B700F8
void CExtremeOperationsForm::OnchkFracturingHighRateKilling() 
{
	UpdateData();	
	UpdateData(FALSE);
	GetDocument()->SetModifiedFlag(TRUE);
	
}

//##ModelId=3C0DC15B0149
BOOL CExtremeOperationsForm::CheckComplete() const
{
	if(!TreeEntry()->Previous()->FormView()->CheckComplete())
	{
		return FALSE;
	}

	if(GetModel()->FormationTopography()->NrOfFormations()<1)
		return FALSE;

	if(GetModel()->Analysis()->NrOfAnalysisPoints()<1)
		return FALSE;

	for(int i=0 ;i< GetModel()->Analysis()->NrOfAnalysisPoints();i++)
	{
		CAnalysisPoint *pAnalysisPoint=GetModel()->Analysis()->GetAnalysisPointAt(i);
		if(!pAnalysisPoint->CheckCompleteExtremeOper())
			return FALSE;
	}
		
	return TRUE;
}

//##ModelId=3C3040B60369
void CExtremeOperationsForm::OnKeydownlstAnalysisPoint(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	*pResult = 1; // 1 ipv 0 to lock use of arrow keys
}

