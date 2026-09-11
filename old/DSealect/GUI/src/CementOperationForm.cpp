// CementOperationForm.cpp : implementation file
//

#include "stdafx.h"
#include "dsealect.h"
#include "CementOperationForm.h"
#include "treeobject.h"
#include "DSealectGlobal.h"
#include "CCementingOperation.h"
#include "CDataModel.h"
#include "CFormationTopography.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[] = __FILE__;#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CCementOperationForm

IMPLEMENT_DYNCREATE(CCementOperationForm, CBaseFormView)

//##ModelId=3C0DC15F004E
CCementOperationForm::CCementOperationForm()
	: CBaseFormView(CCementOperationForm::IDD)
{
	bOptBtnIsAlreadyClicked = FALSE;
	//{{AFX_DATA_INIT(CCementOperationForm)
	m_intFluidDefinition = 0;
	//}}AFX_DATA_INIT
}

//##ModelId=3C0DC15F0128
CCementOperationForm::~CCementOperationForm()
{
	if(m_toolTip)
		delete m_toolTip;
}

//##ModelId=3C0DC15F011B
void CCementOperationForm::DoDataExchange(CDataExchange* pDX)
{

	if(!pDX->m_bSaveAndValidate)
	{
		//update gui
		m_intLeadTailCements= GetCementingOperation()->LeadTailCements() ? 0:1;
	}

			
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCementOperationForm)
	DDX_Control(pDX, IDC_lbUTailCementStressGrad,	m_lbUTailCementStressGrad);
	DDX_Control(pDX, IDC_lbULeadCementStressGrad,	m_lbULeadCementStressGrad);
	DDX_Control(pDX, IDC_lbUCementStressGradient,	m_lbUCementStressGradient);
	DDX_Control(pDX, IDC_lbTailCementStressGrad,	m_lbTailCementStressGrad);
	DDX_Control(pDX, IDC_lbLeadCementStressGrad,	m_lbLeadCementStressGrad);
	DDX_Control(pDX, IDC_lbCementStressGradient,	m_lbCementStressGradient);
	DDX_Control(pDX, IDC_ebLeadCementDensity,		m_debLeadCementDensity);
	DDX_Control(pDX, IDC_ebTailCementDensity,		m_debTailCementDensity);
	DDX_Control(pDX, IDC_ebCementDensity,			m_debCementDensity);
	DDX_Control(pDX, IDC_ebTopOfTailCementTMD,		m_debTopOfTailCementTMD);
	DDX_Control(pDX, IDC_ebTopOfCementTVD,			m_debTopOfCementTVD);
	DDX_Control(pDX, IDC_ebTopOfCementTMD,			m_debTopOfCementsTMD);
	DDX_Control(pDX, IDC_ebTailCementStressGrad,	m_debTailCementStressGrad);
	DDX_Control(pDX, IDC_ebPressureFromFluidsAbove,	m_debPressureFromFluidsAbove);
	DDX_Control(pDX, IDC_ebLeadCementStressGrad,	m_debLeadCementStressGrad);
	DDX_Control(pDX, IDC_ebCementStressGradient,	m_debCementStressGradient);
	DDX_Radio(pDX, IDC_optFluidPressureGradientDefinition, m_intFluidDefinition);
	DDX_Radio(pDX, IDC_optLeadTailCementsYes,		m_intLeadTailCements);
	//}}AFX_DATA_MAP

	if(!CheckEntry())
		return;

	ValidateQuantity(pDX, &m_debTopOfTailCementTMD,			GetCementingOperation()->TopOfTailCement(),				CString("TopOfTailCementTMD"));
	ValidateQuantity(pDX, &m_debTopOfCementsTMD,			GetCementingOperation()->TopOfCementTMD(),				CString("TopOfCementsTMD"));
	ValidateQuantity(pDX, &m_debTailCementStressGrad,		GetCementingOperation()->TailCementStressGradient(),	CString("TailCementStressGrad"));
	ValidateQuantity(pDX, &m_debPressureFromFluidsAbove,	GetCementingOperation()->PressureFromFluidsAbove(),		CString("PressureFromFluidsAbove"));
	ValidateQuantity(pDX, &m_debLeadCementStressGrad,		GetCementingOperation()->LeadCementStressGradient(),	CString("LeadCementStressGrad"));
	
	ShowQuantity(&m_debCementStressGradient, GetCementingOperation()->LeadCementStressGradient());

	if(GetCementingOperation()->LeadCementStressGradient()->Undefined())
	{
		m_debLeadCementDensity.MakeEmpty();
	}
	else
	{
		//calculate lead Density from lead Stress gradient
		double dDensity = GetCementingOperation()->LeadCementStressGradient()->Value();
		CDensityQuantity tempDQ;
		tempDQ.Value(dDensity / ACCELERATION_OF_GRAVITY, CDoubleQuantity::SI_UNIT);
		ShowQuantity(&m_debLeadCementDensity, &tempDQ);
		ShowQuantity(&m_debCementDensity, &tempDQ);
	}

	if(GetCementingOperation()->TailCementStressGradient()->Undefined())
	{
		m_debTailCementDensity.MakeEmpty();
	}
	else
	{
		//calculate tail Density from tail Stress gradient
		double dDensity = GetCementingOperation()->TailCementStressGradient()->Value();
		CDensityQuantity tempDQ;
		tempDQ.Value(dDensity / ACCELERATION_OF_GRAVITY, CDoubleQuantity::SI_UNIT);
		ShowQuantity(&m_debTailCementDensity, &tempDQ);
	}

	//calculate TVD from TMD
	if(GetCementingOperation()->TopOfCementTMD()->Undefined())
	{
		m_debTopOfCementTVD.MakeEmpty();
	}
	else
	{
    std::pair<double, bool> prTVD =
      GetDoc()->DSealectModel()->TMDtoTVD(GetCementingOperation()->TopOfCementTMD()->Value());
		CLengthQuantity tempLQ;
    if(prTVD.second)
		  tempLQ.Value(prTVD.first, CDoubleQuantity::SI_UNIT);
		//if(tempLQ.Value()< GetDoc()->DSealectModel()->FormationTopography()->Overburden()->OverburdenBottomTVD()->Value())
		//{
		//	tempLQ.Invalidate();
		//}
		ShowQuantity(&m_debTopOfCementTVD,&tempLQ);
	}

	
	if (pDX->m_bSaveAndValidate)
	{
		//uadate data
		GetCementingOperation()->LeadTailCements(m_intLeadTailCements == 0 ? TRUE : FALSE);
	}
	else
	{
		//update gui
		SetUnitlabel(IDC_lbUTopOfTailCementTMD,			GetCementingOperation()->TopOfTailCement()); 
		SetUnitlabel(IDC_lbUTopOfCementsTVD,			GetCementingOperation()->TopOfCementTMD()); //use unitstring only
		SetUnitlabel(IDC_lbUTopOfCementsTMD,			GetCementingOperation()->TopOfCementTMD());
		SetUnitlabel(IDC_lbUPressureFromFluidsAbove,	GetCementingOperation()->PressureFromFluidsAbove());
		
		if(m_intFluidDefinition == 0) //stress gradient definition
		{
			SetUnitlabel(IDC_lbUCementStressGradient,		GetCementingOperation()->LeadCementStressGradient());
			SetUnitlabel(IDC_lbUTailCementStressGrad,		GetCementingOperation()->TailCementStressGradient());
			SetUnitlabel(IDC_lbULeadCementStressGrad,		GetCementingOperation()->LeadCementStressGradient());
			m_lbCementStressGradient.SetWindowText("Cement pressure gradient");
			m_lbLeadCementStressGrad.SetWindowText("Lead cement pressure grad");
			m_lbTailCementStressGrad.SetWindowText("Tail cement pressure grad");
		}
		else //density definition
		{
			CDensityQuantity tempDQ;
			SetUnitlabel(IDC_lbUCementStressGradient,&tempDQ);
			SetUnitlabel(IDC_lbUTailCementStressGrad,&tempDQ);
			SetUnitlabel(IDC_lbULeadCementStressGrad,&tempDQ);
			m_lbCementStressGradient.SetWindowText("Cement density");
			m_lbLeadCementStressGrad.SetWindowText("Lead cement density");
			m_lbTailCementStressGrad.SetWindowText("Tail cement density");
		}
		m_debCementDensity.ShowWindow(m_intFluidDefinition);
		m_debLeadCementDensity.ShowWindow(m_intFluidDefinition);
		m_debTailCementDensity.ShowWindow(m_intFluidDefinition);
		m_debCementStressGradient.ShowWindow(!m_intFluidDefinition);
		m_debLeadCementStressGrad.ShowWindow(!m_intFluidDefinition);
		m_debTailCementStressGrad.ShowWindow(!m_intFluidDefinition);
	}

	if(!GetCementingOperation()->LeadTailCements())
	{
		m_debTopOfTailCementTMD.MakeEmpty(); // make empty to avoid inconsistency in dependencies manager
		GetCementingOperation()->TopOfTailCement()->Invalidate();
	}

	m_optLeadTailCementsNo.Enable(!GetCementingOperation()->LeadTailCements()); //dis-/enable lead/tail cem NO frame
	m_optLeadTailCementsYes.Enable(GetCementingOperation()->LeadTailCements()); //dis-/enable lead/tail cem YES frame
	
	DrawView()->Invalidate();
	TreeEntry()->UpdateEnable();
    UpdateButtons();
}


