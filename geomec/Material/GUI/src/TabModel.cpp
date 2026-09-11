#include "stdafx.h"
#include "geomec.h"

#include "TabModel.h"
#include "MatParamDlg.h"
#include "LibraryMaterial.h"
#include "MaterialHelperFactory.h"
#include "GlobalMessage.h"
#include "resourceIDI.h"

BEGIN_MESSAGE_MAP(CTabModel, CDialog)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_SAVEAS, OnSaveAs)
	ON_BN_CLICKED(IDC_RESET, OnReset)
	ON_CBN_SELCHANGE(IDC_CBO_MODEL, OnSelchangeCboModel)
	ON_CBN_SELCHANGE(IDC_CBO_TYPE, OnSelchangeCboType)
	ON_NOTIFY(NM_CLICK, IDC_PARAM_LIST, OnClickParamList)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_PARAM_LIST, OnEndlabeleditParamList)
	ON_NOTIFY(LVN_BEGINLABELEDIT, IDC_PARAM_LIST, OnBeginlabeleditParamList)
//	ON_MESSAGE(WM_UPDATEUNITS, OnUpdateUnits)
END_MESSAGE_MAP()

namespace
{

std::vector <QString> determineMaterialParameterError(
  const CMatParamDlg& materialParameterDialog,
  const CTabModel::TItemParameterMap& itemParameterMap,
  const std::vector <QString>& materialParameterError)
{
  std::vector <QString> newMaterialParameterError = materialParameterError;
  int i = 0;

  for (CTabModel::TItemParameterMap::const_iterator
    itemParameter = itemParameterMap.begin();
    itemParameter != itemParameterMap.end(); ++itemParameter, ++i)
  {
    QString error;
    double value = itemParameter->second->ValueToUserUnit(
      materialParameterDialog.UnitDef());
    bool valid = itemParameter->second->CheckValueFromUserUnit(
      value, materialParameterDialog.UnitDef(), error);

    if (!error.isEmpty())
    {
      assert(!valid);
    }

    newMaterialParameterError[i] = error;
  }

  return newMaterialParameterError;
}

} // anonymous namespace

CTabModel::CTabModel(CMatParamDlg& dlg, CWnd* pParent)
: CDialog(CTabModel::IDD, pParent), m_Dlg(dlg)
, m_nParamTotal(0)
, m_ListCtrl(this)
, m_materialParameterError()
{
	m_iType = -1;
  m_iModel = -1;

	if(EnableMatModel(MLFO_LINEAR))              m_vcListedModels.push_back(MM_LINEAR);
	if(EnableMatModel(MLFO_CAMCLAY))             m_vcListedModels.push_back(MM_CAMCLAY);
	if(EnableMatModel(MLFO_MOHRCOULOMB))         m_vcListedModels.push_back(MM_MOHRCOULOMB);
	if(EnableMatModel(MLFO_MC_COHESION_HARD1))   m_vcListedModels.push_back(MM_MC_COHESION_HARD1);
	if(EnableMatModel(MLFO_MC_COHESION_HARD2))   m_vcListedModels.push_back(MM_MC_COHESION_HARD2);
	if(EnableMatModel(MLFO_MC_COHESION_HARD3))   m_vcListedModels.push_back(MM_MC_COHESION_HARD3);
	if(EnableMatModel(MLFO_MC_FRICTION_HARD1))   m_vcListedModels.push_back(MM_MC_FRICTION_HARD1);
	if(EnableMatModel(MLFO_MC_FRICTION_HARD2))   m_vcListedModels.push_back(MM_MC_FRICTION_HARD2);
	if(EnableMatModel(MLFO_MC_FRICTION_HARD3))   m_vcListedModels.push_back(MM_MC_FRICTION_HARD3);
	if(EnableMatModel(MLFO_MODIFIEDMOHRCOULOMB)) m_vcListedModels.push_back(MM_MODIFIEDMOHRCOULOMB);
	if(EnableMatModel(MLFO_CREEP))               m_vcListedModels.push_back(MM_CREEP);
	if(EnableMatModel(MLFO_UNDRAINED))           m_vcListedModels.push_back(MM_UNDRAINED);
	if(EnableMatModel(MLFO_RIGIDITY))            m_vcListedModels.push_back(MM_RIGIDITY);
	if(EnableMatModel(MLFO_DUALCAP_LINELA))      m_vcListedModels.push_back(MM_DUALCAP_LINELA);
	if(EnableMatModel(MLFO_FRACTURE_ANISOTROPY)) m_vcListedModels.push_back(MM_FRACTURE_ANISOTROPY);
  if(EnableMatModel(MLFO_UPSCALED_ANISOTROPY)) m_vcListedModels.push_back(MM_UPSCALED_ANISOTROPY);
  if(EnableMatModel(MLFO_FRACTURE_APERTURE))   m_vcListedModels.push_back(MM_FRACTURE_APERTURE);
  if(EnableMatModel(MLFO_ANISOTROPIC_CAMCLAY)) m_vcListedModels.push_back(MM_ANISOTROPIC_CAMCLAY);
  if(EnableMatModel(MLFO_FRACTURE_APERTURE2))  m_vcListedModels.push_back(MM_FRACTURE_APERTURE2);
  if(EnableMatModel(MLFO_CAMCLAY_CREEP))             m_vcListedModels.push_back(MM_CAMCLAY_CREEP);

  const CLibraryMaterial* pMat = dlg.Material();
  if(pMat)
  {
    SelectedModel(mlMatModel(pMat->MaterialModel()));
  	m_iType = (pMat->GroupSize() > 0) ? 0 : -1;
  }
}

