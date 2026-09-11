// DetailedResultsForm.cpp: implementation of the CDetailedResultsForm class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dsealect.h"
#include "treeobject.h"
#include "DSealectGlobal.h"
#include "DetailedResultsDraw.h"
#include "DetailedResultsForm.h"
#include "CDataModel.h"
#include "CAnalysis.h"
#include "CFormationTopography.h"

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define RISK_OF_DAMAGE	0
#define DAMAGE_LEVEL	1

#define SHEAR_DETERIORATION_IN_FORMATION	0
#define PLASTIC_DEFORMATION_IN_CASING		1
#define DEBONDING_AT_FORMATION				2
#define DEBONDING_AT_CASING					3
#define SHEAR_DETERIORATION_IN_CEMENT		4
#define RADIAL_CRACKS_IN_CEMENT				5
// 0->5 : same order as DSResults

#define EVOLUTION_OVER_LLT	0
#define DISTRIBUTION_OVER_S	1

static const char* ResultType[]=
{
	"risk of damage bar chart",
	"damage level overview table"
};

/*
static const char* TypeOfDamage[]=
{
	"shear deterioration in formation",
	"plastic deformation in casing",
	"debonding at formation",
	"debonding at casing",
	"shear deterioration in cement",
	"radial cracks in cement"
};
*/

static const char* PlotSpace[]=
{
	"evolution over load level + time",
	"distribution over space"
};


IMPLEMENT_DYNCREATE(CDetailedResultsForm, CBaseFormView)

//##ModelId=3DDA0C580294
CDetailedResultsForm::CDetailedResultsForm()
	: CBaseFormView(CDetailedResultsForm::IDD)
{
	//{{AFX_DATA_INIT(CDetailedResultsForm)
	m_iPresent = -1;
	//}}AFX_DATA_INIT
}

//##ModelId=3DDA0C580320
CDetailedResultsForm::~CDetailedResultsForm()
{
	if(m_toolTip)
		delete m_toolTip;
}

//##ModelId=3DDA0C580310
void CDetailedResultsForm::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	if(CheckEntry() && bActivate)
  {
    SendToDrawView();
		DrawView()->Invalidate();
  }
}

//##ModelId=3DDA0C580305
void CDetailedResultsForm::DoDataExchange(CDataExchange* pDX)
{
	if(!CheckEntry())
		return;


	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDetailedResultsForm)
	DDX_Control(pDX, IDC_cmbResultType, m_cmbResultType);
	DDX_Control(pDX, IDC_cmbTypeOfDamage, m_cmbTypeOfDamage);
	DDX_Control(pDX, IDC_cmbPlotSpace, m_cmbPlotSpace);
	DDX_Control(pDX, IDC_cmbCementScenario, m_cmbCementScenario);
	DDX_Control(pDX, IDC_cmbDepthAlongWell, m_cmbDepthAlongWell);
	DDX_Radio(pDX, IDC_optPresentResultAgainstLoadDamageTypes, m_iPresent);
	//}}AFX_DATA_MAP

	FillDepthCombo();
	FillCemScenarioCombo();
	FillResultTypeCombo();
	FillTypeOfDamageCombo();
	FillPlotSpaceCombo();

	m_fraCompareResults.Enable(m_iPresent == 0);
	m_fraViewResults.Enable(m_iPresent == 1);
	GetDlgItem(IDC_ebLoadType)->EnableWindow(FALSE); // always disabled

	SendToDrawView();
	DrawView()->Invalidate();
}


BEGIN_MESSAGE_MAP(CDetailedResultsForm, CBaseFormView)
	//{{AFX_MSG_MAP(CDetailedResultsForm)
	ON_CBN_CLOSEUP(IDC_cmbResultType, OnCloseupcmbResultType)
	ON_CBN_CLOSEUP(IDC_cmbDepthAlongWell, OnCloseupcmbDepthAlongWell)
	ON_CBN_CLOSEUP(IDC_cmbCementScenario, OnCloseupcmbCementScenario)
	ON_CBN_CLOSEUP(IDC_cmbPlotSpace, OnCloseupcmbPlotSpace)
	ON_CBN_CLOSEUP(IDC_cmbTypeOfDamage, OnCloseupcmbTypeOfDamage)
	ON_BN_CLICKED(IDC_optPresentResultAgainstLoadDamageTypes, OnoptPresentResultAgainstLoadDamageTypes)
	ON_BN_CLICKED(IDC_optPresentResultAgainstSpaceTime, OnoptPresentResultAgainstSpaceTime)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResultOverviewForm diagnostics