BEGIN_MESSAGE_MAP(CCementOperationForm, CBaseFormView)
	//{{AFX_MSG_MAP(CCementOperationForm)
	ON_BN_CLICKED(IDC_optLeadTailCementsNo,			OnoptLeadTailCementsNo)
	ON_BN_CLICKED(IDC_optLeadTailCementsYes,		OnoptLeadTailCementsYes)
	ON_BN_CLICKED(IDC_optFluidDensityDefinition,	OnoptFluidDensityDefinition)
	ON_BN_CLICKED(IDC_optFluidPressureGradientDefinition, OnoptFluidPressureGradientDefinition)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCementOperationForm diagnostics

#ifdef _DEBUG
//##ModelId=3C0DC15F012A
void CCementOperationForm::AssertValid() const
{
	CBaseFormView::AssertValid();
}

//##ModelId=3C0DC15F0138
void CCementOperationForm::Dump(CDumpContext& dc) const
{
	CBaseFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCementOperationForm message handlers


//##ModelId=3C0DC15F003F
CCementingOperation* CCementOperationForm::GetCementingOperation()
{
	return GetModel()->DrillingScheme()->GetDrillingOperation(0)->CementingOperation();
}

//##ModelId=3C0DC15F0177
void CCementOperationForm::OnoptLeadTailCementsNo() 
{
	if(m_intLeadTailCements == 1) //no model change
		return;

	if(bOptBtnIsAlreadyClicked) //to avoid multiple warningmessages
		return;

	bOptBtnIsAlreadyClicked = TRUE;

	if(GetDoc()->ContinueAndLoseResultsWarning())
	{
		UpdateData(); //continue and lose results
		UpdateData(FALSE);
		GetDocument()->SetModifiedFlag(TRUE);
	}
	else
	{
		UpdateData(FALSE); //restore model data
	}

	bOptBtnIsAlreadyClicked = FALSE;
}
//##ModelId=3C0DC15F0179
void CCementOperationForm::OnoptLeadTailCementsYes() 
{
	if(m_intLeadTailCements == 0) //no model change
		return;

	if(bOptBtnIsAlreadyClicked) //to avoid multiple warningmessages
		return;

	bOptBtnIsAlreadyClicked = TRUE;

	if(GetDoc()->ContinueAndLoseResultsWarning())
	{
		UpdateData(); //continue and lose results
		UpdateData(FALSE);
		GetDocument()->SetModifiedFlag(TRUE);
	}
	else
	{
		UpdateData(FALSE); //restore model data
	}

	bOptBtnIsAlreadyClicked = FALSE;
}

//##ModelId=3C0DC15F0119
void CCementOperationForm::OnInitialUpdate() 
{
	m_intLeadTailCements=0;
	m_optLeadTailCementsNo.Set(this, IDC_fraLeadTailCementsNo);
	m_optLeadTailCementsYes.Set(this, IDC_fraLeadTailCementsYes);

	CBaseFormView::OnInitialUpdate();

	GetDlgItem(IDC_fraCementWeightAndPlacement)->SetFont(GetFont(TRUE));
	GetDlgItem(IDC_optLeadTailCementsYes)->SetFont(GetFont(TRUE));
	GetDlgItem(IDC_optLeadTailCementsNo)->SetFont(GetFont(TRUE));

	// TOOLTIP
	static short bInit=0;
	if(!bInit)
	{
		//One time initialization!!
		bInit=1;
		
		m_toolTip.Create(this);
		CRect rect;
		CString sTmp;

		sTmp.LoadString(IDTT_CEO_ebLeadCementStressGrad);
		m_toolTip.AddTool( GetDlgItem(IDC_ebLeadCementStressGrad), sTmp);
		GetDlgItem(IDC_ebLeadCementStressGrad)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_CEO_ebTailCementStressGrad);
		m_toolTip.AddTool( GetDlgItem(IDC_ebTailCementStressGrad), sTmp);
		GetDlgItem(IDC_ebTailCementStressGrad)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		//IDC_ebLeadCementStressGrad
		//IDC_ebTailCementStressGrad

		//IDTT_CEO_ebLeadCementStressGrad
		//IDTT_CEO_ebTailCementStressGrad

		
		CRect rectMargin(TOOL_TIP_MARGE,TOOL_TIP_MARGE,TOOL_TIP_MARGE,TOOL_TIP_MARGE); //left, top, right, bottom
		m_toolTip.SetMargin(rectMargin);
		m_toolTip.Activate(TRUE);
	}
}

