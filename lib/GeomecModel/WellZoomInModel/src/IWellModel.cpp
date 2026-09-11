
#include <algorithm>

#include "IWellModel.h"
#include "GlobalPressure.h"
#include "GlobalTemperature.h"
#include "WellPath.h"
#include "NewWellPath.h"
#include "WellFormation.h"
#include "WellModelEntryTypes.h"
#include "GlobalMessage.h"
#include "ModelProxyConsole.h"
#include "StreamVersion.h"
#include "MeshBase.h"
#include "WellPointList.h"
#include "PressureBase.h"
#include "Temperature.h"
#include "ModifiedHint.h"
#include "ModelLifetimeFacade.h"
#include "CrossSection.h"
#include "ConsistencyGuard.h"

IWellModel::IWellModel(CAnalysisLogger& logger, const CVersionManager& versionManager)
: CModelBase(logger, versionManager),
  m_dRadius(0),
  m_StartPosDef(*this, true),
  m_EndPosDef(*this, false),
  m_modelProxy(0)
{
  setModelProxy(new CModelProxyConsole(*this));
}

IWellModel::IWellModel(const IWellModel& rhs)
: CModelBase(const_cast <CAnalysisLogger&> (rhs.Logger()), rhs.getVersionManager()),
  m_dRadius(rhs.m_dRadius),
  m_StartPosDef(rhs.m_StartPosDef),
  m_EndPosDef(rhs.m_EndPosDef),
  m_modelProxy(0)
{
  setModelProxy(new CModelProxyConsole(*this));
}

IWellModel::~IWellModel()
{
  if (m_modelProxy != 0)
  {
  delete m_modelProxy;
  m_modelProxy= 0;
  }
}

IWellModel& IWellModel::operator=(const IWellModel& rhs)
{
  if(!(*this == rhs))
  {
  m_dRadius = rhs.m_dRadius;
  m_StartPosDef = rhs.m_StartPosDef;
  m_EndPosDef   = rhs.m_EndPosDef;
  }

  return *this;
}

bool IWellModel::operator==(const IWellModel& rhs) const
{
  return (
  fabs(m_dRadius - rhs.m_dRadius) < EPS  &&
  m_StartPosDef == rhs.m_StartPosDef     &&
  m_EndPosDef   == rhs.m_EndPosDef     );
}

void IWellModel::createDefaults()
{
  CModelBase::createDefaults();

  if(!RootModel().Loading())
  CreateChildren();

  IWellModel* pParent = dynamic_cast<IWellModel*>(&ParentModel());
  if(pParent)
  {
  m_StartPosDef.LinkTo(pParent->StartPosition());
  m_EndPosDef.LinkTo(pParent->EndPosition());
  }

  Name(documentType());
}

void IWellModel::createContainers()
{
  CModelBase::createContainers();

  new CCrossSectionEntry(*this);
}

void IWellModel::CreateChildren()
{
  m_StartPosDef.SetDefault();
  m_EndPosDef.SetDefault();

  DetectFormations();
}

const double &IWellModel::Depth(const geo::ICoordinate &coord) const
{
  return coord.Z();
}

double IWellModel::Northing(const geo::ICoordinate& coord) const
{
  return coord.X();
}

double IWellModel::Easting(const geo::ICoordinate& coord) const
{
  return coord.Y();
}

const CModelBase& IWellModel::RootModel() const
{
  const CModelBase* pModel = this;
  while (pModel->parentModel())
  pModel = pModel->parentModel();

  return *pModel;
}

CModelBase& IWellModel::RootModel()
{
  return const_cast<CModelBase&>(((const IWellModel*)this)->RootModel());
}

bool IWellModel::CanCalculateHeatFlow() const
{
  return false;
}

bool IWellModel::CanCalculateMixture() const
{
  return false;
}

bool IWellModel::CanUseCalculatedTemperatures() const
{
  return false;
}

void IWellModel::OnNeighbourModified(const CGraphNode& node, enum ModifiedHint uHint)
{
  if(RootModel().Loading())
  return ;

  if((uHint == MeshCleared && &node == &ParentModel().Mesh()) ||
     (uHint == PD_POSCHANGED && (&node == &m_StartPosDef || &node == &m_EndPosDef)))
  {
  ClearFormations();
  Mesh().InvalidateMesh();
  }

  CModelBase::OnNeighbourModified(node, uHint);
}

