// resultexportdlg.cpp : implementation file
//

#include "stdafx.h"
#include "geomec.h"
#include "Result.h"
#include "ResultTree.h"
#include "resultexportdlg.h"
#include "modelbase.h"
#include "DepletionStage.h"
#include "ResultRegister.h"
#include "Ipointset.h"
#include "FormationBase.h"
#include "horizonbase.h"
#include "BaseEntryTypes.h"
#include "ProgressDlg_MFC.h"
#include "FieldFactor.h"
#include "PropertyFile.h"
#include "PropertyExcel.h"
#include "MeshBase.h"
#include "HexaHorizon.h"
#include "CrossSection.h"
#include "hexaentrytypes.h"
#include "HexaFormation.h"
#include "formationplane.h"
#include "HexaModel.h"
#include "colorscaleentry.h"
#include "HotSpot.h"
#include "KeyFile.h"
#include "geoprogress.h"
#include "TNOFileDialog.h"
#include "ExportFormat.h"
#include "PointSet.h"
#include "SelectionObserver.h"
#include "CoordinateSet.h"
#include "BodyQuadrilateral.h"
#include "MaterialResultTree.h"
#include "SelectionObserver_Delegate.h"
#include "ExportResults.h"
#include "ExportResultData.h"
#include "ExportResultTreeBranch.h"
#include "ExportResultData_Delegate.h"
#include "ExportResultDataEntry.h"
#include "SelectExportMacroDialog.h"
#include "DepletionStageObserver.h"
#include "NonMeshedSurface.h"
#include "GeomecUtils.h"
#include "GVTResult.h"
#include "GlobalMessage.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CResultExportDlg dialog

// private

// static

TResultExportConfiguration CResultExportDlg::defaultResultExportConfiguration;

// public

CResultExportDlg::CResultExportDlg(CModelBase& model,
  const TResultExportConfiguration& resultExportConfiguration,
  const CString& textCancel, bool buttonOK, CWnd* pParent)
: IExportDlg(CResultExportDlg::IDD, pParent),
  m_pSlipHorizonSelection(0),
  m_pHorizonSelection(0),
  m_pFormationSelection(0),
  m_pFormationPlaneSelection(0),
  m_pRTCISelection(0),
  m_pNewWellPathSelection(0),
  m_pPointSetSelection(0),
  m_pElementSetSelection(0),
  m_pNonMeshedSurfaceSelection(0),
  m_resultExportConfiguration(resultExportConfiguration),
  m_model(model),
  m_nWeightingType(0),
  m_nUnit(-1),
  m_bLinear(TRUE),
  m_nTZ(0),
  m_nCenterPoints(0),
  m_nFormationNames(0),
  m_nWellPoints(0),
  m_bExportNewWellpaths(false),
  m_MatRGObserver(0),
  m_RGObserver(0),
  m_GVTRGObserver(0),
  m_InitiallyOn(0),
  m_nInitialIndex(-1)
, m_textCancel(textCancel)
, m_buttonOK(buttonOK)
, m_exportResultData(0)
, m_updateData(false)
, m_bBranchToggle(false)
{
  Init();
}

CResultExportDlg::CResultExportDlg(CResultGroup& result_group,
  const TResultExportConfiguration& resultExportConfiguration,
  const CString& textCancel, bool buttonOK, CWnd* pParent)
: IExportDlg(CResultExportDlg::IDD, pParent), 
  m_pSlipHorizonSelection(0),
  m_pHorizonSelection(0),
  m_pFormationSelection(0),
  m_pFormationPlaneSelection(0),
  m_pRTCISelection(0),
  m_pNewWellPathSelection(0),
  m_pPointSetSelection(0),
  m_pElementSetSelection(0),
  m_pNonMeshedSurfaceSelection(0),
  m_resultExportConfiguration(resultExportConfiguration),
  m_model((CModelBase&)result_group.Model()),
  m_nWeightingType(0),
  m_nUnit(-1),
  m_bLinear(TRUE),
  m_nTZ(0),
  m_nCenterPoints(0),
  m_nFormationNames(0),
  m_nWellPoints(0),
  m_bExportNewWellpaths(false),
  m_MatRGObserver(0),
  m_RGObserver(0),
  m_GVTRGObserver(0),
  m_InitiallyOn(&result_group),
  m_nInitialIndex(-1)
, m_textCancel(textCancel)
, m_buttonOK(buttonOK)
, m_exportResultData(0)
, m_updateData(false)
, m_bBranchToggle(false)
{
  Init();
}

CResultExportDlg::CResultExportDlg(const IValueComposite& composite, int nIndex,
  const TResultExportConfiguration& resultExportConfiguration,
  const CString& textCancel, bool buttonOK, CWnd* pParent)
: IExportDlg(CResultExportDlg::IDD, pParent),
  m_pSlipHorizonSelection(0),
  m_pHorizonSelection(0),
  m_pFormationSelection(0),
  m_pFormationPlaneSelection(0),
  m_pRTCISelection(0),
  m_pNewWellPathSelection(0),
  m_pPointSetSelection(0),
  m_pElementSetSelection(0),
  m_pNonMeshedSurfaceSelection(0),
  m_resultExportConfiguration(resultExportConfiguration),
  m_model((CModelBase&)composite.Model()),
  m_nUnit(-1),
  m_nWeightingType(0),
  m_bLinear(TRUE),
  m_nTZ(0),
  m_nCenterPoints(0),
  m_nFormationNames(0),
  m_nWellPoints(0),
  m_bExportNewWellpaths(false),
  m_MatRGObserver(0),
  m_RGObserver(0),
  m_GVTRGObserver(0),
  m_InitiallyOn(&composite),
  m_nInitialIndex(nIndex)
, m_textCancel(textCancel)
, m_buttonOK(buttonOK)
, m_exportResultData(0)
, m_updateData(false)
, m_bBranchToggle(false)
{
  Init();
}

