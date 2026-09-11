#include "WellZoomInModel.h"
#include "AnalysisPoint.h"
#include "BaseEntryTypes.h"
#include "ColorScaleEntry.h"
#include "DerivedResult.h"
#include "GlobalPressure.h"
#include "ModelLifetimeFacade.h"
#include "NewWellPath.h"
#include "ResultTree.h"
#include "WellCasingModel.h"
#include "WellFormation.h"
#include "WellModelEntryTypes.h"
#include "WellPath.h"
#include "WellZoomInDianaRunner.h"

CWellZoomInModel::CWellZoomInModel(CFemAppModel & /*model*/, CWellPath &wellpath, CAnalysisLogger &logger,
                                   const CVersionManager &versionManager)
    : IWellModel(logger, versionManager), m_pWellPath(&wellpath), m_pNewWellPath(0), m_pCasingModel(0) {
  Radius(30);
  LinkTo(wellpath);
  LinkTo((static_cast<CModelBase &>(wellpath.Model())).Mesh());
}

CWellZoomInModel::CWellZoomInModel(CFemAppModel & /*model*/, CNewWellPath &wellpath, CAnalysisLogger &logger,
                                   const CVersionManager &versionManager)
    : IWellModel(logger, versionManager), m_pWellPath(0), m_pNewWellPath(&wellpath), m_pCasingModel(0) {
  Radius(30);
  LinkTo(wellpath);
  LinkTo((static_cast<CModelBase &>(wellpath.Model())).Mesh());
}

// for the attributes dialog
CWellZoomInModel::CWellZoomInModel(const CWellZoomInModel &rhs)
    : IWellModel(rhs), m_pWellPath(rhs.m_pWellPath), m_pNewWellPath(rhs.m_pNewWellPath),
      m_pMesh(rhs.m_pMesh),              // shallow copy of the mesh
      m_pCasingModel(rhs.m_pCasingModel) // shallow copy of the casing model
{}

CWellZoomInModel::~CWellZoomInModel() {}

CWellZoomInModel &CWellZoomInModel::operator=(const CWellZoomInModel &rhs) {
  if (!(*this == rhs)) {
    m_pMesh->InvalidateMesh();
  }

  return *this;
}

bool CWellZoomInModel::operator==(const CWellZoomInModel &rhs) const {
  assert(m_pWellPath != 0 || m_pNewWellPath != 0);
  assert(!(m_pWellPath != 0 && m_pNewWellPath != 0));

  return (m_pWellPath == rhs.m_pWellPath) && (m_pNewWellPath == rhs.m_pNewWellPath);
}

void CWellZoomInModel::createContainers() {
  IWellModel::createContainers(); // creates the depletion stage entry

  new CWellFormationEntry(MD_WELLMODEL_FORMATION, *this);
}

void CWellZoomInModel::createDefaults() {
  CreateBoundary();
  m_pMesh = new CWellZoomInMesh("Zoom In Mesh", *this);

  IWellModel::createDefaults();

  m_pCasingModel = static_cast<CWellCasingModel *>(
      IModelLifetimeFacade::NewChild(WELL_CASING_MODEL, *this, Logger(), getVersionManager()));

  // there is a new child
  OnNewChildModel(*m_pCasingModel, false);
}

unsigned int CWellZoomInModel::ModelIconId() const { return IDI_WELLZOOMIN_MODEL; }

unsigned int CWellZoomInModel::TypeId() const { return 0; }

QString CWellZoomInModel::documentType() const { return "Wellpath Zoom-in"; }

void CWellZoomInModel::CreateDensityPoints(const geo::IElement & /*element*/, CPointSet & /*pointset*/) {
  assert(FALSE);
}

CGeomecDianaRunnerBase *CWellZoomInModel::OnCreateDianaRunner(CDianaRunController &controller) {
  return new CWellZoomInDianaRunner(*this, controller);
}

bool CWellZoomInModel::IsResult() const { return ResultRegister().Linear() || ResultRegister().NonLinear(); }

void CWellZoomInModel::Calculate() {}

int CWellZoomInModel::Dimension() const { return 3; }