//##ModelId=3C0DC15F006D
BOOL CCementOperationForm::CheckComplete() const
{
	if(!TreeEntry()->Previous()->FormView()->CheckComplete())
	{
		return FALSE;
	}

	if(GetModel()->FormationTopography()->NrOfFormations()<1)
		return FALSE;

	if(!GetModel()->DrillingScheme()->GetDrillingOperation(0)->CementingOperation()->CheckComplete())
    return FALSE;

  if(!GetModel()->DrillingScheme()->GetDrillingOperation(0)->CementingOperation()->TopOfTailCement()->Undefined() && 
     (GetModel()->DrillingScheme()->GetDrillingOperation(0)->CementingOperation()->TopOfTailCement()->Value() >=
      GetModel()->DrillingScheme()->GetDrillingOperation(0)->CasingOperation()->GuideShoeTMD()->Value()))
    return FALSE;

  if(!GetModel()->DrillingScheme()->GetDrillingOperation(0)->CementingOperation()->TopOfCementTMD()->Undefined() &&
     (GetModel()->DrillingScheme()->GetDrillingOperation(0)->CementingOperation()->TopOfCementTMD()->Value() >=
      GetModel()->DrillingScheme()->GetDrillingOperation(0)->CasingOperation()->GuideShoeTMD()->Value()))
    return FALSE;

  return TRUE;
}

