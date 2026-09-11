// CasingStressForm.cpp : implementation file
//

#include "stdafx.h"
#include "dsealect.h"
#include "CasingStressForm.h"
#include "treeobject.h"
#include "DSealectGlobal.h"
#include "CCasingOperation.h"
#include "CDataModel.h"
#include "DSealectDrawView.h"
#include "CFormationTopography.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[] = __FILE__;#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CCasingStressForm

IMPLEMENT_DYNCREATE(CCasingStressForm, CBaseFormView)

//##ModelId=3C0DC15F02FD
CCasingStressForm::CCasingStressForm()
	: CBaseFormView(CCasingStressForm::IDD)
{
	//{{AFX_DATA_INIT(CCasingStressForm)
	m_intAutomatic = -1;
	m_intMud = -1;
	m_intNoWorkString = -1;
	m_intFluidDefinition = 0;
	//}}AFX_DATA_INIT
}

//##ModelId=3C0DC16000AB
CCasingStressForm::~CCasingStressForm()
{
	if(m_toolTip)
		delete m_toolTip;
}

//##ModelId=3C0DC160008C
void CCasingStressForm::DoDataExchange(CDataExchange* pDX)
{
	
	if (!pDX->m_bSaveAndValidate)
	{
		m_intNoWorkString	= GetCasingOperation()->WorkStringUsed()				? 1:0;
		m_intMud			= GetCasingOperation()->NewFluidInCasWSAnnulus()		? 1:0;
		m_intAutomatic		= GetCasingOperation()->AdvancedAxialCasingStresses()	? 1:0;
	}

	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCasingStressForm)
	DDX_Control(pDX, IDC_ebNewFluidDensity,				m_debNewFluidDensity);
	DDX_Control(pDX, IDC_lbNewFluidStressGrad,			m_lbNewFluidStressGrad);
	DDX_Control(pDX, IDC_lbDispFluidStressGradientNWS,	m_lbDispFluidStressGradientNWS);
	DDX_Control(pDX, IDC_lbDispFluidStressGrad,			m_lbDispFluidStressGrad);
	DDX_Control(pDX, IDC_ebDispFluidDensity,			m_debDispFluidDensity);
	DDX_Control(pDX, IDC_ebDispFluidDensityNWS,			m_debDispFluidDensityNWS);
	DDX_Control(pDX, IDC_ebWorkStringPackersTMD,		m_debWorkStringPackersTMD);
	DDX_Control(pDX, IDC_ebNewFluidStressGrad,			m_debNewFluidStressGrad);
	DDX_Control(pDX, IDC_ebNewFluidOverpressure,		m_debNewFluidOverpressure);
	DDX_Control(pDX, IDC_ebNewFluidCompressibility,		m_debNewFluidCompressibility);
	DDX_Control(pDX, IDC_ebHookLoad,					m_debHookLoad);
	DDX_Control(pDX, IDC_ebDispFluidStressGradientNWS,	m_debDispFluidStressGradientNWS);
	DDX_Control(pDX, IDC_ebDispFluidStressGrad,			m_debDispFluidStressGrad);
	DDX_Control(pDX, IDC_ebDispFluidCompressibilityNWS,	m_debDispFluidCompressibilityNWS);
	DDX_Control(pDX, IDC_ebDispFluidCompressibility,	m_debDispFluidCompressibility);
	DDX_Radio(pDX, IDC_optAutomatic,					m_intAutomatic);
	DDX_Radio(pDX, IDC_optMud,							m_intMud);
	DDX_Radio(pDX, IDC_optNoWorkString,					m_intNoWorkString);
	DDX_Radio(pDX, IDC_optFluidPressureGradientDefinition, m_intFluidDefinition);
	//}}AFX_DATA_MAP

	if(!CheckEntry())
		return;
	
	ValidateQuantity(pDX, &m_debNewFluidStressGrad,				GetCasingOperation()->NewFluidStressGradient(),				CString("NewFluidStressGrad"));
	ValidateQuantity(pDX, &m_debNewFluidOverpressure,			GetCasingOperation()->NewFluidOverpressure(),				CString("NewFluidOverpressure"));
	ValidateQuantity(pDX, &m_debNewFluidCompressibility,		GetCasingOperation()->NewFluidCompressibility(),			CString("NewFluidCompressibility"));
	ValidateQuantity(pDX, &m_debHookLoad,						GetCasingOperation()->HookLoad(),							CString("HookLoad"));
	ValidateQuantity(pDX, &m_debDispFluidStressGradientNWS,		GetCasingOperation()->DisplacementFluidStressGradient(),	CString("DispFluidStressGradientNWS"));
	ValidateQuantity(pDX, &m_debDispFluidCompressibilityNWS,	GetCasingOperation()->DispFluidCompressibility(),			CString("DispFluidCompressibilityNWS"));
	ValidateQuantity(pDX, &m_debWorkStringPackersTMD,			GetCasingOperation()->WorkStringPackersTMD(),				CString("WorkStringPackersTMD"));

	ShowQuantity(&m_debDispFluidStressGrad,		GetCasingOperation()->DisplacementFluidStressGradient());
	ShowQuantity(&m_debDispFluidCompressibility,GetCasingOperation()->DispFluidCompressibility());

	if(GetCasingOperation()->DisplacementFluidStressGradient()->Undefined())
	{
		m_debDispFluidDensityNWS.MakeEmpty();
	}
	else
	{
		//calculate FluidDensity from FluidStressGradient
		double dFluidDensity = GetCasingOperation()->DisplacementFluidStressGradient()->Value();
		CDensityQuantity tempDQ;
		tempDQ.Value(dFluidDensity / ACCELERATION_OF_GRAVITY, CDoubleQuantity::SI_UNIT);
		ShowQuantity(&m_debDispFluidDensityNWS,&tempDQ);
		ShowQuantity(&m_debDispFluidDensity,&tempDQ);
	}	
		
	if(GetCasingOperation()->NewFluidStressGradient()->Undefined())
	{
		m_debNewFluidDensity.MakeEmpty();
	}
	else
	{
		//calculate NewFluidDensity from NewFluidStressGradient
		double dNewFluidDensity = GetCasingOperation()->NewFluidStressGradient()->Value();
		CDensityQuantity tempDQ;
		tempDQ.Value(dNewFluidDensity / ACCELERATION_OF_GRAVITY, CDoubleQuantity::SI_UNIT);
		ShowQuantity(&m_debNewFluidDensity,&tempDQ);
	}

	if (pDX->m_bSaveAndValidate)
	{
		//from ctrl to data
		GetCasingOperation()->WorkStringUsed(m_intNoWorkString == 1);
		GetCasingOperation()->NewFluidInCasWSAnnulus(m_intMud == 1);
		GetCasingOperation()->AdvancedAxialCasingStresses(m_intAutomatic == 1);				
	}
	else
	{
		//update controls (from data to ctrls)
		SetUnitlabel(IDC_lbUNewFluidOverpressure,			GetCasingOperation()->NewFluidOverpressure());
		SetUnitlabel(IDC_lbUNewFluidCompressibility,		GetCasingOperation()->NewFluidCompressibility());
		SetUnitlabel(IDC_lbUHookLoad,						GetCasingOperation()->HookLoad());
		SetUnitlabel(IDC_lbUDispFluidCompressibilityNWS,	GetCasingOperation()->DispFluidCompressibility());
		SetUnitlabel(IDC_lbUDispFluidCompressibility,		GetCasingOperation()->DispFluidCompressibility());
		SetUnitlabel(IDC_lbUWorkStringPackersTMD,			GetCasingOperation()->WorkStringPackersTMD());

		if(m_intFluidDefinition == 0) //stress gradient definition
		{
			SetUnitlabel(IDC_lbUNewFluidStressGrad,				GetCasingOperation()->NewFluidStressGradient());
			SetUnitlabel(IDC_lbUDispFluidStressGradientNWS,		GetCasingOperation()->DisplacementFluidStressGradient());
			SetUnitlabel(IDC_lbUDispFluidStressGrad,			GetCasingOperation()->DisplacementFluidStressGradient());
			m_lbDispFluidStressGrad.SetWindowText("Disp.fluid stress gradient");
			m_lbDispFluidStressGradientNWS.SetWindowText("Disp.fluid stress gradient");
			m_lbNewFluidStressGrad.SetWindowText("New fluid stress grad.");

		}
		else //density definition
		{
			CDensityQuantity tempDQ;
			SetUnitlabel(IDC_lbUDispFluidStressGrad, &tempDQ);
			SetUnitlabel(IDC_lbUDispFluidStressGradientNWS, &tempDQ);
			SetUnitlabel(IDC_lbUNewFluidStressGrad, &tempDQ);
			m_lbDispFluidStressGrad.SetWindowText("Disp.fluid density");
			m_lbDispFluidStressGradientNWS.SetWindowText("Disp.fluid density");
			m_lbNewFluidStressGrad.SetWindowText("New fluid density");
			
		}
		m_debDispFluidDensity.ShowWindow(m_intFluidDefinition);
		m_debDispFluidDensityNWS.ShowWindow(m_intFluidDefinition);
		m_debNewFluidDensity.ShowWindow(m_intFluidDefinition);
		m_debDispFluidStressGrad.ShowWindow(!m_intFluidDefinition);
		m_debDispFluidStressGradientNWS.ShowWindow(!m_intFluidDefinition);
		m_debNewFluidStressGrad.ShowWindow(!m_intFluidDefinition);
	}

	if(!GetCasingOperation()->WorkStringUsed())
	{
		m_debWorkStringPackersTMD.MakeEmpty(); // make empty to avoid inconsistency in dependencies manager
		GetCasingOperation()->WorkStringPackersTMD()->Invalidate();
	}

	m_optAdvanced.Enable(m_intAutomatic == 1);						//dis-/enable advanced frame
	m_optNoWorkString.Enable(m_intNoWorkString == 0);				//dis-/enable no-work-string frame
	m_optWorkStringUsed.Enable(m_intNoWorkString == 1);				//dis-/enable work-string-used frame
	m_optNewFluid.Enable(m_intMud == 1 && m_intNoWorkString == 1);	//dis-/enable new fluid frame

	DrawView()->Invalidate();
	TreeEntry()->UpdateEnable();
    UpdateButtons();

}

