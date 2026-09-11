// ModelBase.cpp: implementation of the CModelBase class.
//
//////////////////////////////////////////////////////////////////////

#include <memory>

#include "BaseEntryTypes.h"
#include "ModelBase.h"
#include "MaterialEntry.h"
#include "HorizonBase.h"
#include "GlobalInitialStressNode.h"
#include "GlobalPressure.h"
#include "MeshBase.h"
#include "ColorScaleEntry.h" 
#include "ResultTree.h"
#include "DerivedResult.h"
#include "GVTResult.h"
#include "RpnValueSet.h"
#include "GlobalTemperature.h"
#include "AnalysisPoint.h"
#include "BoundaryBase.h"
#include "MeshResultTree.h"
#include "ElementValueSet.h"
#include "KeyFile.h"
#include "HomogenizationBox.h"
#include "FormationBase.h"
#include "BranchState.h"
#include "GeomecUtils.h"
#include "DianaRunController.h"
#include "LasPointSet.h"
#include "GVTSettings.h"
#include "QBSettings.h"

#include "WellPath.h"
#include "NewWellPath.h"
#include "MaterialResultTree.h"
#include "WellCasingSteelMaterial.h"
#include "InterfaceMaterial.h"
#include "NewArchiveStdStringStream.h"
#include "HDF5File.h"

#include "DocumentProxy.h"
#include "Global.h"
#include "IProgressBase.h"
#include "VersionNumbers.h"
#include "HexaModel.h"
#include "GoCadModel.h"
#include "EclipseModel.h"
#include "AnalysisLogger.h"
#include "VersionManager.h"
#include "Pressure.h"
#include "GeneralDataBlock.h"

#include <fstream>

#include "WellZoomInModel.h"
#include "ZoominModelEntry.h"
#include "ZoominModelPlaceHolder.h"
#include "GeoSurface.h"
#include "GocadElementSet.h"
#include "ExportResultDataEntry.h"
#include "ExportResultData.h"
#include "ResultCache.h"

#include "CLIParser.h"
#include "ModelDiagnostics.h"
#include "NewWellPathInput.h"
#include "ValueMapper.h"
#include "ConsistencyGuard.h"

#include "GlobalMessage.h"
#include "RunAnalysis_CLI.h"

#include "Global.h"
#include "IProgressFactory.h"


#define DEF_ELEMENT_ORDER CElementOrder::EO_LINEAR // the default element order
#define DEF_FAULT_INIT FI_FULLSLIP

const int MAX_NUM_SOLVER_THREADS = 16;



CModelBase::CModelLoadSaveDefault::CModelLoadSaveDefault(CModelBase *pParentModel, IModelDiagnostics *pModelDiagnostics)
  : m_pVersionManager(0)
  , m_pParentModel(pParentModel)
  , m_pModelDiagnostics(pModelDiagnostics)
{
}

CModelBase::CModelLoadSaveDefault::CModelLoadSaveDefault(const QString& appVersion, CModelBase *pParentModel, IModelDiagnostics *pModelDiagnostics)
  : m_pVersionManager(0)
  , m_pParentModel(pParentModel)
  , m_pModelDiagnostics(pModelDiagnostics)
{
  SetAppVersion(appVersion);
}

CModelBase::CModelLoadSaveDefault::~CModelLoadSaveDefault()
{
}

bool CModelBase::CModelLoadSaveDefault::LoadModel(CStorageNode::TSTREAM& stream, CStreamVersion& /*version*/, CStorageNode::TPROGRESS& prog, CModelBase& model)
{
  assert(m_pVersionManager);

  if(m_pParentModel)
  {
  // try to attach zoom-in model
  CZoominModelEntry* pEntry = (CZoominModelEntry*)m_pParentModel->GraphEntry(MD_BASE_ZOOMIN_MODEL);
  if (pEntry)
  {
      if (!pEntry->AddChildModel(model))
    return false;
  }
  }
  else
  {
  model.NewModel(false);
  if (m_pModelDiagnostics)
      model.SetDiagnosticsStrategy(m_pModelDiagnostics);
  }

  return model.Load(stream, m_pVersionManager->FileVersion(), prog, true);
}

bool CModelBase::CModelLoadSaveDefault::SaveModel(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS& prog, CModelBase& model)
{
  stream.PushDataSet(CHDF5File::ModelGroup(model.Index()) + "ModelStream");

  model.SaveFileHeader(stream, GetAppVersion());
  bool retval = model.Save(stream, prog, true);

  stream.PopDataSet();

  return retval;
}

long CModelBase::CModelLoadSaveDefault::SavedItems(CModelBase& model) const
{
  return model.SavedItems();
}

void CModelBase::CModelLoadSaveDefault::SetVersionManager(CVersionManager& versionManager)
{
  m_pVersionManager = &versionManager;
}




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//##ModelId=3BC55D6102A9
CModelBase::CModelBase(CAnalysisLogger& logger, const CVersionManager& versionManager) : CFemAppModel(logger), 
  m_pDepletionStageEntry(0),
  m_pPressure(0),
  m_pGlobalTemperature(0),
  m_gravity(9.8, 0, 15, CQuantity::SI_UNIT),
  m_pBranchState(0),
  m_bNoCSHE(true),
  m_nElementOrder(DEF_ELEMENT_ORDER),
  m_nFaultInit(DEF_FAULT_INIT),
  m_bLargeDeform(false),
  m_bLoading(false),
  m_bInvalidateResultsAfterLoad(false),
  m_bAllowLinearCalc(true),
  m_bSaltStressInitRelax(true),
  m_nIniStressMaxIterations(3),
  m_dIniStressConvCriterion(0.01),
  m_bLinstaOnlyInitialStress(false),
  m_bPressureSupportNodeLoadWarningDone(false),
  m_bAverageResults(false),
  m_pGVTSettings(0),
  m_pQBSettings(0),
  m_bUseParallelSolver(true),
  m_nNumSolverThreads(8),
  m_bUseStressSmootheningOnDSF(true),
  m_nDSFInterSurfType(1),
  m_bUseDecompactionParams(false),
  m_bAllowRedrawing(true),
  m_bForcingRedraw(false),
  m_bAutomaticallyDisableDrawing(false),
  m_bExportCommandFilesWithDefaults(false),
  m_bWriteOutputStreamFile(false),
  m_bCleanupOldBranchResults(false),
  m_bShowConvexHulls(false),
  m_pChildModelLoadSave(0),
  m_versionManager(versionManager),
  m_nIndex(-1),
  m_pParentModel(0),
  m_bSourceDepletionStagesChanged(false),
  m_bSourceGlobalPressureChanged(false),
  m_bSourceGlobalTemperatureChanged(false),
  m_bSourceGravityChanged(false),
  m_ParentResultsDef(PRD_EQUAL),
  m_EnableResultCache(false),
  m_ResultCacheSize(0),
  m_bConvergenceChecks(false),
  m_bConvergenceMinimumAngle(true),
  m_dConvergenceMinimumAngle(3),
  m_bConvergenceMinimumRatio(true),
  m_dConvergenceMinimumRatio(0.01),
  m_bConvergenceMinimumVolume(true),
  m_dConvergenceMinimumVolume(1),
  m_bDefaultDianaZoominSetting(false),
  m_pModelDiagnostics(0),
  m_pConsistencyGuard(0),
  m_pBoundary(0),
  m_materialErrorHandler(logger),
  m_show_cm2_dlg( true ),
  m_mesher_dlg( nullptr )

{
  m_pGlobalStressNode = new CGlobalInitialStressNode(*this, 0, 0.01);

  m_bIsMeshed = false;

  m_pBranchState = new CBranchState(*this);

  m_pConsistencyGuard = new CConsistencyGuard(*this); // we always create one, but will delete it for child models; they will access the root guard
}

//##ModelId=3BC55D6102AB
CModelBase::~CModelBase()
{
  delete m_pBranchState;
  delete m_pGlobalStressNode;
  assert(!m_pDepletionStageEntry);
  if(m_pGVTSettings)
  delete m_pGVTSettings;
  if(m_pQBSettings)
  delete m_pQBSettings;
  delete m_pChildModelLoadSave;
  delete m_pConsistencyGuard;

  for (std::map<QString, CGeneralDataBlock *>::iterator it = m_generalDataBlocks.begin(); it != m_generalDataBlocks.end(); ++it)
  delete it->second;
}

void CModelBase::destroy()
{
  IModelObject::destroy();
}

void CModelBase::CloseModel()
{
  CFemAppModel::CloseModel();
}

void CModelBase::SetDiagnosticsStrategy(const IModelDiagnostics *pModelDiagnostics)
{
  m_pModelDiagnostics = pModelDiagnostics;
}

const IModelDiagnostics *CModelBase::GetDiagnosticsStrategy()
{
  if (m_pModelDiagnostics)
  return m_pModelDiagnostics;

  if (parentModel())
  return parentModel()->GetDiagnosticsStrategy();
  
  return 0;
}

CConsistencyGuard *CModelBase::GetConsistencyGuard()
{
  if (m_pConsistencyGuard)
  return m_pConsistencyGuard;

  if (parentModel())
  return parentModel()->GetConsistencyGuard();

  return 0;
}

bool CModelBase::CanUseCalculatedTemperatures() const
{
  return true;
}

const CGlobalInitialStressNode& CModelBase::GlobalInitialStress() const
{
  return *m_pGlobalStressNode;
}

CGlobalInitialStressNode& CModelBase::GlobalInitialStress()
{
  return *m_pGlobalStressNode;
}

const CGlobalPressure& CModelBase::GlobalPressure() const
{
  return *m_pPressure;
}

CGlobalPressure& CModelBase::GlobalPressure()
{
  return *m_pPressure;
}

const CGlobalTemperature &CModelBase::InitialTemperature() const
{
  return *m_pGlobalTemperature;
}

CGlobalTemperature &CModelBase::InitialTemperature()
{
  return *m_pGlobalTemperature;
}

//##ModelId=3BC55D6102A7
const QString& CModelBase::ProjectInfo() const
{
  return m_strProjectInfo;
}

//##ModelId=3BC55D6102A5
void CModelBase::ProjectInfo(const QString &strProjectInfo)
{
  m_strProjectInfo = strProjectInfo;
}

const CAcceleration& CModelBase::Gravity() const
{
  return m_gravity;
}


bool CModelBase::HasFormations() const
{
  CModelBase* pThis=const_cast<CModelBase*>(this);
  TFormationBaseEntry::TNodeSet formations = ((TFormationBaseEntry*) pThis->GraphEntry(MD_BASE_FORMATION))->EntryNodes();
  return(!(formations.begin() == formations.end()));
}

bool CModelBase::HasHorizons() const
{
  CModelBase* pThis=const_cast<CModelBase*>(this);
  THorizonBaseEntry::TNodeSet horizons = ((THorizonBaseEntry*) pThis->GraphEntry(MD_BASE_HORIZON))->EntryNodes();
  return(!(horizons.begin() == horizons.end()));
}

bool CModelBase::HasFaults() const
{
  CModelBase* pThis=const_cast<CModelBase*>(this);
  THorizonBaseEntry::TNodeSet horizons = ((THorizonBaseEntry*) pThis->GraphEntry(MD_BASE_HORIZON))->EntryNodes();
  THorizonBaseEntry::TNodeSet::const_iterator it;
  for(it = horizons.begin(); it != horizons.end(); ++it)
  {
  if((*it)->Slip())
      return true;
  }

  return false;
}

bool CModelBase::HasDoubleSidedFaults() const
{
  CModelBase* pThis = const_cast<CModelBase*>(this);
  THorizonBaseEntry::TNodeSet horizons = ((THorizonBaseEntry*)pThis->GraphEntry(MD_BASE_HORIZON))->EntryNodes();
  THorizonBaseEntry::TNodeSet::const_iterator it;
  for (it = horizons.begin(); it != horizons.end(); ++it)
  {
  if ((*it)->DoubleSidedFault())
      return true;
  }

  return false;
}

bool CModelBase::CanCreateMesh()
{
  return Mesh().CanCreateMesh();
}

bool CModelBase::IsMesh() const
{
  return Mesh().IsMesh();
}

void CModelBase::CreateMesh()
{
  Mesh().CreateMesh();
  GetConsistencyGuard()->NotifyChange(*this);
}

void CModelBase::InvalidateMesh()
{
  Mesh().InvalidateMesh();
}

void CModelBase::Gravity(const double& value, const CAcceleration::UNIT unit)
{
  assert(value <= m_gravity.MaxValue( unit));
  assert(value >= m_gravity.MinValue( unit));

  m_gravity.Value(value, unit);
  m_GravityNotifier.Modified();
}

CGraphNode& CModelBase::GravityNotifier()
{
  return m_GravityNotifier;
}

const CGVTSettings &CModelBase::GVTSettings() const
{
  return *m_pGVTSettings;
}

CGVTSettings &CModelBase::GVTSettings() 
{
  return *m_pGVTSettings;
}

const CQBSettings &CModelBase::QBSettings() const
{
  return *m_pQBSettings;
}

CQBSettings &CModelBase::QBSettings() 
{
  return *m_pQBSettings;
}

const CDepletionStage& CModelBase::InitialDepletionStage() const
{
  assert(m_pDepletionStageEntry);
  return m_pDepletionStageEntry->InitialStage();
}

CDepletionStage& CModelBase::InitialDepletionStage()
{ 
  assert(m_pDepletionStageEntry);
  return m_pDepletionStageEntry->InitialStage();
}

const CDepletionStage& CModelBase::LastDepletionStage() const
{
  return InitialDepletionStage().LastStage();
}

CDepletionStage& CModelBase::LastDepletionStage()
{
  return InitialDepletionStage().LastStage();
}

/// \brief determine the depl.stage, based on timeStepIndex
//
const CDepletionStage* CModelBase::FindDepletionStage
(int timeStepIndex) const
{
  const CDepletionStage *pFoundStage= 0;
  const CDepletionStage *pStage= &InitialDepletionStage();

  int index= 0;
  while (pStage && pFoundStage==0)
  {
  if ( index == timeStepIndex )
  {
      pFoundStage= pStage;
  }
  else
  {
      if ( pStage->Last() ) 
    pStage= 0;
      else
    pStage= &pStage->Next();
      ++index;
  }
  }
  return pFoundStage;
}
 
const CDepletionStageEntry& CModelBase::DepletionStageEntry() const
{
  return *m_pDepletionStageEntry;
}

CDepletionStageEntry& CModelBase::DepletionStageEntry()
{
  return *m_pDepletionStageEntry;
}

int CModelBase::NrOfDepletionStages() const
{
  assert(m_pDepletionStageEntry);
  return m_pDepletionStageEntry->EntryNodes().size();
}

