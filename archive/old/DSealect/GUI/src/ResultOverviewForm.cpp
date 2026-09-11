// ResultOverviewForm.cpp : implementation file
//

#include "stdafx.h"
#include "dsealect.h"
#include "treeobject.h"
#include "DSealectGlobal.h"
#include "ResultOverviewDraw.h"
#include "ResultOverviewForm.h"
#include "CDataModel.h"
#include "CAnalysis.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[] = __FILE__;#endif  // _MSC_VER
#endif

#define SEALANT		0
#define FORMATION	1
#define CASING		2

//static const char* WellComponent[]=
//{
//	"sealant",
//	"formation",
//	"casing"
//};

/////////////////////////////////////////////////////////////////////////////
// CResultOverviewForm

IMPLEMENT_DYNCREATE(CResultOverviewForm, CBaseFormView)

//##ModelId=3CA0119D032C
CResultOverviewForm::CResultOverviewForm()
	: CBaseFormView(CResultOverviewForm::IDD)
{
	//{{AFX_DATA_INIT(CResultOverviewForm)
	m_iPresentation = -1;
	//}}AFX_DATA_INIT
}

//##ModelId=3CA0119D033D
CResultOverviewForm::~CResultOverviewForm()
{
	if(m_toolTip)
		delete m_toolTip;
}

//##ModelId=3DDA0C4F00B1
void CResultOverviewForm::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	if(CheckEntry() && bActivate)
  {
    SendToDrawView();
		DrawView()->Invalidate();
  }
}

//##ModelId=3CA0119D0331
void CResultOverviewForm::DoDataExchange(CDataExchange* pDX)
{
	

	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CResultOverviewForm)
	DDX_Control(pDX, IDC_cmbDepthAlongWell, m_cmbDepthAlongWell);
	DDX_Control(pDX, IDC_cmbWellComponent, m_cmbWellComponent);
	DDX_Radio(pDX, IDC_optDamageStatusOverviewTable, m_iPresentation);
	//}}AFX_DATA_MAP

	FillDepthCombo();
	FillWellCompCombo();
	SendToDrawView();
	DrawView()->Invalidate();
}


BEGIN_MESSAGE_MAP(CResultOverviewForm, CBaseFormView)
	//{{AFX_MSG_MAP(CResultOverviewForm)
	ON_BN_CLICKED(IDC_optDamageStatusOverviewTable, OnoptDamageStatusOverviewTable)
	ON_BN_CLICKED(IDC_optRiskOfDamageBarChart, OnoptRiskOfDamageBarChart)
	ON_CBN_CLOSEUP(IDC_cmbWellComponent, OnCloseupcmbWellComponent)
	ON_CBN_CLOSEUP(IDC_cmbDepthAlongWell, OnCloseupcmbDepthAlongWell)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResultOverviewForm diagnostics

#ifdef _DEBUG
//##ModelId=3CA0119D033F
void CResultOverviewForm::AssertValid() const
{
	CFormView::AssertValid();
}

//##ModelId=3CA0119D0341
void CResultOverviewForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

//##ModelId=3CA0119D032D
BOOL CResultOverviewForm::CheckComplete() const
{
	return TRUE;
}

//##ModelId=3CA0119D032F
BOOL CResultOverviewForm::CheckEntry() const
{
	

	if(GetDoc()->IsModified())
		return FALSE;

	if(!GetModel()->BatchTable()->HasResults()) 
		return FALSE;

	return TRUE;
}


////////////////////////////////////////////////////////////
// CResultOverviewForm message handlers

//##ModelId=3CA0119D034C
void CResultOverviewForm::OnoptDamageStatusOverviewTable() 
{
	m_iPresentation = 0;
	UpdateData();
}

//##ModelId=3CA0119D035B
void CResultOverviewForm::OnoptRiskOfDamageBarChart() 
{
	m_iPresentation = 1;
	UpdateData();
}

