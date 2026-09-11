// CompletionAndTestingForm.cpp : implementation file
//

#include "stdafx.h"
#include "dsealect.h"
#include "CompletionAndTestingForm.h"
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
// CCompletionAndTestingForm
static BOOL AnalysisPointOk(CAnalysisPoint* point)
{
	return point->CheckCompleteCompAndTest();
		
}

IMPLEMENT_DYNCREATE(CCompletionAndTestingForm, CBaseFormView)

CCompletionAndTestingForm::CUserColumnInfo::CUserColumnInfo()
{
}

CString CCompletionAndTestingForm::CUserColumnInfo::Title() const
{
  CSinglePressure sp;
  CString str;
  str.Format("Ptest [%s]", sp.UnitName(GetDoc()->UnitSystem()).c_str());
  return str;
}

CString CCompletionAndTestingForm::CUserColumnInfo::Text(CAnalysisPoint &analysisPoint) const
{
  CSinglePressure sp(analysisPoint.TestPressure());
  CString str;
  if(!sp.Undefined())
    str.Format("%g", sp.Value(GetDoc()->UnitSystem()));

  return str;
}



//##ModelId=3C0DC15E00FF
CCompletionAndTestingForm::CCompletionAndTestingForm()
	: CBaseFormView(CCompletionAndTestingForm::IDD)
	,m_lstAnalysisPoints(AnalysisPointOk, new CUserColumnInfo)
{
	//{{AFX_DATA_INIT(CCompletionAndTestingForm)
	m_intBalancedCompletion = -1;
	m_chkShutIn = FALSE;
	//}}AFX_DATA_INIT
}

//##ModelId=3C0DC15E01B5
CCompletionAndTestingForm::~CCompletionAndTestingForm()
{
	if(m_toolTip)
		delete m_toolTip;
}

//##ModelId=3C0DC15E01A6
void CCompletionAndTestingForm::DoDataExchange(CDataExchange* pDX)
{
	

	if (!pDX->m_bSaveAndValidate)
	{
		if (GetModel()->Control()->UnderBalancedCompletion())
		{
			m_intBalancedCompletion=1;
		}
		else if(GetModel()->Control()->OverBalancedCompletion())
		{
			m_intBalancedCompletion=2;
		}
		else if (GetModel()->Control()->BalancedCompletion())
		{
			m_intBalancedCompletion=0;
		}
		else 
		{
			ASSERT(FALSE);
		}

		m_chkShutIn	= GetModel()->Control()->ShutIn();
					
	}


	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCompletionAndTestingForm)
	DDX_Control(pDX, IDC_fraAnalysisPoint, m_fraAnalysisPoint);
	DDX_Control(pDX, IDC_lstAnalysisPoints,				m_lstAnalysisPoints);
	DDX_Control(pDX, IDC_cmbWellType,					m_cmbWellType);
	DDX_Control(pDX, IDC_lbPressureDifference,			m_lbPressureDifference);
	DDX_Control(pDX, IDC_ebTimeOfExposToDrilFluid,		m_debTimeOfExposToDrilFluid);
	DDX_Control(pDX, IDC_ebTestPressure,				m_debTestPressure);
  DDX_Control(pDX, IDC_ebFluidPressureGradient, m_debFluidPressureGradient);
	DDX_Control(pDX, IDC_ebPressureDifference,			m_debPressureDifference);
	DDX_Control(pDX, IDC_ebDrillingFluidTemperature,	m_debDrillingFluidTemperature);
	DDX_Radio(pDX, IDC_optBalancedCompletion,			m_intBalancedCompletion);
	DDX_Check(pDX, IDC_chkShutIn, m_chkShutIn);
	//}}AFX_DATA_MAP

	if(!CheckEntry())
	{
		return;
	}

	ValidateQuantity(pDX, &m_debTestPressure,	GetModel()->TestLoads()->TestPressure(), CString("TestPressure"));
  ValidateQuantity(pDX, &m_debFluidPressureGradient, GetModel()->TestLoads()->FluidPressureGradient(), CString("FluidPressureGradient"));

	CAnalysisPoint*	pAnalysisPoint=m_lstAnalysisPoints.GetSelectedAnalysisPoint();
	//m_fraAnalysisPoint.SetFont
	
	if(pAnalysisPoint)
	{
		ValidateQuantity(pDX, &m_debDrillingFluidTemperature,	pAnalysisPoint->PriorToCompletionLoads()->DrillingTemperature(),			CString("Drilling Temperature"));
		ValidateQuantity(pDX, &m_debTimeOfExposToDrilFluid,		pAnalysisPoint->PriorToCompletionLoads()->TimeOfExposureToDrillingFluid(),	CString("TimeOfExposureToDrillingFluid"));
		ValidateQuantity(pDX, &m_debPressureDifference,			pAnalysisPoint->PerforationLoads()->CompletionPressureDiff(),				CString("CompletionPressureDiff"));
		
	}
		

	if (pDX->m_bSaveAndValidate)
	{
		GetModel()->Control()->BalancedCompletion(m_intBalancedCompletion==0);
		GetModel()->Control()->UnderBalancedCompletion(m_intBalancedCompletion==1);
		GetModel()->Control()->OverBalancedCompletion(m_intBalancedCompletion==2);
		GetModel()->Control()->WellType(CControl::eWellType(m_cmbWellType.GetCurSel()+IDS_OILPRODUCER));
		GetModel()->Control()->ShutIn(m_chkShutIn == TRUE);
	}
	else
	{
		// temporary quantities, only used to set right unit-string-label:
		CTemperatureQuantity	tempTQ;
		CTimeDQuantity			tempTDQ;
		CSinglePressure			tempSP;
    CPressureGradient2 tempPG;

		SetUnitlabel(IDC_lbUDrillingFluidTemperature,	&tempTQ);
		SetUnitlabel(IDC_lbUTimeOfExposToDrilFluid,		&tempTDQ);
		SetUnitlabel(IDC_lbUTestPressure,				&tempSP);
		SetUnitlabel(IDC_lbUPressureDifference,			&tempSP);
    SetUnitlabel(IDC_lbUFluidPressureGradient, &tempPG);
		m_cmbWellType.SetCurSel((UINT)GetModel()->Control()->WellType()-IDS_OILPRODUCER);
	}
	
	//BuildAnalysisPointList();
	m_lstAnalysisPoints.Update();
	
	CString strNr; //caption of frame depends on selected analysis point
	long Index=GetDoc()->DSealectModel()->Analysis()->GetAnalysisPointIndex(pAnalysisPoint);
	strNr.Format("%i", Index+1);
	CString strTMD;
	strTMD=m_lstAnalysisPoints.GetItemText(Index,1);
	m_fraAnalysisPoint.SetWindowText(CString("Analysis Point " + strNr + " at TMD= " + strTMD));

	m_fraCompletion.Enable(!m_intBalancedCompletion==0); //disable frame if balanced completion is set
	m_lstAnalysisPoints.EnsureVisible(Index,true);
	DrawView()->Invalidate();
	TreeEntry()->UpdateEnable();
	UpdateButtons();
	
}


