// FormationMaterialDlg.cpp : implementation file
//

#include "stdafx.h"
#include "dsealect.h"
#include "DoubleEdit.h"
#include "FormationMaterial.h"
#include "DDXQuantity.h"
#include "CheckFrm.h"
#include "DSealectGlobal.h"
#include "FormationMaterialDlg.h"
#include "CDataModel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[] = __FILE__;#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CFormationMaterialDlg dialog


//##ModelId=3C60D57503C9
CFormationMaterialDlg::CFormationMaterialDlg(const CFormationMaterial& material,CWnd* pParent /*=NULL*/)
	: CDialog(CFormationMaterialDlg::IDD, pParent)
{
	m_iControlToValidate=0;
	m_Material=material;
	//{{AFX_DATA_INIT(CFormationMaterialDlg)
	m_sMatName = _T("");
	//}}AFX_DATA_INIT
}

//##ModelId=3C60D5760000
CFormationMaterial CFormationMaterialDlg::Material()
{
	return m_Material;
}

//##ModelId=3C60D5760262
void CFormationMaterialDlg::DoDataExchange(CDataExchange* pDX)
{
	
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFormationMaterialDlg)
	DDX_Control(pDX, IDC_cmbHardeningType, m_HardeningType);
	DDX_Control(pDX, IDC_fraElasticity, m_fraElasticity);
	DDX_Control(pDX, IDC_ebYoungsModulus, m_dbYoungsModulus);
	DDX_Control(pDX, IDC_ebVolumetricSpecificHeat, m_dbVolumetricSpecificHeat);
	DDX_Control(pDX, IDC_ebThermalExpansion, m_dbThermalExpansion);
	DDX_Control(pDX, IDC_ebThermalConductivity, m_dbThermalConductivity);
	DDX_Control(pDX, IDC_ebPoissonRatio, m_dbPoissonRatio);
	DDX_Control(pDX, IDC_ebName, m_edName);
	DDX_Control(pDX, IDC_ebHardeningGradient, m_dbHardeningGradient);
	DDX_Control(pDX, IDC_ebFrictionAngle, m_dbFrictionAngle);
	DDX_Control(pDX, IDC_ebFractureEnergy, m_FractureEnergy);
	DDX_Control(pDX, IDC_ebCohesion, m_dbCohesion);
  DDX_Control(pDX, IDC_ebDensity, m_dbDensity);
	DDX_Text(pDX, IDC_ebName, m_sMatName);
	DDX_Check(pDX, IDC_chkPlasticity, m_bPlasticity);
	//}}AFX_DATA_MAP

	DDV_Quantity(pDX,IDC_ebYoungsModulus,m_Material.YoungsModulus(),NULL,"Youngs Modulus");	
	DDV_Quantity(pDX,IDC_ebVolumetricSpecificHeat,m_Material.VolumetricSpecificHeat(),NULL,"Volumetric specific heat");	
	DDV_Quantity(pDX,IDC_ebThermalExpansion,m_Material.ThermalExpansion(),NULL,"Thermal Expansion");	
	DDV_Quantity(pDX,IDC_ebThermalConductivity,m_Material.ThermalConductivity(),NULL,"Thermal Conductivity");	
	DDV_Quantity(pDX,IDC_ebPoissonRatio,m_Material.PoissonRatio(),NULL,"Poissons ratio");	
	DDV_Quantity(pDX,IDC_ebHardeningGradient,m_Material.HardeningGradient(),NULL,"Hardening gradient");	
	DDV_Quantity(pDX,IDC_ebFrictionAngle,m_Material.FrictionAngle(),NULL,"Friction angle");	
	DDV_Quantity(pDX,IDC_ebFractureEnergy,m_Material.FractureEnergy(),NULL,"Fracture energy");	
	DDV_Quantity(pDX,IDC_ebCohesion,m_Material.Cohesion(),NULL,"Cohesion");	
  DDV_Quantity(pDX, IDC_ebDensity, m_Material.Density(), NULL, "Density");

	
	if (pDX->m_bSaveAndValidate)
	{
		m_Material.HardeningType(eHardeningType(m_HardeningType.GetCurSel())); 
		m_Material.Plasticity(m_bPlasticity);
		DDX_Check(pDX, IDC_chkPlasticity, m_bPlasticity);
	}
	else
	{
		m_HardeningType.SetCurSel((UINT)m_Material.HardeningType());
		m_bPlasticity=m_Material.Plasticity();
		DDX_Check(pDX, IDC_chkPlasticity, m_bPlasticity);
	}
	
	m_fraPlasticity.Enable(m_Material.Plasticity());

	if(m_Material.Plasticity())
	{
		switch(m_Material.HardeningType())
		{
			case None:
				m_dbHardeningGradient.EnableWindow(FALSE);
				GetDlgItem(IDC_lbHardeningGradient)->EnableWindow(FALSE);
				GetDlgItem(IDC_lbUHardeningGradient)->EnableWindow(FALSE);
				m_FractureEnergy.EnableWindow(FALSE);
				GetDlgItem(IDC_lbFractureEnergy)->EnableWindow(FALSE);
				GetDlgItem(IDC_lbUFractureEnergy)->EnableWindow(FALSE);
				break;
			case LinearHardening:
				m_dbHardeningGradient.EnableWindow(TRUE);
				GetDlgItem(IDC_lbHardeningGradient)->EnableWindow(TRUE);
				GetDlgItem(IDC_lbUHardeningGradient)->EnableWindow(TRUE);
				m_FractureEnergy.EnableWindow(FALSE);
				GetDlgItem(IDC_lbFractureEnergy)->EnableWindow(FALSE);
				GetDlgItem(IDC_lbUFractureEnergy)->EnableWindow(FALSE);
				break;
			
			case LinearSoftening:
			case ParabolicSoftening:
				m_dbHardeningGradient.EnableWindow(FALSE);
				GetDlgItem(IDC_lbHardeningGradient)->EnableWindow(FALSE);
				GetDlgItem(IDC_lbUHardeningGradient)->EnableWindow(FALSE);
				m_FractureEnergy.EnableWindow(TRUE);
				GetDlgItem(IDC_lbFractureEnergy)->EnableWindow(TRUE);
				GetDlgItem(IDC_lbUFractureEnergy)->EnableWindow(TRUE);
				break;
		}
	}
	
	m_Material.Name(m_sMatName);
	GetDlgItem(IDOK)->EnableWindow(m_Material.CheckComplete());

}


