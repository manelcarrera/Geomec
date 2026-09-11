// GVTStartDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Geomec.h"
#include "BaseEntryTypes.h"
#include "GVTStartDlg.h"
#include "GVTSeismicGridDlg.h"
#include "GVTSettings.h"
#include "GVTSeismicGridDefinition.h"
#include "GeomecUtils.h"
#include "TnoFileDialog.h"
#include "DepletionStage.h"

// CGVTSettingsDlg dialog

IMPLEMENT_DYNAMIC(CGVTSettingsDlg, CDialog)

CGVTSettingsDlg::CGVTSettingsDlg(CGVTSettings &settings, CWnd* pParent /*=NULL*/)
  : CDialog(CGVTSettingsDlg::IDD, pParent),
  m_SourceSettings(settings),
  m_GVTSettings(settings),
  m_strVelocityModel(settings.VelocityModelFile().toStdString().c_str()),
  m_strOutputPrefix(settings.OutputPrefix().toStdString().c_str()),
  m_strOutputDirectory(settings.OutputDirectory().toStdString().c_str()),
  m_ndvp(1),
  m_ndt(1),
  m_nImportBack(1),
  m_lbTimeLapse(settings.Model())
, m_velocitySource(settings.velocitySource())
{
  //{{AFX_DATA_INIT(CGVTSettingsDlg)
  //}}AFX_DATA_INIT
}

CGVTSettingsDlg::~CGVTSettingsDlg()
{
}

BOOL CGVTSettingsDlg::OnInitDialog()
{
  CDialog::OnInitDialog();
  SetupFormationListCtrl();
  SetupParamsListCtrl();
  SetupTimeLapseListCtrl();
  GetDlgItem(IDC_GVT_PREFIX_EDIT)->SetWindowTextA(m_GVTSettings.OutputPrefix().toStdString().c_str());
  GetDlgItem(IDC_GVT_PREFIX_EDIT)->EnableWindow(FALSE);
  GetDlgItem(IDC_GVT_VELMODEL_EDIT)->SetWindowTextA(m_GVTSettings.VelocityModelFile().toStdString().c_str());
  GetDlgItem(IDC_GVT_OUTPUT_EDIT)->SetWindowTextA(m_GVTSettings.OutputDirectory().toStdString().c_str());
  GetDlgItem(IDC_GVT_OUTPUT_EDIT)->EnableWindow(FALSE);

  GetDlgItem(IDC_GVT_DT_CHECK)->EnableWindow(FALSE);
  GetDlgItem(IDC_GVT_DVP_CHECK)->EnableWindow(FALSE);
  GetDlgItem(IDC_GVT_IMPORT_CHECK)->EnableWindow(FALSE);

  m_velocitySource = m_GVTSettings.velocitySource();

  UpdateData(FALSE);

  setupVelocityP();

  return TRUE;
}

void CGVTSettingsDlg::SetupTimeLapseListCtrl()
{
  CRect rect;
  m_lbTimeLapse.GetWindowRect(&rect);
  m_lbTimeLapse.InsertColumn(0, _T("Scenario"), LVCFMT_LEFT,(rect.Width() - 3) * 0.20, 0);
  m_lbTimeLapse.InsertColumn(1, _T("Depl. Stage 1"), LVCFMT_LEFT,(rect.Width() - 3) * 0.40, 1);
  m_lbTimeLapse.InsertColumn(2, _T("Depl. Stage 2"), LVCFMT_LEFT,(rect.Width() - 3) * 0.40, 1);
  m_lbTimeLapse.SetComboColumns(1);
  m_lbTimeLapse.SetComboColumns(2);

  for(size_t i = 0; i < m_GVTSettings.TimeLapses().size(); ++i)
  {
  new CTimeLapseListObject<CGVTSettings>(m_GVTSettings.Model(), m_lbTimeLapse, 
      m_GVTSettings.TimeLapses()[i].first, m_GVTSettings.TimeLapses()[i].second,
      &m_GVTSettings);
  }
  if(m_lbTimeLapse.GetItemCount() == 0)
  {
  CButton *pbut = (CButton*)GetDlgItem(IDC_REMOVE_TIMELAPSE_BUTTON);
  pbut->EnableWindow(FALSE);
  }
}