void CWellZoomInModel::OnCloseModel() {
  VERIFY(DeleteEntry(MD_WELLMODEL_FORMATION));

  if (m_pCasingModel) {
    m_pCasingModel->DestroyZoomInModel();
    m_pCasingModel = 0;
  }

  IWellModel::OnCloseModel();
}

long CWellZoomInModel::SavedItems() const {
  long lRet = IWellModel::SavedItems();

  lRet += SavedItemsPointSets();
  const CAnalysisPointEntry &point_entry =
      dynamic_cast<const CAnalysisPointEntry &>(*GraphEntry(MD_BASE_ANALYSIS_POINT));
  lRet += point_entry.SavedItems();
  lRet += Boundary().SavedItems();
  lRet += GlobalPressure().SavedItems();
  const CWellFormationEntry &formation_entry =
      dynamic_cast<const CWellFormationEntry &>(*GraphEntry(MD_WELLMODEL_FORMATION));
  lRet += formation_entry.SavedItems();
  lRet += Mesh().SavedItems();
  const CColorScaleEntry *pCSEntry = dynamic_cast<const CColorScaleEntry *>(GraphEntry(MD_BASE_COLOR_SCALE));
  lRet += pCSEntry->SavedItems();
  lRet += ResultRegister().SavedItems();
  const CDerivedResultGroup &derivedresults = ResultTree().DerivedResults();
  lRet += derivedresults.SavedItems();
  lRet += SavedItemsChildModel(*m_pCasingModel);

  return lRet;
}

bool CWellZoomInModel::LoadWellZoomIn1(CStorageNode::TSTREAM &stream, CStreamVersion &version,
                                       CStorageNode::TPROGRESS &prog) {
  LoadPointSets(stream, version, prog);

  CAnalysisPointEntry &point_entry = dynamic_cast<CAnalysisPointEntry &>(*GraphEntry(MD_BASE_ANALYSIS_POINT));
  point_entry.LoadStream(*this, stream, version, prog);

  IWellModel::OnLoad(stream, version, prog);

  Boundary().LoadStream(stream, version, prog);

  GlobalPressure().LoadStream(stream, version, prog);

  assert(Mesh().IsLinkedTo(Boundary()));
  Mesh().InvalidateMesh();
  Mesh().Lock();
  Mesh().LoadStream(stream, version, prog);
  assert(Mesh().IsLinkedTo(Boundary()));

  CColorScaleEntry *pCSEntry = dynamic_cast<CColorScaleEntry *>(GraphEntry(MD_BASE_COLOR_SCALE));
  pCSEntry->LoadStream(stream, version, prog);

  // Load the result server
  ResultRegister().LoadStream(stream, version, prog);

  CDerivedResultGroup &resultgroup = ResultTree().DerivedResults();
  resultgroup.LoadStream(stream, version, prog);

  return true;
}

bool CWellZoomInModel::OnLoad(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &prog) {
  return (LoadWellZoomIn1(stream, version, prog) && LoadWellZoomIn2(stream, version, prog));
}

bool CWellZoomInModel::LoadWellZoomIn2(CStorageNode::TSTREAM &stream, CStreamVersion &version,
                                       CStorageNode::TPROGRESS &prog) {
  if (version >= CStreamVersion(3, 7, 9))
    LoadChildModel(stream, version, prog, *m_pCasingModel);
  else
    m_pCasingModel->CreateChildren();

  return true;
}

bool CWellZoomInModel::OnSave(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &progress) {
  // Number value composites
  NumberValueComposites();

  // Number opengl nodes
  NumberOpenGLNodes();

  // Save pointset
  SavePointSets(stream, progress);

  // Save analysis points
  CAnalysisPointEntry &point_entry = dynamic_cast<CAnalysisPointEntry &>(*GraphEntry(MD_BASE_ANALYSIS_POINT));
  point_entry.SaveStream(stream, progress);

  IWellModel::OnSave(stream, progress);

  // Save boundary
  Boundary().SaveStream(stream, progress);

  GlobalPressure().SaveStream(stream, progress);

  Mesh().SaveStream(stream, progress);

  // Save the color scales
  CColorScaleEntry *pCSEntry = dynamic_cast<CColorScaleEntry *>(GraphEntry(MD_BASE_COLOR_SCALE));
  pCSEntry->SaveStream(stream, progress);

  ResultRegister().SaveStream(stream, progress);

  // Save the derived results
  CDerivedResultGroup &derivedresults = ResultTree().DerivedResults();
  derivedresults.SaveStream(stream, progress);

  // save the casing model
  SaveChildModel(stream, progress, *m_pCasingModel);

  return true;
}

