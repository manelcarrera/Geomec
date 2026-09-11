
#include "WellCasingModel.h"

#include "AnalysisPoint.h"
#include "GlobalPressure.h"
#include "ColorScaleEntry.h"
#include "DerivedResult.h"
#include "ResultTree.h"
#include "WellCasingNode.h"
#include "WellCasingCement.h"
#include "WellFormation.h"
#include "WellModelEntryTypes.h"
#include "WellCasingDianaRunner.h"
#include "WellCasingSteel.h"
#include "WellCasingCementInterface.h"
#include "DianaRunController.h"
#include "WellCasingMixtureDianaRunner.h"
#include "WellCasingHeatFlowDianaRunner.h"
#include "WellCasingContainmentMixtureDianaRunner.h"
#include "WellCasingRTCI.h"

#include "IDeformation.h"

bool CWellCasingModel::s_bBetaWarningSeen = false;

CWellCasingModel::CWellCasingModel(CWellZoomInModel& parent, CAnalysisLogger& logger, const CVersionManager& versionManager)
: IWellModel(logger, versionManager),
  m_parent(parent),
  m_pMesh(0),
  m_pCasingNode(0),
  m_pCasingCement(0),
  m_deformation( 0 )
{
  Radius(10);
  LargeDeformations(true, false);
  LinkTo(parent);
  LinkTo(parent.Mesh());
}

// for the attributes dialog
CWellCasingModel::CWellCasingModel(const CWellCasingModel& rhs)
: IWellModel(rhs),
  m_parent(rhs.m_parent),
  m_pMesh(rhs.m_pMesh), // shallow copy of the mesh
  m_deformation( 0 )
{
}

CWellCasingModel::~CWellCasingModel()
{
  if( m_deformation )
  {
    delete m_deformation;
    m_deformation = 0;
  }
}

CWellCasingModel& CWellCasingModel::operator=(const CWellCasingModel& rhs)
{
  if(!(*this == rhs))
  {
  m_pMesh->InvalidateMesh();
  }

  return *this;
}

bool CWellCasingModel::operator==(const CWellCasingModel& /*rhs*/) const
{
  return (
  true
  );
}

void CWellCasingModel::createContainers()
{
  IWellModel::createContainers(); // creates the depletion stage entry

  new CWellCasingFormationEntry(MD_WELLMODEL_FORMATION, *this);
  new CWellCasingRTCIEntry(*this);
}

void CWellCasingModel::createDefaults()
{
  CreateBoundary();
  m_pMesh = new CWellCasingMesh("Mesh", *this);

  IWellModel::createDefaults();
}

void CWellCasingModel::CreateChildren()
{
  assert(!m_pCasingNode && !m_pCasingCement);
  m_pCasingNode = new CWellCasingNode(*this);
  m_pCasingCement = new CWellCasingCement("Cement", *this);

  IWellModel::CreateChildren();
}

unsigned int CWellCasingModel::ModelIconId() const
{
  return IDI_WELLCASINGMODEL;
}

unsigned int CWellCasingModel::TypeId() const
{
  return 0;
}

QString CWellCasingModel::documentType() const
{
  return "Casing Model";
}

void CWellCasingModel::CreateDensityPoints(const geo::IElement &/*element*/, CPointSet &/*pointset*/)
{
  assert(FALSE);
}

CGeomecDianaRunnerBase* CWellCasingModel::OnCreateDianaRunner(CDianaRunController& controller)
{
  switch (controller.AnalysisType().AnalysisType())
  {
    case CAnalysisType::AT_MIXTURE:				return new CWellCasingMixtureDianaRunner(*this, controller);
    case CAnalysisType::AT_MIXTURE_CONTAINMENT: return new CWellCasingMixtureContainmentDianaRunner(*this, controller);
    case CAnalysisType::AT_HEAT:				return new CWellCasingHeatFlowDianaRunner(*this, controller);
    default:									return new CWellCasingDianaRunner(*this, controller);
  }
}

const double &CWellCasingModel::Depth(const geo::ICoordinate &coord) const
{
  return coord.Z();
}

double CWellCasingModel::Northing(const geo::ICoordinate& coord) const
{
  return coord.X();
}

double CWellCasingModel::Easting(const geo::ICoordinate& coord) const
{
  return coord.Y();
}

bool CWellCasingModel::IsResult() const
{
  return ResultRegister().Linear() || ResultRegister().NonLinear();
}

void CWellCasingModel::Calculate()
{
}

int CWellCasingModel::Dimension() const
{
  return 3;
}

const CWellZoomInModel& CWellCasingModel::ParentModel() const
{
  return m_parent;
}

CWellZoomInModel& CWellCasingModel::ParentModel()
{
  return m_parent;
}