bool IWellModel::OnLoad(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &prog)
{
  stream >> m_dRadius;

  if(version < CStreamVersion(4, 1, 0))
  LoadZoominProperties(stream, version, prog);

  CWellFormationEntry& formation_entry = static_cast<CWellFormationEntry&>(*GraphEntry(MD_WELLMODEL_FORMATION));
  assert(formation_entry.EntryNodes().empty());
  formation_entry.LoadStream(*this, stream, version, prog);

  // load formation map
  TFormationBaseEntry& parent_formation_entry = static_cast<TFormationBaseEntry&>(*ParentModel().GraphEntry(MD_BASE_FORMATION));
  int nSize;
  stream >> nSize;
  int i;
  for(i = 0; i < nSize; ++i)
  {
  int i1, i2;
  stream >> i1 >> i2;
  const CFormationBase* pParentForm = parent_formation_entry.FindIndex(i1);
  assert(pParentForm);
  CWellFormation* pForm = formation_entry.FindIndex(i2);
  assert(pForm);
  m_mpFormations.insert(TFormationMap::value_type(pParentForm, pForm));
  }

  m_StartPosDef.LoadStream(stream, version, prog);
  m_EndPosDef.LoadStream(stream, version, prog);

  if (version >= CStreamVersion(4, 1, 59))
  {
  CCrossSectionEntry *pXSecEntry = (CCrossSectionEntry*)GraphEntry(MD_BASE_XSECTION);
  pXSecEntry->LoadStream(*this, stream, version, prog);
  }

  return true;
}

bool IWellModel::OnSave(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &progress)
{
  stream << m_dRadius;

  // Save formations
  CWellFormationEntry& formation_entry = dynamic_cast<CWellFormationEntry&>(*GraphEntry(MD_WELLMODEL_FORMATION));
  formation_entry.SaveStream(stream, progress);

  // save formation map
  stream << int(m_mpFormations.size());
  TFormationMap::iterator it;
  for(it = m_mpFormations.begin(); it != m_mpFormations.end(); ++it)
  stream << it->first->Index() << it->second->Index();

  m_StartPosDef.SaveStream(stream, progress);
  m_EndPosDef.SaveStream(stream, progress);

  CCrossSectionEntry *pXSecEntry = (CCrossSectionEntry*)GraphEntry(MD_BASE_XSECTION);
  pXSecEntry->SaveStream(stream, progress);

  return true;
}

double IWellModel::Radius() const
{
  return m_dRadius;
}

void IWellModel::Radius(double dRadius)
{
  m_dRadius = dRadius;
  //m_pMesh->InvalidateMesh();
  Modified(GEOMETRY_CHANGED);
}

void IWellModel::DestroyZoomInModel()
{
  CConsistencyGuard *guard = GetConsistencyGuard();
  assert(guard != 0);
  guard->IsDeleting(true);
  assert(m_modelProxy != 0);
  m_modelProxy->onCloseModel();
  closeDocument();
  guard->IsDeleting(false);
}

void IWellModel::OnCloseModel()
{
  VERIFY(DeleteEntry(MD_BASE_XSECTION));

  CModelBase::OnCloseModel();
}

void IWellModel::CloseModel()
{
  // This method is called by the framework to close the active model in the document when
  // the application is closed. In this case, however, the main model is supposed to be closed,
  // not this zoom-in model directly.

  assert(m_modelProxy != 0);
  m_modelProxy->switchToParent();
  IModelLifetimeFacade::Close(&ParentModel());

  // don't call base class version ('this' is deleted by the CloseModel call above) !
}

bool IWellModel::Empty() const
{
  return false;
}

CWellFormation* IWellModel::AssociatedFormation(const CFormationBase& formation)
{
  TFormationMap::iterator it = m_mpFormations.find(&formation);
  if(it != m_mpFormations.end())
  return it->second;

  return 0;
}

const CWellFormation* IWellModel::AssociatedFormation(const CFormationBase& formation) const
{
  TFormationMap::const_iterator it = m_mpFormations.find(&formation);
  if(it != m_mpFormations.end())
  return it->second;

  return 0;
}

const IWellModel::CPositionDef& IWellModel::StartPosition() const
{
  return m_StartPosDef;
}

IWellModel::CPositionDef& IWellModel::StartPosition()
{
  return m_StartPosDef;
}

const IWellModel::CPositionDef& IWellModel::EndPosition() const
{
  return m_EndPosDef;
}