void CGVTSettingsDlg::SetupParamsListCtrl()
{
  CRect rect;
  m_lbParameters.GetWindowRect(&rect);
  m_lbParameters.InsertColumn(0, _T("Parameter"), LVCFMT_LEFT,(rect.Width() - 3) * 0.50, 0);
  m_lbParameters.InsertColumn(1, _T("Value"), LVCFMT_LEFT,(rect.Width() - 3) * 0.50, 1);

  std::map<CFormationBase*, CGVTVelocityModel*>::const_iterator it;
  it = m_GVTSettings.FormationToVelModelMp().begin();
  std::map<QString, double>::const_iterator itParams;
  for(itParams = it->second->Parameters().begin(); itParams != it->second->Parameters().end(); ++itParams)
  {
  new CParametersListObject(m_lbParameters, it->second, itParams->first.toStdString().c_str());
  }

}

void CGVTSettingsDlg::SetupFormationListCtrl()
{
  CRect rect;
  m_lbFormations.GetWindowRect(&rect);

  m_lbFormations.InsertColumn(0, _T("Formations"), LVCFMT_LEFT,(rect.Width() - 3) * 0.65, 0);
  m_lbFormations.InsertColumn(1, _T("Velocity Model"), LVCFMT_LEFT,(rect.Width() - 3) * 0.35, 1);

  m_lbFormations.SetComboColumns(1);

  std::map<CFormationBase*, CGVTVelocityModel*>::const_iterator it;
  for(it = m_GVTSettings.FormationToVelModelMp().begin(); it != m_GVTSettings.FormationToVelModelMp().end(); it++)
  {
     new CFormationListObject(m_lbFormations, it->first, it->second, *this);
  }
  m_lbFormations.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);

}

void CGVTSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  UpdateSurveyBox();  

  if(!pDX->m_bSaveAndValidate)
  {
  GetDlgItem(IDC_GVT_PREFIX_EDIT)->GetWindowTextA(m_strOutputPrefix);
  GetDlgItem(IDC_GVT_VELMODEL_EDIT)->GetWindowTextA(m_strVelocityModel);
  GetDlgItem(IDC_GVT_OUTPUT_EDIT)->GetWindowTextA(m_strOutputDirectory);
  }

  //{{AFX_DATA(CGVTSettingsDlg)
  DDX_Control(pDX, IDC_GVT_FORMATION_LIST, m_lbFormations);
  DDX_Control(pDX, IDC_GVT_PARAMETERS_LIST, m_lbParameters);
  DDX_Control(pDX, IDC_GVT_TIMELAPSE_LIST, m_lbTimeLapse);
  DDX_Text(pDX, IDC_GVT_VELMODEL_EDIT, m_strVelocityModel);
  DDX_Text(pDX, IDC_GVT_PREFIX_EDIT, m_strOutputPrefix);
  DDX_Text(pDX, IDC_GVT_OUTPUT_EDIT, m_strOutputDirectory);
  DDX_Check(pDX, IDC_GVT_DT_CHECK, m_ndvp);
  DDX_Check(pDX, IDC_GVT_DVP_CHECK, m_ndt);
  DDX_Check(pDX, IDC_GVT_IMPORT_CHECK, m_nImportBack);
  DDX_Radio(pDX, IDC_GVT_VELOCITY_FROM_FILE, m_velocitySource);
  //}}AFX_DATA

  if(pDX->m_bSaveAndValidate)
  {
  m_GVTSettings.VelocityModelFile((LPCSTR) m_strVelocityModel);
  m_GVTSettings.OutputPrefix((LPCSTR) m_strOutputPrefix);
  m_GVTSettings.OutputDirectory((LPCSTR) m_strOutputDirectory);
  m_GVTSettings.DeltaVp(m_ndvp == 1);
  m_GVTSettings.Deltat(m_ndt == 1);
  m_GVTSettings.ImportBack(m_nImportBack == 1);
  m_GVTSettings.velocitySource(m_velocitySource);
  if (m_GVTSettings.velocitySource() != m_velocitySource)
  {
      m_velocitySource = m_GVTSettings.velocitySource();
      UpdateData(false);
      DDX_Radio(pDX, IDC_GVT_VELOCITY_FROM_FILE, m_velocitySource);
      pDX->Fail();
  }
  m_SourceSettings = m_GVTSettings;
  }
}