unsigned int CTabModel::ID()
{
  return IDD;
}

void CTabModel::SelectedModel(mlMatModel nModel)
{
	m_iModel = -1;

	for(int i = 0; i < (int)m_vcListedModels.size(); i++)
		if(m_vcListedModels[i] == nModel) m_iModel = i;

	assert(m_iModel >= 0);
}

mlMatModel CTabModel::SelectedModel()
{
	return m_vcListedModels[m_iModel];
}

CMatParamDlg& CTabModel::Dlg()
{
  return m_Dlg;
}

void CTabModel::Type(int iType)
{
	m_iType = iType;
}

int CTabModel::Type()
{
	return m_iType;
}

void CTabModel::UpdateControls()
{
  if(GetMaterial() && SelectedModel() != GetMaterial()->MaterialModel())
  {
    SelectedModel(mlMatModel(GetMaterial()->MaterialModel()));
    m_iType = (GetMaterial()->GroupSize() > 0) ? 0 : -1;
    UpdateData(FALSE);
  }

  FillParameterListCtrl();
}

void CTabModel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabModel)
	DDX_Control(pDX, IDC_PARAM_LIST, m_ListCtrl);
	DDX_CBIndex(pDX, IDC_CBO_TYPE, m_iType);
	DDX_CBIndex(pDX, IDC_CBO_MODEL, m_iModel);
	//}}AFX_DATA_MAP

	DDX_Text(pDX, IDC_IPEDIT, m_dCurrent);
}

BOOL CTabModel::OnInitDialog()
{
  CDialog::OnInitDialog();

  CEdit *edit = (CEdit*)GetDlgItem(IDC_IPEDIT);
  edit->ShowWindow(0);

  // Initial empty image list.
  //
  m_ImageList.Create(16, 16, ILC_COLOR, 0, 2);

  RECT rect;
  m_ListCtrl.GetClientRect(&rect);
  long width = rect.right;
  long column_width = width / 2;
  long column_width_small = width / 6;

  // temporary column width, will be adjusted when parameters have been loaded
  int ii= 0;
  m_ListCtrl.InsertColumn(ii++, "Fix", LVCFMT_LEFT, column_width_small);
  m_ListCtrl.InsertColumn(ii++, "Name", LVCFMT_LEFT, column_width);
  m_ListCtrl.InsertColumn(ii++, "Value", LVCFMT_LEFT, column_width_small);
  m_ListCtrl.InsertColumn(ii++, "Unit", LVCFMT_LEFT, column_width_small);

  FillModelListBox();
  UpdateModelFrames();
  if(!GetMaterial()) return FALSE;

  // duplicate!
  // all created variables are used elsewhere at the location of the original

  CLibraryMaterial *mat = GetMaterial();
  bool bGroup = (mat->GroupSize() > 0);
  assert(bGroup ? m_iType >= 0 : m_iType == -1);
  m_nParamTotal = (bGroup ? mat->Group(m_iType).ParameterSize() : mat->ParameterSize());

  FillParameterListCtrl();  // fill 'm_mpItemParameter'

  setMaterialParameterError(determineMaterialParameterError(m_Dlg,
    m_mpItemParameter, getMaterialParameterError()));

  FillParameterListCtrl();

  // initially disable Reset button
  UpdateResetButton();

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
}

