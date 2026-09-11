// facesupportdlg.cpp : implementation file
//

#include "stdafx.h"
#include "geomec.h"

#include "DepletionStage.h"
#include "SupportDlgBase.h"
#include "PressureSupportNode.h"
#include "facesupportdlg.h"
#include "FieldFactor.h"
#include "ModelBase.h"
#include "BranchState.h"
#include "resourceIDP.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif

/*

wedx 20-02-2007 NOTE:

I disabled a lot of code in this dialog (and did not delete it yet) because
the change may very well be temporary. The original dialog resource template
is still available in the resource file (IDD_ATTRI_FACE_SUPPORT).

I introduced a (shorter) copy of the template named IDD_ATTRI_FACE_SUPPORT_SHORT and
disabled all code that would access any dialog items that are only available on the
original.

*/

/*

wedx 17-07-2008:

An additional control (IDC_VERTICALLY) has been added to the IDD_ATTRI_FACE_SUPPORT_SHORT
template and is used in the code below. I have NOT added it to the original template.

*/


/////////////////////////////////////////////////////////////////////////////
// CFaceSupportDlg dialog


CFaceSupportDlg::CFaceSupportDlg(CPressureSupportNode::CFaceSupportDef &supdef, const CPressureSupportNode &node, CWnd* pParent /*=NULL*/)
:	CSupportDlgBase(node, CFaceSupportDlg::IDD, pParent),
	m_SupportDef(supdef)
{
	//{{AFX_DATA_INIT(CFaceSupportDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CFaceSupportDlg::DoDataExchange(CDataExchange* pDX)
{
/*
	double dN, dE, dD;

	if(!pDX->m_bSaveAndValidate)
	{
		EmptyEditBoxes();

		if(PressureType() == CPressureSupportNode::CFaceSupportDef::CSupportDef::PT_LOCAL &&
			SupportType() == CPressureSupportNode::CFaceSupportDef::ST_STRESS)
		{
			dN = m_SupportDef.SupportDef(*m_vcDepletionStages[m_nDeplStage]).LocalVector().X() * UnitFactor();
			dE = m_SupportDef.SupportDef(*m_vcDepletionStages[m_nDeplStage]).LocalVector().Y() * UnitFactor();
			dD = m_SupportDef.SupportDef(*m_vcDepletionStages[m_nDeplStage]).LocalVector().Z() * UnitFactor();
		}
	}
*/
	CSupportDlgBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFaceSupportDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
/*
	if(GetDlgItem(IDC_ED_N)->IsWindowEnabled())
	{
		DDX_Text(pDX, IDC_ED_N, dN);
		DDX_Text(pDX, IDC_ED_E, dE);
		DDX_Text(pDX, IDC_ED_D, dD);
	}
*/
	if(pDX->m_bSaveAndValidate)
	{
//		m_SupportDef.PreScribedDisplacement(GetButton(IDC_PRESCRIBED_DISPLACEMENTS));
/*
		if(PressureType() == CPressureSupportNode::CFaceSupportDef::CSupportDef::PT_LOCAL &&
			SupportType() == CPressureSupportNode::CFaceSupportDef::ST_STRESS)
		{
			m_SupportDef.SupportDef(*m_vcDepletionStages[m_nDeplStage]).LocalVector().X(dN / UnitFactor());
			m_SupportDef.SupportDef(*m_vcDepletionStages[m_nDeplStage]).LocalVector().Y(dE / UnitFactor());
			m_SupportDef.SupportDef(*m_vcDepletionStages[m_nDeplStage]).LocalVector().Z(dD / UnitFactor());
		}
*/
	}
}

void CFaceSupportDlg::EmptyEditBoxes()
{
/*
	GetDlgItem(IDC_ED_N)->SetWindowText(_T(""));
	GetDlgItem(IDC_ED_E)->SetWindowText(_T(""));
	GetDlgItem(IDC_ED_D)->SetWindowText(_T(""));
*/
}

void CFaceSupportDlg::FillUnitLabels()
{
/*
	CString strUnitName = _T("MPa");

	if(Unit() == IQuantityDouble::FIELD_UNIT) strUnitName = _T("psi");

	GetDlgItem(IDC_UN_N)->SetWindowText(strUnitName);
	GetDlgItem(IDC_UN_E)->SetWindowText(strUnitName);
	GetDlgItem(IDC_UN_D)->SetWindowText(strUnitName);
*/
}

void CFaceSupportDlg::UpdateControls()
{
//	bool bStress = (SupportType() == CPressureSupportNode::CFaceSupportDef::ST_STRESS);
	bool bSupported = ((SupportType() == CPressureSupportNode::CFaceSupportDef::ST_FULL)        ||
		                 (SupportType() == CPressureSupportNode::CFaceSupportDef::ST_NORMAL)      ||
					           (SupportType() == CPressureSupportNode::CFaceSupportDef::ST_HORIZONTAL)  ||
					           (SupportType() == CPressureSupportNode::CFaceSupportDef::ST_UNSUPPORTED) ||
                     (SupportType() == CPressureSupportNode::CFaceSupportDef::ST_VERTICAL)       );

	bool bBranch = (static_cast<const CModelBase&>(Node().Model())).BranchState().IsBranch();
//	bool bLocal = (PressureType() == CPressureSupportNode::CFaceSupportDef::CSupportDef::PT_LOCAL);

//	GetDlgItem(IDC_LB_STAGE)->EnableWindow(bStress);
/*
	CComboBox *pStage = (CComboBox *) GetDlgItem(IDC_STAGE);
	if(bStress) pStage->SetCurSel(m_nDeplStage);
	else pStage->SetCurSel(-1);
*/
	CPressureSupportNode::TSurfaceLocation sl = m_SupportDef.SurfaceLocation();
	GetDlgItem(IDC_HORIZONTALLY)->EnableWindow(bSupported && !bBranch && sl == CPressureSupportNode::SL_SIDE);
	GetDlgItem(IDC_NORMALLY)->EnableWindow(bSupported && !bBranch && sl == CPressureSupportNode::SL_SIDE || sl == CPressureSupportNode::SL_BOTTOM);
  GetDlgItem(IDC_FULLY)->EnableWindow(bSupported && !bBranch && sl == CPressureSupportNode::SL_SIDE || sl == CPressureSupportNode::SL_BOTTOM || sl == CPressureSupportNode::SL_TOP);
	GetDlgItem(IDC_UNSUPPORTED)->EnableWindow(bSupported && !bBranch && sl == CPressureSupportNode::SL_TOP);
  GetDlgItem(IDC_VERTICALLY)->EnableWindow(bSupported && !bBranch && sl == CPressureSupportNode::SL_TOP);

//	GetDlgItem(IDC_NO_DISPLACEMENTS)->EnableWindow(bSupported && !bBranch);
//	GetDlgItem(IDC_PRESCRIBED_DISPLACEMENTS)->EnableWindow(bSupported && !bBranch);
//	GetDlgItem(IDC_STAGE)->EnableWindow(bStress);

//	GetDlgItem(IDC_UNLOADED)->EnableWindow(bStress);
//	GetDlgItem(IDC_LOCAL)->EnableWindow(bStress);

//	GetDlgItem(IDC_LB_N)->EnableWindow(bLocal && bStress);
//	GetDlgItem(IDC_ED_N)->EnableWindow(bLocal && bStress);
//	GetDlgItem(IDC_UN_N)->EnableWindow(bLocal && bStress);
//	GetDlgItem(IDC_LB_E)->EnableWindow(bLocal && bStress);
//	GetDlgItem(IDC_ED_E)->EnableWindow(bLocal && bStress);
//	GetDlgItem(IDC_UN_E)->EnableWindow(bLocal && bStress);
//	GetDlgItem(IDC_LB_D)->EnableWindow(bLocal && bStress);
//	GetDlgItem(IDC_ED_D)->EnableWindow(bLocal && bStress);
//	GetDlgItem(IDC_UN_D)->EnableWindow(bLocal && bStress);

	bool bGlobalDefined = Node().GlobalTensorDefined(*m_vcDepletionStages[m_nDeplStage]);
//	GetDlgItem(IDC_GLOBAL)->EnableWindow(bGlobalDefined && bStress);
/*
	if(bStress) SetRadioButton();
	else
	{
		CButton *pBut[3];
		pBut[0] = (CButton*) GetDlgItem(IDC_UNLOADED);
		pBut[1] = (CButton*) GetDlgItem(IDC_LOCAL);
		pBut[2] = (CButton*) GetDlgItem(IDC_GLOBAL);

		for(int i = 0; i < 3; i++) pBut[i]->SetCheck(0);
	}
*/

	GetDlgItem(IDC_APPLYTOALL)->EnableWindow(!bBranch);
}

void CFaceSupportDlg::GetRadioButton()
{
/*
	CButton *pBut[3];
	pBut[0] = (CButton*) GetDlgItem(IDC_UNLOADED);
	pBut[1] = (CButton*) GetDlgItem(IDC_LOCAL);
	pBut[2] = (CButton*) GetDlgItem(IDC_GLOBAL);

	for(int i = 0; i < 3; i++)
	{
		if(pBut[i]->GetCheck() == 1)
		{
			PressureType((TPressureType) i);
			break;
		}
	}
*/
}

void CFaceSupportDlg::SetRadioButton()
{
/*
	CButton *pBut[3];
	pBut[0] = (CButton*) GetDlgItem(IDC_UNLOADED);
	pBut[1] = (CButton*) GetDlgItem(IDC_LOCAL);
	pBut[2] = (CButton*) GetDlgItem(IDC_GLOBAL);

	for(int i = 0; i < 3; i++)
	{
		if((int) PressureType() == i)
			pBut[i]->SetCheck(1);
		else
			pBut[i]->SetCheck(0);
	}
*/
}

void CFaceSupportDlg::FillDepletionStageCombo()
{
//	CComboBox *pCombo = (CComboBox *) GetDlgItem(IDC_STAGE);
//	CSupportDlgBase::FillDepletionStageCombo(pCombo);
}

CFaceSupportDlg::TPressureType CFaceSupportDlg::PressureType()
{
	return m_SupportDef.SupportDef(*m_vcDepletionStages[m_nDeplStage]).PressureType();
}

void CFaceSupportDlg::PressureType(TPressureType nType)
{
	m_SupportDef.SupportDef(*m_vcDepletionStages[m_nDeplStage]).PressureType(nType);
}

double CFaceSupportDlg::UnitFactor()
{
	double dRet = 1;
	if(Unit() == IQuantityDouble::FIELD_UNIT) dRet = FF_FACTOR_STRESS;

	return dRet;
}

void CFaceSupportDlg::SetButton(unsigned int uButton, bool bCheck)
{
	CButton *pButton = (CButton*) GetDlgItem(uButton);
	int nCheck = 0;
	if(bCheck) nCheck = 1;
	pButton->SetCheck(nCheck);
}

bool CFaceSupportDlg::GetButton(unsigned int uButton)
{
	CButton *pButton = (CButton*) GetDlgItem(uButton);
	return pButton->GetCheck() == 1;
}

void CFaceSupportDlg::SetSupportModeButton()
{
//	SetButton(IDC_SUPPORTED, (SupportType() == CPressureSupportNode::CFaceSupportDef::ST_FULL) ||
//								(SupportType() == CPressureSupportNode::CFaceSupportDef::ST_NORMAL));
	SetButton(IDC_HORIZONTALLY, SupportType() == CPressureSupportNode::CFaceSupportDef::ST_HORIZONTAL);
	SetButton(IDC_NORMALLY, SupportType() == CPressureSupportNode::CFaceSupportDef::ST_NORMAL);
	SetButton(IDC_FULLY, SupportType() == CPressureSupportNode::CFaceSupportDef::ST_FULL);
	SetButton(IDC_UNSUPPORTED, SupportType() == CPressureSupportNode::CFaceSupportDef::ST_UNSUPPORTED);
  SetButton(IDC_VERTICALLY, SupportType() == CPressureSupportNode::CFaceSupportDef::ST_VERTICAL);
//	SetButton(IDC_STRESS, SupportType() == CPressureSupportNode::CFaceSupportDef::ST_STRESS);
}

void CFaceSupportDlg::GetSupportModeButton()
{
//	if(GetButton(IDC_SUPPORTED))
//	{
		if(GetButton(IDC_HORIZONTALLY))
			SupportType(CPressureSupportNode::CFaceSupportDef::ST_HORIZONTAL);
		else if(GetButton(IDC_NORMALLY))
			SupportType(CPressureSupportNode::CFaceSupportDef::ST_NORMAL);
		else if(GetButton(IDC_FULLY))
			SupportType(CPressureSupportNode::CFaceSupportDef::ST_FULL);
		else if(GetButton(IDC_UNSUPPORTED))
			SupportType(CPressureSupportNode::CFaceSupportDef::ST_UNSUPPORTED);
    else if(GetButton(IDC_VERTICALLY))
      SupportType(CPressureSupportNode::CFaceSupportDef::ST_VERTICAL);
//	}
//	else
//		SupportType(CPressureSupportNode::CFaceSupportDef::ST_STRESS);
}

void CFaceSupportDlg::OnSupportModeChanged()
{
	if(!UpdateData(TRUE))
	{
		SetSupportModeButton();
		return;
	}

	GetSupportModeButton();
	UpdateControls();
	UpdateData(FALSE);
}

void CFaceSupportDlg::SupportType(TSupportType nType)
{
	m_SupportDef.SupportType(nType);
}

CFaceSupportDlg::TSupportType CFaceSupportDlg::SupportType()
{
	return m_SupportDef.SupportType();
}

BEGIN_MESSAGE_MAP(CFaceSupportDlg, CDialog)
	//{{AFX_MSG_MAP(CFaceSupportDlg)
//	ON_CBN_SELCHANGE(IDC_STAGE, OnSelchangeStage)
//	ON_BN_CLICKED(IDC_LOCAL, OnLocal)
//	ON_BN_CLICKED(IDC_GLOBAL, OnGlobal)
//	ON_BN_CLICKED(IDC_UNLOADED, OnUnloaded)
//	ON_BN_CLICKED(IDC_STRESS, OnStress)
//	ON_BN_CLICKED(IDC_SUPPORTED, OnSupported)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_HORIZONTALLY, OnSupportModeChanged)
	ON_BN_CLICKED(IDC_NORMALLY, OnSupportModeChanged)
	ON_BN_CLICKED(IDC_FULLY, OnSupportModeChanged)
	ON_BN_CLICKED(IDC_UNSUPPORTED, OnSupportModeChanged)
  ON_BN_CLICKED(IDC_VERTICALLY, OnSupportModeChanged)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFaceSupportDlg message handlers

void CFaceSupportDlg::OnSelchangeStage() 
{
//	CComboBox *pCombo = (CComboBox *) GetDlgItem(IDC_STAGE);
//	CSupportDlgBase::OnSelchangeStage(pCombo);
}

void CFaceSupportDlg::OnLocal() 
{
	OnRadioButton();
}

void CFaceSupportDlg::OnGlobal() 
{
	OnRadioButton();
}

void CFaceSupportDlg::OnUnloaded() 
{
	OnRadioButton();
}

void CFaceSupportDlg::OnOK() 
{
	CSupportDlgBase::OnOK(IDP_SUPPORT_FACE_APPLYTOALL);
}

BOOL CFaceSupportDlg::OnInitDialog() 
{
	CSupportDlgBase::OnInitDialog();

	SetSupportModeButton();
//	SetButton(IDC_NO_DISPLACEMENTS, !m_SupportDef.PreScribedDisplacement());
//	SetButton(IDC_PRESCRIBED_DISPLACEMENTS, m_SupportDef.PreScribedDisplacement());

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFaceSupportDlg::OnStress() 
{
//	OnSupportModeChanged();
}

void CFaceSupportDlg::OnSupported() 
{
//	OnSupportModeChanged();
}