void CModelBase::createContainers()
{
  new CColorScaleEntry(*this);
  CFemAppModel::createContainers();

  new CAnalysisPointEntry(*this);
  m_pDepletionStageEntry = new CDepletionStageEntry(MD_BASE_DEPLETION_STAGE, *this);
  new TFormationBaseEntry(MD_BASE_FORMATION, IDI_FORMATIONS, IDS_TREE_FORMATIONS, *this);
  new THorizonBaseEntry(MD_BASE_HORIZON, IDI_HORIZONS, IDS_TREE_HORIZONS, *this);

  new TValueCompositeEntry(MD_BASE_VALUE_COMPOSITE, 0, "GeoValueCompositeEntry", *this);
  new TResultEntry(MD_BASE_RESULT, 0, "ResultEntry", *this);

  // Insert of the registry derived result entry
  m_pMeshResultTree= new CMeshResultTree(*this);

  new CMaterialEntry(MD_ROCK_MATERIAL, IDI_ROCK_MATERIAL, IDS_ROCK_MATERIAL, *this);
  new TPointSetEntry(MD_BASE_POINTSET, IDI_POINTSET, IDS_TREE_POINTSETS, *this);
  new CWellCasingSteelMaterialEntry(MD_WELLCASING_STEELMATERIAL_ENTRY, *this);
  new CInterfaceMaterialEntry(MD_BASE_INTERFACEMATERIAL, *this);
  new CExportResultDataEntry(*this);
}

void CModelBase::createDefaults()
{
  CFemAppModel::createDefaults();

  assert(m_pDepletionStageEntry);
  (new CDepletionStage("Initial", *m_pDepletionStageEntry))->MarkAsInitial();
  new CDepletionStage("Depletion stage 1", InitialDepletionStage());
  m_pPressure = new CGlobalPressure(*this);
  m_pGlobalTemperature = new CGlobalTemperature(*this);
  m_pMatResultTree = new CMaterialResultTree(*this);
  m_pResultTree = new CResultTree(*this);
  m_pGVTResultTree = new CGVTResultTree(*this);

  m_pResultTree->RegistryResults().LoadResultFromRegistry();

  GlobalInitialStress().LinkTo(Mesh());
  m_pGVTSettings = new CGVTSettings(*this);
  m_pQBSettings = new CQBSettings(*this);

  if(parentModel())
  {
  // link to the source model's depletion stage entry, so we can notify the user when the source depletion stages have changed
  LinkTo(parentModel()->DepletionStageEntry());

  // same with initial temperature, pressure and gravity notifier nodes
  LinkTo(parentModel()->GlobalPressure());
  LinkTo(parentModel()->InitialTemperature());
  LinkTo(parentModel()->GravityNotifier());

  BuildDepletionStages();
  DuplicateGlobalPressure();
  DuplicateGlobalTemperature();
  DuplicateGravity();
  }
}

void CModelBase::DuplicateGlobalPressure()
{
  assert(parentModel());
  const CGlobalPressure& parentPressure = parentModel()->GlobalPressure();
  CGlobalPressure& myPressure = GlobalPressure();
  myPressure.Constant().Set(parentPressure.Constant().ReferenceValue().Value(),
              parentPressure.Constant().ReferenceDepth().Value(),
              parentPressure.Constant().Gradient().Value());
}
void CModelBase::DuplicateGlobalTemperature()
{
  assert(parentModel());
  const CGlobalTemperature& parentTemp = parentModel()->InitialTemperature();
  CGlobalTemperature& myTemp = InitialTemperature();
  myTemp.Constant().Set(parentTemp.Constant().ReferenceValue().Value(),
            parentTemp.Constant().ReferenceDepth().Value(),
            parentTemp.Constant().Gradient().Value());
}

void CModelBase::DuplicateGravity()
{
  assert(parentModel());
  Gravity(parentModel()->Gravity());
}

void CModelBase::OnExit()
{
  if (m_EnableResultCache && ResultRegister().cacheExists())
  ResultRegister().Cache().~CResultCache();

  CZoominModelEntry* pZIEntry = static_cast<CZoominModelEntry*>(GraphEntry(MD_BASE_ZOOMIN_MODEL));
  if (pZIEntry)
  {
  const CZoominModelEntry::TNodeSet& stNodes = pZIEntry->EntryNodes();
  for (CZoominModelEntry::TNodeSet::const_iterator it = stNodes.begin(); it != stNodes.end(); ++it)
  {
      (*it)->ChildModel().OnExit();
  }
  }

  CNewWellPathEntry* pNWPEntry = static_cast <CNewWellPathEntry*>(GraphEntry(MD_NEW_WELLPATH));
  if (pNWPEntry)
  {
  const CNewWellPathEntry::TNodeSet& stNodes = pNWPEntry->EntryNodes();
  for (CNewWellPathEntry::TNodeSet::const_iterator it = stNodes.begin(); it != stNodes.end(); ++it)
  {
      if ((*it)->hasZoomInModel())
    (*it)->ZoomInModel().OnExit();
  }
  }

  GetConsistencyGuard()->AttachHDF5File(0);
}

void CModelBase::DestroyCaches()
{
  CPointSetEntry *psEntry = static_cast<CPointSetEntry *>(GraphEntry(MD_BASE_POINTSET));
  psEntry->DestroyCaches();
}

void CModelBase::OnCloseModel()
{
  GetConsistencyGuard()->IsDeleting(true);

  // Delete resulttree before any value composites are deleted.
  delete m_pResultTree;
  m_pResultTree = 0;

  delete m_pMatResultTree;
  m_pMatResultTree = 0;

  delete m_pGVTResultTree;
  m_pGVTResultTree = 0;

  VERIFY(DeleteEntry(MD_BASE_ANALYSIS_POINT));
  VERIFY(DeleteEntry(MD_BASE_FORMATION));
  VERIFY(DeleteEntry(MD_BASE_HORIZON));
  VERIFY(DeleteEntry(MD_BASE_MESH_RESULT));

  delete m_pBoundary;
  m_pBoundary = 0;

  VERIFY(DeleteEntry(MD_BASE_INTERFACEMATERIAL));
  VERIFY(DeleteEntry(MD_WELLCASING_STEELMATERIAL_ENTRY));
  VERIFY(DeleteEntry(MD_BASE_DEPLETION_STAGE)); // moved to here...
  VERIFY(DeleteEntry(MD_BASE_POINTSET));
  VERIFY(DeleteEntry(MD_BASE_VALUE_COMPOSITE));
  // from here, for bug 
  VERIFY(DeleteEntry(MD_BASE_EXPORT_MACROS));
  VERIFY(DeleteEntry(MD_BASE_RESULT));
  m_pDepletionStageEntry = 0;

  // Delete global initial temperature
//	delete m_pGlobalTemperature;
//	m_pGlobalTemperature = 0;

  // At last we delete the components
  VERIFY(DeleteEntry(MD_ROCK_MATERIAL));

  CFemAppModel::OnCloseModel();

  VERIFY(DeleteEntry(MD_BASE_COLOR_SCALE));

  GetConsistencyGuard()->IsDeleting(false);

  if (parentModel())
  GetConsistencyGuard()->DetachChildModel(*this);
  else
  GetConsistencyGuard()->AttachHDF5File(0);
}

CBoundaryBase& CModelBase::Boundary()
{
  assert(m_pBoundary);
  return *m_pBoundary;
}

const CBoundaryBase& CModelBase::Boundary() const
{
  assert(m_pBoundary);
  return *m_pBoundary;
}

const std::set<unsigned int>& CModelBase::DataStorageEntryTypes()
{
  static std::set<unsigned int> stTypes;
  if(stTypes.empty())
  {
  stTypes.insert(MD_BASE_SURFACE);
  stTypes.insert(MD_BASE_POINTSET);
  stTypes.insert(MD_ROCK_MATERIAL);
  stTypes.insert(MD_BASE_INTERFACEMATERIAL);
  stTypes.insert(MD_WELLCASING_STEELMATERIAL_ENTRY);
  stTypes.insert(MD_BASE_VALUE_COMPOSITE);
  }

  return stTypes;
}

CGraphEntry* CModelBase::GraphEntry(const int nEntryType)
{
  if(DataStorageEntryTypes().find(nEntryType) != DataStorageEntryTypes().end())
  {
  return RootModel().CFemAppModel::GraphEntry(nEntryType);
  }

  return CFemAppModel::GraphEntry(nEntryType);
}

const CGraphEntry* CModelBase::GraphEntry(const int nEntryType) const
{
  return (const_cast<CModelBase*>(this))->GraphEntry(nEntryType);
}


void CModelBase::NumberModels()
{
  int nIndex = RootModel().MaxModelIndex() + 1;
  RootModel().NumberModels(nIndex);
}

void CModelBase::NumberModels(int& nIndex)
{
  assert(nIndex >= 0);

  if (Index() == -1)
  Index(nIndex);

  CNewWellPathEntry* wpEntry = dynamic_cast <CNewWellPathEntry*>(GraphEntry(MD_NEW_WELLPATH));
  if (wpEntry != 0)
  {
  CNewWellPathEntry::TNodeSet nodes = wpEntry->EntryNodes();

  for (CNewWellPathEntry::TNodeSet::iterator it = nodes.begin(); it != nodes.end(); ++it)
  {
      if ((*it)->hasZoomInModel())
      {
    // if the well zoomin hierarchy gets more complicated, we may make this method virtual instead and handle it there
    if ((*it)->ZoomInModel().Index() == -1)
          (*it)->ZoomInModel().Index(++nIndex);
    if ((*it)->ZoomInModel().CasingModel().Index() == -1)
          (*it)->ZoomInModel().CasingModel().Index(++nIndex);
      }
  }
  }

  if (!parentModel())
  {
  CZoominModelEntry* ziEntry = dynamic_cast<CZoominModelEntry *>(GraphEntry(MD_BASE_ZOOMIN_MODEL));
  if (ziEntry)
  {
      const CZoominModelEntry::TNodeSet& zoominModels = ziEntry->EntryNodes();

      for (CZoominModelEntry::TNodeSet::iterator it = zoominModels.begin(); it != zoominModels.end(); ++it)
      {
    (*it)->ChildModel().NumberModels(++nIndex);
      }
  }
  }

  CWellPathEntry* oldWpEntry = dynamic_cast <CWellPathEntry*>(GraphEntry(MD_BASE_WELLPATH));
  if (oldWpEntry != 0)
  {
  CWellPathEntry::TNodeSet nodes = oldWpEntry->EntryNodes();

  for (CWellPathEntry::TNodeSet::iterator it = nodes.begin(); it != nodes.end(); ++it)
  {
      if ((*it)->hasZoomInModel())
      {
    // if the well zoomin hierarchy gets more complicated, we may make this method virtual instead and handle it there
    if ((*it)->ZoomInModel().Index() == -1)
          (*it)->ZoomInModel().Index(++nIndex);
    if ((*it)->ZoomInModel().CasingModel().Index() == -1)
          (*it)->ZoomInModel().CasingModel().Index(++nIndex);
      }
  }
  }

}

int CModelBase::MaxModelIndex()
{
  int max = m_nIndex;
  int index;

  CNewWellPathEntry* wpEntry = dynamic_cast <CNewWellPathEntry*>(GraphEntry(MD_NEW_WELLPATH));
  if (wpEntry != 0)
  {
  CNewWellPathEntry::TNodeSet nodes = wpEntry->EntryNodes();

  for (CNewWellPathEntry::TNodeSet::iterator it = nodes.begin(); it != nodes.end(); ++it)
  {
      if ((*it)->hasZoomInModel())
      {
    index = (*it)->ZoomInModel().Index();
    if (index > max)
          max = index;
    index = (*it)->ZoomInModel().CasingModel().Index();
    if (index > max)
          max = index;
      }
  }
  }

  if (!parentModel())
  {
  CZoominModelEntry* ziEntry = dynamic_cast<CZoominModelEntry *>(GraphEntry(MD_BASE_ZOOMIN_MODEL));
  if (ziEntry)
  {
      const CZoominModelEntry::TNodeSet& zoominModels = ziEntry->EntryNodes();

      for (CZoominModelEntry::TNodeSet::iterator it = zoominModels.begin(); it != zoominModels.end(); ++it)
      {
    index = (*it)->ChildModel().MaxModelIndex();
    if (index > max)
          max = index;
      }
  }
  }

  return max;
}

void CModelBase::RemoveModelNumbering()
{
  assert(parentModel());

  m_nIndex = -1;

  CNewWellPathEntry* wpEntry = dynamic_cast <CNewWellPathEntry*>(GraphEntry(MD_NEW_WELLPATH));
  if (wpEntry != 0)
  {
  CNewWellPathEntry::TNodeSet nodes = wpEntry->EntryNodes();

  for (CNewWellPathEntry::TNodeSet::iterator it = nodes.begin(); it != nodes.end(); ++it)
  {
      if ((*it)->hasZoomInModel())
      {
    (*it)->ZoomInModel().Index(-1);
    (*it)->ZoomInModel().CasingModel().Index(-1);
      }
  }
  }

  // there are no further zoom-in models
}

bool CModelBase::LoadHeader(CStorageNode::TSTREAM& stream, CStreamVersion& /*version*/, TPROGRESS& prg)
{
  assert(!stream.IsCompressed());
  int bSuccess;
  stream >> bSuccess;
  if (!bSuccess)
  {
    _m()->error("The file could not be opened. It may be corrupt");
    //_m()->msg("The file could not be opened. It may be corrupt");
    m_bLoading = false;
    m_bInvalidateResultsAfterLoad = false;
    return false;
  }

  unsigned long ulSteps;
  stream >> ulSteps;
  prg.AddSteps(ulSteps);

  return true;
}