/// \brief Vertically resize/move an item
static void ChangeItem
( CWnd &parent //!< the parent of 'win'
, CWnd &win //!< gui item that needs to be moved or resized
, int vdiff //!< the amount in pixels by which to move/resize
, bool resize = false //!< resize (change bottom edge) or move (change top and bottom)
)
{
  CRect rect;
  win.GetWindowRect(&rect);
  parent.ScreenToClient(&rect);
  if ( ! resize)
    rect.top += vdiff; // move
  rect.bottom += vdiff;
  win.MoveWindow(&rect);
}

/// \brief Vertically resize the tabmodel page
void CTabModel::VerticalResize
(const int vdiff //!< the amount of pixels to resize in vertical direction
)
{
  // http://support.microsoft.com/kb/143291
  //
  ChangeItem( *this, m_ListCtrl, vdiff, true);
  ChangeItem( *this, *GetDlgItem(IDC_PARAMETERSBORDER), vdiff, true);
  ChangeItem( *this, *GetDlgItem(IDC_SAVE), vdiff);
  ChangeItem( *this, *GetDlgItem(IDC_SAVEAS), vdiff);
  ChangeItem( *this, *GetDlgItem(IDC_RESET), vdiff);
  ChangeItem( *this, *GetDlgItem(IDC_IPEDIT), vdiff);
}

const int CTabModel::VALUE_COLUMN = 2;

const std::vector <QString> CTabModel::getMaterialParameterError() const
{
  std::vector <QString> materialParameterError(m_nParamTotal);

  if (!m_materialParameterError.empty())
  {
    materialParameterError = m_materialParameterError;
  }

  return materialParameterError;
}

void CTabModel::setMaterialParameterError(
  const std::vector <QString>& materialParameterError)
{
  if (materialParameterError.empty())
  {
    std::vector <QString> emptyMaterialParameterError(m_nParamTotal);

    m_materialParameterError = emptyMaterialParameterError;
  }
  else
  {
    m_materialParameterError = materialParameterError;
  }
}

bool CTabModel::hasMaterialParameterErrors() const
{
  for (std::vector <QString> ::const_iterator
    materialParameterError = m_materialParameterError.begin();
    materialParameterError != m_materialParameterError.end();
    ++materialParameterError)
  {
    if (!(*materialParameterError).isEmpty())
    {
      return true;
    }
  }

  return false;
}

void CTabModel::OnCancel()
{
  m_Dlg.OnCancel();
}

void CTabModel::OnSave()
{
  m_Dlg.OnSave();
  UpdateResetButton();
}

void CTabModel::OnSaveAs()
{
  m_Dlg.OnSaveAs();
  UpdateResetButton();
}

void CTabModel::OnReset()
{
  m_Dlg.OnReset();
	m_iType = (GetMaterial()->GroupSize() > 0) ? 0 : -1;
  UpdateModelFrames();
  InputTypeChanged();
  SelectedModel(mlMatModel(GetMaterial()->MaterialModel()));
  setMaterialParameterError(std::vector <QString> ());
  FillParameterListCtrl();
  UpdateResetButton();
  UpdateData(FALSE);
}