CWellCasingMesh& CWellCasingModel::Mesh()
{
  return *m_pMesh;
}

const CWellCasingMesh& CWellCasingModel::Mesh() const
{
  return *m_pMesh;
}

bool CWellCasingModel::CanCalculate() const
{
  if(!HeatFlowCalculationCriteriaMet())
  return false;

  if(!CasingCement().Material(BranchState().ActiveStage()).Valid())
  return false;

  if(!CasingNode().Steel().Material(BranchState().ActiveStage()).Valid())
  return false;

  if(!CasingNode().CementInterface().Material(BranchState().ActiveStage()).Valid())
  return false;

  return true;
}

bool CWellCasingModel::CanCalculateMixture() const
{
  return CanCalculate();
}

bool CWellCasingModel::CanCalculateHeatFlow() const
{
  return true;
}

void CWellCasingModel::OnCloseModel()
{
  VERIFY(DeleteEntry(MD_WELLCASING_RTCI));
  VERIFY(DeleteEntry(MD_WELLMODEL_FORMATION));
  delete m_pCasingNode;
  m_pCasingNode = 0;
  delete m_pCasingCement;
  m_pCasingCement = 0;

  IWellModel::OnCloseModel();
}

long CWellCasingModel::SavedItems() const
{
  long lRet = IWellModel::SavedItems();

  lRet += SavedItemsPointSets();
  const CAnalysisPointEntry& point_entry = dynamic_cast<const CAnalysisPointEntry&>(*GraphEntry(MD_BASE_ANALYSIS_POINT));
  lRet += point_entry.SavedItems();
  lRet += m_pCasingNode->SavedItems();
  lRet += m_pCasingCement->SavedItems();
  lRet += Boundary().SavedItems();
  lRet += GlobalPressure().SavedItems();
  lRet += Mesh().SavedItems();
  const CColorScaleEntry *pCSEntry = dynamic_cast<const CColorScaleEntry*>(GraphEntry(MD_BASE_COLOR_SCALE));
  lRet += pCSEntry->SavedItems();	
  lRet += ResultRegister().SavedItems();
  const CDerivedResultGroup& derivedresults = ResultTree().DerivedResults();
  lRet += derivedresults.SavedItems();

  return lRet;
}

bool CWellCasingModel::LoadWellCasing(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &prog)
{
  LoadPointSets(stream, version, prog);

  CAnalysisPointEntry& point_entry = dynamic_cast<CAnalysisPointEntry&>(*GraphEntry(MD_BASE_ANALYSIS_POINT));
  point_entry.LoadStream(*this, stream, version, prog);

  IWellModel::OnLoad(stream, version, prog);

  assert(m_pCasingNode == 0);
  m_pCasingNode = new CWellCasingNode(*this);
  m_pCasingNode->LoadStream(stream, version, prog);

  assert(m_pCasingCement == 0);
  m_pCasingCement = new CWellCasingCement(*this);
  m_pCasingCement->LoadStream(stream, version, prog);

  if(version >= CStreamVersion(3, 7, 18))
  {
  CWellCasingRTCIEntry& rtci_entry = static_cast<CWellCasingRTCIEntry&>(*GraphEntry(MD_WELLCASING_RTCI));
  rtci_entry.LoadStream(*this, stream, version, prog);
  }

  Boundary().LoadStream(stream,version,prog);

  GlobalPressure().LoadStream(stream, version, prog);

  assert(Mesh().IsLinkedTo(Boundary()));
  Mesh().InvalidateMesh();
  Mesh().Lock();
  Mesh().LoadStream(stream,version,prog);
  assert(Mesh().IsLinkedTo(Boundary()));

  CColorScaleEntry *pCSEntry = dynamic_cast<CColorScaleEntry*>(GraphEntry(MD_BASE_COLOR_SCALE));	
  pCSEntry->LoadStream(stream, version, prog);

  if (version < CStreamVersion(4, 1, 64))
  {
  LargeDeformations(true, false);
  }

  // Load the result server
  ResultRegister().LoadStream(stream, version, prog);

  CDerivedResultGroup& resultgroup = ResultTree().DerivedResults();
  resultgroup.LoadStream(stream, version, prog);

  return true;
}

bool CWellCasingModel::OnLoad(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &prog)
{
  return LoadWellCasing(stream, version, prog);
}