IWellModel::CPositionDef& IWellModel::EndPosition()
{
  return m_EndPosDef;
}

double IWellModel::FirstValidAHD() const
{
  const IWellModel* pParent = dynamic_cast<const IWellModel*>(&ParentModel());
  if(pParent)
  return pParent->StartPosition().AHD();

  if ( WellPath() )
  {
  const well::CWellPointList& lstFormationPoints =
    const_cast<CWellPath *>(WellPath())->FormIntersecPoints();
  well::CWellPoint* pFirst = lstFormationPoints.First();
  assert(pFirst != 0);
  return pFirst->TMD();
  }
  else // wjrx mantis 3401
  {
  const std::list<CNewGeoWellPoint>& lstFormationPoints =
      const_cast<CNewWellPath *>(NewWellPath())->FormIntersecPoints();
  assert( lstFormationPoints.size() >= 2 );
  return lstFormationPoints.front().TMD();
  }
}

double IWellModel::LastValidAHD() const
{
  const IWellModel* pParent = dynamic_cast<const IWellModel*>(&ParentModel());
  if(pParent)
  return pParent->EndPosition().AHD();

  if ( WellPath() )
  {
  const well::CWellPointList& lstFormationPoints =
      WellPath()->FormIntersecPoints();
  well::CWellPoint* pLast = lstFormationPoints.Last();
  assert(pLast != 0);
  return pLast->TMD();
  }
  else // wjrx mantis 3401
  {
  const std::list<CNewGeoWellPoint>& lstFormationPoints =
      const_cast<CNewWellPath *>(NewWellPath())->FormIntersecPoints();
  assert( lstFormationPoints.size() >= 2 );
  return lstFormationPoints.back().TMD();
  }
}

const CFormationBase* IWellModel::FormationContaining(double dAHD) const
{
  std::vector<const CFormationBase*> vcFormations = ValidFormations();

  if ( WellPath() )
  {
  const well::CWellSectionList& sectionlist = WellPath()->FormationSections();
  well::CWellSectionList::Iterator it = sectionlist.begin();
  while(it != sectionlist.end())
  {
      const well::IWellSection* pSection = *it;
      if(pSection->Contains(dAHD, true))
      {
    assert(dynamic_cast<const CFormationSection*>(pSection));
    const CFormationSection* pFormSection = static_cast<const CFormationSection*>(pSection);
    if(std::find(vcFormations.begin(), vcFormations.end(), pFormSection->Formation()) != vcFormations.end())
    {
          const CFormationBase* pForm = pFormSection->Formation();
          const IWellModel* pParent = dynamic_cast<const IWellModel*>(&ParentModel());
          if(pParent && pForm)
      return pParent->AssociatedFormation(*pForm);
          return pForm;
    }
      }
      ++it;
  }
  }
  else // wjrx mantis 3401
  {
  const std::list<CNewFormationSection>& sectionList= 
      const_cast<CNewWellPath *>(NewWellPath())->FormationSections();

  std::list<CNewFormationSection>::const_iterator it= sectionList.begin();
  while(it != sectionList.end())
  {
      const CNewFormationSection* pSection = &(*it);
      if(pSection->Contains(dAHD, true))
      {
    if(std::find(vcFormations.begin(), vcFormations.end(), pSection->Formation()) != vcFormations.end())
    {
          const CFormationBase* pForm = pSection->Formation();
          const IWellModel* pParent = dynamic_cast<const IWellModel*>(&ParentModel());
          if(pParent && pForm)
      return pParent->AssociatedFormation(*pForm);
          return pForm;
    }
      }
      ++it;
  }
  }

  return 0;
}

const CFormationSection* IWellModel::FormationSection(const CFormationBase& formation) const
{
  assert( WellPath() != 0 );
  assert( NewWellPath() == 0 );
  if ( WellPath() == 0 ) return 0;

  const well::CWellSectionList& sectionlist = WellPath()->FormationSections();
  well::CWellSectionList::Iterator it = sectionlist.begin();
  while(it != sectionlist.end())
  {
  const well::IWellSection* pSection = *it;
  assert(dynamic_cast<const CFormationSection*>(pSection));
  const CFormationSection* pFormSection = static_cast<const CFormationSection*>(pSection);
  if(pFormSection->Formation() == &formation)
      return pFormSection;
  ++it;
  }

  return 0;
}