void CTabModel::OnSelchangeCboModel()
{
	UpdateData(TRUE);
	InitializeModel(m_vcListedModels[m_iModel]);
//	SendMessageToDialog(WM_UPDATEDATA, (WPARAM) TRUE);
	m_iType = (GetMaterial()->GroupSize() > 0) ? 0 : -1;
	UpdateModelFrames();
	InputTypeChanged();
  UpdateResetButton();
  setMaterialParameterError(std::vector <QString> ());
  FillParameterListCtrl();
  m_Dlg.AppendToHistory(getMaterialParameterError());
}

void CTabModel::OnSelchangeCboType()
{
	UpdateData(TRUE);
	InputTypeChanged();
}

void CTabModel::OnClickParamList(NMHDR* pNMHDR, LRESULT* pResult)
{
	HD_NOTIFY *phdn = (HD_NOTIFY *) pNMHDR;
	if(phdn->iItem == -1) return;

	LVITEM item;
	// Specific info so that the GetItem function knows what to look for
	item.mask = LVIF_IMAGE; // we want to know which image is currently used
	item.iItem = phdn->iItem;
	item.iSubItem = 0;

	m_ListCtrl.GetItem(&item);

	int nImage;
	std::string label;
	
	if(item.iImage == m_nParamTotal+2)
		return;
	if(item.iImage == m_nParamTotal+3)
		return;

  CLibraryMaterialParameter *pMatParam = (*m_mpItemParameter.find(item.iItem)).second;
  if(m_ListCtrl.IsLockedParameterName(pMatParam->Name().toStdString().c_str()))
    return;

	if(item.iImage == m_nParamTotal+0)
	{
		nImage = m_nParamTotal+1;
		label = "No";
	}
	else
	{
		nImage = m_nParamTotal+0;
		label = "Yes";
	}

	if(nImage == m_nParamTotal+0)
		pMatParam->CurrentlyFixed(true);
	else
		pMatParam->CurrentlyFixed(false);

  UpdateResetButton();
	m_ListCtrl.SetItem(phdn->iItem, 0, LVIF_IMAGE|LVIF_TEXT, label.c_str(), nImage, LVIS_FOCUSED, 
      (unsigned int)-1, 0);

  m_Dlg.AppendToHistory();

	*pResult = 0;
}

void CTabModel::OnEndlabeleditParamList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	LVITEM item = pDispInfo->item;
	int iItem = item.iItem;
	CLibraryMaterialParameter *pMatParam = (*m_mpItemParameter.find(iItem)).second;
	CLibraryMaterial *pMat = GetMaterial();

	// set the value in the edit box since it has previously been destroyed in the CInPlaceEdit but we want
	//  the value so that we can perform some checks usinf the DDX_Text in the DoDataExchange.
	CEdit *pEdit = (CEdit*)GetDlgItem(IDC_IPEDIT);
	pEdit->SetWindowText(item.pszText);

	if(!UpdateData(TRUE))
		return;

	QString error;
  if(hasMaterialParameterErrors() ||
    !pMatParam->ValueFromUserUnit(m_dCurrent, m_Dlg.UnitDef(), error))
  {
    double oldValue = pMatParam->Value();

    const ml::CMatParam::CUnitType& unitType = pMatParam->UnitConversion();
    double newValue = unitType.FromUserUnit(m_dCurrent, m_Dlg.UnitDef());
    pMatParam->LoadValue(newValue);

    if (pMatParam->CheckValueFromUserUnit(m_dCurrent, m_Dlg.UnitDef(), error))
    {
      // the value has to be loaded into the parameter for the algorithm to work

      setMaterialParameterError(determineMaterialParameterError(m_Dlg,
        m_mpItemParameter, getMaterialParameterError()));

      if (!hasMaterialParameterErrors())
      {
        // will only result in verifying its dependencies when
        // its internal value differs from 'm_dCurrent'

        pMatParam->LoadValue(oldValue);
        pMatParam->ValueFromUserUnit(m_dCurrent, m_Dlg.UnitDef(), error);

        assert(error.isEmpty());
      }
    }
  }

  UpdateResetButton();

	m_Dlg.OnUpdateGraphs();

  m_ListCtrl.OnEndLabelEdit(pNMHDR, pResult);

  setMaterialParameterError(determineMaterialParameterError(m_Dlg,
    m_mpItemParameter, getMaterialParameterError()));

  FillParameterListCtrl();

  m_Dlg.AppendToHistory(getMaterialParameterError());

	*pResult = 0;
}