CWellCasingModel &CWellZoomInModel::CasingModel() {
  assert(m_pCasingModel);
  return *m_pCasingModel;
}

const CWellCasingModel &CWellZoomInModel::CasingModel() const {
  assert(m_pCasingModel);
  return *m_pCasingModel;
}

int CWellZoomInModel::childModelSize() const { return (m_pCasingModel != 0 ? 1 : 0); }

CModelBase &CWellZoomInModel::childModel(int index) {
  assert(index == 0);
  assert(m_pCasingModel);

  return *m_pCasingModel;
}

CModelBase *CWellZoomInModel::parentModel() {
  assert(m_pWellPath != 0 || m_pNewWellPath != 0);
  assert(!(m_pWellPath != 0 && m_pNewWellPath != 0));

  if (m_pWellPath)
    return &static_cast<CModelBase &>(m_pWellPath->Model());
  else
    return &static_cast<CModelBase &>(m_pNewWellPath->Model());
}

CWellZoomInModel::ModelType CWellZoomInModel::modelType() const { return eWellZoomInModel; }

void CWellZoomInModel::setNewModelFunction(bool (CNewModel::*newModel)(bool)) { m_newModelFunction = newModel; }

CNewModel &CWellZoomInModel::getNewModel() { return m_newModel; }

QString CWellZoomInModel::AttributesDialogCaption() const { return "Wellpath Zoomin Model Attributes"; }

bool CWellZoomInModel::IsParentEntry(int nEntryType) const {
  return (nEntryType == MD_BASE_SURFACE || nEntryType == MD_BASE_POINTSET || nEntryType == MD_ROCK_MATERIAL);
}

const CModelBase &CWellZoomInModel::ParentModel() const {
  assert(m_pWellPath != 0 || m_pNewWellPath != 0);
  assert(!(m_pWellPath != 0 && m_pNewWellPath != 0));

  if (m_pWellPath)
    return static_cast<const CModelBase &>(m_pWellPath->Model());
  else
    return static_cast<const CModelBase &>(m_pNewWellPath->Model());
}

CModelBase &CWellZoomInModel::ParentModel() {
  assert(m_pWellPath != 0 || m_pNewWellPath != 0);
  assert(!(m_pWellPath != 0 && m_pNewWellPath != 0));

  if (m_pWellPath)
    return static_cast<CModelBase &>(m_pWellPath->Model());
  else
    return static_cast<CModelBase &>(m_pNewWellPath->Model());
}

CWellPath *CWellZoomInModel::WellPath() {
  assert(m_pWellPath != 0 || m_pNewWellPath != 0);
  assert(!(m_pWellPath != 0 && m_pNewWellPath != 0));

  return m_pWellPath;
}

const CWellPath *CWellZoomInModel::WellPath() const {
  assert(m_pWellPath != 0 || m_pNewWellPath != 0);
  assert(!(m_pWellPath != 0 && m_pNewWellPath != 0));

  return m_pWellPath;
}

CNewWellPath *CWellZoomInModel::NewWellPath() {
  assert(m_pWellPath != 0 || m_pNewWellPath != 0);
  assert(!(m_pWellPath != 0 && m_pNewWellPath != 0));

  return m_pNewWellPath;
}

const CNewWellPath *CWellZoomInModel::NewWellPath() const {
  assert(m_pWellPath != 0 || m_pNewWellPath != 0);
  assert(!(m_pWellPath != 0 && m_pNewWellPath != 0));

  return m_pNewWellPath;
}

const CWellZoomInMesh &CWellZoomInModel::Mesh() const { return *m_pMesh; }

CWellZoomInMesh &CWellZoomInModel::Mesh() { return *m_pMesh; }