#ifdef _DEBUG
//##ModelId=3DDA0C580322
void CDetailedResultsForm::AssertValid() const
{
	CFormView::AssertValid();
}

//##ModelId=3DDA0C580324
void CDetailedResultsForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

//##ModelId=3DDA0C5802F3
BOOL CDetailedResultsForm::CheckComplete() const
{
	return TRUE;
}

//##ModelId=3DDA0C580301
BOOL CDetailedResultsForm::CheckEntry() const
{
	
	if(GetDoc()->IsModified())
		return FALSE;

	if(!GetModel()->BatchTable()->HasResults()) 
		return FALSE;

	return TRUE;
}

////////////////////////////////////////////////////////////
// CDetailedResultsForm message handlers

//##ModelId=3DDA0C580303
void CDetailedResultsForm::OnInitialUpdate() 
{
	m_fraCompareResults.Set(this, IDC_fraCompareResults);
	m_fraViewResults.Set(this, IDC_fraViewResults);

	m_iPresent = 0;
	m_iCurrentResultTypeItem = -1;
	m_iCurrentPlotSpace = -1;
	m_iCurrentScenario = -1;
	m_iCurrentTypeOfDamage = -1;
	m_pCurrentAP = 0;
	m_pCurrentBatchTableRow = 0;
	if(CheckEntry())
	{
		UpdateData(FALSE);
	}

	CBaseFormView::OnInitialUpdate();

	// TOOLTIP
	static short bInit=0;
	if(!bInit)
	{
		//One time initialization!!
		bInit=1;
		
		m_toolTip.Create(this);
		CRect rect;
		CString sTmp;

		sTmp.LoadString(IDTT_DER_cmbPlotSpace);
		m_toolTip.AddTool( GetDlgItem(IDC_cmbPlotSpace), sTmp);
		GetDlgItem(IDC_cmbPlotSpace)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		//sTmp.LoadString(IDTT_DER_cmbResultType);
		//m_toolTip.AddTool( GetDlgItem(IDC_cmbResultType), sTmp);
		//GetDlgItem(IDC_cmbResultType)->GetWindowRect(&rect);	
		//ScreenToClient(rect);

		sTmp.LoadString(IDTT_DER_cmbTypeOfDamage);
		m_toolTip.AddTool( GetDlgItem(IDC_cmbTypeOfDamage), sTmp);
		GetDlgItem(IDC_cmbTypeOfDamage)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		//IDC_cmbPlotSpace
		//IDC_cmbResultType
		//IDC_cmbTypeOfDamage

		//IDTT_DER_cmbResultType
		//IDTT_DER_cmbTypeOfDamage
		
		CRect rectMargin(TOOL_TIP_MARGE,TOOL_TIP_MARGE,TOOL_TIP_MARGE,TOOL_TIP_MARGE); //left, top, right, bottom
		m_toolTip.SetMargin(rectMargin);
		m_toolTip.Activate(TRUE);
	}
		
}

//##ModelId=3DDA0C58032F
void CDetailedResultsForm::OnCloseupcmbResultType() 
{
	SendToDrawView();
	UpdateData();
}

//##ModelId=3DDA0C580331
void CDetailedResultsForm::OnCloseupcmbDepthAlongWell() 
{
	SendToDrawView();
	UpdateData();
	
}


//##ModelId=3DDA0C58036E
void CDetailedResultsForm::FillDepthCombo()
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