bool CModelBase::LoadPre(CStorageNode::TSTREAM &stream, CStorageNode::TSTREAM** ppcstream, CStreamVersion &version, TPROGRESS &prg, bool bLoadHeader)
{
  assert(!Loading());
  assert(!m_bInvalidateResultsAfterLoad);
  m_bLoading = true;

  QString projectInfo;
  stream >> projectInfo;
  ProjectInfo(projectInfo.toStdString().c_str());

  if(bLoadHeader)
  {
  if(!LoadHeader(stream, version, prg))
      return false;
  }

  if (version >= CStreamVersion(4, 1, 45))
  {
  int nIndex;
  stream >> nIndex;

  if (!bLoadHeader || !m_pParentModel)
      m_nIndex = nIndex;
  }

  // Lock mesh so it is not meshed while loading
  Mesh().Lock();

  int bTemp;

  if(version < CStreamVersion(3, 0, 87))
  {
    // read dummy value for time or load analysis type (removed)
    stream >> bTemp;
  }

  CStorageNode::TSTREAM* pcstream;
  if(version >= CStreamVersion(4, 0, 0))
  pcstream = &stream.CreateCompressedStream();
  else
  pcstream = &stream;

  CStorageNode::TSTREAM& cstream = *pcstream;
  if(ppcstream)
  *ppcstream = pcstream;

  // Load general data blocks
  if (version >= CStreamVersion(4, 1, 78))
  {
  int generalDataBlocksSize;
  cstream >> generalDataBlocksSize;

  for (int i = 0; i < generalDataBlocksSize; ++i)
  {
      QString name;
      int type;
      int size;

      cstream >> name;
      cstream >> type;
      cstream >> size;

      CGeneralDataBlock *newBlock = new CGeneralDataBlock(name, type);

      switch (type)
      {
      case 0:
      {
    std::vector<int> values;
    values.reserve(size);

    for (int j = 0; j < size; ++j)
    {
          int value;
          cstream >> value;
          values.push_back(value);
    }
    newBlock->setIntValues(values);
    break;
      }
      case 1:
      {
    std::vector<double> values;
    values.reserve(size);

    for (int j = 0; j < size; ++j)
    {
          double value;
          cstream >> value;
          values.push_back(value);
    }
    newBlock->setDblValues(values);
    break;
      }
      case 2:
      {
    std::vector<QString> values;
    values.reserve(size);

    for (int j = 0; j < size; ++j)
    {
          QString value;
          cstream >> value;
          values.push_back(value);
    }
    newBlock->setStrValues(values);
    break;
      }
      }

      m_generalDataBlocks.insert(std::make_pair(name, newBlock));
  }
  }

  if(version > CStreamVersion(4, 1, 66))
  {
    cstream >> bTemp;
    m_show_cm2_dlg = (bTemp==1);
  }

  cstream >> bTemp;
  m_bNoCSHE = (bTemp==1);

  // from version 3.0.45 solver settings are saved
  if(CStreamVersion(3, 0, 44) < version)
  {
    if(version > CStreamVersion(3, 0, 80))
    {
      if(version < CStreamVersion(3, 0, 103))
      {
    // unused
        int iSolver;
        cstream >> iSolver;
      }
    }
    else if(version > CStreamVersion(3, 0, 65)) 
    {
      int iLinstaSolver;
      int iNonlinSolver;

      cstream >> iLinstaSolver;
      cstream >> iNonlinSolver;
    }
    else
    {
      int bLinstaSolveDirect;
      int bNonlinSolveDirect;

      cstream >> bLinstaSolveDirect;
      cstream >> bNonlinSolveDirect;
    }
  }

  // from version 3.0.23 element order is saved
  if(CStreamVersion(3, 0, 22) < version)
  {
  m_nElementOrder.loadStream(cstream, version, prg);

#ifdef QUADRATIC_KEYFILE
    const CGeomecDoc *pDoc = GetGeomecDoc();
    assert(pDoc != 0);
  if(!pDoc->ExperimentalUnlocked() &&
      (ElementOrder() != CElementOrder::EO_LINEAR))
    {
      _m()->msg("Quadratic elements are selected in this model. This feature is locked in this version, the option is switched back to linear elements");
      ElementOrder(CElementOrder::EO_LINEAR);
    }
#endif

    if(ElementOrder() == CElementOrder::EO_QUADRATIC && !m_bNoCSHE)
    {
      QString msg;
      msg = getStringTableEntry(IDP_LOAD_QUADRATIC_WITH_CSHE);
      _m()->msg(msg);
      m_bNoCSHE = true;
    }
  }

  double grav;
  cstream >> grav;
  m_gravity.Value(grav,CAcceleration::SI_UNIT);

  InitialTemperature().LoadStream(cstream, version, prg);

  if(CStreamVersion(3, 0, 35) < version)
  {
    // load large deformations option
    int bLargeDeform;
    cstream >> bLargeDeform;
    m_bLargeDeform = (bLargeDeform != false);

  if (CStreamVersion(4, 1, 26) < version)
  {
      int bAllowLinearCalc;
      cstream >> bAllowLinearCalc;
      m_bAllowLinearCalc = (bAllowLinearCalc != 0);
  }

  if (CStreamVersion(4, 1, 27) < version)
  {
      int iDummy;
      cstream >> iDummy;
      m_bConvergenceChecks = (iDummy != 0);

      cstream >> iDummy;
      m_bConvergenceMinimumAngle = (iDummy != 0);

      cstream >> m_dConvergenceMinimumAngle;

      cstream >> iDummy;
      m_bConvergenceMinimumRatio = (iDummy != 0);

      cstream >> m_dConvergenceMinimumRatio;

      cstream >> iDummy;
      m_bConvergenceMinimumVolume = (iDummy != 0);

      cstream >> m_dConvergenceMinimumVolume;
  }

  CKeyFile keyFile;

#ifdef LARGEDEF_KEYFILE
    if(m_bLargeDeform && !keyFile.Unlocked())
    {
      _m()->msg(IDP_LOADLOCKED_LARGEDEF);
      m_bLargeDeform = false;
    }
#endif
  }

  // Load depletion stages
  assert(m_pDepletionStageEntry);
  m_pDepletionStageEntry->LoadStream(cstream, version, prg);

  m_pBranchState->Load(cstream, version, prg);

  // load analysis log
  if(CStreamVersion(3, 0, 51) < version)
  {
  IModelDiagnosticsHook *pDiagHook = 0;

  const IModelDiagnostics *pDiagnostics = GetDiagnosticsStrategy();
  if (pDiagnostics)
  {
      pDiagHook = pDiagnostics->Hook(gm_diagnostics::Hook_ModelBase_LoadAnalysisLog);
      if (pDiagHook)
    pDiagHook->start(this);
  }
  
    Logger().LoadStream(cstream, version, prg);

  if (pDiagHook)
      pDiagHook->end(this);
  }

  if(version >= CStreamVersion(3, 7, 4))
  {
  int nFaultInit;
  cstream >> nFaultInit;
  m_nFaultInit = TFaultInit(nFaultInit);
  }

  if(version >= CStreamVersion(3, 7, 9) &&
     (version < CStreamVersion(4, 1, 0) || !parentModel() || !parentModel()->Loading()))
  {
  ((CWellCasingSteelMaterialEntry*)GraphEntry(MD_WELLCASING_STEELMATERIAL_ENTRY))->LoadStream(*this, cstream, version, prg);
  ((CInterfaceMaterialEntry*)GraphEntry(MD_BASE_INTERFACEMATERIAL))->LoadStream(*this, cstream, version, prg);
  }

  if (version >= CStreamVersion(4, 1, 5))
  {
  int boolean;

  cstream >> boolean;
  m_EnableResultCache = (boolean != 0);
  cstream >> m_ResultCacheSize;
  }

  if (version >= CStreamVersion(4, 1, 51))
  {
  GetConsistencyGuard()->LoadStream(cstream, version, prg);
  }

  return true;
}

bool CModelBase::Load(CStorageNode::TSTREAM &stream, CStreamVersion &version, TPROGRESS &prg, bool bLoadHeader)
{
  CStorageNode::TSTREAM* pCompressedStream;

  bool bRet = (
  LoadPre( stream, &pCompressedStream, version, prg, bLoadHeader) &&
  	OnLoad(  *pCompressedStream, version, prg) &&
  LoadPost(*pCompressedStream, version, prg));
  stream.DestroyCompressedStream();

  return bRet;
}

bool CModelBase::LoadPost(CStorageNode::TSTREAM &stream, CStreamVersion &version, TPROGRESS &prg)
{
  // and unlock the mesh again
  Mesh().Unlock();

  if(m_bInvalidateResultsAfterLoad)
  {
    ResultRegister().ClearAll();
    m_bInvalidateResultsAfterLoad = false;
  }

  if(CStreamVersion(3, 0, 61) < version)
  {
    int bSaltStressInitRelax;
    stream >> bSaltStressInitRelax;
    if(bSaltStressInitRelax == 0)
      m_bSaltStressInitRelax = false;
    else
      m_bSaltStressInitRelax = true;
  }

  if(CStreamVersion(3, 0, 94) < version)
  {
  // load linear analysis settings
  stream >> m_nIniStressMaxIterations;
  stream >> m_dIniStressConvCriterion;
  int nTmp;
  stream >> nTmp;
  // always false, already done by the constructor
  //m_bLinstaOnlyInitialStress = (nTmp != 0);
  }

  if(CStreamVersion(3, 0, 116) < version && version < CStreamVersion(3, 7, 16))
  {
  int iUseCalculatedTemperatures;
  stream >> iUseCalculatedTemperatures;
  }

  if(CStreamVersion(3, 0, 119) < version)
  {
  int iUseParallelSolver;
  stream >> iUseParallelSolver;
  m_bUseParallelSolver = (iUseParallelSolver != 0);
  stream >> m_nNumSolverThreads;
  }
  if (CStreamVersion(4, 1, 67) < version)
  {
  int iUseStressSmoothening;
  stream >> iUseStressSmoothening;
  m_bUseStressSmootheningOnDSF = (iUseStressSmoothening != 0);
  }
  if (CStreamVersion(4, 1, 71) < version)
  {
  stream >> m_nDSFInterSurfType;
  }
  if(CStreamVersion(3, 0, 120) < version)
  {
  m_pGVTSettings->LoadStream(stream, version, prg);
  }

  if(CStreamVersion(3, 0, 123) < version)
  {
  int iUseDecompactionParams;
  stream >> iUseDecompactionParams;
  m_bUseDecompactionParams = (iUseDecompactionParams != 0);
  }

  if((version < CStreamVersion(3, 8, 0) && version >= CStreamVersion(3, 7, 25)) || version >= CStreamVersion(3, 8, 4))
  {
  int iWriteOutputStreamFile;
  stream >> iWriteOutputStreamFile;
  m_bWriteOutputStreamFile = (iWriteOutputStreamFile != 0);
  }

  if(CStreamVersion(3, 6, 0) < version)
  {
  m_pQBSettings->LoadStream(stream, version, prg);
  }

  if(version >= CStreamVersion(3, 7, 5))
  {
  int iAutomaticallyDisableDrawing;
  stream >> iAutomaticallyDisableDrawing;
  m_bAutomaticallyDisableDrawing = (iAutomaticallyDisableDrawing != 0);
  }

  if(version >= CStreamVersion(3, 7, 6))
  {
  int iSavedSmoothedResults;
  stream >> iSavedSmoothedResults;
  if(iSavedSmoothedResults != 0)
      _m()->msg(IDP_ONLYSMOOTHEDRESULTS, MB_OK|MB_ICONINFORMATION);
  }

  if(version >= CStreamVersion(3, 7, 22))
  {
  int iExportCommandFilesWithDefaults;
  stream >> iExportCommandFilesWithDefaults;
  m_bExportCommandFilesWithDefaults = (iExportCommandFilesWithDefaults != 0);
  }

  if (m_pModelDiagnostics) // only defined in main model
  {
  if (m_pModelDiagnostics->Setting(gm_diagnostics::Setting_LoadMainModelOnly))
  {
      return true;
  }
  }

  if(version >= CStreamVersion(4, 1, 0))
  {
  prg.StatusMessage("Reading zoom-in properties");

  LoadZoominProperties(stream, version, prg);
  CZoominModelEntry* pZIEntry = static_cast<CZoominModelEntry*>(GraphEntry(MD_BASE_ZOOMIN_MODEL));
  if(pZIEntry)
  {
    pZIEntry->LoadStream(*this, stream, version, prg);
    //pZIEntry->Destroy();
  }
  }

  if (version >= CStreamVersion(4, 1, 4))
  {
  CExportResultDataEntry* exportResultDataEntry =
      static_cast <CExportResultDataEntry*> (GraphEntry(MD_BASE_EXPORT_MACROS));

  if (exportResultDataEntry)
  {
      exportResultDataEntry->LoadStream(*this, stream, version, prg);
  }
  }

  if (version >= CStreamVersion(4, 1, 57))
  {
  GVTResultTree().LoadStream(stream, version, prg);
  }

  EnablePointSets();

  m_bLoading = false;
  
  return true;
}

int CModelBase::LoadSceneMarker(CStorageNode::TSTREAM& stream,
  CStreamVersion& version)
{
  int marker = 0;

  if (version >= CStreamVersion(3, 8, 1))
  {
  stream >> marker;
  }

  return marker;
}

// The result server
int CModelBase::ResultRegisterSize() const
{
  return 1;
}

QString CModelBase::ResultRegisterName(int /*nRegister*/) const
{
  return "Results";
}

CResultRegister& CModelBase::ResultRegister(int nRegister)
{
  assert(nRegister == 0);
  return Mesh().ResultRegister();
}
  
const CResultRegister& CModelBase::ResultRegister(int nRegister) const
{
  assert(nRegister == 0);
  return Mesh().ResultRegister();
}

CMaterialResultTree& CModelBase::MaterialResultTree()
{
  return *m_pMatResultTree;
}

const CMaterialResultTree& CModelBase::MaterialResultTree() const
{
  return *m_pMatResultTree;
}

CResultTree& CModelBase::ResultTree()
{
  assert(m_pResultTree);
  return *m_pResultTree;
}

const CResultTree& CModelBase::ResultTree() const
{
  assert(m_pResultTree);
  return *m_pResultTree;
}

CMeshResultTree& CModelBase::MeshResultTree()
{
  assert(m_pMeshResultTree);
  return *m_pMeshResultTree;
}

const CMeshResultTree& CModelBase::MeshResultTree() const
{
  assert(m_pMeshResultTree);
  return *m_pMeshResultTree;
}

CGVTResultTree& CModelBase::GVTResultTree()
{
  assert(m_pGVTResultTree);
  return *m_pGVTResultTree;
}

const CGVTResultTree& CModelBase::GVTResultTree() const
{
  assert(m_pGVTResultTree);
  return *m_pGVTResultTree;
}

long CModelBase::SavedItems() const
{
  long lRet = 0;

  lRet += Logger().SavedItems();
  if(!parentModel())
  {
  lRet += ((CWellCasingSteelMaterialEntry*)GraphEntry(MD_WELLCASING_STEELMATERIAL_ENTRY))->SavedItems();
  lRet += ((CInterfaceMaterialEntry*)GraphEntry(MD_BASE_INTERFACEMATERIAL))->SavedItems();
  }
  return lRet;
}

bool CModelBase::LoadChildModel(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS& prog, CModelBase& childModel)
{
  assert(childModel.parentModel() == this);
  bool retval = false;

  GetConsistencyGuard()->ChildIsLoading(true);

  int index = childModel.Index();

  if (stream.IsHDF5())
  stream >> index;

  stream.PushDataSet(CHDF5File::ModelGroup(index) + "ModelStream");

  if (m_pChildModelLoadSave)
  retval = m_pChildModelLoadSave->LoadModel(stream, version, prog, childModel);
  else
  retval = childModel.Load(stream, version, prog, false);

  stream.PopDataSet();

  GetConsistencyGuard()->ChildIsLoading(false);

  return retval;
}