BEGIN_MESSAGE_MAP(CCompletionAndTestingForm, CBaseFormView)
	//{{AFX_MSG_MAP(CCompletionAndTestingForm)
	ON_BN_CLICKED(IDC_optBalancedCompletion,		OnoptBalancedCompletion)
	ON_BN_CLICKED(IDC_optOverbalancedCompletion,	OnoptOverbalancedCompletion)
	ON_BN_CLICKED(IDC_optUnderbalancedCompletion,	OnoptUnderbalancedCompletion)
	ON_CBN_CLOSEUP(IDC_cmbWellType, OnCloseupcmbWellType)
	ON_NOTIFY(LVN_KEYDOWN, IDC_lstAnalysisPoints, OnKeydownlstAnalysisPoints)
	ON_BN_CLICKED(IDC_chkShutIn, OnchkShutIn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCompletionAndTestingForm diagnostics

#ifdef _DEBUG
//##ModelId=3C0DC15E01B7
void CCompletionAndTestingForm::AssertValid() const
{
	CFormView::AssertValid();
}

//##ModelId=3C0DC15E01B9
void CCompletionAndTestingForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCompletionAndTestingForm message handlers


//##ModelId=3C0DC15E0197
void CCompletionAndTestingForm::OnInitialUpdate() 
{
	
	GetDlgItem(IDC_fraAnalysisPoint)->SetFont(GetFont(TRUE));
	
	CBaseFormView::OnInitialUpdate();

	static bool bInit=false;
	if(!bInit)
	{
		m_fraCompletion.Set(this, IDC_fraCompletion);
		for(UINT i = IDS_OILPRODUCER;i <= IDS_WATERSOURCE;i++)
		{
			CString strWellName;
			strWellName.LoadString(i);
			m_cmbWellType.AddString(strWellName);
		}
		bInit=true;
	}

	GetDlgItem(IDC_fraBottomHoleSituationPrior)->SetFont(GetFont(TRUE));
	GetDlgItem(IDC_fraPressureTest)->SetFont(GetFont(TRUE));
	GetDlgItem(IDC_fraCompletion)->SetFont(GetFont(TRUE));
	GetDlgItem(IDC_fraShutIn)->SetFont(GetFont(TRUE));

	// TOOLTIP
	static short sInit=0;
	if(!sInit)
	{
		//One time initialization!!
		sInit=1;
		
		m_toolTip.Create(this);
		CRect rect;
		CString sTmp;

		sTmp.LoadString(IDTT_COT_chkShutIn);
		m_toolTip.AddTool( GetDlgItem(IDC_chkShutIn), sTmp);
		GetDlgItem(IDC_chkShutIn)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_COT_cmbWellType);
		m_toolTip.AddTool( GetDlgItem(IDC_cmbWellType), sTmp);
		GetDlgItem(IDC_cmbWellType)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_COT_ebDrillingFluidTemperature);
		m_toolTip.AddTool( GetDlgItem(IDC_ebDrillingFluidTemperature), sTmp);
		GetDlgItem(IDC_ebDrillingFluidTemperature)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_MORE_BottomHolePressure);
		m_toolTip.AddTool( GetDlgItem(IDC_ebTestPressure), sTmp);
		GetDlgItem(IDC_ebTestPressure)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_MORE_Cohesion);
		m_toolTip.AddTool( GetDlgItem(IDC_fraCompletion), sTmp);
		GetDlgItem(IDC_fraCompletion)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_MORE_Cohesion);
		m_toolTip.AddTool( GetDlgItem(IDC_fraCompletionBalance), sTmp);
		GetDlgItem(IDC_fraCompletionBalance)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_COT_fraPressureTest);
		m_toolTip.AddTool( GetDlgItem(IDC_fraPressureTest), sTmp);
		GetDlgItem(IDC_fraPressureTest)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_MORE_AnalysisPoint);
		m_toolTip.AddTool( GetDlgItem(IDC_lstAnalysisPoints), sTmp);
		GetDlgItem(IDC_lstAnalysisPoints)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		//IDTT_COT_chkShutIn
		//IDTT_COT_cmbWellType
		//IDTT_COT_ebDrillingFluidTemperature
		//IDTT_COT_ebTestPressure
		//IDTT_COT_fraCompletion
		//IDTT_COT_fraCompletionBalance
		//IDTT_COT_fraPressureTest
		//IDTT_COT_lstAnalysisPoints

		//IDC_chkShutIn
		//IDC_cmbWellType
		//IDC_ebDrillingFluidTemperature
		//IDC_ebTestPressure
		//IDC_fraCompletion
		//IDC_fraCompletionBalance
		//IDC_fraPressureTest
		//IDC_lstAnalysisPoints

		
		CRect rectMargin(TOOL_TIP_MARGE,TOOL_TIP_MARGE,TOOL_TIP_MARGE,TOOL_TIP_MARGE); //left, top, right, bottom
		m_toolTip.SetMargin(rectMargin);
		m_toolTip.Activate(TRUE);
	}

}