void CResultExportDlg::Init()
{
  CollectDepletionStages();

  m_bNonLinear = m_model.ResultRegister().NonLinear();
  m_bLinear = m_model.ResultRegister().Linear();
  m_bHeat = m_model.ResultRegister().Heat();
  m_bMixture = m_model.ResultRegister().Mixture();
  m_bMixtureContainment = m_model.ResultRegister().MixtureContainment();

  if(m_model.ResultRegister().Linear())
    m_nLinear = 0;
  else if(m_model.ResultRegister().NonLinear())
    m_nLinear = 1;
  else if(m_model.ResultRegister().Mixture())
  m_nLinear = 2;
  else if(m_model.ResultRegister().MixtureContainment())
  m_nLinear = 3;
  else
  m_nLinear = -1;
  
  // Intialize the unit
  CFemAppDoc *pDoc = GetGeomecDoc();
  switch( pDoc->UnitNode().Unit() )
  {
  case CQuantity::SI_UNIT:
    m_nUnit = 0;
    break;
  case CQuantity::FIELD_UNIT:
    m_nUnit = 1;
    break;
  };
}

CResultExportDlg::~CResultExportDlg()
{
}

void CResultExportDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CResultExportDlg)
  DDX_Control(pDX, IDC_CB_HOTSPOT, m_cbHotSpot);
  DDX_Control(pDX, IDC_CB_TIMESTEP, m_cbTimeStep);
  DDX_Control(pDX, IDC_LB_TIMESTEP, m_lbTimeStep);
  DDX_Control(pDX, IDC_TC_OBJECT, m_tcObject);
  DDX_Control(pDX, IDC_TC_VALUE, m_tcValue);
  DDX_Radio(pDX, IDC_RB_POINT_INTERPOLATION, m_nWeightingType);
  DDX_Radio(pDX, IDC_RB_UNIT_SI, m_nUnit);
  DDX_Radio(pDX, IDC_RB_LINEAR, m_nLinear);
  DDX_Check(pDX, IDC_CK_LINEAR, m_bLinear);
  DDX_Check(pDX, IDC_CK_NON_LINEAR, m_bNonLinear);
  DDX_Check(pDX, IDC_CK_HEAT, m_bHeat);
  DDX_Check(pDX, IDC_CK_MIXTURE, m_bMixture);
  DDX_Check(pDX, IDC_CK_MIXTURE_CONTAINMENT, m_bMixtureContainment);
  //}}AFX_DATA_MAP

  DDX_Radio(pDX, IDC_FULL_ELEMENTS, m_nCenterPoints);
  DDX_Check(pDX, IDC_FORMATION_NAMES, m_nFormationNames);
  DDX_Control(pDX, IDC_MACRO_NAME, m_macroName);
  DDX_Check(pDX, IDC_WELLPATH_MESH_INTERSECTION_POINTS, m_nWellPoints);
}


BEGIN_MESSAGE_MAP(CResultExportDlg, CDialog)
  //{{AFX_MSG_MAP(CResultExportDlg)
//	ON_BN_CLICKED(IDC_RB_NONE, OnBrowsePath)
  //}}AFX_MSG_MAP
  ON_BN_CLICKED(IDC_EXPORT, OnExport)
  ON_BN_CLICKED(IDC_CK_LINEAR, OnAnalysisTypeChanged)
  ON_BN_CLICKED(IDC_CK_NON_LINEAR, OnAnalysisTypeChanged)
  ON_BN_CLICKED(IDC_CK_HEAT, OnAnalysisTypeChanged)
  ON_BN_CLICKED(IDC_CK_MIXTURE, OnAnalysisTypeChanged)
  ON_BN_CLICKED(IDC_IMPORT_EXPORT_MACRO, &CResultExportDlg::OnImportExportMacro)
  ON_BN_CLICKED(IDC_SAVE_EXPORT_MACRO, &CResultExportDlg::OnSaveExportMacro)
  ON_EN_CHANGE(IDC_MACRO_NAME, &CResultExportDlg::OnMacroName)
  ON_EN_UPDATE(IDC_MACRO_NAME, &CResultExportDlg::OnMacroName)
  ON_EN_CHANGE(IDC_EXPORT, &CResultExportDlg::OnEnableExport)
  ON_EN_UPDATE(IDC_EXPORT, &CResultExportDlg::OnEnableExport)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResultExportDlg message handlers

