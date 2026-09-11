// ProductionAndInjectionForm.cpp : implementation file
//

#include "stdafx.h"
#include "dsealect.h"
#include "ProductionAndInjectionForm.h"
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
// CProductionAndInjectionForm
static BOOL AnalysisPointOk(CAnalysisPoint* point)
{		
	return point->CheckCompleteProdAndInject();
}

IMPLEMENT_DYNCREATE(CProductionAndInjectionForm, CBaseFormView)

//##ModelId=3C0DC157032C
CProductionAndInjectionForm::CProductionAndInjectionForm()
	: CBaseFormView(CProductionAndInjectionForm::IDD)
	, m_lstAnalysisPoints(AnalysisPointOk)
{
	//{{AFX_DATA_INIT(CProductionAndInjectionForm)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	
}

//##ModelId=3C0DC158010A
CProductionAndInjectionForm::~CProductionAndInjectionForm()
{
	if(m_toolTip)
		delete m_toolTip;
}

//##ModelId=3C0DC15800FC
void CProductionAndInjectionForm::DoDataExchange(CDataExchange* pDX)
{
	if(!pDX->m_bSaveAndValidate)
	{
		//update gui
		m_iConversion = GetModel()->Control()->Conversion() ? 1:0;
	}

	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProductionAndInjectionForm)
	DDX_Control(pDX, IDC_fraAnalysisPoint, m_fraAnalysisPoint);
	DDX_Control(pDX, IDC_lstAnalysisPoint, m_lstAnalysisPoints);
	DDX_Control(pDX, IDC_ebChosenWellType, m_ebChosenWellType);
	DDX_Check(pDX, IDC_chkConversion, m_iConversion);
	DDX_Control(pDX, IDC_ebTimeOfContinuousProd, m_debTimeOfContinuousProd);
	DDX_Control(pDX, IDC_ebTimeOfContinuousInjection, m_debTimeOfContinuousInjection);
	DDX_Control(pDX, IDC_ebPorePressure, m_debPorePressure);
	DDX_Control(pDX, IDC_ebMaximumInjectionPressure, m_debMaximumInjectionPressure);
	DDX_Control(pDX, IDC_ebInjectionTemperature, m_debInjectionTemperature);
	DDX_Control(pDX, IDC_ebBottomHoleFlowingTemp, m_debBottomHoleFlowingTemp);
	DDX_Control(pDX, IDC_ebBottomHoleFlowingPress, m_debBottomHoleFlowingPress);
	//}}AFX_DATA_MAP

	if(!CheckEntry())
		return;
	
	CString strWellName;
	strWellName.LoadString((UINT)GetModel()->Control()->WellType());
	m_ebChosenWellType.SetWindowText(strWellName);

	CAnalysisPoint*	pAnalysisPoint=m_lstAnalysisPoints.GetSelectedAnalysisPoint();

	if(!CheckEntry())
	{
		return;
	}

	

	if(pAnalysisPoint)
	{
		ValidateQuantity(pDX, &m_debTimeOfContinuousProd,	pAnalysisPoint->ProductionLoads()->TimeOfContinuousProduction(),	CString("TimeOfContinuousProduction"));
		ValidateQuantity(pDX, &m_debBottomHoleFlowingPress,	pAnalysisPoint->ProductionLoads()->BottomHoleFlowingPressure(),	CString("BottomHoleFlowingPressure"));
		ValidateQuantity(pDX, &m_debBottomHoleFlowingTemp,	pAnalysisPoint->ProductionLoads()->BottomHoleFlowingTemp(),		CString("BottomHoleFlowingTemp"));

    CSinglePressure pres(pAnalysisPoint->ProductionLoads()->ProductionPorePressure());
		ValidateQuantity(pDX, &m_debPorePressure,	&pres,	CString("ProductionPorePressure"));

		ValidateQuantity(pDX, &m_debMaximumInjectionPressure,	pAnalysisPoint->InjectionLoads()->MaximumInjectionPressure(),		CString("MaximumInjectionPressure"));
		ValidateQuantity(pDX, &m_debInjectionTemperature,		pAnalysisPoint->InjectionLoads()->InjectionTemperature(),			CString("InjectionTemperature"));
		ValidateQuantity(pDX, &m_debTimeOfContinuousInjection,	pAnalysisPoint->InjectionLoads()->TimeOfContinuousInjection(),	CString("TimeOfContinuousInjection"));
	}
	


	if(!pDX->m_bSaveAndValidate)
	{
		//update gui
		//temporary quantities, only used to set right unit-string-label
		CSinglePressure tempSP;
		CTemperatureQuantity tempTQ;
		CTimeDQuantity tempTDQ;
				
		SetUnitlabel(IDC_lbUTimeOfContinuousProd,&tempTDQ);
		SetUnitlabel(IDC_lbUBottomHoleFlowingPress,&tempSP);
		SetUnitlabel(IDC_lbUBottomHoleFlowingTemp,&tempTQ);
		SetUnitlabel(IDC_lbUPorePressure,&tempSP);

		SetUnitlabel(IDC_lbUMaximumInjectionPressure,&tempSP);
		SetUnitlabel(IDC_lbUInjectionTemperature,&tempTQ);
		SetUnitlabel(IDC_lbUTimeOfContinuousInjection,&tempTDQ);
		
	}
	else
	{
		//update data
		GetModel()->Control()->Conversion(m_iConversion == 1);
	}
	
	m_lstAnalysisPoints.Update();
	CString strNr; //caption of frame depends on selected analysis point
	long Index=GetDoc()->DSealectModel()->Analysis()->GetAnalysisPointIndex(pAnalysisPoint);
	strNr.Format("%i", Index+1);
	CString strTMD;
	strTMD=m_lstAnalysisPoints.GetItemText(Index,1);
	m_fraAnalysisPoint.SetWindowText(CString("Analysis Point " + strNr + " at TMD= " + strTMD));


	m_fraProduction.Enable(GetModel()->Control()->IsProduction()); //enable production frame
	m_fraInjection.Enable(GetModel()->Control()->IsInjection()); //enable injection frame
  m_debPorePressure.EnableWindow(FALSE);

	CButton* pChkButton = (CButton*)GetDlgItem(IDC_chkConversion);

	if(GetModel()->Control()->ProductionConversionAllowed())
	{
		pChkButton->EnableWindow(TRUE);
		pChkButton->SetWindowText("Conversion to producer");
	}
	else if(GetModel()->Control()->InjectionConversionAllowed())
	{
		pChkButton->EnableWindow(TRUE);
		pChkButton->SetWindowText("Conversion to injector");
	}
	else
	{ 
		pChkButton->EnableWindow(FALSE); //invisible is beter
		pChkButton->SetWindowText("No conversion");
	}
	
	
		
	m_lstAnalysisPoints.EnsureVisible(Index,true);
	DrawView()->Invalidate();
	TreeEntry()->UpdateEnable();
	UpdateButtons();
	
}