// wjrx mantis 3401
const CNewFormationSection* IWellModel::NewFormationSection
(const CFormationBase& formation) const
{
  assert( WellPath() == 0 );
  assert( NewWellPath() != 0 );
  if ( NewWellPath() == 0 ) return 0;

  const std::list<CNewFormationSection>& sectionList= 
  const_cast<CNewWellPath *>(NewWellPath())->FormationSections();

  std::list<CNewFormationSection>::const_iterator it= sectionList.begin();
  while(it != sectionList.end())
  {
  if ( (*it).Formation() == &formation ) return &(*it);
  ++it;
  }

  return 0;
}

geo::CValue IWellModel::FormationEntryAHD(const CFormationBase& formation) const
{
  if ( WellPath() )
  {
  const CFormationSection* pFormSection = FormationSection(formation);
  if(pFormSection)
      return geo::CValue(pFormSection->Top().TMD());
  }
  else // wjrx mantis 3401
  {
  const CNewFormationSection* pFormSection = NewFormationSection(formation);
  if(pFormSection)
      return geo::CValue(pFormSection->Top().TMD());
  }

  return geo::CValue(); // invalid formation (does not cross)
}

geo::CValue IWellModel::FormationExitAHD(const CFormationBase& formation) const
{
  if ( WellPath() )
  {
  const CFormationSection* pFormSection = FormationSection(formation);
  if(pFormSection)
      return geo::CValue(pFormSection->Bottom().TMD());
  }
  else // wjrx mantis 3401
  {
  const CNewFormationSection* pFormSection = NewFormationSection(formation);
  if(pFormSection)
      return geo::CValue(pFormSection->Bottom().TMD());
  }

  return geo::CValue(); // invalid formation (does not cross)
}

std::vector<const CFormationBase*> IWellModel::ValidFormations() const
{
  std::vector<const CFormationBase*> vcRet;
  std::set<const CFormationBase*> stProcessed;

  geo::CValue valStartAHD;
  geo::CValue valEndAHD;

  const IWellModel* pParentModel = dynamic_cast<const IWellModel*>(&ParentModel());
  if(pParentModel)
  {
  valStartAHD = pParentModel->StartPosition().AHD();
  valEndAHD = pParentModel->EndPosition().AHD();
  }

  if ( WellPath() )
  {
  const well::CWellSectionList& sectionlist = WellPath()->FormationSections();
  well::CWellSectionList::Iterator it = sectionlist.begin();
  while(it != sectionlist.end())
  {
      const well::IWellSection* pSection = *it;
      assert(dynamic_cast<const CFormationSection*>(pSection));
      const CFormationSection* pFormSection = static_cast<const CFormationSection*>(pSection);

      const CFormationBase* pFormation = pFormSection->Formation();
      if(pFormation && stProcessed.insert(pFormation).second)
      {
    bool bInParentModel = true;
    if(valStartAHD.Valid() && valEndAHD.Valid())
    {
          double dSectionStartAHD = pFormSection->Top().TMD();
          double dSectionEndAHD = pFormSection->Bottom().TMD();
          bInParentModel = (!(dSectionEndAHD < valStartAHD.Value() + EPS) && !(dSectionStartAHD > valEndAHD.Value() - EPS));
    }

    if(bInParentModel)
          vcRet.push_back(pFormation);
      }

      ++it;
  }
  }
  else
  {
  const std::list<CNewFormationSection>& sectionList= 
      const_cast<CNewWellPath *>(NewWellPath())->FormationSections();

  std::list<CNewFormationSection>::const_iterator it= sectionList.begin();
  while(it != sectionList.end())
  {
      const CNewFormationSection* pFormSection = &(*it);

      const CFormationBase* pFormation = pFormSection->Formation();
      if(pFormation && stProcessed.insert(pFormation).second)
      {
    bool bInParentModel = true;
    if(valStartAHD.Valid() && valEndAHD.Valid())
    {
          double dSectionStartAHD = pFormSection->Top().TMD();
          double dSectionEndAHD = pFormSection->Bottom().TMD();
          bInParentModel = (!(dSectionEndAHD < valStartAHD.Value() + EPS) && !(dSectionStartAHD > valEndAHD.Value() - EPS));
    }

    if(bInParentModel)
          vcRet.push_back(pFormation);
      }

      ++it;
  }
  }

  return vcRet;
}