BEGIN_MESSAGE_MAP(CGVTSettingsDlg, CDialog)
  ON_BN_CLICKED(IDC_GVT_DEFINE_NEW, &CGVTSettingsDlg::OnBnClickedDefineNew)
  ON_CBN_SELCHANGE(IDC_GVT_SURVEY_COMBO, &CGVTSettingsDlg::OnCbnSelchangeGvtSurveyCombo)
  ON_BN_CLICKED(IDC_GVT_EDIT_BUT, &CGVTSettingsDlg::OnBnClickedGvtEditBut)
  ON_BN_CLICKED(IDC_GVT_BROWSE_VELOCITY, &CGVTSettingsDlg::OnBnClickedGvtBrowseVelocity)
  ON_BN_CLICKED(IDC_GVT_BROWSE_OUTPUT, &CGVTSettingsDlg::OnBnClickedGvtBrowseOutput)
  ON_NOTIFY(NM_CLICK, IDC_GVT_FORMATION_LIST, &CGVTSettingsDlg::OnNMClickGvtFormationList)
  ON_BN_CLICKED(IDC_ADD_TIMELAPSE_BUTTON, &CGVTSettingsDlg::OnBnClickedAddTimelapseButton)
  ON_BN_CLICKED(IDC_REMOVE_TIMELAPSE_BUTTON, &CGVTSettingsDlg::OnBnClickedRemoveTimelapseButton)
  ON_BN_CLICKED(IDC_GVT_DELETE_BUT, &CGVTSettingsDlg::OnBnClickedGvtDeleteBut)
  ON_BN_CLICKED(IDOK, &CGVTSettingsDlg::OnBnClickedOk)
  ON_BN_CLICKED(IDC_GVT_VELOCITY_FROM_FILE,
  &CGVTSettingsDlg::OnBnClickedGvtVelocityFromFile)
  ON_BN_CLICKED(IDC_GVT_VELOCITY_FROM_MATERIAL,
  &CGVTSettingsDlg::OnBnClickedGvtVelocityFromMaterial)
  ON_BN_CLICKED(IDC_GVT_DT_CHECK, &CGVTSettingsDlg::OnBnClickedGvtCheckbox)
  ON_BN_CLICKED(IDC_GVT_DVP_CHECK, &CGVTSettingsDlg::OnBnClickedGvtCheckbox)
  ON_BN_CLICKED(IDC_GVT_IMPORT_CHECK, &CGVTSettingsDlg::OnBnClickedGvtCheckbox)
END_MESSAGE_MAP()


// CGVTSettingsDlg message handlers

void CGVTSettingsDlg::OnBnClickedDefineNew()
{
  CGVTSeismicGridDefinition *grid = m_GVTSettings.NewGrid();
  CGVTSeismicGridDlg dlg(*grid, m_GVTSettings.Model());
  if(dlg.DoModal() != IDOK)
  {
  m_GVTSettings.RemoveGrid(grid);
  }
  else
  {
  GetDlgItem(IDC_GVT_EDIT_BUT)->EnableWindow(TRUE);
  GetDlgItem(IDC_GVT_DELETE_BUT)->EnableWindow(TRUE);
  m_GVTSettings.CurrentGrid(m_GVTSettings.NoOfGrids() - 1);
  UpdateData(FALSE);
  }
}

void CGVTSettingsDlg::OnBnClickedGvtEditBut()
{
  CGVTSeismicGridDlg dlg(m_GVTSettings.Grid(m_GVTSettings.CurrentGrid()), m_GVTSettings.Model());
  dlg.DoModal();
}

void CGVTSettingsDlg::OnBnClickedGvtDeleteBut()
{
  m_GVTSettings.RemoveGrid(m_GVTSettings.CurrentGrid());
  UpdateData(FALSE);
}