void CTabModel::OnBeginlabeleditParamList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here

	// return 1 to prevent user from editing the label (1st column)
	*pResult = 1;
}

LRESULT CTabModel::OnUpdateUnits(WPARAM wParam, LPARAM lParam)
{
  return 0L;
}

LRESULT CTabModel::OnModelChanged(WPARAM wParam, LPARAM lParam)
{
	UpdateModelFrames();
	return 0L;
}

CLibraryMaterial* CTabModel::GetMaterial()
{
  return m_Dlg.Material();
}

const CLibraryMaterial* CTabModel::GetOriginalMaterial() const
{
  return m_Dlg.OriginalMaterial();
}

void CTabModel::UpdateModelFrames()
{
	FillTypeListBox();
	CLibraryMaterial *pMat = GetMaterial();
	GetDlgItem(IDC_FRATYPE)->EnableWindow(pMat->GroupSize() > 0);
	GetDlgItem(IDC_CBO_TYPE)->EnableWindow(pMat->GroupSize() > 0);
}

BOOL CTabModel::EnableMatModel(mlFilterOption nFilter)
{
  return (m_Dlg.MaterialModelFilter() & nFilter);
}

void CTabModel::InputTypeChanged()
{
  UpdateData(TRUE);
  FillParameterListCtrl();
  m_Dlg.OnUpdateGraphs();
}

int CTabModel::GetGroup()
{
	CLibraryMaterial *pMat = GetMaterial();

	if(pMat->GroupSize() == 0)
    return 0;

	assert(m_iType >= 0 && m_iType < pMat->GroupSize());
  return m_iType;
}

void CTabModel::FillModelListBox()
{
	assert(!m_vcListedModels.empty());

	for(size_t i = 0; i < m_vcListedModels.size(); i++)
		AddModelToCombo(m_vcListedModels[i]);
}

void CTabModel::FillTypeListBox()
{
	CLibraryMaterial *pMat = GetMaterial();
	CComboBox *pCombo = (CComboBox *) GetDlgItem(IDC_CBO_TYPE);
	pCombo->ResetContent();

	for(int i = 0; i < pMat->GroupSize(); i++)
		pCombo->AddString(pMat->Group(i).Name().toStdString().c_str());

	UpdateData(FALSE);
}

void CTabModel::InitializeModel(mlMatModel nModel)
{
  m_Dlg.InitializeModel(nModel);
}

void CTabModel::AddModelToCombo(mlMatModel nModel)
{
	CComboBox *pCombo = (CComboBox *) GetDlgItem(IDC_CBO_MODEL);

  const CMaterialHelperFactory *f = CMaterialHelperFactory::Instance();
  ml::CMaterial::CCreator* pCreator = f->getMatCreator(nModel);
  if(pCreator)
  {
    pCombo->AddString(pCreator->MaterialModelName().toStdString().c_str());
  }
}

