// CasingMaterialDlg.cpp: implementation of the CCasingMaterialDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dsealect.h"
#include "DoubleEdit.h"
#include "CasingMaterial.h"
#include "DDXQuantity.h"
#include "CheckFrm.h"
#include "DSealectGlobal.h"
#include "BaseFormView.h"
#include "CasingMatListCtrl.h"
#include "CasingMaterialDlg.h"
#include "CDataModel.h"

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif


//##ModelId=3C60D57E037B
CCasingMaterialDlg::CCasingMaterialDlg( CCasingMaterial& material,CDataModel* pDataModel,BOOL bDependencyWithModel, CWnd* pParent /*=NULL*/)
	: CDialog(CCasingMaterialDlg::IDD, pParent)
{
	m_pDataModel=pDataModel;
	m_Material = material;
	m_pDep1 = new CDep1(&m_Material);

	m_iControlToValidate=0;
	m_pDataModel->DependenciesManager.AddDependency(m_pDep1);

	m_bDependencyWithModel=bDependencyWithModel;
	m_pCasingOuter=m_pDataModel->GetBoreholeDep()->GetCasingOuter();

	if(m_bDependencyWithModel)
		m_pDataModel->GetBoreholeDep()->SetCasingOuter(m_Material.CasingOuterDiameter());


	//{{AFX_DATA_INIT(CCasingMaterialDlg)
	m_sMatName = _T("");
	m_bPlasticity = FALSE;
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

//##ModelId=3C8D9E26030F
CCasingMaterialDlg::~CCasingMaterialDlg()
{
	GetDoc()->DSealectModel()->DependenciesManager.RemoveDependency(m_pDep1);	
	delete m_pDep1;
}

//##ModelId=3C60D57E038C
CCasingMaterial CCasingMaterialDlg::Material()
{
	return m_Material;
}

//##ModelId=3C60D57F00DB
void CCasingMaterialDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCasingMaterialDlg)
	DDX_Control(pDX, IDC_ebCasingType, m_CasingType);
	DDX_Control(pDX, IDC_ebCasingOuterDiameter, m_dbCasingOuterDiameter);
	DDX_Control(pDX, IDC_ebCasingInnerDiameter, m_CasingInnerDiameter);
	DDX_Control(pDX, IDC_ebYoungsModulus, m_dbYoungsModulus);
	DDX_Control(pDX, IDC_ebPoissonRatio, m_dbPoissonRatio);
	DDX_Control(pDX, IDC_ebVolumetricSpecificHeat, m_dbVolumetricSpecificHeat);
	DDX_Control(pDX, IDC_ebThermalConductivity, m_dbThermalConductivity);
	DDX_Control(pDX, IDC_ebThermalExpansion, m_dbThermalExpansion);
	DDX_Control(pDX, IDC_ebWeightPerUnitLength, m_edWeightPerUnitLength);
	DDX_Control(pDX, IDC_chkPlasticity, m_chkPlasticity);
	DDX_Control(pDX, IDC_ebYieldStrength, m_dbYieldStrength);
	DDX_Text(pDX,IDC_ebCasingType, m_sMatName);
	DDX_Check(pDX, IDC_chkPlasticity, m_bPlasticity);
	//}}AFX_DATA_MAP

	CDependenciesManager& DM = GetDoc()->DSealectModel()->DependenciesManager;
	DDV_Quantity(pDX,IDC_ebCasingOuterDiameter,m_Material.CasingOuterDiameter(),&DM,"CasingOuterDiameter");	
	DDV_Quantity(pDX,IDC_ebCasingInnerDiameter,m_Material.CasingInnerDiameter(),&DM,"CasingInnerDiameter");	
	DDV_Quantity(pDX,IDC_ebYoungsModulus,m_Material.YoungsModulus(),&DM,"Youngs Modulus");	
	DDV_Quantity(pDX,IDC_ebPoissonRatio,m_Material.PoissonRatio(),&DM,"Poissons ratio");	
	DDV_Quantity(pDX,IDC_ebVolumetricSpecificHeat,m_Material.VolumetricSpecificHeat(),&DM,"Volumetric specific heat");	
	DDV_Quantity(pDX,IDC_ebThermalExpansion,m_Material.ThermalExpansion(),&DM,"Thermal Expansion");	
	DDV_Quantity(pDX,IDC_ebThermalConductivity,m_Material.ThermalConductivity(),&DM,"Thermal Conductivity");	
	DDV_Quantity(pDX,IDC_ebWeightPerUnitLength,m_Material.WeightPerUnitLength(),&DM,"WeightPerUnitLength");	
	DDV_Quantity(pDX,IDC_ebYieldStrength,m_Material.YieldStrength(),&DM,"YieldStrength");	

	if (pDX->m_bSaveAndValidate)
	{
		m_Material.Plasticity(m_bPlasticity==TRUE);
	}
	else
	{
		m_bPlasticity=m_Material.Plasticity();
		DDX_Check(pDX, IDC_chkPlasticity, m_bPlasticity);
	}

	m_fraPlasticity.Enable(m_Material.Plasticity());
	m_Material.Name(m_sMatName);
	GetDlgItem(IDOK)->EnableWindow(m_Material.CheckComplete());

}


BEGIN_MESSAGE_MAP(CCasingMaterialDlg, CDialog)
	//{{AFX_MSG_MAP(CCasingMaterialDlg)
	ON_EN_KILLFOCUS(IDC_ebCasingType, OnKillfocusName)
	ON_BN_CLICKED(IDC_chkPlasticity, OnchkPlasticity)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_VALIDATE_DOUBLE_EDIT, OnValidateDoubleEdit)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCasingMaterialDlg message handlers