bool CModelBase::SaveChildModel(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS& prog, CModelBase& childModel)
{
  assert(childModel.parentModel() == this);

  if (stream.IsHDF5())
  stream << childModel.Index();

  stream.PushDataSet(CHDF5File::ModelGroup(childModel.Index()) + "ModelStream");

  bool retval = false;

  if(m_pChildModelLoadSave)
  retval = m_pChildModelLoadSave->SaveModel(stream, prog, childModel);
  else
  retval = childModel.Save(stream, prog, false);

  stream.PopDataSet();

  return retval;
}

long CModelBase::SavedItemsChildModel(CModelBase& childModel) const
{
  assert(childModel.parentModel() == this);
  if(m_pChildModelLoadSave)
  return m_pChildModelLoadSave->SavedItems(childModel);

  return childModel.SavedItems();
}

void CModelBase::SaveHeader(CStorageNode::TSTREAM& stream, TPROGRESS& prg, int& nMaxNrOfSteps, quint64& dwStepPos)
{
  assert(!stream.IsCompressed());
  int bSuccess = false;
  unsigned long steps=10000; // We now stream a dummy value and keep the filepointer. In the end we write the real nr of steps
  dwStepPos = stream.GetPosition();
  stream << bSuccess;
  stream << steps;

  nMaxNrOfSteps = SavedItems();
  prg.AddSteps(nMaxNrOfSteps);
}

void CModelBase::FinalizeSave(CStorageNode::TSTREAM& stream, TPROGRESS& /*prg*/, int nMaxNrOfSteps, quint64 dwStepPos)
{
  assert(!stream.IsCompressed());
  stream.Flush();
  quint64 dwPos = stream.GetPosition();
  stream.SetPosition(dwStepPos);
  int bSuccess = true;
  stream << bSuccess;
  stream << nMaxNrOfSteps;
  stream.Flush();
  stream.SetPosition(dwPos);
}

bool CModelBase::Save(CStorageNode::TSTREAM &stream, TPROGRESS &prg, bool bSaveHeader)
{
  bool ret=true;
  stream << ProjectInfo();
  stream.Flush();

  quint64 dwStepPos = 0;
  int nMaxNrOfSteps = 0;

  if(bSaveHeader)
  SaveHeader(stream, prg, nMaxNrOfSteps, dwStepPos);

  assert(m_nIndex >= 0); // someone should have called NumberModels

  stream << m_nIndex; // save it here, as we will want it outside the compression in HDF5 [ that is no longer relevant, as HDF5 uses internal compression, and we're oblivious to it ]

  stream.Flush();

  CStorageNode::TSTREAM& cstream = stream.CreateCompressedStream();

  // Save general data blocks
  cstream << int(m_generalDataBlocks.size());
  for (std::map<QString, CGeneralDataBlock *>::iterator it = m_generalDataBlocks.begin(); it != m_generalDataBlocks.end(); ++it)
  {
  CGeneralDataBlock *gdb = it->second;

  cstream << gdb->Name();
  cstream << gdb->Type();
  cstream << gdb->Size();

  switch (gdb->Type())
  {
  case 0:
  {
      std::vector<int> values;
      gdb->getIntValues(values);
      for (int j = 0; j < gdb->Size(); ++j)
    cstream << values[j];
      break;
  }
  case 1:
  {
      std::vector<double> values;
      gdb->getDblValues(values);
      for (int j = 0; j < gdb->Size(); ++j)
    cstream << values[j];
      break;
  }
  case 2:
  {
      std::vector<QString> values;
      gdb->getStrValues(values);
      for (int j = 0; j < gdb->Size(); ++j)
    cstream << values[j];
      break;
  }
  }
  }

  cstream << int( m_show_cm2_dlg ? 1 : 0 );

  int iDummy = m_bNoCSHE ? 1 : 0;
  cstream << iDummy;

  m_nElementOrder.saveStream(cstream, prg);

  cstream.Flush();
  cstream << m_gravity.Value();

  InitialTemperature().SaveStream(cstream, prg);

  iDummy = m_bLargeDeform ? 1 : 0;
  cstream << iDummy;

  iDummy = m_bAllowLinearCalc ? 1 : 0;
  cstream << iDummy;

  cstream << int(m_bConvergenceChecks ? 1 : 0);
  cstream << int(m_bConvergenceMinimumAngle ? 1 : 0);
  cstream << m_dConvergenceMinimumAngle;
  cstream << int(m_bConvergenceMinimumRatio ? 1 : 0);
  cstream << m_dConvergenceMinimumRatio;
  cstream << int(m_bConvergenceMinimumVolume ? 1 : 0);
  cstream << m_dConvergenceMinimumVolume;

  assert(m_pDepletionStageEntry);
  m_pDepletionStageEntry->SaveStream(cstream, prg);

  m_pBranchState->Save(cstream, prg);

  cstream.Flush();

  // save analysis log
  Logger().SaveStream(cstream, prg);

  cstream << int(m_nFaultInit);

  if(!parentModel())
  {
  ((CWellCasingSteelMaterialEntry*)GraphEntry(MD_WELLCASING_STEELMATERIAL_ENTRY))->SaveStream(cstream, prg);
  ((CInterfaceMaterialEntry*)GraphEntry(MD_BASE_INTERFACEMATERIAL))->SaveStream(cstream, prg);
  }

  {
  int enableResultCache = (m_EnableResultCache ? 1 : 0);

  cstream << enableResultCache;
  cstream << m_ResultCacheSize;
  }

  // we save in each model, as we play around with the singleton-ness depending on parent; need to check what happens on import of models
  GetConsistencyGuard()->SaveStream(cstream, prg);

  if (ret)
    ret = OnSave(cstream,prg);

  if(m_bSaltStressInitRelax)
    cstream << 1;
  else
    cstream << 0;

  // save linear analysis settings (version 3.0.95)
  cstream << m_nIniStressMaxIterations;
  cstream << m_dIniStressConvCriterion;
  // this whole option can be removed, at some opportune time
  cstream << (int)(m_bLinstaOnlyInitialStress ? 1 : 0);

  cstream << (int)(m_bUseParallelSolver ? 1 : 0);
  cstream << m_nNumSolverThreads;

  cstream << (int)(m_bUseStressSmootheningOnDSF ? 1 : 0);
  cstream << m_nDSFInterSurfType;

  // save GVT settings (version 3.0.121)
  m_pGVTSettings->SaveStream(cstream, prg);

  cstream << (int)(m_bUseDecompactionParams ? 1 : 0);
  cstream << (int)(m_bWriteOutputStreamFile ? 1 : 0);

  m_pQBSettings->SaveStream(cstream, prg);

  cstream << (int)(m_bAutomaticallyDisableDrawing ? 1 : 0);

  if(getSavingSmoothedResults())
  cstream << int(1);
  else
  cstream << int(0);

  if(m_bExportCommandFilesWithDefaults)
  cstream << int(1);
  else
  cstream << int(0);

  cstream << int(m_ParentResultsDef);
  cstream << (m_bSourceDepletionStagesChanged ? 1 : 0);
  cstream << (m_bSourceGlobalPressureChanged ? 1 : 0);
  cstream << (m_bSourceGlobalTemperatureChanged ? 1 : 0);
  cstream << (m_bSourceGravityChanged ? 1 : 0);

  CZoominModelEntry* pZIEntry = static_cast<CZoominModelEntry*>(GraphEntry(MD_BASE_ZOOMIN_MODEL));
  if(pZIEntry)
  pZIEntry->SaveStream(cstream, prg);

  CExportResultDataEntry* exportResultDataEntry =
  static_cast <CExportResultDataEntry*> (GraphEntry(MD_BASE_EXPORT_MACROS));

  if (exportResultDataEntry)
  {
  exportResultDataEntry->SaveStream(cstream, prg);
  }

  GVTResultTree().SaveStream(cstream, prg);

  stream.DestroyCompressedStream();

  if (ret && bSaveHeader)
  {
    FinalizeSave(stream, prg, nMaxNrOfSteps, dwStepPos);
  }

  return ret;
}

void CModelBase::SaveFileHeader(CStorageNode::TSTREAM &stream,
  const QString& currentAppVersion)
{
  // Save the magic word and the version tags

  QString Magic(MAGIC);

  stream << Magic << VERSION_MAJOR << VERSION_MINOR << VERSION_REVISION;

  // Which model?

  CHexaModel *pHexa       = dynamic_cast <CHexaModel*>    (this);
  CGoCadModel *pGoCad     = dynamic_cast <CGoCadModel*>   (this);
  CEclipseModel *pEclipse = dynamic_cast <CEclipseModel*> (this);

  if (pHexa)
  {
  assert(pGoCad   == 0);
  assert(pEclipse == 0);
  stream << HEXA_MODEL;
  }
  else if (pGoCad)
  {
  assert(pHexa    == 0);
  assert(pEclipse == 0);
  stream << GOCAD_MODEL;
  }
  else if (pEclipse)
  {
  assert(pHexa  == 0);
  assert(pGoCad == 0);
  stream << ECLIPSE_MODEL;
  }
  else
  {
  assert(FALSE);
  }

  // save application version number as a string

  stream << currentAppVersion;
}

void CModelBase::SaveSceneMarker(CStorageNode::TSTREAM& stream, bool marker)
{
  if (marker)
  {
  stream << 2;
  }
  else
  {
  stream << 0;
  }
}

template <class ENTRY>
void CModelBase::NumberNodes(unsigned int nEntryId, int& nStartIdx, std::set<CStorageNode*>& stDone)
{
  ENTRY* pEntry = (ENTRY*)GraphEntry(nEntryId);
  assert(pEntry);
  typename ENTRY::TNodeSet stNode = pEntry->EntryNodes();

  for(typename ENTRY::TNodeSet::iterator it = stNode.begin(); it != stNode.end(); ++it)
  {
    if((dynamic_cast<IResult*>(*it) == 0) && (dynamic_cast<CMeshResult*>(*it) == 0))
  {
      if(stDone.find(*it) == stDone.end())
      {
    (*it)->Index(nStartIdx++);
    stDone.insert(*it);
      }
  }
  }

  for(int i = 0; i < childModelSize(); ++i)
  childModel(i).NumberNodes<ENTRY>(nEntryId, nStartIdx, stDone);
}

void CModelBase::NumberOpenGLNodes()
{
  if(!parentModel())
  {
  int nStartIdx = 1;
  std::set<CStorageNode*> stDone;
  NumberNodes<TOpenGLNodeBaseEntry>(MD_BASE_OPENGL_NODE, nStartIdx, stDone);
  }
/*
  TOpenGLNodeBaseEntry::TNodeSet stNode = pEntry->EntryNodes();

  int nIndex = 1;
  for(TOpenGLNodeBaseEntry::TNodeSet::iterator it = stNode.begin(); it != stNode.end(); ++it)
  {
    (*it)->Index(nIndex);
    ++nIndex;
  }
*/
}

void CModelBase::NumberValueComposites()
{
  // We number all value composities from COMPOSITE_BASE. All IResult's and CDerivedResult which
  // are stored on file are ignored, because they are defined in resulttree.h.
  if(!parentModel())
  {
  int nStartIdx = COMPOSITE_BASE;
  std::set<CStorageNode*> stDone;
  NumberNodes<TValueCompositeEntry>(MD_BASE_VALUE_COMPOSITE, nStartIdx, stDone);
  }
/*
  TValueCompositeEntry::TNodeSet stComposite = pEntry->EntryNodes();

  int nIndex = COMPOSITE_BASE;

  for(TValueCompositeEntry::TNodeSet::iterator it = stComposite.begin(); it != stComposite.end(); ++it)
  {
    if((dynamic_cast<IResult*>(*it) == 0) && (dynamic_cast<CMeshResult*>(*it) == 0))
    {
      (*it)->Index(nIndex);
      ++nIndex;
    }
  }
*/
}

long CModelBase::SavedItemsPointSets() const
{
  long lRet = 0;

  if(!parentModel())
  {
    // Save pointsets
    assert(GraphEntry(MD_BASE_POINTSET));
    TPointSetEntry::TNodeSet stNode = ((TPointSetEntry*)GraphEntry(MD_BASE_POINTSET))->EntryNodes();
    for (TPointSetEntry::TNodeSet::iterator it = stNode.begin(); it != stNode.end(); it++)
    {
      if((CMeshBase*)&Mesh() != (*it))
      {
        lRet += (*it)->SavedItems();
      }
    }
  }

  return lRet;
}

void CModelBase::SavePointSets(CStorageNode::TSTREAM &stream, TPROGRESS& progress) const
{
  if(!parentModel())
  {
    // Save pointsets
    assert(GraphEntry(MD_BASE_POINTSET));
    TPointSetEntry::TNodeSet stNode = ((TPointSetEntry*)GraphEntry(MD_BASE_POINTSET))->EntryNodes();

  int nMeshes = 0;
  for(TPointSetEntry::TNodeSet::iterator it = stNode.begin(); it != stNode.end(); ++it)
  {
      if(dynamic_cast<const CMeshBase*>(*it) != 0)
    ++nMeshes;
  }

    int iNrOfPointsets = stNode.size();
    stream << iNrOfPointsets - nMeshes;

    for (TPointSetEntry::TNodeSet::iterator it = stNode.begin(); it != stNode.end(); it++)
    {
      if(!dynamic_cast<CMeshBase*>(*it))
      {
    const CGocadElementSet* gocadElementSet =
          dynamic_cast <const CGocadElementSet*> (*it);
    int isGocadElementSet = (gocadElementSet != 0);
    stream << isGocadElementSet;
    if (!isGocadElementSet)
    {
          // Is it an generic volume
          const CElementSet *pVolume = dynamic_cast<const CElementSet*>(*it);
          int bVolume = pVolume != 0;
          stream << bVolume;
          if(!bVolume)
          {
      int bLasPointSet = (dynamic_cast<const CLasPointSet*>(*it) != 0);
      stream << bLasPointSet;
      if (bLasPointSet == 0)
      {
              int isNewWellPathInput =
        (dynamic_cast <const CNewWellPathInput*> (*it) != 0 ? 1 : 0);

              stream << isNewWellPathInput;
      }
          }
    }
        (*it)->SaveStream(stream,progress);
      }
    }
  }
}