BEGIN_MESSAGE_MAP(CFormationMaterialDlg, CDialog)
	//{{AFX_MSG_MAP(CFormationMaterialDlg)
	ON_EN_KILLFOCUS(IDC_ebName, OnKillfocusName)
	ON_BN_CLICKED(IDC_chkPlasticity, OnchkPlasticity)
	ON_CBN_CLOSEUP(IDC_cmbHardeningType, OnCloseupcmbHardeningType)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_VALIDATE_DOUBLE_EDIT, OnValidateDoubleEdit)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFormationMaterialDlg message handlers


//##ModelId=3C60D57602A5
void CFormationMaterialDlg::OnKillfocusName() 
{
	UpdateData();
}


//##ModelId=3C60D57602C5
BOOL CFormationMaterialDlg::OnInitDialog() 
{
	m_sMatName=m_Material.Name();

	CDialog::OnInitDialog();
	
	SetUnitlabel(IDC_lbUYoungsModulus,m_Material.YoungsModulus());
	SetUnitlabel(IDC_lbUPoissonRatio,m_Material.PoissonRatio());
	SetUnitlabel(IDC_lbUVolumetricSpecificHeat,m_Material.VolumetricSpecificHeat());
	SetUnitlabel(IDC_lbUThermalConductivity,m_Material.ThermalConductivity());
	SetUnitlabel(IDC_lbUThermalExpansion,m_Material.ThermalExpansion());
	SetUnitlabel(IDC_lbUCohesion,m_Material.Cohesion());
	SetUnitlabel(IDC_lbUFrictionAngle,m_Material.FrictionAngle());
	SetUnitlabel(IDC_lbUHardeningGradient,m_Material.HardeningGradient());
	SetUnitlabel(IDC_lbUFractureEnergy,m_Material.FractureEnergy());
  SetUnitlabel(IDC_lbUDensity, m_Material.Density());

	//m_fraPlasticity.Set(this,IDC_fraPlasticity);

	//m_HardeningType.AddString(CDSMaterial::GetHardeningName(None));
	//m_HardeningType.AddString(CDSMaterial::GetHardeningName(LinearHardening));
	//m_HardeningType.AddString(CDSMaterial::GetHardeningName(LinearSoftening));
	//m_HardeningType.AddString(CDSMaterial::GetHardeningName(ParabolicSoftening));
	//UpdateData(FALSE);
	
	static bool bInit=false;
	if(!bInit)
	{
		m_fraPlasticity.Set(this,IDC_fraPlasticity);
		for(UINT i = IDS_HARDENING_NONE;i <= IDS_PARABOLIC_SOFT;i++)
		{
			CString strWellName;
			strWellName.LoadString(i);
			m_HardeningType.AddString(strWellName);
		}
		//bInit=true;
	
	}
	UpdateData(FALSE);

	// TOOLTIP
	static short sInit=0;
	if(!sInit)
	{
		//One time initialization!!
		sInit=1;
		
		m_toolTip.Create(this);
		CRect rect;
		CString sTmp;

		sTmp.LoadString(IDTT_FOL_chkPlasticity);
		m_toolTip.AddTool( GetDlgItem(IDC_chkPlasticity), sTmp);
		GetDlgItem(IDC_chkPlasticity)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_MORE_HardeningBehaviour);
		m_toolTip.AddTool( GetDlgItem(IDC_cmbHardeningType), sTmp);
		GetDlgItem(IDC_cmbHardeningType)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_MORE_HardeningBehaviour);
		m_toolTip.AddTool( GetDlgItem(IDC_ebFractureEnergy), sTmp);
		GetDlgItem(IDC_ebFractureEnergy)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_MORE_FrictionAngle);
		m_toolTip.AddTool( GetDlgItem(IDC_ebFrictionAngle), sTmp);
		GetDlgItem(IDC_ebFrictionAngle)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		//IDTT_FOL_chkPlasticity
		//IDTT_FOL_cmbHardeningType
		//IDTT_FOL_ebFractureEnergy
		//IDTT_FOL_ebFrictionAngle

		sTmp.LoadString(IDTT_MORE_HardeningBehaviour);
		m_toolTip.AddTool( GetDlgItem(IDC_ebHardeningGradient), sTmp);
		GetDlgItem(IDC_ebHardeningGradient)->GetWindowRect(&rect);	
		ScreenToClient(rect);

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
		//IDTT_FOL_ebHardeningGradient
		//IDTT_FOL_ebPoissonRatio
		//IDTT_FOL_ebThermalConductivity
		//IDTT_FOL_ebThermalExpansion

		sTmp.LoadString(IDTT_MORE_VolumetricSpecificHeat);
		m_toolTip.AddTool( GetDlgItem(IDC_ebVolumetricSpecificHeat), sTmp);
		GetDlgItem(IDC_ebVolumetricSpecificHeat)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_MORE_Elasticity);
		m_toolTip.AddTool( GetDlgItem(IDC_ebYoungsModulus), sTmp);
		GetDlgItem(IDC_ebYoungsModulus)->GetWindowRect(&rect);	
		ScreenToClient(rect);
		//IDTT_FOL_ebVolumetricSpecificHeat
		//IDTT_FOL_ebYoungsModulus
		

		CRect rectMargin(TOOL_TIP_MARGE,TOOL_TIP_MARGE,TOOL_TIP_MARGE,TOOL_TIP_MARGE); //left, top, right, bottom
		m_toolTip.SetMargin(rectMargin);
		m_toolTip.Activate(TRUE);
	}

	//GetDlgItem(IDC_fraElasticity)->SetFont(GetFont(TRUE)); TODO
	//GetDlgItem(IDC_chkPlasticity)->SetFont(GetFont(TRUE));
	
	//GetDlgItem(IDC_ebName)->SetFocus();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//##ModelId=3C60D57602D0
void CFormationMaterialDlg::OnchkPlasticity() 
{
	UpdateData();
}

//##ModelId=3C6A14C402EC
void CFormationMaterialDlg::OnCloseupcmbHardeningType() 
{
	UpdateData();	
}

//##ModelId=3C6A14C4025F
BOOL CFormationMaterialDlg::PreTranslateMessage(MSG* pMsg)
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

//##ModelId=3C8D9E19009E
void CFormationMaterialDlg::SetUnitlabel(int nID,CDoubleQuantity* pQ)
{
	CStatic *pUnitLabel=0;
	pUnitLabel = (CStatic*) GetDlgItem(nID);
	ASSERT(pUnitLabel!=0);
	::SetUnitlabel(pUnitLabel,pQ);
}

//##ModelId=3CA011A30109
LRESULT CFormationMaterialDlg::OnValidateDoubleEdit(WPARAM id, LPARAM lparam)
{
	m_iControlToValidate=id;
	UpdateData();
	m_iControlToValidate=0;
	return TRUE;
}