BEGIN_MESSAGE_MAP(CCasingStressForm, CBaseFormView)
	//{{AFX_MSG_MAP(CCasingStressForm)
	ON_BN_CLICKED(IDC_optAdvanced, OnoptAdvanced)
	ON_BN_CLICKED(IDC_optAutomatic, OnoptAutomatic)
	ON_BN_CLICKED(IDC_optMud, OnoptMud)
	ON_BN_CLICKED(IDC_optNewFluid, OnoptNewFluid)
	ON_BN_CLICKED(IDC_optNoWorkString, OnoptNoWorkString)
	ON_BN_CLICKED(IDC_optWorkStringUsed, OnoptWorkStringUsed)
	ON_BN_CLICKED(IDC_optFluidDensityDefinition, OnoptFluidDensityDefinition)
	ON_BN_CLICKED(IDC_optFluidPressureGradientDefinition, OnoptFluidPressureGradientDefinition)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCasingStressForm diagnostics

#ifdef _DEBUG
//##ModelId=3C0DC16000AD
void CCasingStressForm::AssertValid() const
{
	CFormView::AssertValid();
}

//##ModelId=3C0DC16000AF
void CCasingStressForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCasingStressForm message handlers

//##ModelId=3C0DC160007D
void CCasingStressForm::OnInitialUpdate() 
{
	m_optAdvanced.Set(this, IDC_fraAdvanced);
	m_optNoWorkString.Set(this, IDC_fraNoWorkString);
	m_optWorkStringUsed.Set(this, IDC_fraWorkStringUsed);
	m_optNewFluid.Set(this, IDC_fraNewFluid);

	CBaseFormView::OnInitialUpdate();

	GetDlgItem(IDC_fraAxialStresses)->SetFont(GetFont(TRUE));
	GetDlgItem(IDC_optAutomatic)->SetFont(GetFont(TRUE));
	GetDlgItem(IDC_optAdvanced)->SetFont(GetFont(TRUE));
	GetDlgItem(IDC_fraCrossSectionalStresses)->SetFont(GetFont(TRUE));
	GetDlgItem(IDC_optNoWorkString)->SetFont(GetFont(TRUE));
	GetDlgItem(IDC_optWorkStringUsed)->SetFont(GetFont(TRUE));
	GetDlgItem(IDC_optMud)->SetFont(GetFont(TRUE));
	GetDlgItem(IDC_optNewFluid)->SetFont(GetFont(TRUE));
	GetDlgItem(IDC_lbFluidInCasingWorkStringA)->SetFont(GetFont(TRUE));

	// TOOLTIP
	static short bInit = 0;
	if(!bInit)
	{
		//One time initialization!!
		bInit = 1;
		
		m_toolTip.Create(this);
		CRect rect;
		CString sTmp;

		sTmp.LoadString(IDTT_MORE_DisplacementFluid);
		m_toolTip.AddTool( GetDlgItem(IDC_ebDispFluidCompressibility), sTmp);
		GetDlgItem(IDC_ebDispFluidCompressibility)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_MORE_DisplacementFluid);
		m_toolTip.AddTool( GetDlgItem(IDC_ebDispFluidCompressibilityNWS), sTmp);
		GetDlgItem(IDC_ebDispFluidCompressibilityNWS)->GetWindowRect(&rect);	
		ScreenToClient(rect);
		
		sTmp.LoadString(IDTT_MORE_DisplacementFluid);
		m_toolTip.AddTool( GetDlgItem(IDC_ebDispFluidStressGrad), sTmp);
		GetDlgItem(IDC_ebDispFluidStressGrad)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_MORE_DisplacementFluid);
		m_toolTip.AddTool( GetDlgItem(IDC_ebDispFluidStressGradientNWS), sTmp);
		GetDlgItem(IDC_ebDispFluidStressGradientNWS)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_CAS_ebHookLoad);
		m_toolTip.AddTool( GetDlgItem(IDC_ebHookLoad), sTmp);
		GetDlgItem(IDC_ebHookLoad)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_CAS_fraAxialStresses);
		m_toolTip.AddTool( GetDlgItem(IDC_fraAxialStresses), sTmp);
		GetDlgItem(IDC_fraAxialStresses)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_CAS_fraNewFluid);
		m_toolTip.AddTool( GetDlgItem(IDC_fraNewFluid), sTmp);
		GetDlgItem(IDC_fraNewFluid)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		//IDTT_CAS_ebDispFluidCompressibility
		//IDTT_CAS_ebDispFluidCompressibilityNWS
		//IDTT_CAS_ebDispFluidStressGrad
		//IDTT_CAS_ebDispFluidStressGradientNWS
		//IDTT_CAS_ebHookLoad
		//IDTT_CAS_fraAxialStresses
		//IDTT_CAS_fraNewFluid

		//IDC_ebDispFluidCompressibility
		//IDC_ebDispFluidCompressibilityNWS
		//IDC_ebDispFluidStressGrad
		//IDC_ebDispFluidStressGradientNWS
		//IDC_ebHookLoad
		//IDC_fraAxialStresses
		//IDC_fraNewFluid

		
		CRect rectMargin(TOOL_TIP_MARGE,TOOL_TIP_MARGE,TOOL_TIP_MARGE,TOOL_TIP_MARGE); //left, top, right, bottom
		m_toolTip.SetMargin(rectMargin);
		m_toolTip.Activate(TRUE);
	}
	
}