void CModelBase::EnablePointSets()
{
  if(!parentModel() || !parentModel()->Loading())
  {
    assert(GraphEntry(MD_BASE_POINTSET));
    TPointSetEntry::TNodeSet stNode = ((TPointSetEntry*)GraphEntry(MD_BASE_POINTSET))->EntryNodes();
    for (TPointSetEntry::TNodeSet::iterator it = stNode.begin(); it != stNode.end(); it++)
    {
      IPointSet& point_set = **it;

      for(int i = 0; i < point_set.RpnValueSetSize(); i++)
        point_set.RpnValueSet(i).EnableProxy(this);
    }
  }
}

// Load functions
void CModelBase::LoadPointSets(CStorageNode::TSTREAM &stream, CStreamVersion &version, TPROGRESS& progress)
{
  if(version < CStreamVersion(4, 1, 0) || !parentModel() || !parentModel()->Loading())
  {
    int iNrOfPointsets;
    stream >> iNrOfPointsets;
    for (int c = 0; c< iNrOfPointsets; c++)
    {	
      IPointSet* pPointSet = 0;
      if(version < CStreamVersion(3, 0, 11))
        pPointSet = new CPointSet(*this);
      else
      {
    int isGocadElementSet = 0;
    if (version >= CStreamVersion(4, 1, 1))
    {
          stream >> isGocadElementSet;
    }
    if (isGocadElementSet)
    {
          pPointSet = new CGocadElementSet(*this);
    }
    else
    {
          int bVolume;
          stream >> bVolume;
          if(bVolume)
      pPointSet = new CElementSet(*this);
          else
          {
      int bLasPointSet = false;
      if(CStreamVersion(3, 0, 108) < version) // new in 3.0.109: allow LAS pointset
              stream >> bLasPointSet;
      if(bLasPointSet)
              pPointSet = new CLasPointSet(*this);
      else
      {
              int isNewWellPathInput = 0;

              if (CStreamVersion(4, 1, 40) < version)
              {
        stream >> isNewWellPathInput;
              }

              if (isNewWellPathInput != 0)
              {
        pPointSet = new CNewWellPathInput(*this);
              }
              else
              {
        pPointSet = new CPointSet(*this);
              }
      }
          }
    }
      }
      pPointSet->LoadStream(stream,version,progress);
    }
  }
}

long CModelBase::SavedItemsSurfaces() const
{
  long lRet = 0;
  if(!parentModel())
  {
    const CSurfaceEntry& surface_entry = static_cast<const CSurfaceEntry&>(*GraphEntry(MD_BASE_SURFACE));
    lRet += surface_entry.SavedItems();
  }

  return lRet;
}

void CModelBase::SaveSurfaces(CStorageNode::TSTREAM& stream, CStorageNode::TPROGRESS& progress)
{
  if(!parentModel())
  {
  CSurfaceEntry& surface_entry = static_cast<CSurfaceEntry&>(*GraphEntry(MD_BASE_SURFACE));
  surface_entry.SaveStream(stream, progress);
  }
}

void CModelBase::LoadSurfaces(CStorageNode::TSTREAM& stream, CStreamVersion& version, CStorageNode::TPROGRESS& progress)
{
  if(parentModel())
  {
  // link surfaces in root model's surface entry to opengl entry
  CSurfaceEntry& surface_entry = static_cast<CSurfaceEntry&>(*RootModel().GraphEntry(MD_BASE_SURFACE));
  TOpenGLNodeBaseEntry& opengl_entry = static_cast<TOpenGLNodeBaseEntry&>(*GraphEntry(MD_BASE_OPENGL_NODE));

  CSurfaceEntry::TNodeSet stNodes = surface_entry.EntryNodes();
  for(CSurfaceEntry::TNodeSet::iterator it = stNodes.begin(); it != stNodes.end(); ++it)
  {
      if(!opengl_entry.IsLinkedTo(**it))
    opengl_entry.LinkTo(**it);
  }
  }

  if(version < CStreamVersion(4, 1, 0) || !parentModel() || !parentModel()->Loading())
  {
    CSurfaceEntry& surface_entry = static_cast<CSurfaceEntry&>(*GraphEntry(MD_BASE_SURFACE));
    surface_entry.LoadStream(*this, stream, version, progress);
  }
}

long CModelBase::SavedItemsRockMaterials() const
{
  long lRet = 0;
  if(!parentModel())
  {
    const CMaterialEntry& rock_material_entry = static_cast<const CMaterialEntry&>(*GraphEntry(MD_ROCK_MATERIAL));
    lRet += rock_material_entry.SavedItems();
  }

  return lRet;
}

void CModelBase::SaveRockMaterials(CStorageNode::TSTREAM& stream, CStorageNode::TPROGRESS& progress)
{
  if(!parentModel())
  {
    CMaterialEntry& rock_material_entry = static_cast<CMaterialEntry&>(*GraphEntry(MD_ROCK_MATERIAL));
    rock_material_entry.SaveStream(stream, progress);
  }
}

void CModelBase::LoadRockMaterials(CStorageNode::TSTREAM& stream, CStreamVersion& version, CStorageNode::TPROGRESS& progress)
{
  if(version < CStreamVersion(4, 1, 0) || !parentModel() || !parentModel()->Loading())
  {
    CMaterialEntry& rock_material_entry = static_cast<CMaterialEntry&>(*GraphEntry(MD_ROCK_MATERIAL));
    rock_material_entry.LoadStream(*this, stream, version, progress);
  }
}

/*
bool CModelBase::TimeAnalysis() const
{
  return m_bTimeAnalysis;
}

void CModelBase::TimeAnalysis(bool bTimeAnalysis)
{
  if(m_bTimeAnalysis != bTimeAnalysis)
    ResultRegister().ClearNonLinear();
  m_bTimeAnalysis = bTimeAnalysis;
}
*/

void CModelBase::CreatePointSet() const // wjrx mantis 2896
{
}

CStreamVersion CModelBase::currentVersion() const
{
  return m_versionManager.CurrentVersion();
}

CStreamVersion CModelBase::documentVersion() const
{
  return m_versionManager.FileVersion();
}

bool CModelBase::HeatFlowCalculationCriteriaMet() const
{
  if(BranchState().IsBranch() && BranchState().BranchStage().Last())
  return false;

  CModelBase *pModel = const_cast<CModelBase*>(this);
  TFormationBaseEntry* pEntry = (TFormationBaseEntry*)(pModel->GraphEntry(MD_BASE_FORMATION));
  TFormationBaseEntry::TNodeSet stFormation = pEntry->EntryNodes();

  if(stFormation.empty())
    return false;

  for(TFormationBaseEntry::TNodeSet::iterator it = stFormation.begin(); it != stFormation.end(); it++)
  {
    if(!(*it)->ActiveMaterial().Valid())
      return false;
  }

  THorizonBaseEntry::TEntryNodeSet stHorizon = pModel->GraphEntry(MD_BASE_HORIZON)->GraphEntryNodes();
  for(THorizonBaseEntry::TEntryNodeSet::iterator it = stHorizon.begin(); it != stHorizon.end(); ++it)
  {
    CHorizonBase& hor = static_cast<CHorizonBase&>(**it);
  if (hor.Slip() && !hor.IgnoreFault())
  {
      if (!hor.InterfaceElementGroup())
    return false;
  }
  }

  return true;
}

bool CModelBase::CanCalculate() const
{
  if(!HeatFlowCalculationCriteriaMet())
  return false;

  CModelBase *pModel = const_cast<CModelBase*>(this);
  TFormationBaseEntry* pEntry = (TFormationBaseEntry*)(pModel->GraphEntry(MD_BASE_FORMATION));
  TFormationBaseEntry::TNodeSet stFormation = pEntry->EntryNodes();

  // check for depleting formations
  bool depleting = false;

  for(TFormationBaseEntry::TNodeSet::iterator it = stFormation.begin(); it != stFormation.end(); it++)
  {
    if ((*it)->Depleting())
      depleting = true;
  }

  return depleting;
}

bool CModelBase::CanCalculateHeatFlow() const
{
  return HeatFlowCalculationCriteriaMet();
}

bool CModelBase::CanCalculateMixture() const
{
  return CanCalculate();
}

bool CModelBase::WriteFilosModel(const QString& getPathName,
                                 IDianaXWrapper* dianaXWrapper,
                                 const std::string &title,
                                 const CAnalysisType& antype,
                                 bool bWriteInputFiles,
                                 bool bWriteQuadDat,
                                 ISaveModel& saveModel,
                                 IRetrieveDianaFileNames& retrieveDianaFileNames,
                        dia::IAnalysisStatusContainer *pAnalysisStatusContainer)
{
  QString strTempPath = GetGeomecTempPathExt(CTempPath::TEMP_CALCULATION);

  CDianaRunController drc(	*this, 
                antype, 
                bWriteInputFiles, 
                bWriteQuadDat, 
                m_bWriteOutputStreamFile, 
                strTempPath);

  dia::IDianaRunner::RunParams p = { getPathName, title, strTempPath.toStdString(), dianaXWrapper, &saveModel, &retrieveDianaFileNames, nullptr, false, _g->dsa() };
  drc.params(p);

  bool res=false;
  if (p.dsa)
  {
    CRunAnalysis_CLI ra(&drc, CDianaRunController::All);
    ra.wait();
    res = ra.res();
  }
  else
  {
    res = drc.run(CDianaRunController::All);
  }
  return res;

}

bool CModelBase::CheckForAnalysis(const CAnalysisType& antype,
                                  bool bWriteInputFiles,
                                  bool bWriteQuadDat)
{
  // check whether the model has branches or phases
  // if so, warn user when analysis type is not non-linear
  if(antype.AnalysisType() != CAnalysisType::AT_NONLIN &&
     (HasBranches() || HasPhases())                    &&
     !bWriteInputFiles                                 &&
     !bWriteQuadDat)
  {
  // reversed the order due to the arguments in the
  // 'IDS_CHECK_FOR_ANALYSIS_WARNING' string

  QString strMsg = QString("%2%1").
      arg(getStringTableEntry(IDS_CHECK_FOR_ANALYSIS_QUESTION)).
      arg(getStringTableEntry(IDS_CHECK_FOR_ANALYSIS_WARNING));

  strMsg = strMsg.arg(antype.Label()).arg(InitialDepletionStage().Name());
  if(_m()->msg(strMsg,
      MB_ICONEXCLAMATION | MB_YESNO) == IDNO)
      return false;
  }

  // first check if there is sufficient memory to read in all results after calculation
  if(!ResultRegister().SufficientFreeMemory() && !bWriteInputFiles && !bWriteQuadDat)
  {
    if(_m()->msg(IDP_INSUFFICIENT_MEMORY_FOR_CALCULATION, MB_YESNO) == IDNO)
      return false;
  }

  // make sure there are no formations with pressure changes that use the undrained material model
  TFormationBaseEntry* pEntry = (TFormationBaseEntry*)GraphEntry(MD_BASE_FORMATION);
  assert(pEntry);
  TFormationBaseEntry::TNodeSet stNode = pEntry->EntryNodes();
  std::vector<const CFormationBase *> vcWrongFormations;

  for(TFormationBaseEntry::TNodeSet::iterator it = stNode.begin(); it != stNode.end(); it++)
  {
    const CFormationBase &formation = **it;
  if(formation.PressureChange() && formation.ActiveMaterial().MaterialModel() == MM_UNDRAINED)
      vcWrongFormations.push_back(&formation);
  }

  if(vcWrongFormations.size() > 0)
  {
    QString msg;
    msg = getStringTableEntry(IDP_DEPLETINGWITHUNDRAINEDMAT);
    msg += "\n";
    for(size_t i = 0; i < vcWrongFormations.size() && i < 10; ++i)
    {
      msg += "\n";
      msg += vcWrongFormations[i]->Name();
    }

  if(vcWrongFormations.size() > 10)
      msg += "\n<more>";

    _m()->error(msg);
    return false;
  }

  if(!Mesh().IsMesh())
  {
    assert(Mesh().CanCreateMesh());
    Mesh().CreateMesh();
  }

  QString strTempPath = GetGeomecTempPathExt(CTempPath::TEMP_CALCULATION);
  if(strTempPath.isEmpty())
  {
    _m()->error(IDP_INVALIDTEMPPATH);
    getDocumentProxy()->onAnalysisProperties();
  }

  if(!OnModelSpecificAnalysisCheck(antype, bWriteInputFiles, bWriteQuadDat))
  return false;

  return true;
}

void CModelBase::NoCSHE(bool bNoCSHE)
{
  if(bNoCSHE != NoCSHE())
  {
    getDocumentProxy()->setModifiedFlag();
    ResultRegister().ClearAll();

    m_bNoCSHE = bNoCSHE;
  }
}

bool CModelBase::LargeDeformations() const
{
  return m_bLargeDeform;
}

void CModelBase::LargeDeformations(bool bLargeDeform, bool bUpdateResults)
{
  if(LargeDeformations() != bLargeDeform)
  {
  if (bUpdateResults)
  {
      getDocumentProxy()->setModifiedFlag();
      ResultRegister().ClearNonLinear();
  }
    m_bLargeDeform = bLargeDeform;
  }
}

bool CModelBase::AllowLinearCalc() const
{
  return m_bAllowLinearCalc;
}

void CModelBase::AllowLinearCalc(bool bAllowLinearCacl)
{
  if(AllowLinearCalc() != bAllowLinearCacl)
  {
    getDocumentProxy()->setModifiedFlag();
    ResultRegister().ClearNonLinear();

    m_bAllowLinearCalc = bAllowLinearCacl;
  }
}

bool CModelBase::UseParallelSolver() const
{
  return m_bUseParallelSolver;
}

void CModelBase::UseParallelSolver(bool bOn)
{
  if(m_bUseParallelSolver != bOn)
  {
  m_bUseParallelSolver = bOn;
  getDocumentProxy()->setModifiedFlag();
  }
}

int CModelBase::NumSolverThreads() const
{
  return m_nNumSolverThreads;
}

void CModelBase::NumSolverThreads(int nThreads)
{
  assert(nThreads > 0 && nThreads <= MaxNumSolverThreads());
  if(nThreads != m_nNumSolverThreads)
  {
  m_nNumSolverThreads = nThreads;
  getDocumentProxy()->setModifiedFlag();
  }
}

int CModelBase::MaxNumSolverThreads() const
{
  return MAX_NUM_SOLVER_THREADS;
}

bool CModelBase::UseStressSmootheningOnDSF() const
{
  return m_bUseStressSmootheningOnDSF;
}

void CModelBase::UseStressSmootheningOnDSF(bool bOn)
{
  if (m_bUseStressSmootheningOnDSF != bOn)
  {
  m_bUseStressSmootheningOnDSF = bOn;
  ResultRegister().ClearAll();
  getDocumentProxy()->setModifiedFlag();
  }
}

int CModelBase::DSFIntermediateSurfaceType() const
{
  return m_nDSFInterSurfType;
}