//##ModelId=3DDA0C4F00AF
void CResultOverviewForm::OnInitialUpdate() 
{
	CBaseFormView::OnInitialUpdate();

	m_iPresentation = 0;
	m_iCurrentWellCompItem = -1;
	m_pCurrentAP = 0;
	if(CheckEntry())
	{
		UpdateData(FALSE);
	}

	// TOOLTIP
	static short bInit=0;
	if(!bInit)
	{
		//One time initialization!!
		bInit=1;
		
		m_toolTip.Create(this);
		CRect rect;
		CString sTmp;

		sTmp.LoadString(IDTT_REO_optDamageStatusOverviewTable);
		m_toolTip.AddTool( GetDlgItem(IDC_optDamageStatusOverviewTable), sTmp);
		GetDlgItem(IDC_optDamageStatusOverviewTable)->GetWindowRect(&rect);	
		ScreenToClient(rect);
		
		sTmp.LoadString(IDTT_REO_optRiskOfDamageBarChart);
		m_toolTip.AddTool( GetDlgItem(IDC_optRiskOfDamageBarChart), sTmp);
		GetDlgItem(IDC_optRiskOfDamageBarChart)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		//IDTT_REO_optDamageStatusOverviewTable
		//IDTT_REO_optRiskOfDamageBarChart

		//IDC_optDamageStatusOverviewTable
		//IDC_optRiskOfDamageBarChart

		CRect rectMargin(TOOL_TIP_MARGE,TOOL_TIP_MARGE,TOOL_TIP_MARGE,TOOL_TIP_MARGE); //left, top, right, bottom
		m_toolTip.SetMargin(rectMargin);
		m_toolTip.Activate(TRUE);
	}

		
}

//##ModelId=3DDA0C4F00CF
void CResultOverviewForm::OnCloseupcmbWellComponent() 
{
	SendToDrawView();
	UpdateData();
}

//##ModelId=3DDA0C4F00DF
void CResultOverviewForm::OnCloseupcmbDepthAlongWell() 
{
	SendToDrawView();
	UpdateData();
}


//##ModelId=3DDA0C4F00EE
void CResultOverviewForm::FillDepthCombo()
{
	CDSealectDoc* pDoc=(CDSealectDoc*)GetDoc();
	QU::UNIT us = pDoc->UnitSystem();
	CSinglePressure tmpSPQ;
	CString sUnit;

	CLengthQuantity tmpLQ;
	CString sTMD;
	
	m_cmbDepthAlongWell.ResetContent();

	for(int i = 0 ; i < GetDoc()->DSealectModel()->Analysis()->NrOfAnalysisPoints() ; i++)
	{
		tmpLQ.Value(GetDoc()->DSealectModel()->Analysis()->GetAnalysisPointAt(i)->TMD()->Value(),CDoubleQuantity::SI_UNIT);
		sTMD.Format("%#1.0f",tmpLQ.Value(us));
		m_cmbDepthAlongWell.AddString(sTMD);
	}
	
	if(m_pCurrentAP)
	{
		int iIndex = GetDoc()->DSealectModel()->Analysis()->GetAnalysisPointIndex(m_pCurrentAP);
		m_cmbDepthAlongWell.SetCurSel(iIndex);
	}
	else
	{
		m_cmbDepthAlongWell.SetCurSel(0); //first Analysis Point
	}

	
}

//##ModelId=3DDA0C4F010D
void CResultOverviewForm::SendToDrawView()
{
	if(m_cmbDepthAlongWell.GetCurSel() == -1)
  {
    if(m_cmbDepthAlongWell.GetCount() == 0)
    {
    	((CResultOverviewDraw*)DrawView())->SetAnalysisPoint(0);
      m_pCurrentAP = 0;
      return;
    }

    m_cmbDepthAlongWell.SetCurSel(0);
  }

	CAnalysisPoint* pAP; 
	pAP = GetDoc()->DSealectModel()->Analysis()->GetAnalysisPointAt(m_cmbDepthAlongWell.GetCurSel());
	m_pCurrentAP = pAP;
	((CResultOverviewDraw*)DrawView())->SetAnalysisPoint(pAP);

	
	if(m_cmbWellComponent.GetCurSel() == -1)
		return;

	((CResultOverviewDraw*)DrawView())->SetWellComponent(m_cmbWellComponent.GetCurSel()); 
	m_iCurrentWellCompItem = m_cmbWellComponent.GetCurSel();

	if(m_iPresentation == -1)
		return;

	((CResultOverviewDraw*)DrawView())->SetPresentationDraw(m_iPresentation) ;

}

//##ModelId=3DDA0C4F00ED
void CResultOverviewForm::FillWellCompCombo()
{
	m_cmbWellComponent.ResetContent();
	m_cmbWellComponent.AddString(WellComponent[SEALANT]);
	if(!GetDoc()->DSealectModel()->Control()->OutputLevel() == 0)
	{
		m_cmbWellComponent.AddString(WellComponent[FORMATION]);
		m_cmbWellComponent.AddString(WellComponent[CASING]);
		if(m_iCurrentWellCompItem >= 0)
		{
			m_cmbWellComponent.SetCurSel(m_iCurrentWellCompItem);
			return;
		}
	}
	m_cmbWellComponent.SetCurSel(SEALANT);

	
	
}