void CGVTSettingsDlg::UpdateSurveyBox()
{
  CComboBox *surveys = (CComboBox*)GetDlgItem(IDC_GVT_SURVEY_COMBO);
  surveys->ResetContent();
  if(m_GVTSettings.NoOfGrids() == 0)
  {
  surveys->AddString("<No Surveys>");
  GetDlgItem(IDC_GVT_EDIT_BUT)->EnableWindow(FALSE);
  GetDlgItem(IDC_GVT_DELETE_BUT)->EnableWindow(FALSE);
  }
  for(int i = 0; i < m_GVTSettings.NoOfGrids(); ++i)
  {
  surveys->AddString(m_GVTSettings.Grid(i).Name().toStdString().c_str());
  }
  if(m_GVTSettings.CurrentGrid() < 0)
  surveys->SetCurSel(0);
  else
  surveys->SetCurSel(m_GVTSettings.CurrentGrid());
}

void CGVTSettingsDlg::OnCbnSelchangeGvtSurveyCombo()
{
  CComboBox *surveys = (CComboBox*)GetDlgItem(IDC_GVT_SURVEY_COMBO);
  m_GVTSettings.CurrentGrid(surveys->GetCurSel());
}

void CGVTSettingsDlg::OnBnClickedGvtBrowseVelocity()
{
  CString sFilter = "All Files (*.*)|*.*||";
  CTnoFileDialog dlg(TRUE, 
          NULL, 
          NULL,
          NULL,
          sFilter);
  if(dlg.DoModal() == IDOK)
  {
  GetDlgItem(IDC_GVT_VELMODEL_EDIT)->SetWindowTextA(dlg.GetPathName());
  }
}

void CGVTSettingsDlg::OnBnClickedGvtBrowseOutput()
{
  CString strPath = BrowseFolder(GetSafeHwnd(), "Browse", (LPCSTR) m_strOutputDirectory).toStdString().c_str();
  if(!strPath.IsEmpty() && DirExists((LPCSTR) strPath))
  {
  GetDlgItem(IDC_GVT_OUTPUT_EDIT)->SetWindowTextA(strPath);
  }
}

void CGVTSettingsDlg::OnNMClickGvtFormationList(NMHDR *pNMHDR, LRESULT *pResult)
{
  POSITION pos = m_lbFormations.GetFirstSelectedItemPosition();
  if (pos == NULL)
  return;
  else
  {
     while (pos)
     {
    int nItem = m_lbFormations.GetNextSelectedItem(pos);
    SetParametersList(nItem);
    // you could do your own processing on nItem here
     }
  }
  *pResult = 0;
}

void CGVTSettingsDlg::SetParametersList(int nItem)
{
  CFormationListObject *pObj = (CFormationListObject*)m_lbFormations.GetObject(nItem);
  CGVTVelocityModel *pVelMod = pObj->VelocityModel();

  m_lbParameters.DeleteAllItems();
  std::map<QString, double>::iterator itParams;
  for(itParams = pVelMod->Parameters().begin(); itParams != pVelMod->Parameters().end(); ++itParams)
  {
  new CParametersListObject(m_lbParameters, pVelMod, itParams->first.toStdString().c_str());
  }
}

void CGVTSettingsDlg::OnBnClickedAddTimelapseButton()
{
  TDepletionStageEntry* pEntry = (TDepletionStageEntry*)(GVTSettings().Model().GraphEntry(MD_BASE_DEPLETION_STAGE));
  TDepletionStageEntry::TNodeSet stDepletionStage = pEntry->EntryNodes();
  TDepletionStageEntry::TNodeSet::const_iterator it;
  it = stDepletionStage.begin();
  CDepletionStage *depl1 = (*it);
  CDepletionStage *depl2 = (*++it);
  m_GVTSettings.AddTimeLapse(depl1, depl2);
  new CTimeLapseListObject<CGVTSettings>(m_GVTSettings.Model(), m_lbTimeLapse, depl1, depl2, &m_GVTSettings);
  CButton *pbut = (CButton*)GetDlgItem(IDC_REMOVE_TIMELAPSE_BUTTON);
  pbut->EnableWindow();
}