//##ModelId=3CA011A803A9
LRESULT CCementOperationForm::OnValidateDoubleEdit(WPARAM id, LPARAM lparam)
{
	m_iControlToValidate=id;

	if(IDC_ebCementStressGradient == m_iControlToValidate)
	{
		if(m_debCementStressGradient.IsEmpty())
			m_debLeadCementStressGrad.MakeEmpty();
		else //copy value to other editbox which is the same in the datamodel
			m_debLeadCementStressGrad.ShowValue(m_debCementStressGradient.GetValue());
		
		UpdateData();
		UpdateData(FALSE); // extra update needed to get old value back when copied value out of range
		m_iControlToValidate = 0;
		return TRUE;
	}

	if(IDC_ebCementDensity == m_iControlToValidate)
	{
		if(m_debCementDensity.IsEmpty())
			m_debLeadCementDensity.MakeEmpty();
		else //copy value to other editbox which is the same in the datamodel
			m_debLeadCementDensity.ShowValue(m_debCementDensity.GetValue());

		//calculate stress gradient from density
		if(m_debLeadCementDensity.IsEmpty())
		{
			m_debLeadCementStressGrad.MakeEmpty();
			UpdateData();
			return TRUE;
		}

		CDensityQuantity tempDQ;
		tempDQ.Value(m_debLeadCementDensity.GetValue(),GetDoc()->UnitSystem());
		
		double dStressGrad = tempDQ.Value() * ACCELERATION_OF_GRAVITY;
		CStressGradientQuantity tempSGQ;
		tempSGQ.Value(dStressGrad, CDoubleQuantity::SI_UNIT);

		m_debLeadCementStressGrad.ShowValue(tempSGQ.Value(GetDoc()->UnitSystem()));

		UpdateData();
		UpdateData(FALSE); // extra update needed to get old value back when copied value out of range
		m_iControlToValidate = 0;
		return TRUE;
	}

	if(IDC_ebTailCementDensity == m_iControlToValidate)
	{
		//calculate stress gradient from density
		if(m_debTailCementDensity.IsEmpty())
		{
			m_debTailCementStressGrad.MakeEmpty();
			UpdateData();
			return TRUE;
		}

		CDensityQuantity tempDQ;
		tempDQ.Value(m_debTailCementDensity.GetValue(),GetDoc()->UnitSystem());
		
		double dStressGrad = tempDQ.Value() * ACCELERATION_OF_GRAVITY;
		CStressGradientQuantity tempSGQ;
		tempSGQ.Value(dStressGrad, CDoubleQuantity::SI_UNIT);

		m_debTailCementStressGrad.ShowValue(tempSGQ.Value(GetDoc()->UnitSystem()));

		UpdateData();
		UpdateData(FALSE); // extra update needed to get old value back when copied value out of range
		m_iControlToValidate = 0;
		return TRUE;
	}

	if(IDC_ebLeadCementDensity == m_iControlToValidate)
	{
		//calculate stress gradient from density
		if(m_debLeadCementDensity.IsEmpty())
		{
			m_debLeadCementStressGrad.MakeEmpty();
			UpdateData();
			return TRUE;
		}

		CDensityQuantity tempDQ;
		tempDQ.Value(m_debLeadCementDensity.GetValue(),GetDoc()->UnitSystem());
		
		double dStressGrad = tempDQ.Value() * ACCELERATION_OF_GRAVITY;
		CStressGradientQuantity tempSGQ;
		tempSGQ.Value(dStressGrad, CDoubleQuantity::SI_UNIT);

		m_debLeadCementStressGrad.ShowValue(tempSGQ.Value(GetDoc()->UnitSystem()));

		UpdateData();
		UpdateData(FALSE); // extra update needed to get old value back when copied value out of range
		m_iControlToValidate = 0;
		return TRUE;
	}

	UpdateData();
	m_iControlToValidate = 0;
	return TRUE;
}

//##ModelId=3DDA0C5B0255
void CCementOperationForm::OnoptFluidDensityDefinition() 
{
	UpdateData();
	UpdateData(FALSE);
	GetDocument()->SetModifiedFlag(TRUE);
}

//##ModelId=3DDA0C5B0265
void CCementOperationForm::OnoptFluidPressureGradientDefinition() 
{
	UpdateData();
	UpdateData(FALSE);
	GetDocument()->SetModifiedFlag(TRUE);
}