BEGIN_MESSAGE_MAP(CProductionAndInjectionForm, CBaseFormView)
	//{{AFX_MSG_MAP(CProductionAndInjectionForm)
	ON_BN_CLICKED(IDC_chkConversion, OnchkConversion)
	ON_NOTIFY(LVN_KEYDOWN, IDC_lstAnalysisPoint, OnKeydownlstAnalysisPoint)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProductionAndInjectionForm diagnostics

#ifdef _DEBUG
//##ModelId=3C0DC1580119
void CProductionAndInjectionForm::AssertValid() const
{
	CFormView::AssertValid();
}

//##ModelId=3C0DC158011B
void CProductionAndInjectionForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CProductionAndInjectionForm message handlers


//##ModelId=3C0DC1580167
void CProductionAndInjectionForm::OnchkConversion() 
{
	UpdateData();	
	UpdateData(FALSE);
	GetDocument()->SetModifiedFlag(TRUE);
}

//##ModelId=3C0DC15800FA
void CProductionAndInjectionForm::OnInitialUpdate() 
{
	m_iConversion=0;
	m_fraProduction.Set(this,IDC_fraProduction);
	m_fraInjection.Set(this,IDC_fraInjection);

	GetDlgItem(IDC_fraAnalysisPoint)->SetFont(GetFont(TRUE));

	CBaseFormView::OnInitialUpdate();

	GetDlgItem(IDC_fraProduction)->SetFont(GetFont(TRUE));
	GetDlgItem(IDC_fraInjection)->SetFont(GetFont(TRUE));

	// TOOLTIP
	static short bInit=0;
	if(!bInit)
	{
		//One time initialization!!
		bInit=1;
		
		m_toolTip.Create(this);
		CRect rect;
		CString sTmp;

		sTmp.LoadString(IDTT_MORE_BottomHolePressure);
		m_toolTip.AddTool( GetDlgItem(IDC_ebBottomHoleFlowingPress), sTmp);
		GetDlgItem(IDC_ebBottomHoleFlowingPress)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_MORE_BottomHoleTemperature);
		m_toolTip.AddTool( GetDlgItem(IDC_ebBottomHoleFlowingTemp), sTmp);
		GetDlgItem(IDC_ebBottomHoleFlowingTemp)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_MORE_BottomHoleTemperature);
		m_toolTip.AddTool( GetDlgItem(IDC_ebInjectionTemperature), sTmp);
		GetDlgItem(IDC_ebInjectionTemperature)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_MORE_BottomHolePressure);
		m_toolTip.AddTool( GetDlgItem(IDC_ebMaximumInjectionPressure), sTmp);
		GetDlgItem(IDC_ebMaximumInjectionPressure)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_MORE_ExposureTimeToFluid);
		m_toolTip.AddTool( GetDlgItem(IDC_ebTimeOfContinuousInjection), sTmp);
		GetDlgItem(IDC_ebTimeOfContinuousInjection)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_MORE_ExposureTimeToFluid);
		m_toolTip.AddTool( GetDlgItem(IDC_ebTimeOfContinuousProd), sTmp);
		GetDlgItem(IDC_ebTimeOfContinuousProd)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_MORE_AnalysisPoint);
		m_toolTip.AddTool( GetDlgItem(IDC_lstAnalysisPoint), sTmp);
		GetDlgItem(IDC_lstAnalysisPoint)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		//IDTT_PRI_ebBottomHoleFlowingPress
		//IDTT_PRI_ebBottomHoleFlowingTemp
		//IDTT_PRI_ebInjectionTemperature
		//IDTT_PRI_ebMaximumInjectionPressure
		//IDTT_PRI_ebTimeOfContinuousInjection
		//IDTT_PRI_ebTimeOfContinuousProd
		//IDTT_PRI_lstAnalysisPoint

		//IDC_ebBottomHoleFlowingPress
		//IDC_ebBottomHoleFlowingTemp
		//IDC_ebInjectionTemperature
		//IDC_ebMaximumInjectionPressure
		//IDC_ebTimeOfContinuousInjection
		//IDC_ebTimeOfContinuousProd
		//IDC_lstAnalysisPoint
		
		CRect rectMargin(TOOL_TIP_MARGE,TOOL_TIP_MARGE,TOOL_TIP_MARGE,TOOL_TIP_MARGE); //left, top, right, bottom
		m_toolTip.SetMargin(rectMargin);
		m_toolTip.Activate(TRUE);
	}


}


//##ModelId=3C0DC1570350
BOOL CProductionAndInjectionForm::CheckComplete() const
{
	if(!TreeEntry()->Previous()->FormView()->CheckComplete())
		return FALSE;


	if(GetModel()->FormationTopography()->NrOfFormations()<1)
		return FALSE;

	if(GetModel()->Analysis()->NrOfAnalysisPoints()<1)
		return FALSE;
	
	for(int i=0 ;i< GetModel()->Analysis()->NrOfAnalysisPoints();i++)
	{
		CAnalysisPoint *pAnalysisPoint=GetModel()->Analysis()->GetAnalysisPointAt(i);
		if(!pAnalysisPoint->CheckCompleteProdAndInject())
			return FALSE;
	}

	return TRUE;

}



//##ModelId=3C3040B102FB
void CProductionAndInjectionForm::OnKeydownlstAnalysisPoint(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	*pResult = 1; // 1 ipv 0 to lock use of arrow keys
}