void CGVTSettingsDlg::OnBnClickedRemoveTimelapseButton()
{
  m_GVTSettings.RemoveTimeLapse(m_GVTSettings.TimeLapses().size()-1);
  m_lbTimeLapse.DeleteItem(m_lbTimeLapse.GetItemCount()-1);
  if(m_lbTimeLapse.GetItemCount() == 0)
  {
  CButton *pbut = (CButton*)GetDlgItem(IDC_REMOVE_TIMELAPSE_BUTTON);
  pbut->EnableWindow(FALSE);
  }
}

void CGVTSettingsDlg::OnBnClickedOk()
{
  UpdateData(FALSE);
  /*
  if(m_strOutputPrefix == "")
  {
  _m()->msg("The 'Output prefix' can not be empty");
  return;
  }
  */
  OnOK();
}

void CGVTSettingsDlg::OnBnClickedGvtVelocityFromFile()
{
  UpdateData(TRUE);
  setupVelocityP();
}

void CGVTSettingsDlg::OnBnClickedGvtVelocityFromMaterial()
{
  UpdateData(TRUE);
  setupVelocityP();
}

void CGVTSettingsDlg::OnBnClickedGvtCheckbox()
{
  UpdateData(TRUE);
}

// ******************************************************************
// CTimeLapseListObject implementation
// ******************************************************************
/*CGVTSettingsDlg::CTimeLapseListObject::CTimeLapseListObject(CListCtrl &ctrl, CDepletionStage *t1, CDepletionStage *t2, CGVTSettings *settings)
:IListObject(ctrl),
 m_t1(t1),
 m_t2(t2),
 m_GVTSettings(settings)
{
  Ctrl().SetItemData(Ctrl().GetItemCount() - 1, (DWORD_PTR) this);
  new CTimeLapseSubObject(*this, 1);
  new CTimeLapseSubObject(*this, 2);
}

QString CGVTSettingsDlg::CTimeLapseListObject::Text() const
{
  QString strText;
  strText = QString("Scenario %1").arg(Index());
  return strText;
}

BOOL CGVTSettingsDlg::CTimeLapseListObject::CanEditTet() const
{
  return FALSE;
}

unsigned int CGVTSettingsDlg::CTimeLapseListObject::Icon() const
{
  return 0;
}

void CGVTSettingsDlg::CTimeLapseListObject::SetTime1(const CString &strDepl)
{
  m_t1 = Update(strDepl);
  m_GVTSettings->SetTimeLapse(Index(), m_t1, m_t2);
}

void CGVTSettingsDlg::CTimeLapseListObject::SetTime2(const CString &strDepl)
{
  m_t2 = Update(strDepl);
  m_GVTSettings->SetTimeLapse(Index(), m_t1, m_t2);
}

CDepletionStage *CGVTSettingsDlg::CTimeLapseListObject::Update(const CString &strDepl)
{
  TDepletionStageEntry* pEntry = (TDepletionStageEntry*)(m_GVTSettings->Model().GraphEntry(MD_BASE_DEPLETION_STAGE));
  TDepletionStageEntry::TNodeSet stDepletionStage = pEntry->EntryNodes();
  TDepletionStageEntry::TNodeSet::const_iterator it;
  for(it = stDepletionStage.begin(); it != stDepletionStage.end(); ++it)
  {
  if((*it)->Name() == strDepl)
      return *it;
  }
  assert(FALSE);
  return 0;
}

// ******************************************************************
// CTimeLapseSubObject implementation
// ******************************************************************
CGVTSettingsDlg::CTimeLapseSubObject::CTimeLapseSubObject(CTimeLapseListObject &parent, int nIndex)
:ISubListObject(parent, nIndex),
 m_parent(parent)
{
}

QString CGVTSettingsDlg::CTimeLapseSubObject::Text() const
{
  if(Index() == 1)
  return m_parent.Time1()->Name();
  else
  return m_parent.Time2()->Name();
}

BOOL CGVTSettingsDlg::CTimeLapseSubObject::EditText(const QString& strText)
{
  if(Index() == 1)
  m_parent.SetTime1(strText);
  else
  m_parent.SetTime2(strText);
  return TRUE;
}

BOOL CGVTSettingsDlg::CTimeLapseSubObject::CanEditText() const
{
  return TRUE;
}*/