bool IWellModel::HasDistributedBoundaryLoads() const
{
  // see if there are any distributed values in the support nodes
  const CBoundary& boundary = static_cast<const CBoundary&>(Boundary());
  const CBoundary::CDisplacementSupport& displasup = boundary.DisplacementSupport();
  const CBoundary::CPressureSupport& pressusup = boundary.PressureSupport();

  bool bDistributed = false;
  CDepletionStageEntry::const_iterator it;
  for(it = DepletionStageEntry().begin(); it != DepletionStageEntry().end(); ++it)
  {
  if(displasup.Distributed(*it) || pressusup.Distributed(*it))
  {
      bDistributed = true;
      break;
  }
  }

  return bDistributed;
}

IWellModel::ModelType IWellModel::modelType() const
{
  return eWellModel;
}

void IWellModel::setModelProxy(CModelProxy* modelProxy)
{
  /*
   * It could be possible to initialize m_closeModel while it still contains
   * a valid pointer. Since CFemAppModel takes ownership it has the duty to
   * destruct the already existing CCloseModel object.
   */

  if (m_modelProxy != 0)
  {
  delete m_modelProxy;
  }

  m_modelProxy = modelProxy;
}

void IWellModel::CreateBoundary()
{
  m_pBoundary = new CBoundary(*this);
}

void IWellModel::SwitchTo(CFemAppModel* currentModel)
{
  assert(currentModel == this);

  if(m_mpFormations.empty())
  DetectFormations();

  CModelBase::SwitchTo(currentModel);
}

void IWellModel::DetectFormations()
{
  ClearFormations();

  if ( WellPath() )
  {
  const well::CWellSectionList& lstFormationSections = WellPath()->FormationSections();
  well::CWellSectionList::Iterator its = lstFormationSections.begin();
  while(its != lstFormationSections.end())
  {
      const well::IWellSection* pSection = *its;
      const CFormationSection* pFormSection = static_cast<const CFormationSection*>(pSection);
      const CFormationBase* pForm = pFormSection->Formation();
      const IWellModel* pParent = dynamic_cast<const IWellModel*>(&ParentModel());
      if(pParent && pForm)
    pForm = pParent->AssociatedFormation(*pForm);

      assert(pForm);
      TFormationMap::iterator it = m_mpFormations.find(pForm);
      if(it == m_mpFormations.end())
    m_mpFormations.insert(TFormationMap::value_type(pForm, new CWellFormation(*pForm, MD_WELLMODEL_FORMATION, *this)));

      ++its;
  }
  }
  else // wjrx mantis 3401
  {
  const std::list<CNewFormationSection>& sectionList= 
      NewWellPath()->FormationSections();
  std::list<CNewFormationSection>::const_iterator its= sectionList.begin();
  while( its != sectionList.end() )
  {
      const CNewFormationSection* pFormSection = &(*its);
      const CFormationBase* pForm = pFormSection->Formation();
      const IWellModel* pParent = dynamic_cast<const IWellModel*>(&ParentModel());
      if(pParent && pForm)
    pForm = pParent->AssociatedFormation(*pForm);

      assert(pForm);
      TFormationMap::iterator itf = m_mpFormations.find(pForm);
      if(itf == m_mpFormations.end())
    m_mpFormations.insert(TFormationMap::value_type(pForm, new CWellFormation(*pForm, MD_WELLMODEL_FORMATION, *this)));

      ++its;
  }
  }
}

void IWellModel::ClearFormations()
{
  TFormationMap::iterator it;
  for(it = m_mpFormations.begin(); it != m_mpFormations.end(); ++it)
  delete it->second;

  m_mpFormations.clear();
}

void IWellModel::addFormation(const CFormationBase* formation)
{
  m_mpFormations.insert(TFormationMap::value_type(formation,
  new CWellFormation(*formation, MD_WELLMODEL_FORMATION, *this)));
}

/////

IWellModel::CPositionDef::CPositionDef(IWellModel& model, bool bStart)
: CGraphNode(""),
  m_model(model),
  m_nMethod(PDM_DEFAULT),
  m_dAHD(0),
  m_pFormation(0),
  m_bStart(bStart)
{
//  SetDefault();
  LinkTo(model);
}

IWellModel::CPositionDef::CPositionDef(const CPositionDef& rhs)
: CGraphNode(""),
  m_model(rhs.m_model),
  m_nMethod(rhs.m_nMethod),
  m_dAHD(rhs.m_dAHD),
  m_pFormation(rhs.m_pFormation),
  m_bStart(rhs.m_bStart)
{
}

