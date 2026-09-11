// intfaceattrdlg.cpp : implementation file
//

#include "stdafx.h"
#include "geomec.h"
#include "intfaceattrdlg.h"
#include "ddxquantity.h"
#include "BoundaryInterfaceDef.h"
#include "ModelBase.h"
#include "BranchState.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CIntFaceAttrDlg dialog


CIntFaceAttrDlg::CIntFaceAttrDlg(CBoundaryInterfaceDef &def, CDoubleQuantity::UNIT unit, CWnd* pParent /*=NULL*/)
: CDialog(CIntFaceAttrDlg::IDD, pParent),
  m_def(def),
  m_unit(unit),
  m_Shear(def.Shear()),
  m_KtanTop(def.KtanTop()),
  m_KtanBottom(def.KtanBottom()),
  m_KtanSides(def.KtanSides()),
  m_KradTop(def.KradTop()),
  m_KradBottom(def.KradBottom()),
  m_KradSides(def.KradSides()),
  m_Lambda(def.Lambda()),
  m_nRadio(-1)
{
	//{{AFX_DATA_INIT(CIntFaceAttrDlg)
	//}}AFX_DATA_INIT

}


void CIntFaceAttrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIntFaceAttrDlg)
  DDX_Control(pDX, IDC_RADIO_LAMBDA, m_RadioLambda);
  DDX_Control(pDX, IDC_RADIO_SHEAR, m_RadioShear);
	DDX_Control(pDX, IDC_RADIO_KS, m_RadioKs);
  DDX_Control(pDX, IDC_EDIT_LAMBDA, m_EdLambda);
  DDX_Control(pDX, IDC_EDIT_SHEAR, m_EdShear);
	DDX_Control(pDX, IDC_EDIT_KRAD_TOP,  m_EdKradTop);
	DDX_Control(pDX, IDC_EDIT_KRAD_BOTTOM,  m_EdKradBottom);
	DDX_Control(pDX, IDC_EDIT_KRAD_SIDES,  m_EdKradSides);
	DDX_Control(pDX, IDC_EDIT_KTAN_TOP,  m_EdKtanTop);
	DDX_Control(pDX, IDC_EDIT_KTAN_BOTTOM,  m_EdKtanBottom);
	DDX_Control(pDX, IDC_EDIT_KTAN_SIDES,  m_EdKtanSides);
  DDX_Radio(pDX, IDC_RADIO_LAMBDA, m_nRadio);
	//}}AFX_DATA_MAP

	DDV_Quantity(pDX, IDC_EDIT_LAMBDA, &m_Lambda, m_unit, "Lambda");
	DDV_Quantity(pDX, IDC_EDIT_SHEAR, &m_Shear, m_unit, "Shear modulus");
	DDV_Quantity(pDX, IDC_EDIT_KRAD_TOP, &m_KradTop, m_unit, "Top Krad");
	DDV_Quantity(pDX, IDC_EDIT_KTAN_TOP, &m_KtanTop, m_unit, "Top Ktan");
	DDV_Quantity(pDX, IDC_EDIT_KRAD_SIDES, &m_KradSides, m_unit, "Sides Krad");
	DDV_Quantity(pDX, IDC_EDIT_KTAN_SIDES, &m_KtanSides, m_unit, "Sides Ktan");
	DDV_Quantity(pDX, IDC_EDIT_KRAD_BOTTOM, &m_KradBottom, m_unit, "Bottom Krad");
	DDV_Quantity(pDX, IDC_EDIT_KTAN_BOTTOM, &m_KtanBottom, m_unit, "Bottom Ktan");

  m_RadioLambda.EnableWindow(!IsBranch());
	m_RadioShear.EnableWindow(!IsBranch());
	m_RadioKs.EnableWindow(!IsBranch());

  if (m_nRadio == 2)
  {
    m_EdLambda.EnableWindow(FALSE);
    m_EdShear.EnableWindow(!IsBranch());
    m_EdKradTop.EnableWindow(FALSE);
    m_EdKtanTop.EnableWindow(FALSE);
    m_EdKradSides.EnableWindow(FALSE);
    m_EdKtanSides.EnableWindow(FALSE);
    m_EdKradBottom.EnableWindow(FALSE);
    m_EdKtanBottom.EnableWindow(FALSE);
  }
  else if (m_nRadio == 1)
  {
    m_EdLambda.EnableWindow(FALSE);
    m_EdShear.EnableWindow(FALSE);
    m_EdKradTop.EnableWindow(!IsBranch());
    m_EdKtanTop.EnableWindow(!IsBranch());
    m_EdKradSides.EnableWindow(!IsBranch());
    m_EdKtanSides.EnableWindow(!IsBranch());
    m_EdKradBottom.EnableWindow(!IsBranch());
    m_EdKtanBottom.EnableWindow(!IsBranch());
  }
  else if (m_nRadio == 0)
  {
    m_EdLambda.EnableWindow(!IsBranch());
    m_EdShear.EnableWindow(FALSE);
    m_EdKradTop.EnableWindow(FALSE);
    m_EdKtanTop.EnableWindow(FALSE);
    m_EdKradSides.EnableWindow(FALSE);
    m_EdKtanSides.EnableWindow(FALSE);
    m_EdKradBottom.EnableWindow(FALSE);
    m_EdKtanBottom.EnableWindow(FALSE);
  }

	if(pDX->m_bSaveAndValidate)
	{
    m_def.Lambda(m_Lambda.Value(m_unit));
		m_def.Shear(m_Shear.Value(m_unit), m_unit);
		m_def.KradTop(m_KradTop.Value(m_unit), m_unit);
		m_def.KtanTop(m_KtanTop.Value(m_unit), m_unit);
		m_def.KradSides(m_KradSides.Value(m_unit), m_unit);
		m_def.KtanSides(m_KtanSides.Value(m_unit), m_unit);
		m_def.KradBottom(m_KradBottom.Value(m_unit), m_unit);
		m_def.KtanBottom(m_KtanBottom.Value(m_unit), m_unit);

    if (m_nRadio == 2)
      m_def.IntMatType(CBoundaryInterfaceDef::G);
    else if (m_nRadio == 1)
      m_def.IntMatType(CBoundaryInterfaceDef::K);
    else if (m_nRadio == 0)
      m_def.IntMatType(CBoundaryInterfaceDef::L);
	}
}

