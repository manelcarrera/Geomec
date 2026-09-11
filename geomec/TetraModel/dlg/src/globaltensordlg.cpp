// globaltensordlg.cpp : implementation file
//

#include "stdafx.h"
#include <cmath>
#include "geomec.h"
#include "globaltensordlg.h"
#include "ModelBase.h"
#include "DepletionStage.h"
#include "ResultRegister.h"
#include "DisplacementSupportNode.h"
#include "BranchState.h"
#include "GlobalMessage.h"
#include "resourceIDP.h"
#include "TetraSupportNode_Delegate.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CGlobalTensorDlg dialog


CGlobalTensorDlg::CGlobalTensorDlg(C3DSupportNode &supnode, CWnd* pParent /*=NULL*/)
	: CAttributesTemplate<C3DSupportNode>(IDD_GLOBAL_TENSOR, supnode, pParent),
	m_SourceNode(supnode),
	m_nDeplStage(0),
	m_bInitialized(false)
{
	//{{AFX_DATA_INIT(CGlobalTensorDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CGlobalTensorDlg::DoDataExchange(CDataExchange* pDX)
{
	int nGlobalTensorInput;
	C3DSupportNode& tetraSupportNode =
		dynamic_cast <C3DSupportNode&> (Copy().Source());
	C3DSupportNode_Delegate* tetraSupportNode_Delegate =
		dynamic_cast <C3DSupportNode_Delegate*> (tetraSupportNode.getDelegate());
	double dUnitFactor = tetraSupportNode_Delegate->UnitFactor();
	CString strUnit = tetraSupportNode_Delegate->UnitName().toStdString().c_str();
	double dNN, dEE, dVV, dNE, dNV, dEV;
	double dV, dH, dh, dAzimuth;
	const CDepletionStage &stage = *m_vcDepletionStages[m_nDeplStage];

	bool bStrain = (dynamic_cast<CDisplacementSupportNode *> (&m_SourceNode) != 0);

	nGlobalTensorInput = (int) Copy().GlobalTensorInput(stage);
	if(!pDX->m_bSaveAndValidate)
	{
		EmptyEditBoxes();

		if(nGlobalTensorInput == C3DSupportNode::GTI_GENERAL)
		{
			dNN = Copy().GlobalTensor(stage).XX() * dUnitFactor;
			dEE = Copy().GlobalTensor(stage).YY() * dUnitFactor;
			dVV = Copy().GlobalTensor(stage).ZZ() * dUnitFactor;
			dNE = Copy().GlobalTensor(stage).XY() * dUnitFactor;
			dEV = Copy().GlobalTensor(stage).YZ() * dUnitFactor;
			dNV = Copy().GlobalTensor(stage).XZ() * dUnitFactor;

			if(bStrain)
			{
				// internally we use scientific strains, so convert to engineering strains for user
				dNE *= 2.0;
				dEV *= 2.0;
				dNV *= 2.0;
			}
		}
		else if(nGlobalTensorInput == C3DSupportNode::GTI_VERTICAL)
		{
			// ignore the source's vertical shear terms
			const ITensor &tensor = Copy().GlobalTensor(stage);
			CTensor copy_tensor;

			if(fabs(tensor.XX()) > EPS ||
			   fabs(tensor.YY()) > EPS ||
			   fabs(tensor.ZZ()) > EPS ||
			   fabs(tensor.XY()) > EPS)
			{
				copy_tensor = CTensor(tensor.XX(), tensor.YY(), tensor.ZZ(), tensor.XY(), 0, 0);
				geo::CMatrix mat_val = copy_tensor.GetPrincipalStr();
				geo::CMatrix mat_dir = copy_tensor.GetPrincipalDir();

				geo::CVector vcMax(mat_dir.Value(0, 0), mat_dir.Value(1, 0), mat_dir.Value(2, 0));
				geo::CVector vcMed(mat_dir.Value(0, 1), mat_dir.Value(1, 1), mat_dir.Value(2, 1));
				geo::CVector vcMin(mat_dir.Value(0, 2), mat_dir.Value(1, 2), mat_dir.Value(2, 2));

				geo::CVector *pvcH;

				// one of these vectors should be the Z axis
				if(vcMax == geo::CVector::Zaxis)
				{
					dV = mat_val.Value(0, 0);
					if(mat_val.Value(1, 1) > mat_val.Value(2, 2))
					{
						dH = mat_val.Value(1, 1);
						dh = mat_val.Value(2, 2);
						pvcH = &vcMed;
					}
					else
					{
						dh = mat_val.Value(1, 1);
						dH = mat_val.Value(2, 2);
						pvcH = &vcMin;
					}
				}
				else if(vcMed == geo::CVector::Zaxis)
				{
					dV = mat_val.Value(1, 1);
					if(mat_val.Value(0, 0) > mat_val.Value(2, 2))
					{
						dH = mat_val.Value(0, 0);
						dh = mat_val.Value(2, 2);
						pvcH = &vcMax;
					}
					else
					{
						dh = mat_val.Value(0, 0);
						dH = mat_val.Value(2, 2);
						pvcH = &vcMin;
					}
				}
				else if(vcMin == geo::CVector::Zaxis)
				{
					dV = mat_val.Value(2, 2);
					if(mat_val.Value(0, 0) > mat_val.Value(1, 1))
					{
						dH = mat_val.Value(0, 0);
						dh = mat_val.Value(1, 1);
						pvcH = &vcMax;
					}
					else
					{
						dh = mat_val.Value(0, 0);
						dH = mat_val.Value(1, 1);
						pvcH = &vcMin;
					}
				}
				else
				{
					assert(FALSE);
					pDX->Fail();
					return;
				}

				assert(fabs(pvcH->Length()) > EPS);

				double dInProd = pvcH->DotProduct(geo::CVector::Xaxis);
				double dCosAng = dInProd / pvcH->Length(); // length of Xaxis equals 1
				dAzimuth = RAD2DEG(acos(dCosAng));
			}
			else
			{
				dH = 0;
				dh = 0;
				dV = 0;
				dAzimuth = 0;
			}
		}

		DDX_Text(pDX, IDC_GEN_UN_NN, strUnit);
		DDX_Text(pDX, IDC_GEN_UN_EE, strUnit);
		DDX_Text(pDX, IDC_GEN_UN_VV, strUnit);
		DDX_Text(pDX, IDC_GEN_UN_NE, strUnit);
		DDX_Text(pDX, IDC_GEN_UN_NV, strUnit);
		DDX_Text(pDX, IDC_GEN_UN_EV, strUnit);
		DDX_Text(pDX, IDC_VER_UN_V, strUnit);
		DDX_Text(pDX, IDC_VER_UN_MAXH, strUnit);
		DDX_Text(pDX, IDC_VER_UN_MINH, strUnit);
	}

	CAttributesTemplate<C3DSupportNode>::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGlobalTensorDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

	assert(nGlobalTensorInput >= 0 && nGlobalTensorInput < 3);
	if(GetDlgItem(IDC_GEN_ED_NN)->IsWindowEnabled())
	{
		DDX_Text(pDX, IDC_GEN_ED_NN, dNN);
		DDX_Text(pDX, IDC_GEN_ED_EE, dEE);
		DDX_Text(pDX, IDC_GEN_ED_VV, dVV);
		DDX_Text(pDX, IDC_GEN_ED_NE, dNE);
		DDX_Text(pDX, IDC_GEN_ED_NV, dNV);
		DDX_Text(pDX, IDC_GEN_ED_EV, dEV);
	}
	else if(GetDlgItem(IDC_VER_ED_V)->IsWindowEnabled())
	{
		DDX_Text(pDX, IDC_VER_ED_V, dV);
		DDX_Text(pDX, IDC_VER_ED_MAXH, dH);
		DDX_Text(pDX, IDC_VER_ED_MINH, dh);
		DDX_Text(pDX, IDC_VER_ED_AZI, dAzimuth);
	}

	if(pDX->m_bSaveAndValidate)
	{
		if(nGlobalTensorInput == C3DSupportNode::GTI_GENERAL)
		{
			Copy().GlobalTensor(stage).XX(dNN / dUnitFactor);
			Copy().GlobalTensor(stage).YY(dEE / dUnitFactor);
			Copy().GlobalTensor(stage).ZZ(dVV / dUnitFactor);

			if(bStrain)
			{
				// user puts in engineering strains, so convert to scientific
				Copy().GlobalTensor(stage).XY(dNE / (dUnitFactor * 2.0));
				Copy().GlobalTensor(stage).YZ(dEV / (dUnitFactor * 2.0));
				Copy().GlobalTensor(stage).XZ(dNV / (dUnitFactor * 2.0));
			}
			else
			{
				Copy().GlobalTensor(stage).XY(dNE / (dUnitFactor));
				Copy().GlobalTensor(stage).YZ(dEV / (dUnitFactor));
				Copy().GlobalTensor(stage).XZ(dNV / (dUnitFactor));
			}
		}
		else if(nGlobalTensorInput == C3DSupportNode::GTI_VERTICAL)
		{
			geo::CVector vcH(cos(DEG2RAD(dAzimuth)), sin(DEG2RAD(dAzimuth)));
			geo::CVector vch(-sin(DEG2RAD(dAzimuth)), cos(DEG2RAD(dAzimuth)));

			CTensor tensor_new = CTensor(dH / dUnitFactor, dh / dUnitFactor, dV / dUnitFactor, vcH, vch, geo::CVector::Zaxis);
			Copy().GlobalTensor(stage).XX(tensor_new.XX());
			Copy().GlobalTensor(stage).YY(tensor_new.YY());
			Copy().GlobalTensor(stage).ZZ(tensor_new.ZZ());
			Copy().GlobalTensor(stage).XY(tensor_new.XY());
			Copy().GlobalTensor(stage).YZ(tensor_new.YZ());
			Copy().GlobalTensor(stage).XZ(tensor_new.XZ());
		}
	}
}

void CGlobalTensorDlg::SetInitialDepletionStage(int nStage)
{
	assert(!m_bInitialized);
	m_nDeplStage = nStage;
}

void CGlobalTensorDlg::UpdateControls()
{
	const CDepletionStage &stage = *m_vcDepletionStages[m_nDeplStage];
	BOOL bGeneral = Copy().GlobalTensorInput(stage) == C3DSupportNode::GTI_GENERAL;
	BOOL bVertical = Copy().GlobalTensorInput(stage) == C3DSupportNode::GTI_VERTICAL;

	bool bBranch = (static_cast<const CModelBase&>(Copy().Model())).BranchState().IsBranch();

	GetDlgItem(IDC_UNDEFINED)->EnableWindow(!bBranch);
	GetDlgItem(IDC_GENERAL)->EnableWindow(!bBranch);
	GetDlgItem(IDC_VERTICAL)->EnableWindow(!bBranch);

	GetDlgItem(IDC_GEN_LBL_NN)->EnableWindow(bGeneral && !bBranch);
	GetDlgItem(IDC_GEN_LBL_EE)->EnableWindow(bGeneral && !bBranch);
	GetDlgItem(IDC_GEN_LBL_VV)->EnableWindow(bGeneral && !bBranch);
	GetDlgItem(IDC_GEN_LBL_NE)->EnableWindow(bGeneral && !bBranch);
	GetDlgItem(IDC_GEN_LBL_NV)->EnableWindow(bGeneral && !bBranch);
	GetDlgItem(IDC_GEN_LBL_EV)->EnableWindow(bGeneral && !bBranch);
	GetDlgItem(IDC_GEN_ED_NN)->EnableWindow(bGeneral && !bBranch);
	GetDlgItem(IDC_GEN_ED_EE)->EnableWindow(bGeneral && !bBranch);
	GetDlgItem(IDC_GEN_ED_VV)->EnableWindow(bGeneral && !bBranch);
	GetDlgItem(IDC_GEN_ED_NE)->EnableWindow(bGeneral && !bBranch);
	GetDlgItem(IDC_GEN_ED_NV)->EnableWindow(bGeneral && !bBranch);
	GetDlgItem(IDC_GEN_ED_EV)->EnableWindow(bGeneral && !bBranch);
	GetDlgItem(IDC_GEN_UN_NN)->EnableWindow(bGeneral && !bBranch);
	GetDlgItem(IDC_GEN_UN_EE)->EnableWindow(bGeneral && !bBranch);
	GetDlgItem(IDC_GEN_UN_VV)->EnableWindow(bGeneral && !bBranch);
	GetDlgItem(IDC_GEN_UN_NE)->EnableWindow(bGeneral && !bBranch);
	GetDlgItem(IDC_GEN_UN_NV)->EnableWindow(bGeneral && !bBranch);
	GetDlgItem(IDC_GEN_UN_EV)->EnableWindow(bGeneral && !bBranch);

	GetDlgItem(IDC_VER_LBL_V)->EnableWindow(bVertical && !bBranch);
	GetDlgItem(IDC_VER_LBL_MAXH)->EnableWindow(bVertical && !bBranch);
	GetDlgItem(IDC_VER_LBL_MINH)->EnableWindow(bVertical && !bBranch);
	GetDlgItem(IDC_VER_LBL_AZI)->EnableWindow(bVertical && !bBranch);
	GetDlgItem(IDC_VER_ED_V)->EnableWindow(bVertical && !bBranch);
	GetDlgItem(IDC_VER_ED_MAXH)->EnableWindow(bVertical && !bBranch);
	GetDlgItem(IDC_VER_ED_MINH)->EnableWindow(bVertical && !bBranch);
	GetDlgItem(IDC_VER_ED_AZI)->EnableWindow(bVertical && !bBranch);
	GetDlgItem(IDC_VER_UN_V)->EnableWindow(bVertical && !bBranch);
	GetDlgItem(IDC_VER_UN_MAXH)->EnableWindow(bVertical && !bBranch);
	GetDlgItem(IDC_VER_UN_MINH)->EnableWindow(bVertical && !bBranch);
	GetDlgItem(IDC_VER_UN_AZI)->EnableWindow(bVertical && !bBranch);
}

void CGlobalTensorDlg::EmptyEditBoxes()
{
	GetDlgItem(IDC_GEN_ED_NN)->SetWindowText(_T(""));
	GetDlgItem(IDC_GEN_ED_EE)->SetWindowText(_T(""));
	GetDlgItem(IDC_GEN_ED_VV)->SetWindowText(_T(""));
	GetDlgItem(IDC_GEN_ED_NE)->SetWindowText(_T(""));
	GetDlgItem(IDC_GEN_ED_NV)->SetWindowText(_T(""));
	GetDlgItem(IDC_GEN_ED_EV)->SetWindowText(_T(""));

	GetDlgItem(IDC_VER_ED_V)->SetWindowText(_T(""));
	GetDlgItem(IDC_VER_ED_MAXH)->SetWindowText(_T(""));
	GetDlgItem(IDC_VER_ED_MINH)->SetWindowText(_T(""));
	GetDlgItem(IDC_VER_ED_AZI)->SetWindowText(_T(""));
}

void CGlobalTensorDlg::FillDepletionStageVector()
{
	const CModelBase *pModel = dynamic_cast<const CModelBase *> (&m_SourceNode.Model());
	assert(pModel != 0);

	const CDepletionStage *pStage = &pModel->InitialDepletionStage();
	assert(pStage != 0);
	assert(!pStage->Last());
	pStage = &pStage->Next();

	while(pStage)
	{
		m_vcDepletionStages.push_back(pStage);

		if(!pStage->Last()) pStage = &pStage->Next();
		else pStage = 0;
	}
}

void CGlobalTensorDlg::FillDepletionStageCombo()
{
	const CModelBase *pModel = dynamic_cast<const CModelBase *> (&Copy().Model());
	assert(pModel != 0);

	CComboBox *pCombo = (CComboBox *) GetDlgItem(IDC_STAGE);
	pCombo->ResetContent();
	const CDepletionStage *pStage = &pModel->InitialDepletionStage();
	assert(pStage != 0);
	assert(!pStage->Last());
	pStage = &pStage->Next();
	int nIndex = 0;
	while(pStage)
	{
		m_vcDepletionStages.push_back(pStage);
		pCombo->AddString(pStage->Name().toStdString().c_str());

		if(!pStage->Last()) pStage = &pStage->Next();
		else pStage = 0;
		nIndex++;
	}

	pCombo->SetCurSel(m_nDeplStage);
}

void CGlobalTensorDlg::SetRadioButton()
{
	CButton *pBut[3];
	pBut[0] = (CButton*) GetDlgItem(IDC_UNDEFINED);
	pBut[1] = (CButton*) GetDlgItem(IDC_GENERAL);
	pBut[2] = (CButton*) GetDlgItem(IDC_VERTICAL);

	for(int i = 0; i < 3; i++)
	{
		if((int) Copy().GlobalTensorInput(*m_vcDepletionStages[m_nDeplStage]) == i)
			pBut[i]->SetCheck(1);
		else
			pBut[i]->SetCheck(0);
	}
}

void CGlobalTensorDlg::GetRadioButton()
{
	CButton *pBut[3];
	pBut[0] = (CButton*) GetDlgItem(IDC_UNDEFINED);
	pBut[1] = (CButton*) GetDlgItem(IDC_GENERAL);
	pBut[2] = (CButton*) GetDlgItem(IDC_VERTICAL);

	for(int i = 0; i < 3; i++)
	{
		if(pBut[i]->GetCheck() == 1)
		{
			Copy().GlobalTensorInput(*m_vcDepletionStages[m_nDeplStage], (C3DSupportNode::TGlobalTensorInput) i);
			break;
		}
	}
}

BEGIN_MESSAGE_MAP(CGlobalTensorDlg, CDialog)
	//{{AFX_MSG_MAP(CGlobalTensorDlg)
	ON_BN_CLICKED(IDC_GENERAL, OnGeneral)
	ON_BN_CLICKED(IDC_UNDEFINED, OnUndefined)
	ON_BN_CLICKED(IDC_VERTICAL, OnVertical)
	ON_CBN_SELCHANGE(IDC_STAGE, OnSelchangeStage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGlobalTensorDlg message handlers

BOOL CGlobalTensorDlg::OnInitDialog() 
{
	FillDepletionStageVector();
	CAttributesTemplate<C3DSupportNode>::OnInitDialog();

	SetRadioButton();
	FillDepletionStageCombo();

	UpdateControls();
	UpdateData(FALSE);

	m_bInitialized = true;
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CGlobalTensorDlg::OnGeneral() 
{
	if(!UpdateData(TRUE))
	{
		SetRadioButton();
		return;
	}
	GetRadioButton();
	UpdateControls();
	UpdateData(FALSE);
}

void CGlobalTensorDlg::OnUndefined() 
{
	if(!UpdateData(TRUE))
	{
		SetRadioButton();
		return;
	}
	GetRadioButton();
	UpdateControls();
	UpdateData(FALSE);
}

void CGlobalTensorDlg::OnVertical() 
{
	const CDepletionStage &stage = *m_vcDepletionStages[m_nDeplStage];
	if(Copy().GlobalTensorDefined(stage))
	{
		const ITensor &tensor = Copy().GlobalTensor(stage);
		if(fabs(tensor.XZ()) > EPS || fabs(tensor.YZ()) > EPS)
		{
			if(_m()->msg(IDP_GLOBALTENSORVERTICAL, MB_YESNO) == IDNO)
			{
				SetRadioButton();
				return;
			}
		}
	}
	if(!UpdateData(TRUE))
	{
		SetRadioButton();
		return;
	}
	GetRadioButton();
	UpdateControls();
	UpdateData(FALSE);
}

void CGlobalTensorDlg::OnSelchangeStage() 
{
	CComboBox *pCombo = (CComboBox *) GetDlgItem(IDC_STAGE);
	if(!UpdateData(TRUE))
	{
		pCombo->SetCurSel(m_nDeplStage);
		return;
	}
	m_nDeplStage = pCombo->GetCurSel();
	SetRadioButton();
	UpdateControls();
	UpdateData(FALSE);
}

void CGlobalTensorDlg::OnOK() 
{
	for(size_t i = 0; i < m_vcDepletionStages.size(); i++)
	{
		const CDepletionStage *pStage = m_vcDepletionStages[i];
		if(m_SourceNode.GlobalTensorDefined(*pStage) && !Copy().GlobalTensorDefined(*pStage) &&
			m_SourceNode.UsingGlobalTensor(*pStage))
		{
			if(_m()->msg(IDP_SUPPORT_RESETGLOBALTENSOR, MB_YESNO | MB_DEFBUTTON2) == IDNO) return;
			break; // don't ask again
		}
	}

	if(!(Copy() == Source()))
	{
    CModelBase& model = static_cast<CModelBase&>(Copy().Model());
	  model.ResultRegister().ClearLinear(false);
	  model.ResultRegister().ClearNonLinear(false);
	  model.ResultRegister().ClearMixture();
	}

	CAttributesTemplate<C3DSupportNode>::OnOK();
}