//##ModelId=3C3040BD0146
CCasingOperation* CCasingStressForm::GetCasingOperation()  const
{
	return GetModel()->DrillingScheme()->GetDrillingOperation(0)->CasingOperation();
}

//##ModelId=3C0DC16000FE
void CCasingStressForm::OnoptAdvanced() 
{
	UpdateData();	
	UpdateData(FALSE);
	GetDocument()->SetModifiedFlag(TRUE);

}

//##ModelId=3C0DC1600100
void CCasingStressForm::OnoptAutomatic() 
{
	UpdateData();	
	UpdateData(FALSE);
	GetDocument()->SetModifiedFlag(TRUE);

}

//##ModelId=3C0DC160010A
void CCasingStressForm::OnoptMud() 
{
	UpdateData();	
	UpdateData(FALSE);
	GetDocument()->SetModifiedFlag(TRUE);
	
}

//##ModelId=3C0DC160010C
void CCasingStressForm::OnoptNewFluid() 
{
	UpdateData();	
	UpdateData(FALSE);
	GetDocument()->SetModifiedFlag(TRUE);

}

//##ModelId=3C0DC160010E
void CCasingStressForm::OnoptNoWorkString() 
{
	UpdateData();	
	UpdateData(FALSE);
	GetDocument()->SetModifiedFlag(TRUE);

}