IWellModel::CPositionDef& IWellModel::CPositionDef::operator=(const CPositionDef& rhs)
{
  assert(&m_model == &rhs.m_model);

  if(!operator==(rhs))
  {
  m_nMethod    = rhs.m_nMethod;
  m_dAHD       = rhs.m_dAHD;
  m_pFormation = rhs.m_pFormation;
  m_bStart     = rhs.m_bStart;

  Modified(PD_POSCHANGED);
  }

  return *this;
}

bool IWellModel::CPositionDef::operator==(const CPositionDef& rhs) const
{
  return (
  &m_model     == &rhs.m_model       &&
  m_nMethod    == rhs.m_nMethod      &&
  m_dAHD       == rhs.m_dAHD         &&
  m_bStart     == rhs.m_bStart       &&
  m_pFormation == rhs.m_pFormation);
}

void IWellModel::CPositionDef::OnNeighbourModified(const CGraphNode& node, enum ModifiedHint uHint)
{
  if(uHint == PD_POSCHANGED)
  {
  const CPositionDef* pDef = dynamic_cast<const CPositionDef*>(&node);
  if(pDef && &pDef->m_model == &m_model.ParentModel())
  {
      if(m_bStart)
      {
    if(pDef->AHD() > AHD())
    {
          SetDefault();
          if(m_model.EndPosition().AHD() <= AHD())
      m_model.EndPosition().SetDefault();
    }
      }
      else
      {
    if(pDef->AHD() < AHD())
    {
          SetDefault();
          if(m_model.StartPosition().AHD() >= AHD())
      m_model.StartPosition().SetDefault();
    }
      }
  }
  }
}

IWellModel::CPositionDef::TMethod IWellModel::CPositionDef::Method() const
{
  return m_nMethod;
}

double IWellModel::CPositionDef::AHD() const
{
  return m_dAHD;
}

bool IWellModel::CPositionDef::InsideFormation() const
{
  return m_pFormation != 0;
}

const CFormationBase& IWellModel::CPositionDef::Formation() const
{
  assert(m_pFormation);
  return *m_pFormation;
}

void IWellModel::CPositionDef::SetDefault()
{
  m_nMethod = PDM_DEFAULT;

  if(m_bStart)
  m_dAHD = m_model.FirstValidAHD();
  else
  m_dAHD = m_model.LastValidAHD();

  m_pFormation = m_model.FormationContaining(m_dAHD);

  Modified(PD_POSCHANGED);
}

void IWellModel::CPositionDef::SetAHD(double dAHD)
{
  m_dAHD = dAHD;
  m_nMethod = PDM_AHD;

  m_pFormation = m_model.FormationContaining(dAHD);

  Modified(PD_POSCHANGED);
}

void IWellModel::CPositionDef::SetFormation(const CFormationBase& formation)
{
  m_pFormation = &formation;
  geo::CValue valAHD;
  if(m_bStart)
  valAHD = m_model.FormationEntryAHD(formation);
  else
  valAHD = m_model.FormationExitAHD(formation);
  assert(valAHD.Valid());
  m_dAHD = valAHD.Value();
  m_nMethod = PDM_FORMATION;

  Modified(PD_POSCHANGED);
}

void IWellModel::CPositionDef::LoadStream(CStorageNode::TSTREAM& stream, CStreamVersion& /*version*/, CStorageNode::TPROGRESS& /*progress*/)
{
  int nMethod;
  stream >> nMethod;
  m_nMethod = (TMethod)nMethod;
  stream >> m_dAHD;
  int nFormIndex;
  stream >> nFormIndex;
  if(nFormIndex >= 0)
  {
  TFormationBaseEntry& form_entry = (TFormationBaseEntry&)(*m_model.ParentModel().GraphEntry(MD_BASE_FORMATION));
  m_pFormation = form_entry.FindIndex(nFormIndex);
  assert(!m_model.parentModel()->Mesh().IsMesh() || m_pFormation);
  }
  else
  m_pFormation = 0;

  Modified(PD_POSCHANGED);
}

void IWellModel::CPositionDef::SaveStream(CStorageNode::TSTREAM& stream, CStorageNode::TPROGRESS& /*progress*/)
{
  stream << int(m_nMethod);
  stream << m_dAHD;
  stream << (m_pFormation ? m_pFormation->Index() : -1);
}


/////