// ******************************************************************
// CParametersListObject implementation
// ******************************************************************
CGVTSettingsDlg::CParametersListObject::CParametersListObject(CListCtrl &ctrl, CGVTVelocityModel *velmod, CString param)
:IListObject(ctrl),
 m_VelMod(velmod),
 m_Parameter(param)
{
  Ctrl().SetItemData(Ctrl().GetItemCount() - 1, (DWORD_PTR) this);
  new CParamSubObject(*this);
}

BOOL CGVTSettingsDlg::CParametersListObject::CanEditText() const
{
  return FALSE;
}

QString CGVTSettingsDlg::CParametersListObject::Text() const
{
  return (LPCSTR) m_Parameter;
}

unsigned int CGVTSettingsDlg::CParametersListObject::Icon() const
{
  return 0;
}

CGVTVelocityModel *CGVTSettingsDlg::CParametersListObject::VelocityModel()
{
  return m_VelMod;
}

const CString &CGVTSettingsDlg::CParametersListObject::Parameter() const
{
  return m_Parameter;
}

// ******************************************************************
// CParamSubObject implementation
// ******************************************************************
CGVTSettingsDlg::CParamSubObject::CParamSubObject(CParametersListObject &parent)
:ISubListObject(parent, 1),
 m_parent(parent)
{
}

QString CGVTSettingsDlg::CParamSubObject::Text() const
{
  QString strText;
  strText = QString("%1").arg(m_parent.VelocityModel()->Parameter((LPCSTR) m_parent.Parameter()));
  return strText;
}

BOOL CGVTSettingsDlg::CParamSubObject::EditText(const QString& strText)
{
  double newValue = atof(strText.toStdString().c_str());
  m_parent.VelocityModel()->SetParameter((LPCSTR) m_parent.Parameter(), newValue);
  return TRUE;
}

BOOL CGVTSettingsDlg::CParamSubObject::CanEditText() const
{
  return TRUE;
}

// ******************************************************************
// CFormationListObject implementation
// *******************************************************************
CGVTSettingsDlg::CFormationListObject::CFormationListObject(CListCtrl& ctrl, CFormationBase *formation, 
                              CGVTVelocityModel *velmodel, CGVTSettingsDlg &parentDlg)
  :IListObject(ctrl), 
   m_Formation(formation), 
   m_VelModel(velmodel),
   m_ParentDlg(parentDlg)
{
  Ctrl().SetItemData(Ctrl().GetItemCount() - 1, (DWORD_PTR) this);
  new CVelModelSubObject(*this);
}
QString CGVTSettingsDlg::CFormationListObject::Text() const 
{ 
  return m_Formation->Name(); 
}

BOOL CGVTSettingsDlg::CFormationListObject::CanEditText() const 
{ 
  return FALSE;
}
unsigned int CGVTSettingsDlg::CFormationListObject::Icon() const 
{ 
  return m_Formation->IconId(); 
}

CGVTVelocityModel *CGVTSettingsDlg::CFormationListObject::VelocityModel()  
{ 
  return m_VelModel; 
}

const CGVTVelocityModel *CGVTSettingsDlg::CFormationListObject::VelocityModel() const 
{ 
  return m_VelModel; 
}

void CGVTSettingsDlg::CFormationListObject::SetVelocityModel(eVelocityModel velmodel)
{
  m_VelModel->ChangeModel(velmodel);
  m_ParentDlg.SetParametersList(Index());
}

// ******************************************************************
// CVelModelSubObject implementation
// ******************************************************************
CGVTSettingsDlg::CVelModelSubObject::CVelModelSubObject(CFormationListObject& parent)
: ISubListObject(parent, 1),
  m_parent(parent)
{
}

QString CGVTSettingsDlg::CVelModelSubObject::Text() const
{
  switch(m_parent.VelocityModel()->ModelType())
  {
  case RFactor:
  return "R-Factor";
  case SFactor:
  return "S-Factor";
  default:
  assert(FALSE);
  }
  return "";
}