BOOL CResultExportDlg::OnInitDialog() 
{
  CDialog::OnInitDialog();

  initializeDialog(true);

  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

void CResultExportDlg::UpdateHotSpot()
{
  m_cbHotSpot.ResetContent();

  // Insert the no-hotspot
  int nIndex1 = m_cbHotSpot.AddString("No hotspot export");
  m_cbHotSpot.SetItemData(nIndex1, 0);
  m_cbHotSpot.SetCurSel(nIndex1);

  // Just walk over the hotspot
  CColorScaleEntry& entry = dynamic_cast<CColorScaleEntry&>(*Model().GraphEntry(MD_BASE_COLOR_SCALE));
  CColorScaleEntry::TNodeSet stNode = entry.EntryNodes();
  for(CColorScaleEntry::TNodeSet::iterator it = stNode.begin(); it != stNode.end(); it++)
  {
    CHotSpot *pHotSpot = dynamic_cast<CHotSpot*> (*it);
    if(pHotSpot)
    {
      int nIndex2 = m_cbHotSpot.AddString(pHotSpot->Name().toStdString().c_str());
      m_cbHotSpot.SetItemData(nIndex2, (DWORD_PTR)(*it));
    
    }
  }
}

void CResultExportDlg::CollectDepletionStages()
{
  const CDepletionStage* pStage = &m_model.InitialDepletionStage();
  while(pStage)
  {
    VERIFY(m_stTimeStep.insert(pStage).second);

    if(pStage->Last())
      pStage = 0;
    else
      pStage = &pStage->Next();
  }
}

void CResultExportDlg::UpdateTimeStep()
{
  m_lbTimeStep.DeleteAllItems();

  // Insert depletion stages
  m_lbTimeStep.InsertColumn(0, "Depletion stage", LVCFMT_LEFT, 130);

  const CDepletionStage* pStage = &m_model.InitialDepletionStage();
  while(pStage)
  {
    // Insert in list control
    new CDepletionStageObserver <CResultExportDlg> (*pStage, *this, m_lbTimeStep);

    // and in combo box
    int nIndex = m_cbTimeStep.AddString(pStage->Name().toStdString().c_str());
    m_cbTimeStep.SetItemData(nIndex, (DWORD_PTR)pStage);

    if(pStage->Initial())
      m_cbTimeStep.SelectString(-1, pStage->Name().toStdString().c_str());
    
    if(pStage->Last())
      pStage = 0;
    else
      pStage = &pStage->Next();
  }
}

bool CResultExportDlg::TimeStep(const CDepletionStage& stage) const
{
  return m_stTimeStep.find(&stage) != m_stTimeStep.end();
}

void CResultExportDlg::TimeStep(const CDepletionStage& stage, bool bEnable)
{
  if(!bEnable) m_stTimeStep.erase( &stage );
  if(bEnable ) m_stTimeStep.insert( &stage );
  m_tcValue.UpdateTree();
}

/*!
  Check all modes and components to ensure that some things exists
*/
BOOL CResultExportDlg::Valid(const IValueComposite& composite) const
{

  assert(composite.ModeSize() > 0);
  for(int i = 0; i < composite.ComponentSize(); i++)
  {
    if(Valid(composite.Component(i)))
      return TRUE;
  }
  
  return FALSE;

}

static bool CheckResult(const CDepletionStage& stage, const CAnalysisType& antype, const IResultComponent& component)
{
  const IResult& result = dynamic_cast<const IResult&>(component.Parent());
  const IResultComponent* pComponent = result.ResultComponent( stage, antype, component.RegisterIndex(), component.ComponentIndex());
  if(pComponent) {
    if(pComponent->Defined()) return TRUE;
  }
  return FALSE;
}

bool CResultExportDlg::ValueComponentCanMapSelection(const IValueComponentBase& component) const
{
  for(COpenGLNodeSelection::const_iterator it = Selection().begin(); it != Selection().end(); ++it)
  {
  if(component.CanMap(**it))
      return true;
  }

  return false;
}

BOOL CResultExportDlg::Valid(const IValueComponentBase& component) const
{
  if(!ValueComponentCanMapSelection(component))
  return FALSE;
//  std::pair <bool, bool> categoriesResult = m_vcCategory.canMap(component);

//  if (categoriesResult.first && !categoriesResult.second)
//  {
//    return FALSE;
//  }

  const CMaterialResult::CMaterialResultComponent* pMatComp = dynamic_cast<const CMaterialResult::CMaterialResultComponent*>(&component);
  if(pMatComp)
  return pMatComp->Defined();

  // Walk over modes
  if (dynamic_cast<const CGVTFactor::CGVTFactorComponent *>(&component))
  return TRUE;

  const IResultComponent* pComponent = dynamic_cast<const IResultComponent*>(&component);
  if(pComponent) {
    const IResult* pResult = dynamic_cast<const IResult*>(&pComponent->Parent());
    for(TTimeStepSet::const_iterator it = m_stTimeStep.begin(); it != m_stTimeStep.end(); it++) {
      const CDepletionStage& stage = **it;
      if(m_bLinear             && CheckResult( stage, CAnalysisType::AT_LINEAR,              *pComponent)) return TRUE;
      if(m_bNonLinear          && CheckResult( stage, CAnalysisType::AT_NONLIN,              *pComponent)) return TRUE;
      if(m_bHeat               && CheckResult( stage, CAnalysisType::AT_HEAT,                *pComponent)) return TRUE;
      if(m_bMixture            && CheckResult( stage, CAnalysisType::AT_MIXTURE,             *pComponent)) return TRUE;
      if(m_bMixtureContainment && CheckResult( stage, CAnalysisType::AT_MIXTURE_CONTAINMENT, *pComponent)) return TRUE;
    }

  return CExportResults::isInputResult(pResult);
  } else {
    return component.Defined();
  }

  return FALSE;
}

void CResultExportDlg::UpdateButtons()
{
  CGeomecApp &App = static_cast<CGeomecApp &> (*AfxGetApp());
  bool bNoResults = !Model().ResultRegister().Linear() && !Model().ResultRegister().NonLinear() && !Model().ResultRegister().Mixture();

  //bool bElementSet = m_vcCategory.isDerivedCategory <CElementSetCategory> ();
  bool bElementSet =
  ((m_pElementSetSelection != 0) && !m_pElementSetSelection->empty());

  bool bPointSet =
  ((m_pPointSetSelection != 0) && !m_pPointSetSelection->empty());

  bool bDeformation =
  ((m_pHorizonSelection != 0) && !m_pHorizonSelection->empty()) ||
  ((m_pFormationSelection != 0) && !m_pFormationSelection->empty()) ||
  ((m_pFormationPlaneSelection != 0) && !m_pFormationPlaneSelection->empty());

  // wjrx mantis 3322
  bool bNewWellPath =
  ((m_pNewWellPathSelection != 0) && !m_pNewWellPathSelection->empty());

  bool bEnableMapping = bElementSet;
#ifdef MAPPING_KEYFILE
  bEnableMapping = (bEnableMapping && App.KeyFile().Unlocked());
#endif

  GetDlgItem(IDC_RB_POINT_INTERPOLATION)->EnableWindow(bElementSet);
  GetDlgItem(IDC_RB_UNWEIGTHED)->EnableWindow(bEnableMapping);
  GetDlgItem(IDC_RB_RUESS)->EnableWindow(bEnableMapping);
  GetDlgItem(IDC_RB_VOIGHT)->EnableWindow(bEnableMapping);
  GetDlgItem(IDC_RB_LINEAR)->EnableWindow(m_model.ResultRegister().Linear() && bDeformation && !bNoResults);
  GetDlgItem(IDC_RB_NON_LINEAR)->EnableWindow(m_model.ResultRegister().NonLinear() && bDeformation && !bNoResults);
  GetDlgItem(IDC_RADIO_MIXTURE)->EnableWindow(m_model.ResultRegister().Mixture() && bDeformation && !bNoResults);
  GetDlgItem(IDC_CB_TIMESTEP)->EnableWindow(bDeformation && !bNoResults);

  bool bFormations =
  ((m_pFormationSelection != 0) && !m_pFormationSelection->empty()) ||
  ((m_pFormationPlaneSelection != 0) && !m_pFormationPlaneSelection->empty());

  bool bHorizons =
  ((m_pHorizonSelection != 0) && !m_pHorizonSelection->empty()) ||
  ((m_pSlipHorizonSelection != 0) && !m_pSlipHorizonSelection->empty());

  GetDlgItem(IDC_FULL_ELEMENTS)->EnableWindow(bFormations || bHorizons);
  GetDlgItem(IDC_CENTER_POINTS)->EnableWindow(bFormations || bHorizons);
  GetDlgItem(IDC_WELLPATH_CENTER_POINTS)->
  EnableWindow((bFormations || bHorizons) && (Model().parentModel() != 0) &&
      (dynamic_cast <IWellModel*> (&(Model())) != 0));

    //wjrx mantis 2951
  GetDlgItem(IDC_FORMATION_NAMES)->EnableWindow(bFormations || bPointSet || bElementSet || bNewWellPath);

  OnEnableExport();
}

bool CResultExportDlg::ResultComposite(const IValueComposite& composite) const
{
  if(m_RGObserver->ResultComposite(composite))
  return true;
  return m_MatRGObserver->ResultComposite(composite);
}

void CResultExportDlg::ResultComposite(const IValueComposite& composite, bool bEnable)
{
  if(dynamic_cast<const CMaterialResult*>(&composite))
  m_MatRGObserver->ResultComposite(composite, bEnable);
  else
  m_RGObserver->ResultComposite(composite, bEnable);
}

void CResultExportDlg::MeshResultComposite(const IValueComposite& composite, bool bEnable)
{
  ResultComposite(composite, bEnable);

  if (!m_bBranchToggle)
  OnEnableExport();
}

void CResultExportDlg::BranchToggle(bool bBranchToggle)
{
  if (m_bBranchToggle && !bBranchToggle)
      OnEnableExport();
  m_bBranchToggle = bBranchToggle;
}

// wjrx mantis 3322
void CResultExportDlg::SelectAllNewWellpaths(bool bSelect)
{
  m_bExportNewWellpaths = bSelect;
}

namespace
{

bool areInputResultsPresent(CLocalResultGroupObserver<CResultExportDlg>* exportResultGroupObserver)
{
  const TResultComponentSet& results = exportResultGroupObserver->Results();
  bool inputResultsArePresent = false;

  for (TResultComponentSet::const_iterator result = results.begin();
  (result != results.end()) && !inputResultsArePresent; ++result)
  {
  const CResultTree::CScalarResult* scalarResult =
      dynamic_cast <const CResultTree::CScalarResult*> ((*result).first);

  inputResultsArePresent = CExportResults::isInputResult(scalarResult);
  }

  return inputResultsArePresent;
}

} // anonymous namespace

void CResultExportDlg::OnExport() 
{
  UpdateData(TRUE);

  // Check the following
  // A. At least one depletionstage is selected
  // B. At least one analysis type is selected
  // C. At least one geometrical object or pointset is selected
  // D. At least one result is selected
  if(Model().ResultRegister().ResultsAvailable())
  {
  bool inputResultsArePresent = areInputResultsPresent(m_RGObserver);

  if(!inputResultsArePresent && !m_bLinear && !m_bNonLinear && !m_bHeat &&
      !m_bMixture && !m_bMixtureContainment)
    {
      _m()->msg(IDS_NO_ANALYSIS_TYPE_SELECTED);
      return;
    }

    if(!m_stTimeStep.size())
    {
      _m()->msg(IDS_NO_TIMESTEP_SELECTED);
      return;
    }
  }

  if (Selection().empty())
  {
    _m()->msg(IDS_NO_EXPORT_GEOMETRY_SELECTED);
    return;
  }

  if ( m_RGObserver->Results().size() == 0
  && m_MatRGObserver->Results().size() == 0
  && m_GVTRGObserver->Results().size() == 0
  && !m_bExportNewWellpaths
  )
  {
    //_m()->msg(IDS_NO_RESULT_SELECTED);
    if( _m()->msg( IDS_NO_RESULT_SELECTED, MB_YESNO | MB_ICONQUESTION ) == IDNO )
      return;
  }

  CString pathName;
  DWORD filterIndex;

  if(getPathName(pathName, filterIndex) == IDOK)
  {
    // Get file node

  if ((pathName.Right(4).CompareNoCase(".las") == 0) &&
      m_resultExportConfiguration.export2LAS)
  {
      if (Selection().getSelection().size() > 1)
      {
    _m()->msg(IDS_TOO_MANY_WELL_PATHS_SELECTED);
      }
      else
      {
    OnExport(pathName, false, false);
      }
  }

    if(pathName.Right(4).CompareNoCase(".dat") == 0)
      OnExport(pathName, false, filterIndex == 3);
    if(pathName.Right(4).CompareNoCase(".xls") == 0 || pathName.Right(5).Left(4).CompareNoCase(".xls") == 0)
      OnExport(pathName, true, filterIndex == 4);
    if(pathName.Right(4).CompareNoCase(".mx") == 0)
      OnExportGoCad(pathName);
  }

}

void CResultExportDlg::OnSelect(const COpenGLNode& node) 
{
  if (m_RGObserver)
  {
  adjustVisibleResultComponents(m_RGObserver);
  }

  if (m_MatRGObserver)
  {
  adjustVisibleResultComponents(m_MatRGObserver);
  }

  if (m_GVTRGObserver)
  {
  adjustVisibleResultComponents(m_GVTRGObserver);
  }
  
  m_tcValue.UpdateTree();

  UpdateButtons();
}

void CResultExportDlg::getExportResultData(CExportResultData* exportResultData, bool bUnbind)
{
  CString macroName;

  UpdateData(TRUE);

  m_macroName.GetWindowText(macroName);
  exportResultData->Name((LPCTSTR) macroName);
  exportResultData->m_selection = Selection();
  exportResultData->m_nUnit = m_nUnit;
  exportResultData->m_nTZ = m_nTZ;

  assert(m_cbHotSpot.GetCurSel() != -1);

  exportResultData->m_bLinear = m_bLinear;
  exportResultData->m_bNonLinear = m_bNonLinear;
  exportResultData->m_bHeat = m_bHeat;
  exportResultData->m_bMixture = m_bMixture;
  exportResultData->m_bMixtureContainment = m_bMixtureContainment;

  if (!exportResultData->ReservoirMacro())
  {
  exportResultData->m_pHotSpot =
      (const CHotSpot*)m_cbHotSpot.GetItemData(m_cbHotSpot.GetCurSel());

  exportResultData->m_RGObserverData = m_RGObserver->Results();
  exportResultData->m_MatRGObserverData = m_MatRGObserver->Results();
  exportResultData->m_GVTRGObserverData = m_GVTRGObserver->Results();
  }

  assert(m_cbTimeStep.GetCurSel() != -1);

  exportResultData->m_pTimeStep =
  (const CDepletionStage*) m_cbTimeStep.GetItemData(m_cbTimeStep.GetCurSel());
  exportResultData->m_nLinear = m_nLinear;
  exportResultData->m_nCenterPoints = m_nCenterPoints;
  exportResultData->m_nWellPoints = m_nWellPoints;
  exportResultData->m_nFormationNames = m_nFormationNames;
  exportResultData->m_nWeightingType = m_nWeightingType;
  exportResultData->m_stTimeStep = m_stTimeStep;

  if (bUnbind)
  exportResultData->unbind(true);
}

void CResultExportDlg::setExportResultData(CExportResultData* exportResultData,
  bool updateData)
{
  m_exportResultData = exportResultData;
  m_updateData = updateData;
}

bool CResultExportDlg::OnExport(const CString& sFileName, bool bExcel, bool bOld)
{
  CExportResultData exportResultData(m_model);

  getExportResultData(&exportResultData, false);

  CExportResults exportResults(exportResultData, m_model);

  bool retval = exportResults.onExport((LPCSTR)sFileName, bExcel, bOld,
  m_resultExportConfiguration.export2LAS);

  exportResultData.unbind(false);

  return retval;
}

bool CResultExportDlg::OnExportGoCad(const CString& sFileName)
{
  return true;
}

void CResultExportDlg::adjustVisibleResultComponents(CLocalResultGroupObserver<CResultExportDlg>* observer)
{
  // First formation, maybe there are some results selected that are not allowed
  // to be selected!!

  TResultComponentSet::const_iterator it;

  it = observer->Results().begin();

  while (it != observer->Results().end())
  {
  if(!ValueComponentCanMapSelection((*it).first->Component((*it).second)))
//    if (!m_vcCategory.canMap(((*it).first)->Component((*it).second)).second)
  {
      observer->ResultComponent(*(*it).first, (*it).second, false);
      it = observer->Results().begin();
      continue;
  }

  ++it;
  }
}

void CResultExportDlg::OnAnalysisTypeChanged()
{
  UpdateData(TRUE);
  m_tcValue.UpdateTree();
}

void CResultExportDlg::OnOK()
{
  assert(m_exportResultData != 0);

  getExportResultData(m_exportResultData);

  IExportDlg::OnOK();
}

INT_PTR CResultExportDlg::getPathName(CString& pathName, DWORD& filterIndex)
{
  // Start file dialog
  // as soon as eclipse export is available, enable next line and delete line after that!!
  //CString	sFilter = "ASCII Files (*.dat)|*.dat|Excel Files (*.xls;*.xls*)|*.xls;*.xls*|Eclipse Files (*.grid)|*.grid|";
  CString	sFilter = "ASCII Files (*.dat)|*.dat|Excel Files (*.xls;*.xls*)|*.xls;*.xls*|";

  if (m_resultExportConfiguration.export2LAS)
  {
  sFilter += "LAS 3.0 Files (*.las)|*.las|";
  }

  // Add type for older format. Note that the index is used later to check the type!
  sFilter += "Old format ASCII Files (*.dat)|*.dat|Old format Excel Files (*.xls;*.xls*)|*.xls;*.xls*|";
  if(FALSE)
    sFilter += "Gocad Files (*.mx)|*.mx|";

  // Default file name is documentfile name
  CString sDefaultFileName = GetGeomecDoc()->GetTitle();
  sDefaultFileName = RemoveExtension((const char *)sDefaultFileName, true).toStdString().c_str();

  // Make some file dialog execute it and make it the path...
  CTnoFileDialog dlg(FALSE, "dat", sDefaultFileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, sFilter);
  if(m_bExportNewWellpaths) // default to xls
  dlg.m_ofn.nFilterIndex = 2;

  if (m_resultExportConfiguration.export2LAS)
  {
  dlg.m_ofn.nFilterIndex = 3;
  }

  INT_PTR result = dlg.DoModal();

  if(result == IDOK)
  {
  pathName = dlg.GetPathName();
  filterIndex = dlg.m_ofn.nFilterIndex;
  }

  return result;
}

void CResultExportDlg::OnImportExportMacro()
{
  // Should only be activated if number of export macros > 0!

  CExportResultDataEntry* exportResultDataEntry =
  (CExportResultDataEntry*) m_model.GraphEntry(MD_BASE_EXPORT_MACROS);

  if ((exportResultDataEntry != 0) &&
  (exportResultDataEntry->EntryNodes().size() > 0))
  {
  CExportResultData* exportResultData;
  CSelectExportMacroDialog selectExportMacroDialog(&exportResultData,
      exportResultDataEntry->GraphEntryNodes());

  if (selectExportMacroDialog.DoModal() == IDOK)
  {
      setExportResultData(exportResultData, true);
      initializeDialog(false);
  }
  }
}

void CResultExportDlg::OnSaveExportMacro()
{
  const CExportResultDataEntry* exportResultDataEntry =
  (CExportResultDataEntry*) m_model.GraphEntry(MD_BASE_EXPORT_MACROS);
  const CGraphEntry::TEntryNodeSet& entryNodeSet =
  exportResultDataEntry->GraphEntryNodes();
  CString macroName;
  bool duplicateName = false;

  m_macroName.GetWindowText(macroName);

  if (exportResultDataEntry != 0)
  {
  for (CGraphEntry::TEntryNodeSet::const_iterator iterator =
      entryNodeSet.begin(); !duplicateName &&
      (iterator != entryNodeSet.end()); ++iterator)
  {
      duplicateName = ((*iterator)->Name() == macroName);
  }
  }

  if (duplicateName)
  {
  QString message = macroName + "already exists." + "\n" +
      "Do you want to replace it?";

  if (_m()->msg(message, MB_YESNO) == IDNO)
  {
      return;
  }
  }

  if (!duplicateName)
  {
  CExportResultData* exportResultData =
      new CExportResultData((LPCTSTR) macroName, m_model);
  CExportResultData_Delegate* exportResultData_Delegate =
      new CExportResultData_Delegate(exportResultData);

  getExportResultData(exportResultData);
  }
  else
  {
  if (m_exportResultData == 0)
  {
      for (CGraphEntry::TEntryNodeSet::const_iterator iterator =
    entryNodeSet.begin(); iterator != entryNodeSet.end(); ++iterator)
      {
    if ((*iterator)->Name() == macroName)
    {
          CExportResultData* exportResultData = (CExportResultData*) *iterator;
          CExportResultData_Delegate* exportResultData_Delegate =
      new CExportResultData_Delegate(exportResultData);

          getExportResultData(exportResultData);
    }
      }
  }
  else
  {
      getExportResultData(m_exportResultData);
  }
  }
}

void CResultExportDlg::OnMacroName()
{
  GetDlgItem(IDC_SAVE_EXPORT_MACRO)->
  EnableWindow(m_macroName.GetWindowTextLength() > 0);
}

void CResultExportDlg::OnEnableExport()
{
  // Since both m_pSlipHorizonSelection and m_pHorizonSelection are updated with
  // the selected horizons I create a temporary set containing the difference
  // between m_pSlipHorizonSelection and m_pHorizonSelection that can be
  // interpreted as the actual slipping horizons.

  bool nothingSelected =
  ((m_pSlipHorizonSelection != 0) && m_pSlipHorizonSelection->empty()) &&
  ((m_pHorizonSelection != 0) && m_pHorizonSelection->empty()) &&
  ((m_pFormationSelection != 0) && m_pFormationSelection->empty()) &&
  ((m_pFormationPlaneSelection != 0) && m_pFormationPlaneSelection->empty()) &&
  ((m_pRTCISelection != 0) && m_pRTCISelection->empty()) &&
  ((m_pNewWellPathSelection != 0) && m_pNewWellPathSelection->empty()) &&
  ((m_pPointSetSelection != 0) && m_pPointSetSelection->empty()) &&
  ((m_pElementSetSelection != 0) && m_pElementSetSelection->empty()) &&
  ((m_pNonMeshedSurfaceSelection != 0) && m_pNonMeshedSurfaceSelection->empty());
  bool onlyHorizons =
  ((m_pSlipHorizonSelection != 0) && !m_pSlipHorizonSelection->empty()) &&
  ((m_pHorizonSelection != 0) && !m_pHorizonSelection->empty()) &&
  ((m_pFormationSelection != 0) && m_pFormationSelection->empty()) &&
  ((m_pFormationPlaneSelection != 0) && m_pFormationPlaneSelection->empty()) &&
  ((m_pRTCISelection != 0) && m_pRTCISelection->empty()) &&
  ((m_pNewWellPathSelection != 0) && m_pNewWellPathSelection->empty()) &&
  ((m_pPointSetSelection != 0) && m_pPointSetSelection->empty()) &&
  ((m_pElementSetSelection != 0) && m_pElementSetSelection->empty()) &&
  ((m_pNonMeshedSurfaceSelection != 0) && m_pNonMeshedSurfaceSelection->empty());
  bool noResultsSelected = (m_exportResultData && m_exportResultData->ReservoirMacro()) || (m_MatRGObserver->Results().empty() && m_RGObserver->Results().empty() && m_GVTRGObserver->Results().empty());
  bool onlyMaterialParameters = (!m_exportResultData || !m_exportResultData->ReservoirMacro()) && !m_MatRGObserver->Results().empty() && m_RGObserver->Results().empty() && m_GVTRGObserver->Results().empty();

  bool bEnable = !nothingSelected && !noResultsSelected && !(onlyHorizons && onlyMaterialParameters);

  GetDlgItem(IDC_EXPORT)->EnableWindow(bEnable);

  if (bEnable)
  SetDefID(IDC_EXPORT);
  else if (m_buttonOK)
  SetDefID(IDOK);
  else
  SetDefID(0);

}

void CResultExportDlg::toggleState() const
{
  const_cast <CResultExportDlg*> (this)->OnEnableExport();
}

// private

void CResultExportDlg::initializeDialog(bool firstTime)
{
  if (m_exportResultData)
  m_exportResultData->bind();

  if (m_updateData)
  {
  m_nUnit = m_exportResultData->m_nUnit;
  m_nTZ = m_exportResultData->m_nTZ;
  m_bLinear = m_exportResultData->m_bLinear;
  m_bNonLinear = m_exportResultData->m_bNonLinear;
  m_bHeat = m_exportResultData->m_bHeat;
  m_bMixture = m_exportResultData->m_bMixture;
  m_bMixtureContainment = m_exportResultData->m_bMixtureContainment;
  m_nLinear = m_exportResultData->m_nLinear;
  m_nCenterPoints = m_exportResultData->m_nCenterPoints;
  m_nFormationNames = m_exportResultData->m_nFormationNames;
  m_nWellPoints = m_exportResultData->m_nWellPoints;
  m_nWeightingType = m_exportResultData->m_nWeightingType;
  m_stTimeStep = m_exportResultData->m_stTimeStep;

  UpdateData(FALSE);
  }

  if (m_exportResultData)
  {
  m_macroName.SetWindowText(m_exportResultData->Name().toStdString().c_str());
  }

  if (firstTime)
  {
  UpdateTimeStep();
  }

  if (m_exportResultData)
  {
  CDepletionStage* depletionStage = 0;

  for (int i = 0; i < m_cbTimeStep.GetCount(); ++i)
  {
      depletionStage = (CDepletionStage*) m_cbTimeStep.GetItemData(i);

      if (depletionStage == m_exportResultData->m_pTimeStep)
      {
    m_cbTimeStep.SetCurSel(i);
    break;
      }
  }
  }

  //TWellPathCategory* pWPCat = new TWellPathCategory( *this, m_tcObject, Model() );
  //TNewWellPathCategory* pNWPCat = new TNewWellPathCategory( *this, m_tcObject, Model() );

  //m_vcCategory.onInitDialog(*this, m_tcObject, Model(), pWPCat, pNWPCat,
  //  Model().AxisTranslator().Is2D());

  if (firstTime)
  {
  if (m_resultExportConfiguration.enableSlipHorizonSelection)
  {
      m_pSlipHorizonSelection = new THorizonBaseSelection(Selection(), new CSlipHorizonSelectionFilter);
      re::CreateExportResultTreeBranch<re::CSlipHorizonEntryObserver, THorizonBaseEntry, MD_BASE_HORIZON, THorizonBaseSelection>(m_model, *m_pSlipHorizonSelection, m_tcObject);
  }

  if (m_resultExportConfiguration.enableHorizonSelection)
  {
      m_pHorizonSelection = new THorizonBaseSelection(Selection(), new CHorizonSelectionFilter);
      re::CreateExportResultTreeBranch<re::CHorizonEntryObserver, THorizonBaseEntry, MD_BASE_HORIZON, THorizonBaseSelection>(m_model, *m_pHorizonSelection, m_tcObject);
  }

  if (m_resultExportConfiguration.enableFormationSelection)
  {
      m_pFormationSelection = new TFormationSelection(Selection());
      re::CreateExportResultTreeBranch<re::CFormationBaseEntryObserver, TFormationBaseEntry, MD_BASE_FORMATION, TFormationSelection>(m_model, *m_pFormationSelection, m_tcObject);
  }

  if (m_resultExportConfiguration.enableFormationPlaneSelection)
  {
      m_pFormationPlaneSelection = new TFormationPlaneSelection(Selection());
      re::CreateExportResultTreeBranch<re::CFormationPlaneEntryObserver, CHexaFormationEntry, MD_HEXA_FORMATION, TFormationPlaneSelection>(m_model, *m_pFormationPlaneSelection, m_tcObject);
  }

  if (m_resultExportConfiguration.enableRTCISelection)
  {
      m_pRTCISelection = new TRTCISelection(Selection());
      re::CreateExportResultTreeBranch<re::CRTCIEntryObserver, CWellCasingRTCIEntry, MD_WELLCASING_RTCI, TRTCISelection>(m_model, *m_pRTCISelection, m_tcObject);
  }

  if (m_resultExportConfiguration.enableNewWellPathSelection)
  {
      m_pNewWellPathSelection = new TNewWellPathSelection(Selection());
      re::CreateExportResultTreeBranch<re::CNewWellPathEntryObserver, CNewWellPathEntry, MD_NEW_WELLPATH, TNewWellPathSelection>(m_model, *m_pNewWellPathSelection, m_tcObject);
  }

  if (m_resultExportConfiguration.enablePointSetSelection)
  {
      m_pPointSetSelection = new TPointSetSelection(Selection());
      re::CreateExportResultTreeBranch<re::CPointSetEntryObserver, TPointSetEntry, MD_BASE_POINTSET, TPointSetSelection>(m_model, *m_pPointSetSelection, m_tcObject);
  }

  if (m_resultExportConfiguration.enableElementSetSelection)
  {
      m_pElementSetSelection = new TElementSetSelection(Selection(), new CElementSetSelectionFilter);
      re::CreateExportResultTreeBranch<re::CElementSetEntryObserver, TPointSetEntry, MD_BASE_POINTSET, TElementSetSelection>(m_model, *m_pElementSetSelection, m_tcObject);
  }

  if (m_resultExportConfiguration.enableNonMeshedSurfaceSelection)
  {
      m_pNonMeshedSurfaceSelection = new TNonMeshedSurfaceSelection(Selection());
      re::CreateExportResultTreeBranch<re::CNonMeshedSurfaceEntryObserver, CNonMeshedSurfaceEntry, MD_BASE_NONMESHEDSURFACE, TNonMeshedSurfaceSelection>(m_model, *m_pNonMeshedSurfaceSelection, m_tcObject);
  }
  }

  if (m_updateData)
  {
  Selection() = m_exportResultData->m_selection;
  }

  if (!m_exportResultData || !m_exportResultData->ReservoirMacro())
  {
  if (firstTime)
  {
      m_MatRGObserver = new CLocalResultGroupObserver<CResultExportDlg>(*this, m_model.MaterialResultTree(), m_tcValue);
      m_GVTRGObserver = new CLocalResultGroupObserver<CResultExportDlg>(*this, m_model.GVTResultTree(), m_tcValue);
      m_RGObserver = new CLocalResultGroupObserver<CResultExportDlg>(*this, m_model.ResultTree(), m_tcValue);
  }

  // for result macro attributes initialization, we need to update the selections
  m_MatRGObserver->newSelection(m_exportResultData == 0 ? TResultComponentSet() : m_exportResultData->m_MatRGObserverData);
  m_RGObserver->newSelection(m_exportResultData == 0 ? TResultComponentSet() : m_exportResultData->m_RGObserverData);
  m_GVTRGObserver->newSelection(m_exportResultData == 0 ? TResultComponentSet() : m_exportResultData->m_GVTRGObserverData);

  if (m_bExportNewWellpaths)
  {
      CNewWellPathEntry& entry = (CNewWellPathEntry&)*m_model.GraphEntry(MD_NEW_WELLPATH);
      const CNewWellPathEntry::TNodeSet& stNodes = entry.EntryNodes();
      for (CNewWellPathEntry::TNodeSet::const_iterator it = stNodes.begin(); it != stNodes.end(); ++it)
    if (m_resultExportConfiguration.export2LAS)
    {
          if (m_resultExportConfiguration.selectedNewWellPath == *it)
          {
      m_pNewWellPathSelection->SetSelected(**it, true);
          }
    }
    else if (m_pNewWellPathSelection != 0)
          m_pNewWellPathSelection->SetSelected(**it, true);
  }

  m_tcObject.UpdateTree();

  if (m_InitiallyOn)
  {
      const CResultGroup *rg = dynamic_cast<const CResultGroup*>(m_InitiallyOn);
      if (rg)
      {
    if (rg->Parent() != 0)
    {
          if (dynamic_cast<const CMaterialResultTree*>(rg))
      m_MatRGObserver->ResultGroup(*rg, true);
          else
      m_RGObserver->ResultGroup(*rg, true);
    }
      }
      else
      {
    const IValueComposite *vc = dynamic_cast<const IValueComposite*>(m_InitiallyOn);
    assert(vc);
    if (m_nInitialIndex == -1)
    {
          if (dynamic_cast<const CMaterialResult*>(vc))
      m_MatRGObserver->ResultComposite(*vc, true);
          else
      m_RGObserver->ResultComposite(*vc, true);
    }
    else
          m_RGObserver->ResultComponent(*vc, m_nInitialIndex, true);
      }
  }

  typedef CSelectionLeafObserver_Delegate<IValueComposite, IValueComposite_Delegate, CResultExportDlg, FALSE> TCompositeObs;
  typedef CSelectionBranchObserver_Delegate<CGraphEntry, CGraphEntry_Delegate, IValueComposite, TCompositeObs, IValueComposite, CResultExportDlg, FALSE, FIXED_ITEM> TCompositeEntryObs;

  // Insert resulttree
  if (firstTime)
  {
      new TCompositeEntryObs(*Model().GraphEntry(MD_BASE_MESH_RESULT), m_tcValue,
    *this, &CResultExportDlg::ResultComposite,
    &CResultExportDlg::MeshResultComposite);
  }

  m_tcValue.UpdateTree();

  // Reset tree control and populate object tree
  //  m_vcCategory.populateTree(m_tcObject);

  UpdateHotSpot();

  if (m_exportResultData && (m_exportResultData->m_pHotSpot != 0))
  {
      CHotSpot* hotSpot = 0;

      for (int i = 0; i < m_cbHotSpot.GetCount(); ++i)
      {
    hotSpot = (CHotSpot*)m_cbHotSpot.GetItemData(i);

    if (hotSpot == m_exportResultData->m_pHotSpot)
    {
          m_cbHotSpot.SetCurSel(i);
          break;
    }
      }
  }
  }
  else
  m_tcObject.UpdateTree();


  UpdateButtons();

  GetDlgItem(IDCANCEL)->SetWindowText(m_textCancel);

  if (m_buttonOK)
  {
  GetDlgItem(IDC_EXPORT)->EnableWindow(FALSE);
  GetDlgItem(IDC_EXPORT)->ShowWindow(SW_HIDE);
  GetDlgItem(IDOK)->ShowWindow(SW_SHOW);
  GetDlgItem(IDC_IMPORT_EXPORT_MACRO)->ShowWindow(SW_HIDE);
  GetDlgItem(IDC_SAVE_EXPORT_MACRO)->ShowWindow(SW_HIDE);
  SetDefID(IDOK);

  if (m_exportResultData && m_exportResultData->ReservoirMacro())
  {
      GetDlgItem(IDC_EXPORT_OPTIONS)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_EXPORT_OPTIONS1)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_EXPORT_OPTIONS2)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_EXPORT_OPTIONS3)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_RB_POINT_INTERPOLATION)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_RB_UNWEIGTHED)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_RB_RUESS)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_RB_VOIGHT)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_RB_LINEAR)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_RB_NON_LINEAR)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_RADIO_MIXTURE)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_CB_TIMESTEP)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_LB_TIMESTEP)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_FULL_ELEMENTS)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_CENTER_POINTS)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_WELLPATH_CENTER_POINTS)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_FORMATION_NAMES)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_WELLPATH_MESH_INTERSECTION_POINTS)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_RB_UNIT_SI)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_RB_UNIT_FIELD)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_CK_LINEAR)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_CK_NON_LINEAR)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_CK_HEAT)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_CK_MIXTURE)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_CK_MIXTURE_CONTAINMENT)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_CB_HOTSPOT)->ShowWindow(SW_HIDE);

      CRect rect;
      GetWindowRect(rect);
      SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height() - 500, SWP_NOZORDER | SWP_NOMOVE);

      SetWindowText("Reservoirs for coupled analysis");

      m_macroName.SetReadOnly();
  }
  }
  else
  {
  GetDlgItem(IDOK)->EnableWindow(FALSE);
  GetDlgItem(IDOK)->ShowWindow(SW_HIDE);
  GetDlgItem(IDC_EXPORT)->ShowWindow(SW_SHOW);
  GetDlgItem(IDC_IMPORT_EXPORT_MACRO)->ShowWindow(SW_SHOW);
  GetDlgItem(IDC_SAVE_EXPORT_MACRO)->ShowWindow(SW_SHOW);
  SetDefID(IDC_EXPORT);
  OnEnableExport();
  }

  OnMacroName();

  if (m_exportResultData)
  m_exportResultData->unbind(false);
}