//##ModelId=3C60D57F00EB
void CCasingMaterialDlg::OnKillfocusName() 
{
	UpdateData();
}

//##ModelId=3C6A14CD0359
void CCasingMaterialDlg::OnchkPlasticity() 
{
	UpdateData();
}



//##ModelId=3C6A14CD035B
BOOL CCasingMaterialDlg::OnInitDialog()
{
	m_sMatName=m_Material.Name();
	m_fraPlasticity.Set(this,IDC_fraPlasticity);
	//GetDlgItem(IDC_fraPlasticity)->SetFont(CBaseFormView::GetFont(TRUE));	

	CDialog::OnInitDialog();

	SetUnitlabel(IDC_lbUCasingOuterDiameter,m_Material.CasingOuterDiameter());
	SetUnitlabel(IDC_lbUCasingInnerDiameter,m_Material.CasingInnerDiameter());
	SetUnitlabel(IDC_lbUYoungsModulus,m_Material.YoungsModulus());
	SetUnitlabel(IDC_lbUPoissonRatio,m_Material.PoissonRatio());
	SetUnitlabel(IDC_lbUVolumetricSpecificHeat,m_Material.VolumetricSpecificHeat());
	SetUnitlabel(IDC_lbUThermalConductivity,m_Material.ThermalConductivity());
	SetUnitlabel(IDC_lbUThermalExpansion,m_Material.ThermalExpansion());
	SetUnitlabel(IDC_lbUWeightPerUnitLength,m_Material.WeightPerUnitLength());
	SetUnitlabel(IDC_lbUYieldStrength,m_Material.YieldStrength());


	// TOOLTIP
	static short bInit=0;
	if(!bInit)
	{
		//One time initialization!!
		bInit=1;
		
		m_toolTip.Create(this);
		CRect rect;
		CString sTmp;

		sTmp.LoadString(IDTT_MORE_Elasticity);
		m_toolTip.AddTool( GetDlgItem(IDC_ebPoissonRatio), sTmp);
		GetDlgItem(IDC_ebPoissonRatio)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_MORE_ThermalConductivity);
		m_toolTip.AddTool( GetDlgItem(IDC_ebThermalConductivity), sTmp);
		GetDlgItem(IDC_ebThermalConductivity)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_MORE_ThermalExpansion);
		m_toolTip.AddTool( GetDlgItem(IDC_ebThermalExpansion), sTmp);
		GetDlgItem(IDC_ebThermalExpansion)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_MORE_VolumetricSpecificHeat);
		m_toolTip.AddTool( GetDlgItem(IDC_ebVolumetricSpecificHeat), sTmp);
		GetDlgItem(IDC_ebVolumetricSpecificHeat)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_MORE_Elasticity);
		m_toolTip.AddTool( GetDlgItem(IDC_ebYoungsModulus), sTmp);
		GetDlgItem(IDC_ebYoungsModulus)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		//IDTT_CAL_ebPoissonRatio
		//IDTT_CAL_ebThermalConductivity
		//IDTT_CAL_ebThermalExpansion
		//IDTT_CAL_ebVolumetricSpecificHeat
		//IDTT_CAL_ebYoungsModulus
		
		CRect rectMargin(TOOL_TIP_MARGE,TOOL_TIP_MARGE,TOOL_TIP_MARGE,TOOL_TIP_MARGE); //left, top, right, bottom
		m_toolTip.SetMargin(rectMargin);
		m_toolTip.Activate(TRUE);
	}

	return TRUE;
}

//##ModelId=3C6A14CD02FB
BOOL CCasingMaterialDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message==WM_KEYDOWN)
	{
		CButton* pOK=(CButton*)GetDlgItem(IDOK);
		CButton* pCNL=(CButton*)GetDlgItem(IDCANCEL);

		if(GetFocus() == pOK || GetFocus() == pCNL)
		{
		}
		else
		{
			if(pMsg->wParam==VK_RETURN)
				pMsg->wParam=VK_TAB;
		}
	}

	if(GetDoc()->m_bShowToolTip)
		m_toolTip.RelayEvent(pMsg);

	return CDialog::PreTranslateMessage(pMsg);

}

//##ModelId=3C8D9E2503CA
void CCasingMaterialDlg::SetUnitlabel(int nID,CDoubleQuantity* pQ)
{
	CStatic *pUnitLabel=0;
	pUnitLabel = (CStatic*) GetDlgItem(nID);
	ASSERT(pUnitLabel!=0);
	::SetUnitlabel(pUnitLabel,pQ);
}

//##ModelId=3CA011AE0242
LRESULT CCasingMaterialDlg::OnValidateDoubleEdit(WPARAM id, LPARAM lparam)
{
	m_iControlToValidate=id;
	UpdateData();
	m_iControlToValidate=0;
	return TRUE;
}


//##ModelId=3C8D9E270206
CCasingMaterialDlg::CDep1::CDep1(CCasingMaterial* pCM)
{
	
	m_qVec.push_back(pCM->CasingInnerDiameter());
	m_qVec.push_back(pCM->CasingOuterDiameter());

}


//##ModelId=3C8D9E270216
bool CCasingMaterialDlg::CDep1::CheckQuantity(const CDoubleQuantity* pQuantity) const 
{
		if (RemainingsUndefined(pQuantity))
		return TRUE;

	//THE RULE !!!
	return m_qVec[0]->Value() < m_qVec[1]->Value();
}


//##ModelId=3C8D9E270219
std::string CCasingMaterialDlg::CDep1::GetDependencyString() const
{
	return  "Casing Inner Diameter < Casing Outer Diameter";
}