void CModelBase::DSFIntermediateSurfaceType(int type)
{
  if (m_nDSFInterSurfType != type)
  {
  m_nDSFInterSurfType = type;
  if (HasDoubleSidedFaults())
      InvalidateMesh();
  getDocumentProxy()->setModifiedFlag();
  }
}

CElementOrder::TElementOrder CModelBase::ElementOrder() const
{
  return m_nElementOrder.elementOrder();
}

void CModelBase::ElementOrder(CElementOrder::TElementOrder nElementOrder)
{
  if(nElementOrder != ElementOrder())
  {
    getDocumentProxy()->setModifiedFlag();
    ResultRegister().ClearAll();

  m_nElementOrder.elementOrder(nElementOrder);
  }
}

void CModelBase::FaultInit(TFaultInit nFaultInit)
{
  if(nFaultInit != m_nFaultInit)
  {
  getDocumentProxy()->setModifiedFlag();

  // only invalidate results when model contains faults
  if(HasFaults())
  {
      // no need to clear heat flow results...
      ResultRegister().ClearLinear();
      ResultRegister().ClearNonLinear();
      ResultRegister().ClearMixture();
  }

  m_nFaultInit = nFaultInit;
  }
}

void CModelBase::InvalidateResultsAfterLoad()
{
  m_bInvalidateResultsAfterLoad = true;
}

/*
static int value_set_index(const IValueSet& value_set)
{
  for(int i = 0; i < value_set.PointSet().ValueSetSize(); i++)
    if(&value_set.PointSet().ValueSet(i) == &value_set) return i;
  assert(false);
  return -1;
}
*/

/*
static std::vector<int> value_set_vec(const CValueType& value_type)
{
  std::vector<int> vcRet(value_type.ComponentSize());
  for(int nComponent = 0; nComponent < value_type.ComponentSize(); nComponent++)
  {
    const CValueComponent& component = dynamic_cast<const CValueComponent&>(value_type.Component(nComponent));
    if(component.Defined())
      vcRet[nComponent] = value_set_index(component.ValueSet());
    else
      vcRet[nComponent] = -1;
  }

  return vcRet;
}
*/

void CModelBase::CopyDepletionStages(const CDepletionStageEntry& source,
                   CDepletionStageEntry& target, 
                   TPROGRESS& progress)
{
  CNewArchiveStdStringStream file(std::fstream::in | std::fstream::out | std::fstream::binary);
  source.SaveStream(file, progress);
  file.SetPosition(0);
  CStreamVersion version = currentVersion();
  target.LoadStream(file, version, progress);
}

void CModelBase::CopyMaterial(const CModelBase& source, TPROGRESS& /*progress*/)
{
  CNewArchiveStdStringStream file(std::fstream::in | std::fstream::out | std::fstream::binary);
  // Convert rock material
  CMaterialEntry& rock_tetra_entry = (CMaterialEntry&)*GraphEntry(MD_ROCK_MATERIAL);
  CMaterialEntry& rock_hexa_entry = (CMaterialEntry&)*source.GraphEntry(MD_ROCK_MATERIAL);

  IProgressBase progressBase;
  rock_hexa_entry.SaveStream(file, progressBase);
  file.SetPosition(0);
  CStreamVersion version = currentVersion();
  rock_tetra_entry.LoadStream(*this, file, version, progressBase);
}

void CModelBase::CopyHorizon(const CHorizonBase& source, CHorizonBase& target)
{
  CNewArchiveStdStringStream file(std::fstream::in | std::fstream::out | std::fstream::binary);
  IProgressBase progress;
  const_cast<CHorizonBase&>(source).CHorizonBase::SaveStream(file, progress);
  file.SetPosition(0);
  CStreamVersion version = currentVersion();
  target.CHorizonBase::LoadStream(file, version, progress);
}

void CModelBase::CopyFormationElementSet(const IFormationElementSet& source, 
                     IFormationElementSet& target)
{
  assert(&source.Model() != &target.Model());
  assert(this == &target.Model());

  QString strName = source.Name();
  if(strName.isEmpty()) strName = source.Formation().Name();
  target.Name(strName);
  target.Color(source.Color());
}

void CModelBase::CopyFormation(const CFormationBase& source, CFormationBase& target)
{
  assert(&source.Model() != &target.Model());
  assert(this == &target.Model());

  CNewArchiveStdStringStream file(std::fstream::in | std::fstream::out | std::fstream::binary);
  IProgressBase progress;
  const_cast<CFormationBase&>(source).CFormationBase::SaveStream(file, progress);
  file.SetPosition(0);
  CStreamVersion version = currentVersion();
  target.CFormationBase::LoadStream(file, version, progress);
}

void CModelBase::CopyHomogenizationBox(const CHomogenizationBox &source, CHomogenizationBox &target)
{
  CNewArchiveStdStringStream file(std::fstream::in | std::fstream::out | std::fstream::binary);
  IProgressBase progress;
  const_cast<CHomogenizationBox&>(source).CHomogenizationBox::SaveStream(file, progress);
  file.SetPosition(0);
  CStreamVersion version = currentVersion();
  target.CHomogenizationBox::LoadStream(file, version, progress);
}

void CModelBase::CopyInitialStressAndPressure(const CModelBase& source)
{
  // Initial stress
  CNewArchiveStdStringStream file(std::fstream::in | std::fstream::out | std::fstream::binary);
  IProgressBase progress;
  const_cast<CGlobalInitialStressNode&>(source.GlobalInitialStress()).SaveStream(file, progress);
  file.SetPosition(0);
  CStreamVersion version = currentVersion();
  GlobalInitialStress().LoadStream(file, version, progress);

  // Initial pressure
  CNewArchiveStdStringStream anotherFile(std::fstream::in | std::fstream::out | std::fstream::binary);
  const_cast<CGlobalPressure&>(source.GlobalPressure()).SaveStream(anotherFile, progress);
  anotherFile.SetPosition(0);
  GlobalPressure().LoadStream(anotherFile, version, progress);

}

void CModelBase::CopyTemperature(const CModelBase& source)
{
  CNewArchiveStdStringStream file(std::fstream::in | std::fstream::out | std::fstream::binary);
  IProgressBase progress;
  const_cast<CGlobalTemperature&>(source.InitialTemperature()).SaveStream(file, progress);
  file.SetPosition(0);
  CStreamVersion version = currentVersion();
  InitialTemperature().LoadStream(file, version, progress);
}

CDepletionStage::eIterationScheme CModelBase::DefaultIterationScheme(const CDepletionStage& source_stage) const
{
  return source_stage.NLIterationScheme();
}

void CModelBase::LoadZoominProperties(CStorageNode::TSTREAM& stream, CStreamVersion& version, CStorageNode::TPROGRESS& /*progress*/)
{
  if(version > CStreamVersion(3, 0, 110))
  {
  int nDef;
  stream >> nDef;
  m_ParentResultsDef = (TParentResultsDef)nDef;
  }

  if(version > CStreamVersion(3, 0, 111))
  {
  int n;
  stream >> n;
  m_bSourceDepletionStagesChanged = (n != 0);
  }

  if(version > CStreamVersion(3, 0, 112))
  {
  int n;
  stream >> n;
  m_bSourceGlobalPressureChanged = (n != 0);
  stream >> n;
  m_bSourceGlobalTemperatureChanged = (n != 0);
  }

  if(version > CStreamVersion(3, 0, 113))
  {
  int n;
  stream >> n;
  m_bSourceGravityChanged = (n != 0);
  }
}

bool CModelBase::DeleteEntry(const int nEntryType)
{
  if(!CFemAppModel::GraphEntry(nEntryType))
  return false;

  delete CFemAppModel::GraphEntry(nEntryType);

  return true;
}

void CModelBase::LoadMaterial(const QString &/*sPath*/, IProgressBase &/*progress*/)
{
/* TODO wedx
  assert(!sPath.IsEmpty());
  progress.Step();
  // Get material ..
  CMaterialEntry *pEntry = (CMaterialEntry*)( GraphEntry(MD_ROCK_MATERIAL));

  // Add material 
  CMatLib mlx;
  mlx.SetDatabasePath(sPath);
  int nMats = mlx.GetMaterialCount();
  
  for(int i = 0; i < nMats; ++i)
  {
    VERIFY(pEntry->Append(mlx, mlx.GetMaterialName(i)));
    QString sName = pEntry->Library().GetDatabasePath();
  }
*/
}

void CModelBase::DoDensityCorrection()
{
  assert(IsMesh());

  // Get the formation entry
  TFormationBaseEntry* pEntry = dynamic_cast<TFormationBaseEntry*>(GraphEntry(MD_BASE_FORMATION));
  TFormationBaseEntry::TNodeSet stFormation = pEntry->EntryNodes();

  // Create a pointset with points of mesh
  IPointSet::DIMENSION dim = IPointSet::DIM_1D;
  switch(Dimension())
  {
  case 2:
    dim = IPointSet::DIM_2D;
    break;
  case 3:
    dim = IPointSet::DIM_3D;
    break;
  default:
    assert(false);
  }

  QString title;
  title = getStringTableEntry(IDS_VALUENAME_RHOB);
  CPointSet *pPointSet = new CPointSet(title, *this, 1, dim, true);

  std::auto_ptr <IProgressBase> dlg( _g->prog()->create( eProgress::Dual, getStringTableEntry(IDS_CALC_DENSITIES), false, 1 ));

  int nTotalSteps = 0;
  int nStep = 0;

  TFormationBaseEntry::TNodeSet::iterator it;
  for(it = stFormation.begin(); it != stFormation.end(); it++)
  {
    int i;
    for(i = 0; i < (*it)->ElementSetSize(); i++)
    {
      nTotalSteps += (*it)->ElementSet(i).ElementSet().ElementSize();
    }
  }

  dlg->AddSteps(nTotalSteps);

  for(it = stFormation.begin(); it != stFormation.end(); it++)
  {
    int j;
    for(j = 0; j < (*it)->ElementSetSize(); j++)
    {
      IFormationElementSet &set = (*it)->ElementSet(j);

      int i;
      for(i = 0; i < set.ElementSet().ElementSize(); i++)
      {
        CreateDensityPoints(set.ElementSet().Element(i), *pPointSet);
        dlg->Step();
        ++nStep;
      }
    }
  }

  // Create density and assign
  const CValueTypeFactory *factory = CValueTypeFactory::instance();
  assert(factory->ValueTypeAvailable(IDT_VALUETYPE_RHOB));
  CValueType *pType = factory->BuildValueType(*pPointSet, IDT_VALUETYPE_RHOB, IDS_VALUENAME_RHOB);
  assert(pType);
  assert(pType->ComponentSize() == 1);
  assert(pPointSet->ValueSet(pPointSet->NodalValueSetSize() - 1).Component() == 0);
  pType->Component(0).LinkTo(pPointSet->ValueSet(pPointSet->NodalValueSetSize() - 1));

  // Apply density
  for(it = stFormation.begin(); it != stFormation.end(); it++)
  {
    if((*it)->Material(InitialDepletionStage()).Valid()) (*it)->Material(InitialDepletionStage()).LinkTo(*pType);
  }
}

bool CModelBase::OnModelSpecificAnalysisCheck(const CAnalysisType& /*antype*/, bool /*bWriteInputFiles*/, bool /*bWriteQuadDat*/)
{
  return true;
}

void CModelBase::ConvertSet(IPointSet* pSet, bool b3DSystem)
{
  assert(b3DSystem);

  // check if we have a 3D set with multiple points that have equal depth; if so, we convert to 2D
  if (dynamic_cast<CPointSet *>(pSet) && pSet->pointSetType() == IPointSet::POINTSET && pSet->PointSize() > 1)
  {
  bool bSameDepth = true;
  double z = pSet->PointAt(0).Z();
  for (size_t i = 1; i < pSet->PointSize(); ++i)
  {
      if (pSet->PointAt(i).Z() != z) // compare without epsilon (for now; it may be useful to alter this)
      {
    bSameDepth = false;
    break;
      }
  }
  if (bSameDepth)
      static_cast<CPointSet *>(pSet)->Coordinates().Dimension(IPointSet::DIM_2D);
  }
}

int CModelBase::IniStressMaxIterations() const
{
  return m_nIniStressMaxIterations;
}

void CModelBase::IniStressMaxIterations(int n)
{
  if(m_nIniStressMaxIterations != n)
  {
    getDocumentProxy()->setModifiedFlag();
    ResultRegister().ClearLinear();

    m_nIniStressMaxIterations = n;
  }
}

double CModelBase::IniStressConvCriterion() const
{
  return m_dIniStressConvCriterion;
}

void CModelBase::IniStressConvCriterion(double c)
{
  if(fabs(m_dIniStressConvCriterion - c) > 1e-15)
  {
    getDocumentProxy()->setModifiedFlag();
    ResultRegister().ClearLinear();

    m_dIniStressConvCriterion = c;
  }
}

bool CModelBase::LinstaOnlyInitialStress() const
{
  return m_bLinstaOnlyInitialStress;
}

void CModelBase::LinstaOnlyInitialStress(bool b)
{
  if(b != m_bLinstaOnlyInitialStress)
  {
    getDocumentProxy()->setModifiedFlag();
    ResultRegister().ClearLinear();

    m_bLinstaOnlyInitialStress = b;
  }
}

bool CModelBase::isPressureSupportNodeLoadWarningDone() const
{
  return m_bPressureSupportNodeLoadWarningDone;
}

void CModelBase::setPressureSupportNodeLoadWarningDone()
{
  m_bPressureSupportNodeLoadWarningDone = true;
}

const CBranchState& CModelBase::BranchState() const
{
  return *m_pBranchState;
}

CBranchState& CModelBase::BranchState()
{
  return *m_pBranchState;
}

void CModelBase::ClearBranch()
{
  if(m_pBranchState->IsBranch())
  {
    m_pBranchState->ClearBranch();
    DepletionStageEntry().Modified();
  }
}

bool CModelBase::HasBranches() const
{
  const CDepletionStage* pStage = &InitialDepletionStage();
  while(pStage)
  {
  if(pStage->OutputType() == CDepletionStage::BRANCH)
      return true;

  if(pStage->Last())
      pStage = 0;
  else
      pStage = &pStage->Next();
  }

  return false;
}

bool CModelBase::HasPhases() const
{
  const CDepletionStage* pStage = &InitialDepletionStage();
  while(pStage)
  {
  if(pStage->OutputType() == CDepletionStage::PHASE)
      return true;

  if(pStage->Last())
      pStage = 0;
  else
      pStage = &pStage->Next();
  }

  return false;
}

CGeomecDianaRunnerBase* CModelBase::CreateDianaRunner(CDianaRunController& controller)
{
  return OnCreateDianaRunner(controller);
}