void CTabModel::FillParameterListCtrl()
{
  // http://social.msdn.microsoft.com/Forums/vstudio/en-US/15919c4a-6397-4b75-bb93-7e7ed8aacc9f/how-to-set-image-for-subitem-only-of-clistctrl?forum=vclanguage
  ::SendMessage
  ( m_ListCtrl.m_hWnd
  , LVM_SETEXTENDEDLISTVIEWSTYLE
  , LVS_EX_SUBITEMIMAGES
  , LVS_EX_SUBITEMIMAGES); //Use Extended style

  // { Fill the list of icons
  //
  // Remove icons that go with the previously selected
  // 'Model' or 'Input type'
  while (m_ImageList.GetImageCount()>0)
  {
    m_ImageList.Remove(m_ImageList.GetImageCount()-1);
  }

  CLibraryMaterial *mat = GetMaterial();
  bool bGroup = (mat->GroupSize() > 0);
  assert(bGroup ? m_iType >= 0 : m_iType == -1);
  m_nParamTotal = (bGroup ? mat->Group(m_iType).ParameterSize() : mat->ParameterSize());

  if (getMaterialParameterError().size() != m_nParamTotal)
  {
    m_materialParameterError.resize(m_nParamTotal);
  }

  for(size_t i = 0; i < m_nParamTotal; ++i)
  {
    ml::CMatParam& matparam =
      (bGroup ? mat->Group(m_iType).Parameter(i) : mat->Parameter(i));
    assert(dynamic_cast<CLibraryMaterialParameter*>(&matparam));
    CLibraryMaterialParameter& param =
      static_cast<CLibraryMaterialParameter&>(matparam);
    m_ImageList.Add(AfxGetApp()->LoadIcon(param.IconID()));
  }

  m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_ICON_TRUE));
  m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_CROSS_RED));
  m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_ICON_FORCE_TRUE));
  m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_ICON_NA));

  m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_EXCLAMATION_POINT));

  m_ListCtrl.SetImageList(&m_ImageList, LVSIL_SMALL);
  //
  // Fill the list of icons }

  m_ListCtrl.DeleteAllItems();

  CString strValue;
  int iColom, nIndex, nImage;
  int iItem = 0;
  m_mpItemParameter.clear();

  for(int i = 0; i < m_nParamTotal; ++i)
  {
    ml::CMatParam& matparam = (bGroup ? mat->Group(m_iType).Parameter(i) : mat->Parameter(i));
    assert(dynamic_cast<CLibraryMaterialParameter*>(&matparam));
    CLibraryMaterialParameter& param = static_cast<CLibraryMaterialParameter&>(matparam);
    m_mpItemParameter.insert(TItemParameterMap::value_type(iItem, &param));

    bool bReadOnly = m_ListCtrl.IsLockedParameterName(param.Name().toStdString().c_str());

    iColom = 1;
    if(param.CalibrationPath().isEmpty())
    {
      nImage = m_nParamTotal+3;
      CString sText = bReadOnly ? "R/O" : "N/A";
      nIndex = m_ListCtrl.InsertItem(iItem++, sText, nImage);
    }
    else if(param.IsFixedCalibrationParameter() || bReadOnly)
    {
      nImage = m_nParamTotal+2;
      CString sText = bReadOnly ? "R/O" : "Yes";
      nIndex = m_ListCtrl.InsertItem(iItem++, sText, nImage);
    }
    else if(param.IsCurrentlyFixed())
    {
      nImage = m_nParamTotal+0;
      nIndex = m_ListCtrl.InsertItem(iItem++, "Yes", nImage);
    }
    else
    {
      nImage = m_nParamTotal+1;
      nIndex = m_ListCtrl.InsertItem(iItem++, "No", nImage);
    }

    //http://stackoverflow.com/questions/14401355/mfc-listview-change-item-image
    m_ListCtrl.SetItemText(nIndex, iColom, param.Name().toStdString().c_str());

    LVITEM item;
    ZeroMemory(&item, sizeof(item));
    item.iItem = nIndex;
    item.iSubItem = iColom++;
    m_ListCtrl.GetItem(&item);

    item.mask  = LVIF_IMAGE;
    item.iImage= i;
    m_ListCtrl.SetItem(&item);

    strValue.Format("%g", param.ValueToUserUnit(Dlg().UnitDef()));
    m_ListCtrl.SetItemText(nIndex, iColom, strValue);

    ZeroMemory(&item, sizeof(item));
    item.iItem = nIndex;
    item.iSubItem = iColom++;
    m_ListCtrl.GetItem(&item);

    item.mask = LVIF_IMAGE;
    item.iImage = (getMaterialParameterError()[nIndex].isEmpty() ?
      m_nParamTotal + 5 : m_nParamTotal + 4);
    m_ListCtrl.SetItem(&item);

    m_ListCtrl.SetItemText(nIndex, iColom++,
      param.UnitName(Dlg().UnitDef()));
  }

  AdjustColumnWidth(this, &m_ListCtrl);
  Invalidate();
  UpdateWindow();
}

void CTabModel::UpdateResetButton()
{
  GetDlgItem(IDC_RESET)->EnableWindow(*GetMaterial() != *GetOriginalMaterial());
}