BOOL CGVTSettingsDlg::CVelModelSubObject::EditText(const QString& strText)
{
  if(strText == "R-Factor")
  m_parent.SetVelocityModel(RFactor);
  else if(strText == "S-Factor")
  m_parent.SetVelocityModel(SFactor);

  return TRUE;

}

BOOL CGVTSettingsDlg::CVelModelSubObject::CanEditText() const
{
  return TRUE;
}

// ******************************************************************
// CFormationComboListCtrl implementation
// ******************************************************************
void CGVTSettingsDlg::CFormationComboListCtrl::OnGetComboItems(int iRowIndex, int iColumnIndex, CStringList& lstItems)
{
  lstItems.AddTail(_T("R-Factor"));
  lstItems.AddTail(_T("S-Factor"));
}

// ******************************************************************
// CTimeLapseComboListCtrl implementation
// ******************************************************************
/*CGVTSettingsDlg::CTimeLapseComboListCtrl::CTimeLapseComboListCtrl()
{
}

void CGVTSettingsDlg::CTimeLapseComboListCtrl::OnGetComboItems(int iRowIndex, int iColumnIndex, CStringList& lstItems)
{
  const CFemAppModel &model = ((CGVTSettingsDlg*)GetParent())->GVTSettings().Model();
  TDepletionStageEntry* pEntry = (TDepletionStageEntry*)(model.GraphEntry(MD_BASE_DEPLETION_STAGE));
  TDepletionStageEntry::TNodeSet stDepletionStage = pEntry->EntryNodes();
  TDepletionStageEntry::TNodeSet::const_iterator it;
  for(it = stDepletionStage.begin(); it != stDepletionStage.end(); ++it)
  {
  lstItems.AddTail((*it)->Name());
  }
  //m_nTimeSteps = stDepletionStage.size();
}*/

bool CGVTSettingsDlg::allMaterialModelsHaveVelocityP() const
{
  const TFormationBaseEntry* formationBaseEntry =
  dynamic_cast <const TFormationBaseEntry*> (
      m_GVTSettings.Model().GraphEntry(MD_BASE_FORMATION));
  bool allMaterialModelsHaveVelocityP = true;

  for (TFormationBaseEntry::TNodeSet::const_iterator formation =
  formationBaseEntry->EntryNodes().begin();
  formation != formationBaseEntry->EntryNodes().end(); ++formation)
  {
  const CDepletionStageEntry* depletionStageEntry =
      dynamic_cast <const CDepletionStageEntry*> (
    m_GVTSettings.Model().GraphEntry(MD_BASE_DEPLETION_STAGE));

  for (CDepletionStageEntry::const_iterator stage =
      depletionStageEntry->begin(); stage != depletionStageEntry->end();
      ++stage)
  {
      if ((*formation)->ConnectedMaterial((*stage).InitialStage()) != 0)
      {
    CMaterialServer& materialServer = (*formation)->Material(*stage);
    IMaterialRock* materialRock = materialServer.LibraryMaterial();
    bool foundVelocityP = false;

    if (materialRock != 0)
    {
          CLibraryMaterial& libraryMaterial = materialRock->LibraryMaterial();

          for (size_t i = 0; i < libraryMaterial.ParameterSize(); ++i)
          {
      if (libraryMaterial.Parameter(i).ValueTypeID() ==
              IDT_VALUETYPE_VELOCITYP)
      {
              foundVelocityP = true;
      }
          }
    }

    allMaterialModelsHaveVelocityP =
          allMaterialModelsHaveVelocityP && foundVelocityP;
      }
      else
      {
    allMaterialModelsHaveVelocityP = false;
      }
  }
  }

  return allMaterialModelsHaveVelocityP;
}

void CGVTSettingsDlg::setupVelocityP()
{
  GetDlgItem(IDC_GVT_VELMODEL_EDIT)->
  EnableWindow(m_GVTSettings.velocitySource() == GVT::VELOCITY_FROM_FILE);
  GetDlgItem(IDC_GVT_BROWSE_VELOCITY)->
  EnableWindow(m_GVTSettings.velocitySource() == GVT::VELOCITY_FROM_FILE);
  GetDlgItem(IDC_GVT_VELOCITY_FROM_MATERIAL)->
  EnableWindow(allMaterialModelsHaveVelocityP());
}