//##ModelId=3DDA0C580392
void CDetailedResultsForm::SendToDrawView()
{
	((CDetailedResultsDraw*)DrawView())->m_iPresent = m_iPresent;

	if(m_cmbDepthAlongWell.GetCurSel() == -1)
  {
    if(m_cmbDepthAlongWell.GetCount() == 0)
    {
    	((CDetailedResultsDraw*)DrawView())->SetAnalysisPoint(0);
      m_pCurrentAP = 0;
		  return;
    }

    m_cmbDepthAlongWell.SetCurSel(0);
  }

	CAnalysisPoint* pAP; 
	pAP = GetDoc()->DSealectModel()->Analysis()->GetAnalysisPointAt(m_cmbDepthAlongWell.GetCurSel());
	m_pCurrentAP = pAP;
	((CDetailedResultsDraw*)DrawView())->SetAnalysisPoint(pAP);

	
	if(m_cmbCementScenario.GetCurSel() == -1)
		return;
	m_iCurrentScenario = m_cmbCementScenario.GetCurSel();
	((CDetailedResultsDraw*)DrawView())->m_iScenario = m_iCurrentScenario;
	
	FillCorrespondingCemMat();
	((CDetailedResultsDraw*)DrawView())->m_pBatchTableRow = m_pCurrentBatchTableRow;


	if(m_cmbResultType.GetCurSel() == -1)
		return;
	m_iCurrentResultTypeItem = m_cmbResultType.GetCurSel();
	((CDetailedResultsDraw*)DrawView())->m_iResultType =  m_iCurrentResultTypeItem; 

	if(m_cmbTypeOfDamage.GetCurSel() == -1)
		return;
	m_iCurrentTypeOfDamage = GetCurrentDamageType(m_cmbTypeOfDamage.GetCurSel());
	((CDetailedResultsDraw*)DrawView())->m_iTypeOfDamage =  m_iCurrentTypeOfDamage; 

	FillLoadType();

	if(m_cmbPlotSpace.GetCurSel() == -1)
		return;
	m_iCurrentPlotSpace = m_cmbPlotSpace.GetCurSel();
	((CDetailedResultsDraw*)DrawView())->m_iPlotSpace =  m_iCurrentPlotSpace; 

}

//##ModelId=3DDA0C580370
void CDetailedResultsForm::FillResultTypeCombo()
{
	
	m_cmbResultType.ResetContent();
	m_cmbResultType.AddString(ResultType[RISK_OF_DAMAGE]);
	m_cmbResultType.AddString(ResultType[DAMAGE_LEVEL]);
	if(m_iCurrentResultTypeItem >= 0)
	{
		m_cmbResultType.SetCurSel(m_iCurrentResultTypeItem);
		return;
	}
	m_cmbResultType.SetCurSel(RISK_OF_DAMAGE);
		
}

//##ModelId=3DDA0C58036F
void CDetailedResultsForm::FillCemScenarioCombo()
{
	CString sInt;
	int NrScenarios = GetDoc()->DSealectModel()->DrillingScheme()->GetDrillingOperation(0)->CementScenariosTable()->NumberOfCementScenarios();
	m_cmbCementScenario.ResetContent();
	for(int i = 1 ; i <= NrScenarios ; i++)
	{
		sInt.Format("%#i",i);
		m_cmbCementScenario.AddString(sInt);
	}
	
	if(m_iCurrentScenario >= 0)
	{
		m_cmbCementScenario.SetCurSel(m_iCurrentScenario);
		return;
	}
	m_cmbCementScenario.SetCurSel(0);
		
}
//##ModelId=3DDA0C580371
void CDetailedResultsForm::FillTypeOfDamageCombo()
{
	if(!m_pCurrentAP)
		return;
	if(m_iCurrentScenario == -1)
		return;

	//CBatchTableRow* pRow = GetDoc()->DSealectModel()->BatchTable()->GetRow(m_pCurrentAP, m_iCurrentScenario);

  assert(GetDoc()->DSealectModel()->TMDtoTVD(m_pCurrentAP->TMD()->Value()).second);
	double dDepth = GetDoc()->DSealectModel()->TMDtoTVD(m_pCurrentAP->TMD()->Value()).first ;
	CString name = GetDoc()->DSealectModel()->FormationTopography()->GetFormationAtDepth(dDepth)->MaterialName();
	CFormationMaterial* pFM = GetDoc()->DSealectModel()->FormationMatList()->GetMaterial(name);
	
	name = GetDoc()->DSealectModel()->DrillingScheme()->GetDrillingOperation(0)->CasingOperation()->CasingMaterialName();
	CCasingMaterial* pCM = GetDoc()->DSealectModel()->CasingMatList()->GetMaterial(name);
	
	m_cmbTypeOfDamage.ResetContent();
	for(int i = 0 ; i <= RADIAL_CRACKS_IN_CEMENT ; i++) // for all damage types
	{
		if(i == SHEAR_DETERIORATION_IN_FORMATION)
		{	
			if(pFM->Plasticity() && GetDoc()->DSealectModel()->Control()->OutputLevel() == 2) 
				m_cmbTypeOfDamage.AddString(TypeOfDamage[i]); // only if FormationPlasticity = true && Outputlevel = 3
		}
		else if(i == PLASTIC_DEFORMATION_IN_CASING)
		{
			if(pCM->Plasticity() && GetDoc()->DSealectModel()->Control()->OutputLevel() == 2)
				m_cmbTypeOfDamage.AddString(TypeOfDamage[i]); // only if CasingMaterialPlasticity = true && Outputlevel = 3
		}
		else
		{
			m_cmbTypeOfDamage.AddString(TypeOfDamage[i]);
		}
	}
	
	if(m_iCurrentTypeOfDamage >= 0)
	{
		m_cmbTypeOfDamage.SetCurSel(GetCurrentComboSelection(m_iCurrentTypeOfDamage));
		return;
	}
	m_cmbTypeOfDamage.SetCurSel(0);
		
}

