// CementMaterialDlg.cpp: implementation of the CCementMaterialDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dsealect.h"
#include "DoubleEdit.h"
#include "CementMaterial.h"
#include "DDXQuantity.h"
#include "DSealectGlobal.h"
#include "CementMaterialDlg.h"
#include "CDataModel.h"

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif


/////////////////////////////////////////////////////////////////////////////
// CCementMaterialDlg dialog


//##ModelId=3C60D57C0177
CCementMaterialDlg::CCementMaterialDlg(const CCementMaterial& Material,CWnd* pParent /*=NULL*/)
	: CDialog(CCementMaterialDlg::IDD, pParent)
{
	m_iControlToValidate=0;
	m_Material=Material;
	//{{AFX_DATA_INIT(CCementMaterialDlg)
	m_sMatName = _T("");
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

//##ModelId=3C60D57C0189
CCementMaterial CCementMaterialDlg::Material()
{
	return m_Material;
}

//##ModelId=3C60D57D008D
void CCementMaterialDlg::DoDataExchange(CDataExchange* pDX)
{
	CPercentageQuantity	tempPQ;
	tempPQ = *m_Material.VolumetricShrinkage();
	if(!tempPQ.Undefined())
	{	
		// the GUI input is 'Hydration volume change' but internally the 'Volumetric shrinkage' is stored
		// so the value is multiplied with -1
		tempPQ.Value(-tempPQ.Value(),CDoubleQuantity::SI_UNIT);
	}
	

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCementMaterialDlg)
	DDX_Control(pDX, IDC_ebCementName, m_edName);
	DDX_Control(pDX, IDC_ebYoungsModulus, m_dbYoungsModulus);
	DDX_Control(pDX, IDC_ebPoissonRatio, m_dbPoissonRatio);
	DDX_Control(pDX, IDC_ebVolumetricSpecificHeat, m_dbVolumetricSpecificHeat);
	DDX_Control(pDX, IDC_ebThermalConductivity, m_dbThermalConductivity);
	DDX_Control(pDX, IDC_ebThermalExpansion, m_dbThermalExpansion);
	DDX_Control(pDX, IDC_ebTensileStrength, m_dbTensileStrength);
	DDX_Control(pDX, IDC_ebTensFractureEnergy, m_dbTensFractureEnergy);
	DDX_Control(pDX, IDC_ebCohesion, m_dbCohesion);
	DDX_Control(pDX, IDC_ebFrictionAngle, m_dbFrictionAngle);
	DDX_Control(pDX, IDC_cmbCompHardeningType, m_HardeningType);
	DDX_Control(pDX, IDC_ebCompHardeningGradient, m_dbCompHardeningGradient);
	DDX_Control(pDX, IDC_ebCompFractureEnergy, m_CompFractureEnergy);
	DDX_Control(pDX, IDC_ebVolumetricShrinkage, m_VolumetricShrinkage);
	DDX_Control(pDX, IDC_ebHydrationHeat, m_HydrationHeat);
	DDX_Text(pDX, IDC_ebCementName, m_sMatName);
	//}}AFX_DATA_MAP

	DDV_Quantity(pDX,IDC_ebYoungsModulus,m_Material.YoungsModulus(),NULL,"Youngs Modulus");	
	DDV_Quantity(pDX,IDC_ebPoissonRatio,m_Material.PoissonRatio(),NULL,"Poissons ratio");	
	DDV_Quantity(pDX,IDC_ebVolumetricSpecificHeat,m_Material.VolumetricSpecificHeat(),NULL,"Volumetric specific heat");	
	DDV_Quantity(pDX,IDC_ebThermalConductivity,m_Material.ThermalConductivity(),NULL,"Thermal Conductivity");	
	DDV_Quantity(pDX,IDC_ebThermalExpansion,m_Material.ThermalExpansion(),NULL,"Thermal Expansion");	
	DDV_Quantity(pDX,IDC_ebTensileStrength,m_Material.TensileStrength(),NULL,"Tensile strength");	
	DDV_Quantity(pDX,IDC_ebTensFractureEnergy,m_Material.TensFractureEnergy(),NULL,"Tens.fracture energy");	
	DDV_Quantity(pDX,IDC_ebCohesion,m_Material.Cohesion(),NULL,"Cohesion");	
	DDV_Quantity(pDX,IDC_ebFrictionAngle,m_Material.FrictionAngle(),NULL,"Friction angle");	
	DDV_Quantity(pDX,IDC_ebCompHardeningGradient,m_Material.CompHardeningGradient(),NULL,"Comp.hardening gradient");	
	DDV_Quantity(pDX,IDC_ebCompFractureEnergy,m_Material.CompFractureEnergy(),NULL,"Comp. fracture energy");	
	DDV_Quantity(pDX,IDC_ebVolumetricShrinkage,&tempPQ,NULL,"Value");	
	//DDV_Quantity(pDX,IDC_ebVolumetricShrinkage,m_Material.VolumetricShrinkage(),NULL,"Value");	
	DDV_Quantity(pDX,IDC_ebHydrationHeat,m_Material.HydrationHeat(),NULL,"Hydration heat");	
	
		
	if (pDX->m_bSaveAndValidate)
	{
		//update data
		m_Material.HardeningType(eHardeningType(m_HardeningType.GetCurSel())); 
		
		*m_Material.VolumetricShrinkage()=tempPQ;
		// internally the 'Volumetric shrinkage' is stored but the GUI shows 'Hydration volume change'  
		// so the value is multiplied with -1
		if(!m_Material.VolumetricShrinkage()->Undefined())
			m_Material.VolumetricShrinkage()->Value(-m_Material.VolumetricShrinkage()->Value(),CDoubleQuantity::SI_UNIT);
		
	}
	else
	{
		m_HardeningType.SetCurSel((UINT)m_Material.HardeningType());

		
	}
	
	switch(m_Material.HardeningType())
	{
		case None:
			m_dbCompHardeningGradient.EnableWindow(FALSE);
			GetDlgItem(IDC_lbCompHardeningGradient)->EnableWindow(FALSE);
			GetDlgItem(IDC_lbUCompHardeningGradient)->EnableWindow(FALSE);
			m_CompFractureEnergy.EnableWindow(FALSE);
			GetDlgItem(IDC_lbCompFractureEnergy)->EnableWindow(FALSE);
			GetDlgItem(IDC_lbUCompFractureEnergy)->EnableWindow(FALSE);
			break;
		case LinearHardening:
			m_dbCompHardeningGradient.EnableWindow(TRUE);
			GetDlgItem(IDC_lbCompHardeningGradient)->EnableWindow(TRUE);
			GetDlgItem(IDC_lbUCompHardeningGradient)->EnableWindow(TRUE);
			m_CompFractureEnergy.EnableWindow(FALSE);
			GetDlgItem(IDC_lbCompFractureEnergy)->EnableWindow(FALSE);
			GetDlgItem(IDC_lbUCompFractureEnergy)->EnableWindow(FALSE);
			break;
		
		case LinearSoftening:
		case ParabolicSoftening:
			m_dbCompHardeningGradient.EnableWindow(FALSE);
			GetDlgItem(IDC_lbCompHardeningGradient)->EnableWindow(FALSE);
			GetDlgItem(IDC_lbUCompHardeningGradient)->EnableWindow(FALSE);
			m_CompFractureEnergy.EnableWindow(TRUE);
			GetDlgItem(IDC_lbCompFractureEnergy)->EnableWindow(TRUE);
			GetDlgItem(IDC_lbUCompFractureEnergy)->EnableWindow(TRUE);
			break;
	}

	m_Material.Name(m_sMatName);
	GetDlgItem(IDOK)->EnableWindow(m_Material.CheckComplete());
}


