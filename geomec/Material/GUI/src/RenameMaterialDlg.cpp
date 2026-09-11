// RenameMaterialDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "RenameMaterialDlg.h"
#include "mlMaterial.h"
#include "GlobalMessage.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CRenameMaterialDlg dialog


CRenameMaterialDlg::CRenameMaterialDlg(const QString& sName, mlMatModel matmodel, bool bAllowModelChange, int nFilter, CWnd* pParent /*=NULL*/)
: CDialog(CRenameMaterialDlg::IDD, pParent),
  m_strName(sName),
  m_matmodel(matmodel),
  m_bAllowModelChange(bAllowModelChange),
  m_nModelFilter(nFilter),
  m_iModelComboIndex(-1)
{
  AddListedModel(MM_LINEAR);
  AddListedModel(MM_CAMCLAY);
  AddListedModel(MM_MOHRCOULOMB);
  AddListedModel(MM_MC_COHESION_HARD1);
  AddListedModel(MM_MC_COHESION_HARD2);
  AddListedModel(MM_MC_COHESION_HARD3);
  AddListedModel(MM_MC_FRICTION_HARD1);
  AddListedModel(MM_MC_FRICTION_HARD2);
  AddListedModel(MM_MC_FRICTION_HARD3);
  AddListedModel(MM_MODIFIEDMOHRCOULOMB);
  AddListedModel(MM_CREEP);
  AddListedModel(MM_UNDRAINED);
  AddListedModel(MM_RIGIDITY);
  AddListedModel(MM_DUALCAP_LINELA);
  AddListedModel(MM_FRACTURE_ANISOTROPY);
  AddListedModel(MM_UPSCALED_ANISOTROPY);
  AddListedModel(MM_ANISOTROPIC_CAMCLAY);
  AddListedModel(MM_FRACTURE_APERTURE2);
  AddListedModel(MM_CAMCLAY_CREEP);
}

const QString& CRenameMaterialDlg::Name() const
{
  return m_strName;
}

mlMatModel CRenameMaterialDlg::MaterialModel() const
{
  return m_matmodel;
}

void CRenameMaterialDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);

  DDX_Control(pDX, IDC_COMBO_MAT_TYPE, m_MaterialTypeCombo);
  DDX_CBIndex(pDX, IDC_COMBO_MAT_TYPE, m_iModelComboIndex);
}

void CRenameMaterialDlg::OnOK() 
{
  // let's see if the entered name is valid
  CString strName;
  GetDlgItem( IDC_EDITNAME )->GetWindowText( strName );

  if(strName.IsEmpty())
  {
    _m()->msg( "Please enter a valid name", MB_OK | MB_ICONEXCLAMATION | MB_HELP );
    GetDlgItem( IDC_EDITNAME )->SetFocus();
    ( (CEdit *) GetDlgItem( IDC_EDITNAME ) )->SetSel( 0, -1 );
    return;
  }

  UpdateData(TRUE);

  if(m_iModelComboIndex >= 0)
  {
  assert(m_iModelComboIndex < m_vcListedModels.size());
  m_matmodel = (mlMatModel)m_vcListedModels[m_iModelComboIndex];
  }

  m_strName = (LPCSTR)strName;
  CDialog::OnOK();
}

void CRenameMaterialDlg::AddListedModel(mlMatModel mm)
{
  if(FilterContainsMaterialModel(mm, m_nModelFilter))
  m_vcListedModels.push_back(mm);
}

BOOL CRenameMaterialDlg::OnInitDialog() 
{
  CDialog::OnInitDialog();

  if(!m_vcListedModels.empty())
  FillModelListBox();

  size_t i;
  for(i = 0; i < m_vcListedModels.size(); ++i)
  {
  if(m_vcListedModels[i] == m_matmodel)
  {
      m_iModelComboIndex = i;
      break;
  }
  }

  m_MaterialTypeCombo.EnableWindow(m_bAllowModelChange);

  CEdit *pEdit = (CEdit *) GetDlgItem( IDC_EDITNAME );
  pEdit->SetFocus();
  pEdit->SetWindowText( m_strName.toStdString().c_str() );
  pEdit->SetSel( 0, -1 );

  UpdateData(FALSE);
  
  return FALSE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

void CRenameMaterialDlg::AddModelToCombo(mlMatModel nModel)
{
  m_MaterialTypeCombo.AddString(MaterialModelName(nModel).toStdString().c_str());
}

void CRenameMaterialDlg::FillModelListBox()
{
  assert(!m_vcListedModels.empty());

  for(size_t i = 0; i < m_vcListedModels.size(); i++)
    AddModelToCombo(m_vcListedModels[i]);
  m_MaterialTypeCombo.SetCurSel(0);
}