//##ModelId=3C0DC160011A
void CCasingStressForm::OnoptWorkStringUsed() 
{
	UpdateData();	
	UpdateData(FALSE);
	GetDocument()->SetModifiedFlag(TRUE);

}

//##ModelId=3C0DC15F034B
BOOL CCasingStressForm::CheckComplete() const
{
	if(!TreeEntry()->Previous()->FormView()->CheckComplete())
	{
		return FALSE;
	}

	if(GetModel()->FormationTopography()->NrOfFormations()<1)
		return FALSE;
	
	if (GetCasingOperation()->AdvancedAxialCasingStresses()) // Advanced-option
	{
		if( GetCasingOperation()->HookLoad()->Undefined())
			return FALSE;
	}

	if(!GetCasingOperation()->CheckCompleteCasingStresses())
		return FALSE;
		
	return TRUE;
}

//##ModelId=3CA011AB01F3
LRESULT CCasingStressForm::OnValidateDoubleEdit(WPARAM id, LPARAM lparam)
{
	m_iControlToValidate=id;

	if(IDC_ebDispFluidStressGrad == m_iControlToValidate)
	{
		if(m_debDispFluidStressGrad.IsEmpty())
			m_debDispFluidStressGradientNWS.MakeEmpty();
		else //copy value to other editbox which is the same in the datamodel
			m_debDispFluidStressGradientNWS.ShowValue(m_debDispFluidStressGrad.GetValue());
		
		UpdateData();
		UpdateData(FALSE); // extra update needed to get old value back when copied value out of range
		m_iControlToValidate = 0;
		return TRUE;
	}

	if(IDC_ebDispFluidDensity == m_iControlToValidate)
	{
		if(m_debDispFluidDensity.IsEmpty())
			m_debDispFluidDensityNWS.MakeEmpty();
		else //copy value to other editbox which is the same in the datamodel
			m_debDispFluidDensityNWS.ShowValue(m_debDispFluidDensity.GetValue());

		//// calculate stress gradient from density
		if(m_debDispFluidDensityNWS.IsEmpty())
		{
			m_debDispFluidStressGradientNWS.MakeEmpty();
			UpdateData();
			return TRUE;
		}

		CDensityQuantity tempDQ;
		tempDQ.Value(m_debDispFluidDensityNWS.GetValue(),GetDoc()->UnitSystem());
		
		double dStressGrad = tempDQ.Value() * ACCELERATION_OF_GRAVITY;
		CStressGradientQuantity tempSGQ;
		tempSGQ.Value(dStressGrad, CDoubleQuantity::SI_UNIT);

		m_debDispFluidStressGradientNWS.ShowValue(tempSGQ.Value(GetDoc()->UnitSystem()));

		UpdateData();
		UpdateData(FALSE); // extra update needed to get old value back when copied value out of range
		m_iControlToValidate = 0;
		return TRUE;
	}

	if(IDC_ebDispFluidCompressibility == m_iControlToValidate)
	{
		if(m_debDispFluidCompressibility.IsEmpty())
			m_debDispFluidCompressibilityNWS.MakeEmpty();
		else //copy value to other editbox which is the same in the datamodel
			m_debDispFluidCompressibilityNWS.ShowValue(m_debDispFluidCompressibility.GetValue());

		UpdateData();
		UpdateData(FALSE); // extra update needed to get old value back when copied value out of range
		m_iControlToValidate = 0;
		return TRUE;
	}

	if(IDC_ebDispFluidDensityNWS == m_iControlToValidate)
	{
		if(m_debDispFluidDensityNWS.IsEmpty())
		{
			m_debDispFluidStressGradientNWS.MakeEmpty();
			UpdateData();
			return TRUE;
		}

		CDensityQuantity tempDQ;
		tempDQ.Value(m_debDispFluidDensityNWS.GetValue(),GetDoc()->UnitSystem());
		
		double dStressGrad = tempDQ.Value() * ACCELERATION_OF_GRAVITY;
		CStressGradientQuantity tempSGQ;
		tempSGQ.Value(dStressGrad, CDoubleQuantity::SI_UNIT);

		m_debDispFluidStressGradientNWS.ShowValue(tempSGQ.Value(GetDoc()->UnitSystem()));

		UpdateData();
		UpdateData(FALSE); // extra update needed to get old value back when copied value out of range
		m_iControlToValidate = 0;
		return TRUE;
	}

	if(IDC_ebNewFluidDensity == m_iControlToValidate)
	{
		//calculate stress gradient from density
		if(m_debNewFluidDensity.IsEmpty())
		{
			m_debNewFluidStressGrad.MakeEmpty();
			UpdateData();
			return TRUE;
		}

		CDensityQuantity tempDQ;
		tempDQ.Value(m_debNewFluidDensity.GetValue(),GetDoc()->UnitSystem());
		
		double dStressGrad = tempDQ.Value() * ACCELERATION_OF_GRAVITY;
		CStressGradientQuantity tempSGQ;
		tempSGQ.Value(dStressGrad, CDoubleQuantity::SI_UNIT);

		m_debNewFluidStressGrad.ShowValue(tempSGQ.Value(GetDoc()->UnitSystem()));

		UpdateData();
		UpdateData(FALSE); // extra update needed to get old value back when copied value out of range
		m_iControlToValidate = 0;
		return TRUE;
	}

	UpdateData();
	m_iControlToValidate = 0;
	return TRUE;
}

//##ModelId=3DDA0C5D013B
void CCasingStressForm::OnoptFluidDensityDefinition() 
{
	UpdateData();	
	UpdateData(FALSE);
	GetDocument()->SetModifiedFlag(TRUE);
	
}

//##ModelId=3DDA0C5D013D
void CCasingStressForm::OnoptFluidPressureGradientDefinition() 
{
	UpdateData();	
	UpdateData(FALSE);
	GetDocument()->SetModifiedFlag(TRUE);
	
}