bool CWellCasingModel::OnSave(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &progress)
{
  // Number value composites
  NumberValueComposites();

  // Number opengl nodes
  NumberOpenGLNodes();

  // Save pointset
  SavePointSets(stream, progress);

  // Save analysis points
  CAnalysisPointEntry& point_entry = dynamic_cast<CAnalysisPointEntry&>(*GraphEntry(MD_BASE_ANALYSIS_POINT));
  point_entry.SaveStream(stream, progress);

  IWellModel::OnSave(stream, progress);

  m_pCasingNode->SaveStream(stream, progress);
  m_pCasingCement->SaveStream(stream, progress);

  CWellCasingRTCIEntry& rtci_entry = static_cast<CWellCasingRTCIEntry&>(*GraphEntry(MD_WELLCASING_RTCI));
  rtci_entry.SaveStream(stream, progress);

  Boundary().SaveStream(stream,progress);

  GlobalPressure().SaveStream(stream, progress);

  Mesh().SaveStream(stream,progress);

  // Save the color scales
  CColorScaleEntry *pCSEntry = dynamic_cast<CColorScaleEntry*>(GraphEntry(MD_BASE_COLOR_SCALE));	
  pCSEntry->SaveStream(stream, progress);

  ResultRegister().SaveStream(stream,progress);

  // Save the derived results
  CDerivedResultGroup& derivedresults = ResultTree().DerivedResults();
  derivedresults.SaveStream(stream, progress);

  return true;
}

CWellPath* CWellCasingModel::WellPath()
{
  return m_parent.WellPath();
}

const CWellPath* CWellCasingModel::WellPath() const
{
  return m_parent.WellPath();
}

CNewWellPath* CWellCasingModel::NewWellPath()
{
  return m_parent.NewWellPath();
}

const CNewWellPath* CWellCasingModel::NewWellPath() const
{
  return m_parent.NewWellPath();
}

CWellCasingNode& CWellCasingModel::CasingNode()
{
  return *m_pCasingNode;
}

const CWellCasingNode& CWellCasingModel::CasingNode() const
{
  return *m_pCasingNode;
}

CWellCasingCement& CWellCasingModel::CasingCement()
{
  return *m_pCasingCement;
}

const CWellCasingCement& CWellCasingModel::CasingCement() const
{
  return *m_pCasingCement;
}

void CWellCasingModel::CollectBoundaryNodes(TNodeElementsMap& mpNodeElements) const
{
  Mesh().CollectBoundaryNodes(mpNodeElements);
}

void CWellCasingModel::SwitchToWarning()
{
  if(!s_bBetaWarningSeen)
  {
#ifndef _DEBUG
  _m()->msg("The well casing model is still in beta stage");
#endif
  s_bBetaWarningSeen = true;
  }
}

void CWellCasingModel::SwitchTo(CFemAppModel* currentModel)
{
  SwitchToWarning();

  IWellModel::SwitchTo(currentModel);
}

int CWellCasingModel::childModelSize() const
{
  return 0;
}

CModelBase& CWellCasingModel::childModel(int /*index*/)
{
  assert(false);

  return *this;
}

CModelBase* CWellCasingModel::parentModel()
{
  return &ParentModel();
}

CWellCasingModel::ModelType CWellCasingModel::modelType() const
{
  return eWellCasingModel;
}

QString CWellCasingModel::AttributesDialogCaption() const
{
  return "Wellpath Casing Model Attributes";
}

CDepletionStage::eIterationScheme CWellCasingModel::DefaultIterationScheme(const CDepletionStage& source_stage) const
{
  if(!source_stage.Initial())
  return CDepletionStage::REGULAR;

  return IWellModel::DefaultIterationScheme(source_stage);
}

void CWellCasingModel::InvalidateDeformation()
{ 
  if( m_deformation ) 
    m_deformation->Invalidate(); 
}

///// CWellCasingFormationEntry

CWellCasingFormationEntry::CWellCasingFormationEntry(int nEntryId, CWellCasingModel& model)
: CWellFormationEntry(nEntryId, model)
{
}

void CWellCasingFormationEntry::OnShowSkin()
{
  CWellFormationEntry::OnShowSkin();

  CWellCasingModel& model = static_cast<CWellCasingModel&>(Model());
  model.CasingCement().OnShowElements();
  model.CasingNode().Steel().OnShowElements();
  model.CasingNode().CementInterface().OnShowElements();
}

void CWellCasingFormationEntry::OnShowMidpoints()
{
  CWellFormationEntry::OnShowMidpoints();

  CWellCasingModel& model = static_cast<CWellCasingModel&>(Model());
  model.CasingCement().OnShowMidpoints();
  model.CasingNode().Steel().OnShowMidpoints();
  model.CasingNode().CementInterface().OnShowMidpoints();
}

void CWellCasingFormationEntry::OnShowFullMesh()
{
  CWellFormationEntry::OnShowFullMesh();

  CWellCasingModel& model = static_cast<CWellCasingModel&>(Model());
  model.CasingCement().OnShowElements();
  model.CasingNode().Steel().OnShowElements();
  model.CasingNode().CementInterface().OnShowElements();
}