void CModelBase::PrepareFaultsForAnalysis(bool bSilent)
{
  THorizonBaseEntry::TEntryNodeSet stNode = GraphEntry(MD_BASE_HORIZON)->GraphEntryNodes();

  std::auto_ptr <IProgressBase> prog;

  prog = std::auto_ptr<IProgressBase>(_g->prog()->create( bSilent ? eProgress::Silent : eProgress::MainFrame ));

 
  prog->StatusMessage("Preparing faults for analysis");
  prog->AddSteps(stNode.size());

  THorizonBaseEntry::TEntryNodeSet::iterator it;
  for(it = stNode.begin(); it != stNode.end(); ++it)
  {
    CHorizonBase& hor = static_cast<CHorizonBase&>(**it);
  if (!hor.IgnoreFault())
      hor.PrepareForAnalysis();
    prog->Step();
  }
}

bool CModelBase::AverageResults() const
{
  return m_bAverageResults;
}

void CModelBase::AverageResults(bool bAverage)
{
  if(bAverage != m_bAverageResults)
  {
  m_bAverageResults = bAverage;
  Modified();
  }
}

void CModelBase::CollectBoundaryNodes(TNodeElementsMap& /*mpNodeElements*/) const
{
  // must be implemented in derived class
  assert(false);
}

void CModelBase::CollectDepletingFormationNodes(TNodeElementsMap& mpNodeElements, CAnalysisType::TAnalysisType analysisType) const
{
  const TFormationBaseEntry& entry = (const TFormationBaseEntry&)(*GraphEntry(MD_BASE_FORMATION));
  const TFormationBaseEntry::TNodeSet& stNodes = entry.EntryNodes();
  TFormationBaseEntry::TNodeSet::const_iterator it;
  for(it = stNodes.begin(); it != stNodes.end(); ++it)
  {
  const CFormationBase& formation = **it;

  bool bChange = false;
  switch (analysisType)
  {
  case CAnalysisType::AT_HEAT:
      bChange = formation.TemperatureChange();
      break;
  case CAnalysisType::AT_MIXTURE:
  case CAnalysisType::AT_MIXTURE_CONTAINMENT:
      bChange = formation.PressureChange() || formation.ActiveMaterial().MaterialModel() == MM_UNDRAINED;
      break;
  default:
      assert(false); // we're only called from heat/mixture calculation
  }

  if(bChange)
  {
      int i;
      for(i = 0; i < formation.ElementSetSize(); ++i)
      {
    const IFormationElementSet& elementset = formation.ElementSet(i);
    int el;
    for(el = 0; el < elementset.ElementSet().ElementSize(); ++el)
    {
          const geo::IElement& elm = elementset.ElementSet().Element(el);
          int n;
          for(n = 0; n < elm.NrOfNodes(); ++n)
          {
      TNodeElementsMap::iterator itInsert = mpNodeElements.insert(TNodeElementsMap::value_type(elm.Node(n).Index(), std::set<int>())).first;
      itInsert->second.insert(elm.Index());
          }
    }
      }
  }
  }
}
//wjrx mantis 2976
bool CModelBase::CanImportResults() const
{
  if(!(IsMesh() != false))
  return false;

  // Each formation needs to have materials attached since a lot of results
  // are calculated using material parameters
  TFormationBaseEntry &entry =
  (TFormationBaseEntry&)*GraphEntry(MD_BASE_FORMATION);
  TFormationBaseEntry::TNodeSet stNode = entry.EntryNodes();
  for( TFormationBaseEntry::TNodeSet::const_iterator it = stNode.begin();
       it != stNode.end(); it++)
  {
  if(!((*it)->Material(InitialDepletionStage()).LibraryMaterial()))
  return false;
  }

  return true;
}
// wjrx mantis 2976
bool CModelBase::CanInsertNewWellPath() const
{
  return GraphEntry(MD_BASE_HORIZON)->GraphEntryNodes().size() > 1;
}

bool CModelBase::UseDecompactionParameters() const
{
  return m_bUseDecompactionParams;
}

void CModelBase::UseDecompactionParameters(bool bUse)
{
  if(bUse != m_bUseDecompactionParams)
  {
  m_bUseDecompactionParams = bUse;
  Modified();
  }
}

bool CModelBase::WriteOutputStreamFile() const
{
  return m_bWriteOutputStreamFile;
}

void CModelBase::WriteOutputStreamFile(bool bWrite)
{
  if(bWrite != m_bWriteOutputStreamFile)
  {
  m_bWriteOutputStreamFile = bWrite;
  Modified();
  }
}

bool CModelBase::CleanupOldBranchResults() const
{
  return m_bCleanupOldBranchResults;
}

void CModelBase::CleanupOldBranchResults(bool bCleanup)
{
  if(bCleanup != m_bCleanupOldBranchResults)
  {
  m_bCleanupOldBranchResults = bCleanup;
  Modified();
  }
}

void CModelBase::AutomaticallyDisableDrawing(bool bDisable)
{
  m_bAutomaticallyDisableDrawing = bDisable;
}

bool CModelBase::AutomaticallyDisableDrawing() const
{
  return m_bAutomaticallyDisableDrawing;
}

bool CModelBase::AutomaticallyDisableDrawingFor(const CGraphNode& node) const
{
  const COpenGLNode* pOpenGLNode = dynamic_cast<const COpenGLNode*>(&node);
  if(pOpenGLNode && !pOpenGLNode->BlocksDrawing())
  return false;

  return AutomaticallyDisableDrawing();
}

void CModelBase::DisableRedrawing()
{
  if(!m_bForcingRedraw)
  m_bAllowRedrawing = false;
}

void CModelBase::setAllowRedrawing(bool allowRedrawing)
{
  m_bAllowRedrawing = allowRedrawing;
}

void CModelBase::setForcingRedraw(bool forcingRedraw)
{
  m_bForcingRedraw = forcingRedraw;
}

bool CModelBase::RedrawingDisabled() const
{
  return !m_bAllowRedrawing;
}

bool CModelBase::SceneCanDraw() const
{
  return m_bAllowRedrawing;
}

bool CModelBase::ShowConvexHulls() const
{
  return m_bShowConvexHulls;
}

void CModelBase::ShowConvexHulls(bool bShowConvexHulls)
{
  m_bShowConvexHulls = bShowConvexHulls;
}


const CModelBase& CModelBase::RootModel() const
{
  return (const_cast<CModelBase*>(this))->RootModel();
}

CModelBase& CModelBase::RootModel()
{
  if(parentModel())
  return parentModel()->RootModel();

  return *this;
}

bool CModelBase::ExportCommandFilesWithDefaults() const
{
  return m_bExportCommandFilesWithDefaults;
}

void CModelBase::ExportCommandFilesWithDefaults(bool b)
{
  m_bExportCommandFilesWithDefaults = b;
}

int CModelBase::childModelSize() const
{
  CWellPathEntry* entry = dynamic_cast <CWellPathEntry*> (
  const_cast <CGraphEntry*> (GraphEntry(MD_BASE_WELLPATH)));
  int count = 0;

  if (entry != 0)
  {
  CWellPathEntry::TNodeSet nodes = entry->EntryNodes();

  for (CWellPathEntry::TNodeSet::const_iterator iterator = nodes.begin();
      iterator != nodes.end(); ++iterator)
  {
      count += (dynamic_cast <CWellPath*> (*iterator)->hasZoomInModel()) ? 1 : 0;
  }
  }

  // wjrx mantis 3687
  CNewWellPathEntry* newEntry = dynamic_cast <CNewWellPathEntry*> (
  const_cast <CGraphEntry*> (GraphEntry(MD_NEW_WELLPATH)));

  if (newEntry != 0)
  {
  CNewWellPathEntry::TNodeSet nodes = newEntry->EntryNodes();

  for (CNewWellPathEntry::TNodeSet::const_iterator iterator = nodes.begin();
      iterator != nodes.end(); ++iterator)
  {
      count += (dynamic_cast <CNewWellPath*> (*iterator)->hasZoomInModel()) ? 1 : 0;
  }
  }

  CZoominModelEntry* ziEntry = (CZoominModelEntry*)GraphEntry(MD_BASE_ZOOMIN_MODEL);
  if(ziEntry)
  count += ziEntry->EntryNodes().size();

  return count;
}

CModelBase& CModelBase::childModel(int index)
{
  CWellPathEntry* entry = dynamic_cast <CWellPathEntry*> (
  const_cast <CGraphEntry*> (GraphEntry(MD_BASE_WELLPATH)));
  if(entry)
  {
  CWellPathEntry::TNodeSet nodes = entry->EntryNodes();
  CWellPathEntry::TNodeSet::const_iterator iterator = nodes.begin();

  for (; iterator != nodes.end(); ++iterator)
  {
      if (dynamic_cast <CWellPath*> (*iterator)->hasZoomInModel())
      {
    if (index-- == 0)
    {
          return (*iterator)->ZoomInModel();
    }
      }
  }
  }

  // wjrx mantis 3687
  CNewWellPathEntry* newEntry = dynamic_cast <CNewWellPathEntry*> (
  const_cast <CGraphEntry*> (GraphEntry(MD_NEW_WELLPATH)));
  if(newEntry)
  {
  CNewWellPathEntry::TNodeSet nodes = newEntry->EntryNodes();
  CNewWellPathEntry::TNodeSet::const_iterator iterator = nodes.begin();

  for (; iterator != nodes.end(); ++iterator)
  {
      if (dynamic_cast <CNewWellPath*> (*iterator)->hasZoomInModel())
      {
    if (index-- == 0)
    {
          return (*iterator)->ZoomInModel();
    }
      }
  }
  }


  CZoominModelEntry* pziEntry = (CZoominModelEntry*)GraphEntry(MD_BASE_ZOOMIN_MODEL);
  if(pziEntry)
  {
  const CZoominModelEntry::TNodeSet& stNodes = pziEntry->EntryNodes();
  for(CZoominModelEntry::TNodeSet::const_iterator it = stNodes.begin(); it != stNodes.end(); ++it)
  {
      if(index-- == 0)
    return (*it)->ChildModel();
  }
  }

  assert(false);
  return *((CModelBase*)0);
}

const CModelBase& CModelBase::childModel(int index) const
{
  return (const_cast<CModelBase*>(this))->childModel(index);
}

CModelBase* CModelBase::parentModel()
{
  return m_pParentModel;
}

const CModelBase* CModelBase::parentModel() const
{
  return m_pParentModel;
}

void CModelBase::SetChildModelLoadSave(IModelLoadSave* pChildModelLoadSave)
{
  delete m_pChildModelLoadSave;
  m_pChildModelLoadSave = pChildModelLoadSave;
}

CVersionManager CModelBase::getVersionManager() const
{
  return m_versionManager;
}

int CModelBase::Index() const
{
  return m_nIndex;
}

void CModelBase::Index(int nIndex)
{
  m_nIndex = nIndex;
}

void CModelBase::SetAppVersion(const QString& applicationVersion)
{
  m_versionManager.AppVersion(applicationVersion);
}

CModelBase *CModelBase::Model(int nIndex)
{
  CModelBase *pModel = 0;

  if (nIndex == m_nIndex)
  {
  pModel = this;
  }
  else if (nIndex == 0)
  {
  pModel = &RootModel();
  assert(pModel->Index() == 0);
  }
  else
  {
  const CZoominModelEntry* pEntry = dynamic_cast<const CZoominModelEntry *>(RootModel().GraphEntry(MD_BASE_ZOOMIN_MODEL));
  if (pEntry)
  {
      const CZoominModelEntry::TNodeSet& zoominModels = pEntry->EntryNodes();

      for (CZoominModelEntry::TNodeSet::const_iterator it = zoominModels.begin(); it != zoominModels.end(); ++it)
      {
    CModelBase *pChildModel = &(*it)->ChildModel();
    if (pChildModel->Index() == nIndex)
    {
          pModel = pChildModel;
          break;
    }
      }
  }
  }

  return pModel;
}


void CModelBase::OnNewChildModel(CModelBase& child, bool attachToDocument)
{
  /*
   * This function must always be called when a child model has to be
   * created. The call to Modified() will result in the creation of
   * the accompanying GUI object (if required).
   * The m_NewModel object will take care of the proper model creation.
   */
  child.m_pParentModel = this;
  
  delete child.m_pConsistencyGuard;
  child.m_pConsistencyGuard = 0;

  GetConsistencyGuard()->AttachChildModelStart(child);

  m_NewModel.setModel(&child);
  Modified();

  m_NewModel.newModel(attachToDocument);

  GetConsistencyGuard()->AttachChildModelFinish(child);
}

CNewModel& CModelBase::getNewModelObject()
{
  return m_NewModel;
}

bool CModelBase::CanSwitchTo() const
{
  assert(parentModel());
  return parentModel()->Mesh().IsMesh();
}

void CModelBase::SwitchTo(CFemAppModel* currentModel)
{
  assert(currentModel == this);

  // did the user modify the source depletion stages?
  if(m_bSourceDepletionStagesChanged)
  {
  // should we update?
  if(_m()->msg("The parent model's depletion stages have been modified.\n\nWould you like to reinitialize this wellpath zoom-in model's depletion stage list?", MB_YESNO) == IDYES)
  {
      BuildDepletionStages();
      ResultRegister().ClearAll();
  }

  m_bSourceDepletionStagesChanged = false;
  }

  if(m_bSourceGlobalPressureChanged)
  {
  // should we update?
  if(_m()->msg("The parent model's global pressure definition has been modified.\n\nWould you like to copy the values to this zoom-in model?", MB_YESNO) == IDYES)
  {
      DuplicateGlobalPressure();
      ResultRegister().ClearAll();
  }

  m_bSourceGlobalPressureChanged = false;
  }

  if(m_bSourceGlobalTemperatureChanged)
  {
  // should we update?
  if(_m()->msg("The parent model's global temperature definition has been modified.\n\nWould you like to copy the values to this zoom-in model?", MB_YESNO) == IDYES)
  {
      DuplicateGlobalTemperature();
      ResultRegister().ClearAll();
  }

  m_bSourceGlobalTemperatureChanged = false;
  }

  if(m_bSourceGravityChanged)
  {
  if(_m()->msg("The parent model's gravity value has been modified.\n\nWould you like to copy the values to this zoom-in model?", MB_YESNO) == IDYES)
  {
      DuplicateGravity();
      ResultRegister().ClearAll();
  }
  }

  if (!Mesh().IsMesh() && Mesh().CanCreateMesh())
  Mesh().CreateMesh();

  FindParentFormations();
}