bool CIntFaceAttrDlg::IsBranch() const
{
	return (static_cast<const CModelBase&>(m_def.Model())).BranchState().IsBranch();
}

BEGIN_MESSAGE_MAP(CIntFaceAttrDlg, CDialog)
	//{{AFX_MSG_MAP(CIntFaceAttrDlg)
  ON_BN_CLICKED(IDC_RADIO_LAMBDA, OnRadioLambda)
  ON_BN_CLICKED(IDC_RADIO_SHEAR, OnRadioShear)
	ON_BN_CLICKED(IDC_RADIO_KS, OnRadioKs)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIntFaceAttrDlg message handlers

BOOL CIntFaceAttrDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	GetDlgItem(IDC_UNIT_G)->SetWindowText(m_def.Shear().UnitName(m_unit).c_str());
	GetDlgItem(IDC_UNIT_KR_TOP)->SetWindowText(m_def.KradTop().UnitName(m_unit).c_str());
	GetDlgItem(IDC_UNIT_KT_TOP)->SetWindowText(m_def.KtanTop().UnitName(m_unit).c_str());
	GetDlgItem(IDC_UNIT_KR_SIDES)->SetWindowText(m_def.KradSides().UnitName(m_unit).c_str());
	GetDlgItem(IDC_UNIT_KT_SIDES)->SetWindowText(m_def.KtanSides().UnitName(m_unit).c_str());
	GetDlgItem(IDC_UNIT_KR_BOTTOM)->SetWindowText(m_def.KradBottom().UnitName(m_unit).c_str());
	GetDlgItem(IDC_UNIT_KT_BOTTOM)->SetWindowText(m_def.KtanBottom().UnitName(m_unit).c_str());

	switch(m_def.IntMatType())
	{
	case CBoundaryInterfaceDef::G:
    m_nRadio = 2;
		break;
	case CBoundaryInterfaceDef::K:
    m_nRadio = 1;
		break;
  case CBoundaryInterfaceDef::L:
    m_nRadio = 0;
    break;
  default:
		assert(FALSE);
	};
	
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CIntFaceAttrDlg::OnRadioLambda()
{
  m_nRadio = 0;
  UpdateData(FALSE);
}

void CIntFaceAttrDlg::OnRadioShear() 
{
  m_nRadio = 2;
	UpdateData(FALSE);
}

void CIntFaceAttrDlg::OnRadioKs() 
{
  m_nRadio = 1;
	UpdateData(FALSE);	
}

void CIntFaceAttrDlg::OnOK() 
{
	m_def.ClearInterfaceMaterials();	
	CDialog::OnOK();
}