IWellModel::CBoundary::CBoundary(CFemAppModel& model)
: CBoundaryBase(geo::CPoint::NullPoint, geo::CPoint::NullPoint, model),
  m_pBoundaryInterfaceDef(new CBoundaryInterfaceDef(*this))
{
  CreateChildren();
}

IWellModel::CBoundary::~CBoundary()
{
  delete m_pBoundaryInterfaceDef;
}

IWellModel::CBoundary::TMinMax IWellModel::CBoundary::SnapToGrid(const TMinMax& minmax) const
{
  return minmax;
}

const geo::IObject& IWellModel::CBoundary::DisplayList(int nIndex) const
{
  assert(nIndex == 0);
  return m_arBoundaryElements;
}

int IWellModel::CBoundary::DisplayListSize() const
{
  return 1;
}

void IWellModel::CBoundary::OnNeighbourModified(const CGraphNode& node, enum ModifiedHint uHint)
{
  const CModelBase& model = static_cast<const CModelBase&>(Model());
  if(&node == &model.Mesh() && (uHint == MeshCleared || uHint == MeshCreated))
  {
  if(uHint == MeshCleared)
      Clear();
  else if(uHint == MeshCreated)
      OnSet(TMinMax(model.Mesh().Min(), model.Mesh().Max()));

  Modified();
  }
}

void IWellModel::CBoundary::Clear()
{
  m_arBoundaryElements.Clear();
  m_mpFace2BoundarySurface.clear();
}

void IWellModel::CBoundary::AddBoundaryElement(geo::CInterfaceElement& iface, CBoundaryInterfaceDef::TBoundarySurface surfacedef)
{
  m_arBoundaryElements.PushBack(iface);
  VERIFY(m_mpFace2BoundarySurface.insert(TFace2BoundarySurfaceMap::value_type(&iface, surfacedef)).second);
}

const CBoundaryInterfaceDef &IWellModel::CBoundary::InterfaceDefinition() const
{
  return *m_pBoundaryInterfaceDef;
}

int IWellModel::CBoundary::BoundaryElementSize() const
{
  return m_arBoundaryElements.Size();
}

const geo::CInterfaceElement& IWellModel::CBoundary::BoundaryElement(int nIndex) const
{
  assert(nIndex >= 0 && nIndex < BoundaryElementSize());
  return m_arBoundaryElements.Object(nIndex);
}

CBoundaryInterfaceDef::TBoundarySurface IWellModel::CBoundary::BoundarySurfaceDef(const geo::CInterfaceElement& iface) const
{
  TFace2BoundarySurfaceMap::const_iterator it = m_mpFace2BoundarySurface.find(&iface);
  assert(it != m_mpFace2BoundarySurface.end());

  return it->second;
}

const CBoundaryInterfaceMaterial& IWellModel::CBoundary::InterfaceMaterial(const geo::CInterfaceElement& iface) const
{
  CBoundaryInterfaceDef::TBoundarySurface surfdef = BoundarySurfaceDef(iface);
  switch(surfdef)
  {
  case CBoundaryInterfaceDef::BSURF_TOP:
  return m_pBoundaryInterfaceDef->InterfaceMaterialTop(iface);
  case CBoundaryInterfaceDef::BSURF_BOTTOM:
  return m_pBoundaryInterfaceDef->InterfaceMaterialBottom(iface);
  case CBoundaryInterfaceDef::BSURF_SIDE:
  break;
  }

  assert(surfdef == CBoundaryInterfaceDef::BSURF_SIDE);
  return m_pBoundaryInterfaceDef->InterfaceMaterialSides(iface);
}

void IWellModel::CBoundary::CreateChildren()
{
  m_pDisplacementSupport = new CDisplacementSupport(*this);
  m_pPressureSupport = new CPressureSupport(*this);
}

void IWellModel::CBoundary::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
  if(version >= CStreamVersion(3, 7, 9))
  CBoundaryBase::LoadStream(stream, version, progress);

  m_pBoundaryInterfaceDef->LoadStream(stream, version, progress);

  if(version >= CStreamVersion(3, 7, 15))
  {
  m_pDisplacementSupport->LoadStream(stream, version, progress);
  m_pPressureSupport->LoadStream(stream, version, progress);
  }
}

void IWellModel::CBoundary::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
  CBoundaryBase::SaveStream(stream, progress);

  m_pBoundaryInterfaceDef->SaveStream(stream, progress);
  m_pDisplacementSupport->SaveStream(stream, progress);
  m_pPressureSupport->SaveStream(stream, progress);
}