void CModelBase::FindParentFormations()
{
  if (Mesh().ElementSet().ElementSize() > 0)
  {
  TFormationBaseEntry::TNodeSet stNodes = static_cast<TFormationBaseEntry *>(GraphEntry(MD_BASE_FORMATION))->EntryNodes();
  for (TFormationBaseEntry::TNodeSet::iterator formationIt = stNodes.begin(); formationIt != stNodes.end(); ++formationIt)
      (*formationIt)->FindParentFormation();
  }
}

void CModelBase::SwitchToParent(CModelBase& parent)
{
  GetConsistencyGuard()->SwitchToModel(parent);
}

void CModelBase::OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint)
{
  if(parentModel())
  {
  if(RootModel().Loading())
      return;

  const CDepletionStage* pDeplStage = dynamic_cast<const CDepletionStage*>(&node);

  if(&node == &parentModel()->DepletionStageEntry() || (pDeplStage && &pDeplStage->Model() == parentModel()))
      m_bSourceDepletionStagesChanged = true; // something changed in the source depletion stages

  if(&node == &parentModel()->GlobalPressure())
      m_bSourceGlobalPressureChanged = true; // something changed in the source global pressure

  if(&node == &parentModel()->InitialTemperature())
      m_bSourceGlobalTemperatureChanged = true; // something changed in the source global temperature

  if(&node == &parentModel()->GravityNotifier())
      m_bSourceGravityChanged = true;
  }
}

void CModelBase::MenuInvokedBuildDepletionStages()
{
  if(_m()->msg("Are you sure you want to reinitialize all depletion stages from the parent model's depletion stages?", MB_YESNO) == IDYES)
  BuildDepletionStages();
}

void CModelBase::BuildDepletionStages()
{
  assert(parentModel());

  bool bClearAll = false;
  bool bClearNonlin = false;

  // duplicate the depletion stages from the parent model
  const CDepletionStageEntry& SourceEntry = parentModel()->DepletionStageEntry();
  CDepletionStageEntry& MyEntry = DepletionStageEntry();

  int nLastSourceStage = SourceEntry.LastStage().Index();
  int nLastMyStage = MyEntry.LastStage().Index();

  if(nLastMyStage < nLastSourceStage)
  {
  bClearAll = true;
  CDepletionStage* pStage = &MyEntry.LastStage();
  const CDepletionStage* pSourceStage = &SourceEntry.StageByIndex(nLastMyStage + 1);

  // add stages to match
  while(pSourceStage)
  {
      pStage = new CDepletionStage(pSourceStage->Name().toStdString().c_str(), *pStage);

      if(pSourceStage->Last())
    pSourceStage = 0;
      else
    pSourceStage = &pSourceStage->Next();
  }
  }
  else if(nLastMyStage > nLastSourceStage)
  {
  bClearAll = true;
  // delete stages to match
  CDepletionStage* pStage = &MyEntry.LastStage();
  while(pStage->Index() > nLastSourceStage)
  {
      CDepletionStage* pToDelete = pStage;

      pStage = &pStage->Previous();

      delete pToDelete;
  }
  }

  assert(SourceEntry.LastStage().Index() == MyEntry.LastStage().Index());

  CDepletionStage* pStage = &MyEntry.InitialStage();
  const CDepletionStage* pSrcStage = &SourceEntry.InitialStage();

  while(pStage && pSrcStage)
  {
  pStage->Name(pSrcStage->Name());

  if(pStage->NLSteps() != pSrcStage->NLSteps() ||
       fabs(pStage->NLStepScalingFactor() - pSrcStage->NLStepScalingFactor()) > 1e-10 ||
       fabs(pStage->NLConvNormVal() - pSrcStage->NLConvNormVal()) > 1e-10 ||
       pStage->NLIterationSteps() != pSrcStage->NLIterationSteps() ||
       pStage->NLConvergenceType() != pSrcStage->NLConvergenceType() ||
       pStage->NLIterationScheme() != DefaultIterationScheme(*pSrcStage))
  {
      pStage->SetNonLinParams(pSrcStage->AutomaticStepSizes(), pSrcStage->MaxAutomaticStepSize(), pSrcStage->NLSteps(), pSrcStage->NLStepScalingFactor(), pSrcStage->NLConvNormVal(),
                              pSrcStage->NLIterationSteps(), pSrcStage->NLConvergenceType(), DefaultIterationScheme(*pSrcStage));
      bClearNonlin = true;
  }

  if(pStage->Time() != pSrcStage->Time())
  {
      pStage->Time(pSrcStage->Time());
      bClearAll = true;
  }

  if(pStage->OutputType() != pSrcStage->OutputType())
  {
      pStage->setOutputType(pSrcStage->OutputType());
      bClearNonlin = true;
  }

  if(pSrcStage->IsMarkedAsInitial())
      pStage->MarkAsInitial();

  if(!IsLinkedTo(*pSrcStage))
      LinkTo(const_cast<CDepletionStage&>(*pSrcStage));

  if(pStage->Last())
  {
      assert(pSrcStage->Last());
      pStage = 0;
      pSrcStage = 0;
  }
  else
  {
      assert(!pSrcStage->Last());
      pStage = &pStage->Next();
      pSrcStage = &pSrcStage->Next();
  }
  }

  if(bClearAll)
  ResultRegister().ClearAll();
  else if(bClearNonlin)
  ResultRegister().ClearNonLinear();
}

CModelBase::TParentResultsDef CModelBase::ParentResultsDefinition() const
{
  return m_ParentResultsDef;
}

void CModelBase::ParentResultsDefinition(TParentResultsDef def)
{
  if(m_ParentResultsDef != def)
  {
  m_ParentResultsDef = def;
  ResultRegister().ClearAll();
  }
}

bool CModelBase::ParentLinearResults(const CAnalysisType& antype) const
{
  TParentResultsDef resultdef = ParentResultsDefinition();

  bool bRet = false;

  switch(resultdef)
  {
  case PRD_EQUAL:
  bRet = (antype == CAnalysisType::AT_LINEAR); // only use linear results for a linear analysis, otherwise nonlin (also for mixture)
  break;
  case PRD_LINEAR:
  bRet = true;
  break;
  case PRD_NONLIN:
  bRet = false;
  break;
  default:
  assert(FALSE);
  }

  return bRet;
}

const CDepletionStage& CModelBase::ParentEquivalentDepletionStage(const CDepletionStage& stage) const
{
  assert(parentModel());

  const CGeomecTime& time = stage.Time();

  const CDepletionStage* pParentStage = &parentModel()->InitialDepletionStage();

  if(time < pParentStage->Time())
  return *pParentStage;

  while(pParentStage)
  {
  const CDepletionStage* pParentNextStage = 0;
  if(!pParentStage->Last())
      pParentNextStage = &pParentStage->Next();

  if(!pParentNextStage)
      return *pParentStage;

  if(time < pParentNextStage->Time())
      return *pParentStage;

  pParentStage = pParentNextStage;
  }

  assert(FALSE);
  return *pParentStage;
}

bool CModelBase::CheckZoominSecant()
{
  if (!parentModel() || InitialDepletionStage().NLIterationScheme() != CDepletionStage::SECANT)
  return true;

  if (_m()->msg("Secant iteration scheme not allowed in zoom-in inital stage\nContinue with Regular for initial stage?", MB_ICONEXCLAMATION | MB_YESNO) == IDYES)
  {
  InitialDepletionStage().setNLIterationScheme(CDepletionStage::REGULAR);
  return true;
  }

  return false;
}

bool CModelBase::CheckParentResults(const CAnalysisType& antype) const
{
  if(!parentModel())
  return true;

  TParentResultsDef resultdef = ParentResultsDefinition();

  QString sParentRes;
  sParentRes = QString("\n\nRight-click '%1' in the tree and select 'Set parent model results type' to specify the result type from the parent model.").arg(Name());
  const QString sLinearMsg  = "No linear results are available in the parent model, please run the linear analysis for the parent model first." + sParentRes;
  const QString sNonlinMsg  = "No nonlinear results are available in the parent model, please run the nonlinear analysis for the parent model first." + sParentRes;
  const QString sMixtureMsg = "You have chosen to use equal results in the parent model.\nFor a mixture analysis nonlinear results are selected in that case, but no nonlinear "
                              "results are available in the parent model.\n\nPlease run the nonlinear analysis for the parent model first." + sParentRes;

  switch(resultdef)
  {
  case PRD_EQUAL:

  if(antype == CAnalysisType::AT_LINEAR && !parentModel()->ResultRegister().Linear())
  {
      _m()->msg(sLinearMsg);
      return false;
  }

  if(antype == CAnalysisType::AT_NONLIN && !parentModel()->ResultRegister().NonLinear())
  {
      _m()->msg(sNonlinMsg);
      return false;
  }

  if((antype == CAnalysisType::AT_MIXTURE || antype == CAnalysisType::AT_MIXTURE_CONTAINMENT) && !parentModel()->ResultRegister().NonLinear())
  {
      _m()->msg(sMixtureMsg);
      return false;
  }

  break;
  case PRD_LINEAR:

  if(!parentModel()->ResultRegister().Linear())
  {
      _m()->msg(sLinearMsg);
      return false;
  }

  break;
  case PRD_NONLIN:

  if(!parentModel()->ResultRegister().NonLinear())
  {
      _m()->msg(sNonlinMsg);
      return false;
  }

  break;
  default:
  assert(FALSE);
  }

  return true;
}

bool CModelBase::CheckCasingContainedInWell() const
{
  const CWellCasingModel *casingModel = dynamic_cast<const CWellCasingModel *>(this);

  if (casingModel)
  {
  const geo::IMesh& casingMesh = casingModel->Mesh().Mesh();
  const geo::IMesh& wellMesh = casingModel->ParentModel().Mesh().Mesh();

  int size = casingMesh.ElementSize();

  for (int i = 0; i < size; ++i)
  {
      const geo::IElement& elt = casingMesh.Element(i);

      for (int j = 0; j < elt.NrOfNodes(); ++j)
    if (!wellMesh.Contains(elt.Node(j), false))
    {
          QString msg = "Some of the mesh points are not contained in the well mesh, and not all parent stresses will be assigned. "
      "This may lead to inaccuracies in the results. You can check the parent stresses under Results->Input, or adjust your "
      "mesh by going back to the well zoomin model and adjust the casing model's attributes.\n"
      "Do you want to continue with the calculation?";
          return _m()->msg(msg, MB_YESNO) == IDYES;
    }
  }
  }

  return true;
}

bool CModelBase::getEnableResultCache() const
{
  return m_EnableResultCache;
}

int CModelBase::getResultCacheSize() const
{
  return m_ResultCacheSize;
}

void CModelBase::setResultCache(bool enableResultCache, int resultCacheSize)
{
  bool oldEnableResultCache = m_EnableResultCache;
  int oldResultCacheSize = m_ResultCacheSize;

  m_EnableResultCache = enableResultCache;
  m_ResultCacheSize = resultCacheSize;

  // Handle the cases:
  // - from enabled to disabled,
  // - from disabled to enabled.
  // - new resultCacheSize is greater than old m_ResultCacheSize,
  // - new resultCacheSize is less than old m_ResultCacheSize.

  if (ResultRegister().cacheExists())
  {
  CResultCache& resultCache = ResultRegister().Cache();

  if (oldEnableResultCache != m_EnableResultCache)
  {
      resultCache.switchRawCacher(m_EnableResultCache, m_ResultCacheSize);
  }
  else if (m_EnableResultCache && (oldResultCacheSize != m_ResultCacheSize))
  {
      resultCache.resizeRawCacher(oldResultCacheSize, m_ResultCacheSize);
  }
  }
}

CMaterialErrorHandler& CModelBase::getMaterialErrorHandler()
{
  return m_materialErrorHandler;
}

void CModelBase::ConvergenceChecks(bool bCheck)
{
  if(bCheck != m_bConvergenceChecks)
  {
  m_bConvergenceChecks = bCheck;
  ConvergenceChecksChanged();
  }
}

bool CModelBase::ConvergenceChecks() const
{
  return m_bConvergenceChecks;
}

void CModelBase::ConvergenceMinimumAngleCheck(bool bCheck)
{
  if(bCheck != m_bConvergenceMinimumAngle)
  {
  m_bConvergenceMinimumAngle = bCheck;
  ConvergenceChecksChanged();
  }
}

bool CModelBase::ConvergenceMinimumAngleCheck() const
{
  return m_bConvergenceMinimumAngle;
}

void CModelBase::ConvergenceMinimumAngleValue(double dValue)
{
  if(fabs(dValue - m_dConvergenceMinimumAngle) > 1e-8)
  {
  m_dConvergenceMinimumAngle = dValue;
  ConvergenceChecksChanged();
  }
}

double CModelBase::ConvergenceMinimumAngleValue() const
{
  return m_dConvergenceMinimumAngle;
}

void CModelBase::ConvergenceMinimumRatioCheck(bool bCheck)
{
  if(bCheck != m_bConvergenceMinimumRatio)
  {
  m_bConvergenceMinimumRatio = bCheck;
  ConvergenceChecksChanged();
  }
}

bool CModelBase::ConvergenceMinimumRatioCheck() const
{
  return m_bConvergenceMinimumRatio;
}

void CModelBase::ConvergenceMinimumRatioValue(double dValue)
{
  if(fabs(dValue - m_dConvergenceMinimumRatio) > 1e-8)
  {
  m_dConvergenceMinimumRatio = dValue;
  ConvergenceChecksChanged();
  }
}

double CModelBase::ConvergenceMinimumRatioValue() const
{
  return m_dConvergenceMinimumRatio;
}

void CModelBase::ConvergenceMinimumVolumeCheck(bool bCheck)
{
  if(bCheck != m_bConvergenceMinimumVolume)
  {
  m_bConvergenceMinimumVolume = bCheck;
  ConvergenceChecksChanged();
  }
}

bool CModelBase::ConvergenceMinimumVolumeCheck() const
{
  return m_bConvergenceMinimumVolume;
}

void CModelBase::ConvergenceMinimumVolumeValue(double dValue)
{
  if(fabs(dValue - m_dConvergenceMinimumVolume) > 1e-8)
  {
  m_dConvergenceMinimumVolume = dValue;
  ConvergenceChecksChanged();
  }
}

double CModelBase::ConvergenceMinimumVolumeValue() const
{
  return m_dConvergenceMinimumVolume;
}

void CModelBase::ConvergenceChecksChanged()
{
  // clear the nonlinear results
  ResultRegister().ClearMixture(false);
  ResultRegister().ClearMixtureContainment(false);
  ResultRegister().ClearNonLinear();
}

void CModelBase::DefaultDianaZoominSetting(bool bDefaultDianaZoominSetting)
{
  m_bDefaultDianaZoominSetting = bDefaultDianaZoominSetting;
}

bool CModelBase::DefaultDianaZoominSetting() const
{
  return m_bDefaultDianaZoominSetting;
}
