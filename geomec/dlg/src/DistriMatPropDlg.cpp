#include "StdAfx.h"
#include "geomec.h"

#include "DistriMatPropDlg.h"
#include "ValueType.h"

BEGIN_MESSAGE_MAP(CDistriMatPropDlg, CDialog)
  ON_BN_CLICKED(IDC_RD_EXTRA_POLATE, OnExtrapolate)
  ON_BN_CLICKED(IDC_RD_MATLIB_VAL, OnMatLibVal)
  ON_BN_CLICKED(IDC_APPLYTOALL, OnApplyToAll)
END_MESSAGE_MAP()

CDistriMatPropDlg::CDistriMatPropDlg(const std::vector<const CValueType*>& vcValueTypes, std::set<const CValueType*>& stExtrapolatingValueTypes, CWnd* pParent)
: CDialog(IDD_CONFIG_DISTRI_MAT, pParent),
  m_vcValueTypes(vcValueTypes),
  m_stSource(stExtrapolatingValueTypes),
  m_stCopy(stExtrapolatingValueTypes),
  m_pCurrent(0)
{
}

CDistriMatPropDlg::~CDistriMatPropDlg()
{
}

void CDistriMatPropDlg::OnValueTypeSelected(const CValueType& valuetype)
{
  m_pCurrent = &valuetype;
  UpdateRadioButtons();
}

void CDistriMatPropDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);

  DDX_Control(pDX, IDC_TREE_PROPERTIES, m_tcProperties);
}

BOOL CDistriMatPropDlg::OnInitDialog()
{
  CDialog::OnInitDialog();

  PopulateTree();
  HTREEITEM hRoot = m_tcProperties.GetRootItem();
  m_tcProperties.Expand(hRoot, TVE_EXPAND);

  UpdateRadioButtons();

  return TRUE;
}

void CDistriMatPropDlg::OnOK()
{
  m_stSource = m_stCopy;
  CDialog::OnOK();
}

void CDistriMatPropDlg::PopulateTree()
{
  for(size_t i = 0; i < m_vcValueTypes.size(); ++i)
    new CMatPropObserver(*m_vcValueTypes[i], m_tcProperties, *this);
}

// wjrx mantis 3132
void CDistriMatPropDlg::OnApplyToAll()
{
  assert(m_pCurrent);
  bool bExtrapolate = (m_stCopy.find(m_pCurrent) != m_stCopy.end());

  for(size_t i = 0; i < m_vcValueTypes.size(); ++i)
  {
    if (bExtrapolate)
      m_stCopy.insert(m_vcValueTypes[i]);
    else
      m_stCopy.erase(m_vcValueTypes[i]);
  }
}

void CDistriMatPropDlg::OnExtrapolate()
{
  assert(m_pCurrent);
  m_stCopy.insert(m_pCurrent);
  UpdateRadioButtons();
}

void CDistriMatPropDlg::OnMatLibVal()
{
  assert(m_pCurrent);
  m_stCopy.erase(m_pCurrent);
  UpdateRadioButtons();
}

void CDistriMatPropDlg::UpdateRadioButtons()
{
  CButton* pRadioExtrapolate = (CButton*)GetDlgItem(IDC_RD_EXTRA_POLATE);
  CButton* pRadioMatLibVal = (CButton*)GetDlgItem(IDC_RD_MATLIB_VAL);
  CButton* pApplyToAll = (CButton*)GetDlgItem(IDC_APPLYTOALL);

  pRadioExtrapolate->EnableWindow(m_pCurrent != 0);
  pRadioMatLibVal->EnableWindow(m_pCurrent != 0);
  pApplyToAll->EnableWindow(m_pCurrent != 0);

  if(m_pCurrent)
  {
    bool bExtrapolate = (m_stCopy.find(m_pCurrent) != m_stCopy.end());
    pRadioExtrapolate->SetCheck(bExtrapolate ? 1 : 0);
    pRadioMatLibVal->SetCheck(bExtrapolate ? 0 : 1);
  }
  else
  {
    pRadioExtrapolate->SetCheck(0);
    pRadioMatLibVal->SetCheck(0);
  }
}


///// CDistrMatPropDlg::CMatPropObserver

CDistriMatPropDlg::CMatPropObserver::CMatPropObserver(const CValueType& valuetype, CTreeCtrl& ctrl, CDistriMatPropDlg& dlg)
: ITreeObject(ctrl, TVI_ROOT, TVI_LAST),
  m_valuetype(valuetype),
  m_dlg(dlg)
{
  ctrl.SetItemData(Handle(), (DWORD_PTR)this);
}

BOOL CDistriMatPropDlg::CMatPropObserver::CanDelete()
{
  return FALSE;
}

BOOL CDistriMatPropDlg::CMatPropObserver::OnSelect()
{
  m_dlg.OnValueTypeSelected(m_valuetype);
  return TRUE;
}

QString CDistriMatPropDlg::CMatPropObserver::Text() const
{
  return m_valuetype.Name();
}

unsigned int CDistriMatPropDlg::CMatPropObserver::Icon() const
{
  return m_valuetype.IconId();
}