long IWellModel::CBoundary::SavedItems() const
{
  return CBoundaryBase::SavedItems() + m_pDisplacementSupport->SavedItems() + m_pPressureSupport->SavedItems();
}

bool IWellModel::CBoundary::HasInterfaces() const
{
  return true;
}

int IWellModel::CBoundary::InterfaceNodeSize() const
{
  if(m_vcInterfaceNode.empty())
  {
  std::set<int> stNode;

  for(size_t i = 0; i < m_arBoundaryElements.Size(); ++i)
  {
      const geo::CInterfaceElement& iface = m_arBoundaryElements.Object(i);
      assert(iface.NrOfNodes() == 8);
      assert(iface.FrontFace() == iface.BackFace());
      int j;
      for(j = 4; j < 8; ++j)
    stNode.insert(iface.Node(j).Index());
  }

  m_vcInterfaceNode.resize(stNode.size());
  std::set<int>::iterator it;
  int j;
  const CModelBase& model = static_cast<const CModelBase&>(Model());
  for(j = 0, it = stNode.begin(); it != stNode.end(); ++it, ++j)
      m_vcInterfaceNode[j] = &model.Mesh().Mesh().Node(*it);
  }

  return m_vcInterfaceNode.size();
}

const geo::INode& IWellModel::CBoundary::InterfaceNode(int nIndex) const
{
  assert(nIndex >= 0 && nIndex < m_vcInterfaceNode.size());
  assert(m_vcInterfaceNode[nIndex]);
  return *m_vcInterfaceNode[nIndex];
}

const IWellModel::CBoundary::CDisplacementSupport& IWellModel::CBoundary::DisplacementSupport() const
{
  assert(m_pDisplacementSupport);
  return *m_pDisplacementSupport;
}

const IWellModel::CBoundary::CPressureSupport& IWellModel::CBoundary::PressureSupport() const
{
  assert(m_pPressureSupport);
  return *m_pPressureSupport;
}


/////

IWellModel::CBoundary::IWellSupportNode::IWellSupportNode(unsigned int uName, IWellModel::CBoundary& boundary)
: CBaseSupportNode(uName, boundary),
  m_boundary(boundary)
{
}

unsigned int IWellModel::CBoundary::IWellSupportNode::TypeId() const
{
  return 0;
}

bool IWellModel::CBoundary::IWellSupportNode::Empty() const
{
  return false;
}

int IWellModel::CBoundary::IWellSupportNode::DisplayListSize() const
{
  return m_boundary.DisplayListSize();
}

const geo::IObject& IWellModel::CBoundary::IWellSupportNode::DisplayList(int nIndex) const
{
  return m_boundary.DisplayList(nIndex);
}

IWellModel::CBoundary::IWellSupportNode::TColor IWellModel::CBoundary::IWellSupportNode::Color() const
{
  return m_boundary.Color();
}


/////

IWellModel::CBoundary::CDisplacementSupport::CDisplacementSupport(IWellModel::CBoundary& boundary)
: IWellSupportNode(IDS_DISPLACEMENT_SUPPORT_NODE, boundary)
{
}

bool IWellModel::CBoundary::CDisplacementSupport::IsValidValueTypeId(unsigned int uValueType) const
{
  return (uValueType == IDT_VALUETYPE_DISPLACEMENT);
}

unsigned int IWellModel::CBoundary::CDisplacementSupport::IconId() const
{
  if (static_cast<const IWellModel&>(Model()).HasDistributedBoundaryLoads())
  return IDI_NODALSUPPORT;
  else
  return IDI_NODALSUPPORT_PARENT;
}


/////

IWellModel::CBoundary::CPressureSupport::CPressureSupport(IWellModel::CBoundary& boundary)
: IWellSupportNode(IDS_PRESSURE_SUPPORT_NODE, boundary)
{
}

bool IWellModel::CBoundary::CPressureSupport::IsValidValueTypeId(unsigned int uValueType) const
{
  return (uValueType == IDT_VALUETYPE_STRESS_TENSOR);
}

unsigned int IWellModel::CBoundary::CPressureSupport::IconId() const
{
  if (static_cast<const IWellModel&>(Model()).HasDistributedBoundaryLoads())
  return IDI_FACESUPPORT;
  else
  return IDI_FACESUPPORT_PARENT;
}
