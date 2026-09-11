
#include "stdafx.h"
#include "Geomec.h"
#include "ResultMaterialSelectDlg.h"
#include "ModelBase.h"
#include "RpnMaterialParameterProxy.h"
#include "DepletionStage.h"
#include "MatLibListCtrl.h"


// CResultMaterialSelectDlg dialog

IMPLEMENT_DYNAMIC(CResultMaterialSelectDlg, CDialog)

CResultMaterialSelectDlg::CResultMaterialSelectDlg(CModelBase& model, const std::vector<IRpnMaterialParameterProxy*> &materialProxies, CWnd* pParent /*=NULL*/)
	: CDialog(CResultMaterialSelectDlg::IDD, pParent)
  , m_Model(model)
  , m_vcMaterialProxies(materialProxies)
  , m_pTimeStep(0)
{

}

CResultMaterialSelectDlg::~CResultMaterialSelectDlg()
{
}

BOOL CResultMaterialSelectDlg::OnInitDialog() 
{
  CDialog::OnInitDialog();
  SetupDeplCombo();
  SetupMatParamListBox();
  return TRUE;
}

void CResultMaterialSelectDlg::SetupDeplCombo()
{
  // the depletion stages combo
  const CDepletionStage* pStage = &m_Model.InitialDepletionStage();
  while(pStage)
  {
	  int nIndex = m_cbTimeSteps.AddString(pStage->Name().toStdString().c_str());
      m_cbTimeSteps.SetItemData(nIndex, (DWORD_PTR)pStage);

      // Go to next stage if any
	  if(pStage->Last())
		  pStage = 0;
	  else
		  pStage =  &pStage->Next();
  }
  m_cbTimeSteps.SetCurSel(0);
  OnCbnSelchangeTimeStep();
  m_cbTimeSteps.EnableWindow(FALSE);
}

void CResultMaterialSelectDlg::SetupMatParamListBox()
{
  m_lcParameters.InsertColumn( 0, "Material Parameters");

  m_ImageList.Create(16, 16, ILC_COLOR, 0, 2);


  // waij TFS 80121 add icons
  for(size_t i = 0; i < m_vcMaterialProxies.size(); ++i)
  if ( ! m_vcMaterialProxies[i]->Obsolete() )
  {
    m_ImageList.Add( AfxGetApp()->LoadIcon(m_vcMaterialProxies[i]->IconId()));
  }

  m_lcParameters.SetImageList(&m_ImageList, LVSIL_SMALL);

  int nItem= 0;
  for(size_t i = 0; i < m_vcMaterialProxies.size(); ++i)
  if ( ! m_vcMaterialProxies[i]->Obsolete() ) // waij TFS 77889
  {
    int nIndex = m_lcParameters.InsertItem(nItem, m_vcMaterialProxies[i]->DisplayName().toStdString().c_str(), nItem);
    m_lcParameters.SetItemData(nIndex, (DWORD_PTR)m_vcMaterialProxies[i]);
    ++nItem;
  }

  OnLbnSelchangeRmsParametersList();

  AdjustColumnWidth(this, &m_lcParameters);
  Invalidate();
  UpdateWindow();
}

void CResultMaterialSelectDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_RMS_PARAMETERS_LIST, m_lcParameters);
  DDX_Control(pDX, IDC_RMS_TIMESTEP_COMBO, m_cbTimeSteps);

  DDX_Control(pDX, IDC_CHECK_SET_DEPLETION, m_chSetDepletion);
}


BEGIN_MESSAGE_MAP(CResultMaterialSelectDlg, CDialog)
  ON_CBN_SELCHANGE(IDC_RMS_TIMESTEP_COMBO, &CResultMaterialSelectDlg::OnCbnSelchangeTimeStep)
  ON_LBN_SELCHANGE(IDC_RMS_PARAMETERS_LIST, &CResultMaterialSelectDlg::OnLbnSelchangeRmsParametersList)
  ON_BN_CLICKED(IDOK, &CResultMaterialSelectDlg::OnBnClickedOk)
  ON_BN_CLICKED(IDC_CHECK_SET_DEPLETION, &CResultMaterialSelectDlg::OnBnClickedCheckSetDepletion)
END_MESSAGE_MAP()


// CResultMaterialSelectDlg message handlers
void CResultMaterialSelectDlg::OnCbnSelchangeTimeStep()
{
  m_pTimeStep = (CDepletionStage*)(m_cbTimeSteps.GetItemData(m_cbTimeSteps.GetCurSel()));
}

void CResultMaterialSelectDlg::OnLbnSelchangeRmsParametersList()
{
}

void CResultMaterialSelectDlg::OnBnClickedOk()
{
  POSITION pos = m_lcParameters.GetFirstSelectedItemPosition();
  while ( pos != 0 )
  {
    int nItem = m_lcParameters.GetNextSelectedItem(pos);
    IRpnMaterialParameterProxy* pMaterialParameterProxy= 
      (IRpnMaterialParameterProxy*) (m_lcParameters.GetItemData(nItem));
    m_pSelectedProxies.insert( m_pSelectedProxies.begin(), pMaterialParameterProxy);
  }

  if((m_chSetDepletion.GetCheck() == BST_CHECKED))
  {
    for (std::vector <IRpnMaterialParameterProxy*>::const_iterator
      selectedProxy = m_pSelectedProxies.begin();
      selectedProxy != m_pSelectedProxies.end(); ++selectedProxy)
    {
      (*selectedProxy)->TimeStep(m_pTimeStep);
      (*selectedProxy)->FixedStage(true);
    }
  }

  OnOK();
}

void CResultMaterialSelectDlg::OnBnClickedCheckSetDepletion()
{
  m_cbTimeSteps.EnableWindow((m_chSetDepletion.GetCheck() == BST_CHECKED) ? TRUE : FALSE);
}