//##ModelId=3DDA0C580372
void CDetailedResultsForm::FillPlotSpaceCombo()
{
	m_cmbPlotSpace.ResetContent();
	m_cmbPlotSpace.AddString(PlotSpace[EVOLUTION_OVER_LLT]);
	m_cmbPlotSpace.AddString(PlotSpace[DISTRIBUTION_OVER_S]);
	if(m_iCurrentPlotSpace >= 0)
	{
		m_cmbPlotSpace.SetCurSel(m_iCurrentPlotSpace);
		return;
	}
	m_cmbPlotSpace.SetCurSel(EVOLUTION_OVER_LLT);
		
}

//##ModelId=3DDA0C58037D
void CDetailedResultsForm::FillCorrespondingCemMat()
{
	if(m_cmbDepthAlongWell.GetCurSel() == -1)
		return;

	if(m_cmbCementScenario.GetCurSel() == -1)
		return;

	CBatchTableRow* pRow = GetDoc()->DSealectModel()->BatchTable()->GetRow(m_pCurrentAP, m_iCurrentScenario);
	GetDlgItem(IDC_ebCorrespondingCementMaterial)->SetWindowText(pRow->CementMaterial()->Name());
	m_pCurrentBatchTableRow = pRow;
	
}

//##ModelId=3DDA0C58037E
void CDetailedResultsForm::FillLoadType()
{
	if(m_cmbTypeOfDamage.GetCurSel() == -1)
		return;

	CBatchTableRow* pRow = GetDoc()->DSealectModel()->BatchTable()->GetRow(m_pCurrentAP, m_iCurrentScenario);
	
	int LoadPhase = pRow->DSResult()->Space()->GetSpaceTable(m_iCurrentTypeOfDamage)->GetFirstLoadPhaseItemIndex();
	
	if(LoadPhase>=0)
	{
		GetDlgItem(IDC_ebLoadType)->SetWindowText(LoadPhaseNames[LoadPhase]);
	}
	else
	{
		GetDlgItem(IDC_ebLoadType)->SetWindowText("");
	}
}

//##ModelId=3DDA0C580333
void CDetailedResultsForm::OnCloseupcmbCementScenario() 
{
	SendToDrawView();
	UpdateData();
		
}

//##ModelId=3DDA0C580335
void CDetailedResultsForm::OnCloseupcmbPlotSpace() 
{
	SendToDrawView();
	UpdateData();
	
}

//##ModelId=3DDA0C580337
void CDetailedResultsForm::OnCloseupcmbTypeOfDamage() 
{
	SendToDrawView();
	UpdateData();
	
}

//##ModelId=3DDA0C58033F
void CDetailedResultsForm::OnoptPresentResultAgainstLoadDamageTypes() 
{
	UpdateData();
	UpdateData(FALSE);
	
}

//##ModelId=3DDA0C580341
void CDetailedResultsForm::OnoptPresentResultAgainstSpaceTime() 
{
	UpdateData();
	UpdateData(FALSE);
	
}

//##ModelId=3DDA0C580345
int CDetailedResultsForm::GetCurrentComboSelection(int iCurrentDamageType)
{
	CString sText;
	int i = 0;
	for (; i < m_cmbTypeOfDamage.GetCount() ; i++)
	{
		m_cmbTypeOfDamage.SetCurSel(i);
		m_cmbTypeOfDamage.GetWindowText(sText);
		if(sText == TypeOfDamage[iCurrentDamageType])
			return i;
	}
	return i;
}

//##ModelId=3DDA0C580343
int CDetailedResultsForm::GetCurrentDamageType(int iCurrentComboSelection)
{
	CString sText;
	int i = 0;
	for(; i < RADIAL_CRACKS_IN_CEMENT ; i++)
	{
		
		m_cmbTypeOfDamage.GetWindowText(sText);
		if(sText == TypeOfDamage[i])
			return i;
	}
	return i;
}