BEGIN_MESSAGE_MAP(CCementMaterialDlg, CDialog)
	//{{AFX_MSG_MAP(CCementMaterialDlg)
	ON_EN_KILLFOCUS(IDC_ebName, OnKillfocusName)
	ON_CBN_CLOSEUP(IDC_cmbCompHardeningType, OnCloseupcmbCompHardeningType)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_VALIDATE_DOUBLE_EDIT, OnValidateDoubleEdit)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCementMaterialDlg message handlers

//##ModelId=3C60D57D00FB
void CCementMaterialDlg::OnKillfocusName() 
{
	UpdateData();
}

//##ModelId=3C60D57D012D
BOOL CCementMaterialDlg::OnInitDialog()
{
	m_sMatName=m_Material.Name();
	CDialog::OnInitDialog();

	SetUnitlabel(IDC_lbUYoungsModulus,m_Material.YoungsModulus());
	SetUnitlabel(IDC_lbUPoissonRatio,m_Material.PoissonRatio());
	SetUnitlabel(IDC_lbUVolumetricSpecificHeat,m_Material.VolumetricSpecificHeat());
	SetUnitlabel(IDC_lbUThermalConductivity,m_Material.ThermalConductivity());
	SetUnitlabel(IDC_lbUThermalExpansion,m_Material.ThermalExpansion());
	SetUnitlabel(IDC_lbUTensileStrenght,m_Material.TensileStrength());
	SetUnitlabel(IDC_lbUTensFractureEnergy,m_Material.TensFractureEnergy());
	SetUnitlabel(IDC_lbUCohesion,m_Material.Cohesion());
	SetUnitlabel(IDC_lbUFrictionAngle,m_Material.FrictionAngle());
	SetUnitlabel(IDC_lbUCompHardeningGradient,m_Material.CompHardeningGradient());
	SetUnitlabel(IDC_lbUCompFractureEnergy,m_Material.CompFractureEnergy());
	SetUnitlabel(IDC_lbUVolumetricShrinkage,m_Material.VolumetricShrinkage());
	SetUnitlabel(IDC_lbUHydrationHeat,m_Material.HydrationHeat());
	

	m_HardeningType.AddString(CDSMaterial::GetHardeningName(None));
	m_HardeningType.AddString(CDSMaterial::GetHardeningName(LinearHardening));
	m_HardeningType.AddString(CDSMaterial::GetHardeningName(LinearSoftening));
	m_HardeningType.AddString(CDSMaterial::GetHardeningName(ParabolicSoftening));
	UpdateData(FALSE);

	// TOOLTIP
	static short bInit=0;
	if(!bInit)
	{
		//One time initialization!!
		bInit=1;
		
		m_toolTip.Create(this);
		CRect rect;
		CString sTmp;

		sTmp.LoadString(IDTT_MORE_HardeningBehaviour);
		m_toolTip.AddTool( GetDlgItem(IDC_cmbCompHardeningType), sTmp);
		GetDlgItem(IDC_cmbCompHardeningType)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_MORE_Cohesion);
		m_toolTip.AddTool( GetDlgItem(IDC_ebCohesion), sTmp);
		GetDlgItem(IDC_ebCohesion)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_MORE_HardeningBehaviour);
		m_toolTip.AddTool( GetDlgItem(IDC_ebCompFractureEnergy), sTmp);
		GetDlgItem(IDC_ebCompFractureEnergy)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_MORE_HardeningBehaviour);
		m_toolTip.AddTool( GetDlgItem(IDC_ebCompHardeningGradient), sTmp);
		GetDlgItem(IDC_ebCompHardeningGradient)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		//IDTT_CEL_cmbCompHardeningType
		//IDTT_CEL_ebCohesion
		//IDTT_CEL_ebCompFractureEnergy
		//IDTT_CEL_ebCompHardeningGradient

		sTmp.LoadString(IDTT_MORE_FrictionAngle);
		m_toolTip.AddTool( GetDlgItem(IDC_ebFrictionAngle), sTmp);
		GetDlgItem(IDC_ebFrictionAngle)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_MORE_Curing);
		m_toolTip.AddTool( GetDlgItem(IDC_ebHydrationHeat), sTmp);
		GetDlgItem(IDC_ebHydrationHeat)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_MORE_Elasticity);
		m_toolTip.AddTool( GetDlgItem(IDC_ebPoissonRatio), sTmp);
		GetDlgItem(IDC_ebPoissonRatio)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_MORE_Cracking);
		m_toolTip.AddTool( GetDlgItem(IDC_ebTensFractureEnergy), sTmp);
		GetDlgItem(IDC_ebTensFractureEnergy)->GetWindowRect(&rect);	
		ScreenToClient(rect);
		//IDTT_CEL_ebFrictionAngle
		//IDTT_CEL_ebHydrationHeat
		//IDTT_CEL_ebPoissonRatio
		//IDTT_CEL_ebTensFractureEnergy

		sTmp.LoadString(IDTT_MORE_Cracking);
		m_toolTip.AddTool( GetDlgItem(IDC_ebTensileStrength), sTmp);
		GetDlgItem(IDC_ebTensileStrength)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_MORE_ThermalConductivity);
		m_toolTip.AddTool( GetDlgItem(IDC_ebThermalConductivity), sTmp);
		GetDlgItem(IDC_ebThermalConductivity)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_MORE_ThermalExpansion);
		m_toolTip.AddTool( GetDlgItem(IDC_ebThermalExpansion), sTmp);
		GetDlgItem(IDC_ebThermalExpansion)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_MORE_Curing);
		m_toolTip.AddTool( GetDlgItem(IDC_ebVolumetricShrinkage), sTmp);
		GetDlgItem(IDC_ebVolumetricShrinkage)->GetWindowRect(&rect);	
		ScreenToClient(rect);
		//IDTT_CEL_ebTensileStrength
		//IDTT_CEL_ebThermalConductivity
		//IDTT_CEL_ebThermalExpansion
		//IDTT_CEL_ebVolumetricShrinkage

		sTmp.LoadString(IDTT_MORE_VolumetricSpecificHeat);
		m_toolTip.AddTool( GetDlgItem(IDC_ebVolumetricSpecificHeat), sTmp);
		GetDlgItem(IDC_ebVolumetricSpecificHeat)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		sTmp.LoadString(IDTT_MORE_Elasticity);
		m_toolTip.AddTool( GetDlgItem(IDC_ebYoungsModulus), sTmp);
		GetDlgItem(IDC_ebYoungsModulus)->GetWindowRect(&rect);	
		ScreenToClient(rect);

		
		//IDTT_CEL_ebVolumetricSpecificHeat
		//IDTT_CEL_ebYoungsModulus
		

		CRect rectMargin(TOOL_TIP_MARGE,TOOL_TIP_MARGE,TOOL_TIP_MARGE,TOOL_TIP_MARGE); //left, top, right, bottom
		m_toolTip.SetMargin(rectMargin);
		m_toolTip.Activate(TRUE);
	}

	return TRUE;
	
}


//##ModelId=3C6A14CB0211
void CCementMaterialDlg::OnCloseupcmbCompHardeningType() 
{
	UpdateData();	
}

//##ModelId=3C6A14CB0194
BOOL CCementMaterialDlg::PreTranslateMessage(MSG* pMsg)
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

//##ModelId=3C8D9E2102C0
void CCementMaterialDlg::SetUnitlabel(int nID,CDoubleQuantity* pQ)
{
	CStatic *pUnitLabel=0;
	pUnitLabel = (CStatic*) GetDlgItem(nID);
	ASSERT(pUnitLabel!=0);
	::SetUnitlabel(pUnitLabel,pQ);
}

//##ModelId=3CA011AA00DA
LRESULT CCementMaterialDlg::OnValidateDoubleEdit(WPARAM id, LPARAM lparam)
{
	m_iControlToValidate=id;
	UpdateData();
	m_iControlToValidate=0;
	return TRUE;
}