//##ModelId=3C0DC15E0203
void CCompletionAndTestingForm::OnoptBalancedCompletion() 
{
	//m_lbPressureDifference.SetWindowText("Pressure difference between completions fluid and pore pressure");
	UpdateData();	
	UpdateData(FALSE);
	GetDocument()->SetModifiedFlag(TRUE);
}

//##ModelId=3C0DC15E0205
void CCompletionAndTestingForm::OnoptOverbalancedCompletion() 
{
	//m_lbPressureDifference.SetWindowText("Overbalanced pressure");
	UpdateData();	
	UpdateData(FALSE);
	GetDocument()->SetModifiedFlag(TRUE);
}

//##ModelId=3C0DC15E0213
void CCompletionAndTestingForm::OnoptUnderbalancedCompletion() 
{
	//m_lbPressureDifference.SetWindowText("Underbalanced pressure");
	UpdateData();	
	UpdateData(FALSE);
	GetDocument()->SetModifiedFlag(TRUE);
}

//##ModelId=3C0DC15E0222
void CCompletionAndTestingForm::OnCloseupcmbWellType() 
{
	UpdateData();
}



//##ModelId=3C0DC15E0119
BOOL CCompletionAndTestingForm::CheckComplete() const
{	
	if(!TreeEntry()->Previous()->Previous()->FormView()->CheckComplete())
	{
		return FALSE;
	}

	if(GetModel()->FormationTopography()->NrOfFormations()<1)
		return FALSE;

	if(GetModel()->Analysis()->NrOfAnalysisPoints()<1)
		return FALSE;

  if(!GetModel()->TestLoads()->CheckComplete())
    return FALSE;

	for(int i=0 ;i< GetModel()->Analysis()->NrOfAnalysisPoints();i++)
	{
		if(!GetModel()->Analysis()->GetAnalysisPointAt(i)->CheckCompleteCompAndTest())
			return FALSE;
	}

	return TRUE;
}

//##ModelId=3C3040BA0211
void CCompletionAndTestingForm::OnKeydownlstAnalysisPoints(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	*pResult = 1; //0; 1 ipv 0 to lock use of arrow keys
}

//##ModelId=3DDA0C5A01F7
void CCompletionAndTestingForm::OnchkShutIn() 
{
	UpdateData();	
	UpdateData(FALSE);
	GetDocument()->SetModifiedFlag(TRUE);
	
}


