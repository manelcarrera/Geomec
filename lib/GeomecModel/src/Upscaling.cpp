
#include <memory>

#include "Upscaling.h"
#include "ThinLayerUpscaling.h"
#include "CamClayUpscaling.h"
#include "3dmodel.h"
#include "BaseEntryTypes.h"
#include "HexaModel.h"
#include "GeoSurface.h"
#include "HexaMesh.h"
#include "resourceIDI.h"
#include "resourceIDS.h"
#include "MeshBase.h"
#include "Global.h"
#include "IProgressFactory.h"
#include "ValueTypeFactory.h"
#include "NodalValueSet.h"

IUpscalingNodeBase::IUpscalingNodeBase(CFemAppModel& model)
: CStorageNode(model)
{
}

IUpscalingNodeBase::IUpscalingNodeBase(const QString& strName, C3DModel& model)
: CStorageNode(strName, model)
{
  m_pFormationsBranch     = new CUpscalingFormationsBranch(*this);
  m_pPointsetRangesBranch = new CUpscalingPointsetRangesBranch(*this);
  m_pTargetPointsetBranch = new CUpscalingTargetPointsetBranch(*this);
}

IUpscalingNodeBase::IUpscalingNodeBase(const unsigned int uName, C3DModel& model)
: CStorageNode(uName, model)
{
}

IUpscalingNodeBase::IUpscalingNodeBase(const IUpscalingNodeBase &rhs)
: CStorageNode(rhs)
{
}

bool IUpscalingNodeBase::Empty() const
{
  return false;
}

long IUpscalingNodeBase::SavedItems() const
{
  return m_pFormationsBranch->SavedItems()     +
         m_pPointsetRangesBranch->SavedItems() +
         m_pTargetPointsetBranch->SavedItems() +
         CStorageNode::SavedItems();
}

void IUpscalingNodeBase::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
  m_pFormationsBranch->SaveStream(stream, progress);
  m_pPointsetRangesBranch->SaveStream(stream, progress);
  m_pTargetPointsetBranch->SaveStream(stream, progress);

  CStorageNode::SaveStream(stream, progress);
}

void IUpscalingNodeBase::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
  m_pFormationsBranch     = new CUpscalingFormationsBranch(Model());
  m_pPointsetRangesBranch = new CUpscalingPointsetRangesBranch(Model());
  m_pTargetPointsetBranch = new CUpscalingTargetPointsetBranch(Model());

  m_pFormationsBranch->LoadStream(stream, version, progress);
  m_pPointsetRangesBranch->LoadStream(stream, version, progress);
  m_pTargetPointsetBranch->LoadStream(stream, version, progress);

  CStorageNode::LoadStream(stream, version, progress);

  m_pFormationsBranch->reParent(this);
  m_pPointsetRangesBranch->reParent(this);
  m_pTargetPointsetBranch->reParent(this);

  assert(Model().GraphEntry(MD_BASE_UPSCALING));
  reParent(Model().GraphEntry(MD_BASE_UPSCALING));
}

unsigned int IUpscalingNodeBase::IconId() const
{
  return IDI_UPSCALING;
}

unsigned int IUpscalingNodeBase::TypeId() const
{
  return 0;
}

const CUpscalingFormationsBranch& IUpscalingNodeBase::FormationsBranch() const
{
  return *m_pFormationsBranch;
}

CUpscalingFormationsBranch& IUpscalingNodeBase::FormationsBranch()
{
  return *m_pFormationsBranch;
}

const CUpscalingPointsetRangesBranch &IUpscalingNodeBase::PointsetRangesBranch() const
{
  return *m_pPointsetRangesBranch;
}

CUpscalingPointsetRangesBranch &IUpscalingNodeBase::PointsetRangesBranch()
{
  return *m_pPointsetRangesBranch;
}

const CUpscalingTargetPointsetBranch &IUpscalingNodeBase::TargetPointsetBranch() const
{
  return *m_pTargetPointsetBranch;
}

CUpscalingTargetPointsetBranch &IUpscalingNodeBase::TargetPointsetBranch()
{
  return *m_pTargetPointsetBranch;
}

// protected

const QString IUpscalingNodeBase::DEFAULT_2D_POINT_SET_NAME =
  QObject::tr("%1 2D Pointset");
const QString IUpscalingNodeBase::DEFAULT_2D_POINT_SET_NUMBER =
  QObject::tr(" %1");
const QString IUpscalingNodeBase::DEFAULT_3D_POINT_SET_NAME =
  QObject::tr("%1 3D Pointset");
const QString IUpscalingNodeBase::DEFAULT_3D_POINT_SET_NUMBER =
  QObject::tr(" %1");
const size_t IUpscalingNodeBase::DEFAULT_INITIAL_NUMBER = 1;

/////

CUpscalingNodeFactory::CUpscalingNodeFactory(C3DModel& model)
: m_model(model)
{
}

IUpscalingNodeBase* CUpscalingNodeFactory::Create(int nType)
{
  IUpscalingNodeBase* p = 0;
  switch(nType)
  {
  case FACTORYTYPE_THINLAYERUPSCALING:
    p = new CThinLayerUpscalingNode(m_model);
    break;
  case FACTORYTYPE_CAMCLAYUPSCALING:
    p = new CCamClayUpscalingNode(m_model);
    break;
  default:
    assert(false);
  }

  return p;
}


/////

CUpscalingEntry::CUpscalingEntry(C3DModel& model)
: CGraphEntryTemp<IUpscalingNodeBase>(MD_BASE_UPSCALING, IDI_UPSCALING, IDS_UPSCALING, model)
{
}

long CUpscalingEntry::SavedItems() const
{
  long lRet = 0;

  const TNodeSet& stNodes = EntryNodes();
  for(TNodeSet::const_iterator it = stNodes.begin(); it != stNodes.end(); ++it)
    lRet += (*it)->SavedItems();

  return lRet;
}

void CUpscalingEntry::SaveStream(CStorageNode::TSTREAM& stream, CStorageNode::TPROGRESS& progress)
{
  const TNodeSet& stNodes = EntryNodes();
  stream << int(stNodes.size());
  for(TNodeSet::const_iterator it = stNodes.begin(); it != stNodes.end(); ++it)
  {
    stream << (*it)->FactoryType();
    (*it)->SaveStream(stream, progress);
  }
}

void CUpscalingEntry::LoadStream(CStorageNode::TSTREAM& stream, CStorageNode::TSTREAMVERSION& version, CStorageNode::TPROGRESS& progress)
{
  C3DModel& model = static_cast<C3DModel&>(Model());
  CUpscalingNodeFactory factory(model);

  int nNodes;
  stream >> nNodes;
  for(int i = 0; i < nNodes; ++i)
  {
    int nFactoryType;
    stream >> nFactoryType;
    IUpscalingNodeBase* pNode = factory.Create(nFactoryType);
    pNode->LoadStream(stream, version, progress);
  }
}

namespace
{

const QString DEFAULT_THIN_LAYER_UPSCALING_NAME =
  QObject::tr("New Thin Layer Upscaling %1");
const size_t DEFAULT_INITIAL_NUMBER = 1;

} // anonymous namespace

void CUpscalingEntry::CreateThinLayerUpscaling()
{
  QString strName = createUniqueName(DEFAULT_THIN_LAYER_UPSCALING_NAME,
    DEFAULT_INITIAL_NUMBER);
  new CThinLayerUpscalingNode(strName.toStdString().c_str(), static_cast<C3DModel&>(Model()));
}

namespace
{

const QString DEFAULT_CAMCLAY_UPSCALING_NAME =
  QObject::tr("New CamClay Upscaling %1");

} // anonymous namespace

void CUpscalingEntry::CreateCamClayUpscaling()
{
  QString strName = createUniqueName(DEFAULT_CAMCLAY_UPSCALING_NAME,
    DEFAULT_INITIAL_NUMBER);
  new CCamClayUpscalingNode(strName.toStdString().c_str(), static_cast<C3DModel&>(Model()));
}


/////

static const char FORMATIONS_BRANCH_NAME[] = "Target Formations";

///// CUpscalingFormationsBranch
CUpscalingFormationsBranch::CUpscalingFormationsBranch(CFemAppModel& model)
: COpenGLNode(model),
  m_formationFilter(0)
{
}

CUpscalingFormationsBranch::CUpscalingFormationsBranch(IUpscalingNodeBase& parent)
: COpenGLNode(FORMATIONS_BRANCH_NAME, parent.Model()),
  m_formationFilter(0)
{
  reParent(&parent);
}

CUpscalingFormationsBranch::CUpscalingFormationsBranch(const CUpscalingFormationsBranch &rhs)
: COpenGLNode(rhs),
  m_formationFilter(0)
{
}

bool CUpscalingFormationsBranch::HasFormations() const
{
  return !m_stFormations.empty();
}

bool CUpscalingFormationsBranch::ConnectItem(const CGraphNode &item)
{
  assert(CanConnectItem(item));
  LinkTo(const_cast<CGraphNode&>(item));

  return true;
}

bool CUpscalingFormationsBranch::CanConnectItem(const CGraphNode &item) const
{
  const CFormationBase* pForm = dynamic_cast<const CFormationBase*>(&item);
  const CDepletionStageEntry& depletionStageEntry =
    static_cast <const CModelBase&> (Model()).DepletionStageEntry();
  bool fractureApertureModel = false;

  for (CDepletionStageEntry::const_iterator
    depletionStage = depletionStageEntry.begin(); (pForm != 0) &&
    (depletionStage != depletionStageEntry.end()) && !fractureApertureModel;
    ++depletionStage)
  {
    const CMaterialServer& materialServer = pForm->Material(*depletionStage);
    const IMaterialRock* libraryMaterial = materialServer.LibraryMaterial();
    int materialModel = (libraryMaterial == 0 ? MM_LINEAR :
      materialServer.MaterialModel());

    fractureApertureModel = fractureApertureModel ||
      (materialModel == MM_FRACTURE_APERTURE) ||
      (materialModel == MM_FRACTURE_APERTURE2);
  }

  return !fractureApertureModel && (pForm != 0) &&
    (m_stFormations.find(pForm) == m_stFormations.end());
}

bool CUpscalingFormationsBranch::Empty() const
{
  return false;
}

long CUpscalingFormationsBranch::SavedItems() const
{
  return 1 + COpenGLNode::SavedItems();
}

void CUpscalingFormationsBranch::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
  stream << int(m_stFormations.size());
  for(TFormationsSet::const_iterator it = m_stFormations.begin(); it != m_stFormations.end(); ++it)
    stream << (*it)->Index();

  progress.Step();

  COpenGLNode::SaveStream(stream, progress);
}

void CUpscalingFormationsBranch::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
  int nFormations;
  stream >> nFormations;
  for(int i = 0; i < nFormations; ++i)
  {
    int idx;
    stream >> idx;
    CFormationBase* pForm = static_cast<TFormationBaseEntry*>(Model().GraphEntry(MD_BASE_FORMATION))->FindIndex(idx);
    LinkTo(*pForm);
  }

  progress.Step();

  COpenGLNode::LoadStream(stream, version, progress);

  Name(FORMATIONS_BRANCH_NAME);
}

unsigned int CUpscalingFormationsBranch::IconId() const
{
  return IDI_FORMATIONS;
}

unsigned int CUpscalingFormationsBranch::TypeId() const
{
  return 0;
}

int CUpscalingFormationsBranch::DisplayListSize() const
{
  int sz = 0;
  TFormationsSet::const_iterator it;
  for(it = m_stFormations.begin(); it != m_stFormations.end(); ++it)
    sz += (*it)->DisplayListSize();

  return sz;
}

const geo::IObject& CUpscalingFormationsBranch::DisplayList(int nIndex) const
{
  const geo::IObject* pObject = 0;

  TFormationsSet::const_iterator it;
  for(it = m_stFormations.begin(); it != m_stFormations.end(); ++it)
  {
    int sz = (*it)->DisplayListSize();
    if(nIndex < sz)
    {
      pObject = &(*it)->DisplayList(nIndex);
      break;
    }

    nIndex -= sz;
  }

  assert(pObject);
  return *pObject;
}

COpenGLNode::CDrawDef* CUpscalingFormationsBranch::OnCreateDrawDef(const geo::IObject &object) const
{
  TFormationsSet::const_iterator it;
  for(it = m_stFormations.begin(); it != m_stFormations.end(); ++it)
  {
    int sz = (*it)->DisplayListSize();
    int i;
    for(i = 0; i < sz; ++i)
    {
      if(&(*it)->DisplayList(i) == &object)
        return new CFormationDrawDef(**it);
    }
  }

  assert(false);
  return COpenGLNode::OnCreateDrawDef(object);
}

CUpscalingFormationsBranch::TColor CUpscalingFormationsBranch::Color() const
{
  assert(false);
  return TColor();
}

bool CUpscalingFormationsBranch::IsInRange(const geo::IBody& body) const
{
  if(m_stFormations.empty())
    return true;

  const CModelBase& model = static_cast<const CModelBase&>(Model());
  const CFormationBase& formation = *model.Mesh().Formation(body);

  return (m_stFormations.find(&formation) != m_stFormations.end());
}

void CUpscalingFormationsBranch::OnNewNeighbour(const CGraphNode &node)
{
  const CFormationBase* pForm = dynamic_cast<const CFormationBase*>(&node);
  if(pForm)
  {
    VERIFY(m_stFormations.insert(pForm).second);
    Modified();
  }
}

void CUpscalingFormationsBranch::OnNeighbourDeleted(const CGraphNode &node)
{
  TFormationsSet::iterator it = m_stFormations.find((CFormationBase*)(&node));
  if(it != m_stFormations.end())
  {
    m_stFormations.erase(it);
    Modified();
  }
}

void CUpscalingFormationsBranch::OnNeighbourModified(const CGraphNode& node, enum ModifiedHint /*uHint*/)
{
  const CFormationBase* pForm = dynamic_cast<const CFormationBase*>(&node);
  if(pForm && m_stFormations.find(pForm) != m_stFormations.end())
    Modified();
}


///// CUpscalingFormationsBranch::CFormationDrawDef

CUpscalingFormationsBranch::CFormationDrawDef::CFormationDrawDef(const CFormationBase& formation)
: COpenGLNode::CDrawDef(formation)
{
}

COpenGLNode::CDrawDef::DrawDecisionVector CUpscalingFormationsBranch::CFormationDrawDef::Color(const geo::IObject &/*object*/) const
{
  assert(dynamic_cast<const CFormationBase*>(&OpenGLNode()));
  const CFormationBase& formation = static_cast<const CFormationBase&>(OpenGLNode());
  return std::make_pair(true, std::vector<TColor>(1, formation.Color()));
}


///// IUpscalingPointsetBranchBase
IUpscalingPointsetBranchBase::IUpscalingPointsetBranchBase(CFemAppModel& model)
: COpenGLNode(model)
{
}

IUpscalingPointsetBranchBase::IUpscalingPointsetBranchBase(const QString& strName, IUpscalingNodeBase& parent)
: COpenGLNode(strName, parent.Model())
{
  reParent(&parent);
}

IUpscalingPointsetBranchBase::IUpscalingPointsetBranchBase(const IUpscalingPointsetBranchBase& rhs)
: COpenGLNode(rhs)
{
}

unsigned int IUpscalingPointsetBranchBase::TypeId() const
{
  return 0;
}

int IUpscalingPointsetBranchBase::DisplayListSize() const
{
  return 0;
}

const geo::IObject& IUpscalingPointsetBranchBase::DisplayList(int /*nIndex*/) const
{
  assert(false);
  const geo::IObject* pBogus = 0;
  return *pBogus;
}

IUpscalingPointsetBranchBase::TColor IUpscalingPointsetBranchBase::Color() const
{
  assert(false);
  return TColor();
}


static const char POINTSET_RANGES_BRANCH_NAME[] = "Target Volume From Pointsets";

///// CUpscalingPointsetRangesBranch
CUpscalingPointsetRangesBranch::CUpscalingPointsetRangesBranch(CFemAppModel& model)
: IUpscalingPointsetBranchBase(model),
  m_bAllPointsets(false)
{
}

CUpscalingPointsetRangesBranch::CUpscalingPointsetRangesBranch(IUpscalingNodeBase& parent)
: IUpscalingPointsetBranchBase(POINTSET_RANGES_BRANCH_NAME, parent),
  m_bAllPointsets(false)
{
}

CUpscalingPointsetRangesBranch::CUpscalingPointsetRangesBranch(const CUpscalingPointsetRangesBranch& rhs)
: IUpscalingPointsetBranchBase(rhs),
  m_bAllPointsets(rhs.m_bAllPointsets)
{
}

bool CUpscalingPointsetRangesBranch::Empty() const
{
  return false;
}

long CUpscalingPointsetRangesBranch::SavedItems() const
{
  return 1 + IUpscalingPointsetBranchBase::SavedItems();
}

void CUpscalingPointsetRangesBranch::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
  stream << int(m_stPointsets.size());
  for(TPointsetSet::const_iterator it = m_stPointsets.begin(); it != m_stPointsets.end(); ++it)
    stream << (*it)->Index();

  stream << int(m_bAllPointsets ? 1 : 0);

  progress.Step();

  IUpscalingPointsetBranchBase::SaveStream(stream, progress);
}

void CUpscalingPointsetRangesBranch::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
  int nPointsets;
  stream >> nPointsets;
  for(int i = 0; i < nPointsets; ++i)
  {
    int idx;
    stream >> idx;
    IPointSet* pPointset = static_cast<TPointSetEntry*>(Model().GraphEntry(MD_BASE_POINTSET))->FindIndex(idx);
    LinkTo(*pPointset);
  }

  int nAllPointsets;
  stream >> nAllPointsets;
  m_bAllPointsets = (nAllPointsets != 0);

  progress.Step();

  IUpscalingPointsetBranchBase::LoadStream(stream, version, progress);

  Name(POINTSET_RANGES_BRANCH_NAME);
}

unsigned int CUpscalingPointsetRangesBranch::IconId() const
{
  return IDI_POINTSETS;
}

bool CUpscalingPointsetRangesBranch::CanConnectItem(const CGraphNode& node) const
{
  return (dynamic_cast<const CPointSet*>(&node) != 0);
}

int CUpscalingPointsetRangesBranch::DisplayListSize() const
{
  int sz = 0;
  TPointsetSet::const_iterator it;
  for(it = m_stPointsets.begin(); it != m_stPointsets.end(); ++it)
    sz += (*it)->DisplayListSize();

  return sz;
}

const geo::IObject& CUpscalingPointsetRangesBranch::DisplayList(int nIndex) const
{
  const geo::IObject* pObject = 0;

  TPointsetSet::const_iterator it;
  for(it = m_stPointsets.begin(); it != m_stPointsets.end(); ++it)
  {
    int sz = (*it)->DisplayListSize();
    if(nIndex < sz)
    {
      pObject = &(*it)->DisplayList(nIndex);
      break;
    }

    nIndex -= sz;
  }

  assert(pObject);
  return *pObject;
}

COpenGLNode::CDrawDef* CUpscalingPointsetRangesBranch::OnCreateDrawDef(const geo::IObject& object) const
{
  TPointsetSet::const_iterator it;
  for(it = m_stPointsets.begin(); it != m_stPointsets.end(); ++it)
  {
    int i;
    int sz = (*it)->DisplayListSize();
    for(i = 0; i < sz; ++i)
    {
      if(&(*it)->DisplayList(i) == &object)
        return new CPointsetDrawDef(**it);
    }
  }

  assert(false);
  return COpenGLNode::OnCreateDrawDef(object);
}

bool CUpscalingPointsetRangesBranch::UseAllPointsets() const
{
  return m_bAllPointsets;
}

void CUpscalingPointsetRangesBranch::UseAllPointsets(bool b)
{
  m_bAllPointsets = b;
}

bool CUpscalingPointsetRangesBranch::IsInRange(const geo::IPoint& point) const
{
  if(m_stPointsets.empty())
    return true;

  // check whether the body's mid point is inside any of the pointsets' convex hulls
  CUpscalingPointsetRangesBranch::TPointsetSet::const_iterator it;
  for(it = m_stPointsets.begin(); it != m_stPointsets.end(); ++it)
  {
    bool bInside = (*it)->PointInConvexHull(point);
    if(bInside && !m_bAllPointsets)
      return true;
    else if(!bInside && m_bAllPointsets)
      return false;
  }

  return m_bAllPointsets;
}

void CUpscalingPointsetRangesBranch::OnNewNeighbour(const CGraphNode &node)
{
  const CPointSet* pPointSet = dynamic_cast<const CPointSet*>(&node);
  if(pPointSet)
  {
    VERIFY(m_stPointsets.insert(pPointSet).second);
    Modified();
  }
}

void CUpscalingPointsetRangesBranch::OnNeighbourDeleted(const CGraphNode &node)
{
  const CPointSet* pPointSet = dynamic_cast<const CPointSet*>(&node);
  if(pPointSet)
  {
    TPointsetSet::iterator it = m_stPointsets.find(pPointSet);
    assert(it != m_stPointsets.end());
    m_stPointsets.erase(it);
    Modified();
  }
}

void CUpscalingPointsetRangesBranch::OnNeighbourModified(const CGraphNode& node, enum ModifiedHint /*uHint*/)
{
  const CPointSet* pPointSet = dynamic_cast<const CPointSet*>(&node);
  if(pPointSet && m_stPointsets.find(pPointSet) != m_stPointsets.end())
    Modified();
}

///// CUpscalingPointsetRangesBranch::CPointsetDrawDef

CUpscalingPointsetRangesBranch::CPointsetDrawDef::CPointsetDrawDef(const CPointSet& pointset)
: COpenGLNode::CDrawDef(pointset)
{
}

COpenGLNode::CDrawDef::DrawDecisionVector CUpscalingPointsetRangesBranch::CPointsetDrawDef::Color(const geo::IObject &/*object*/) const
{
  assert(dynamic_cast<const CPointSet*>(&OpenGLNode()));
  const CPointSet& pointset = static_cast<const CPointSet&>(OpenGLNode());
  return std::make_pair(true, std::vector<TColor>(1, pointset.Color()));
}


static const char TARGET_POINTSET_BRANCH_NAME[] = "Upscaling 3D Pointset Selection";

///// CUpscalingTargetPointsetBranch
CUpscalingTargetPointsetBranch::CUpscalingTargetPointsetBranch(CFemAppModel& model)
: IUpscalingPointsetBranchBase(model),
  m_pPillarMap(0)
{
}

CUpscalingTargetPointsetBranch::CUpscalingTargetPointsetBranch(IUpscalingNodeBase& parent)
: IUpscalingPointsetBranchBase(TARGET_POINTSET_BRANCH_NAME, parent),
  m_pPillarMap(0)
{
  CModelBase& model = static_cast<CModelBase&>(parent.Model());
  LinkTo(model.Mesh());
}

CUpscalingTargetPointsetBranch::CUpscalingTargetPointsetBranch(const CUpscalingTargetPointsetBranch& rhs)
: IUpscalingPointsetBranchBase(rhs),
  m_pPillarMap(rhs.m_pPillarMap)
{
}

CUpscalingTargetPointsetBranch::~CUpscalingTargetPointsetBranch()
{
  if(m_pPillarMap)
    delete m_pPillarMap;
}

bool CUpscalingTargetPointsetBranch::Empty() const
{
  return false;
}

long CUpscalingTargetPointsetBranch::SavedItems() const
{
  return (m_pPillarMap ? m_pPillarMap->SavedItems() : 0) +
         IUpscalingPointsetBranchBase::SavedItems();
}

void CUpscalingTargetPointsetBranch::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
  stream << int(m_pPillarMap ? 1 : 0);
  if(m_pPillarMap)
    m_pPillarMap->SaveStream(stream, progress);

  IUpscalingPointsetBranchBase::SaveStream(stream, progress);
}

void CUpscalingTargetPointsetBranch::LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress)
{
  int nHasPillarMap;
  stream >> nHasPillarMap;
  if(nHasPillarMap)
  {
    CPillarMap* pPillarMap = new CPillarMap(Model());
    pPillarMap->LoadStream(stream, version, progress);
    LinkTo(*pPillarMap);
  }

  IUpscalingPointsetBranchBase::LoadStream(stream, version, progress);

  Name(TARGET_POINTSET_BRANCH_NAME);

  CModelBase& model = static_cast<CModelBase&>(Model());
  LinkTo(model.Mesh());
}

unsigned int CUpscalingTargetPointsetBranch::IconId() const
{
  return IDI_POINTSETS;
}

bool CUpscalingTargetPointsetBranch::CanConnectItem(const CGraphNode& item) const
{
  const CPointSet* pPointSet = dynamic_cast<const CPointSet*>(&item);
  if(pPointSet)
    return (pPointSet->Dimension() == IPointSet::DIM_2D);

  const CSurfaceBase* pSurface = dynamic_cast<const CSurfaceBase*>(&item);
  return (pSurface != 0);
}

bool CUpscalingTargetPointsetBranch::ConnectItem(const CGraphNode& item)
{
  const CPointSet* pPointSet = dynamic_cast<const CPointSet*>(&item);
  if(pPointSet)
  {
    assert(pPointSet->Dimension() == IPointSet::DIM_2D);
    QString msg = QString("Do you want to generate a target pointset from the 2D pointset '%1'?").arg(pPointSet->Name());
    if(_m()->msg(msg, MB_YESNO | MB_ICONQUESTION) == IDYES)
    {
      CPointsetSource source(*pPointSet);
      CreateTargetPointset(source);
      return true;
    }
  }

  const CSurfaceBase* pSurface = dynamic_cast<const CSurfaceBase*>(&item);
  if(pSurface)
  {
    QString msg = QString("Do you want to generate a target pointset from the surface '%1'?").arg(pSurface->Name());
    if(_m()->msg(msg, MB_YESNO | MB_ICONQUESTION) == IDYES)
    {
      CPointsetSource source(*pSurface);
      CreateTargetPointset(source);
      return true;
    }
  }

  return false;
}

void CUpscalingTargetPointsetBranch::OnNewNeighbour(const CGraphNode& node)
{
  const CPillarMap* pPillarMap = dynamic_cast<const CPillarMap*>(&node);
  if(pPillarMap)
  {
    assert(m_pPillarMap == 0);
    m_pPillarMap = const_cast<CPillarMap*>(pPillarMap);
    Modified();
  }
}

void CUpscalingTargetPointsetBranch::OnNeighbourModified(const CGraphNode& node, enum ModifiedHint /*uHint*/)
{
  const CMeshBase* pMesh = dynamic_cast<const CMeshBase*>(&node);
  if(pMesh && !pMesh->IsMesh())
  {
    CPillarMap* pMap = m_pPillarMap;
    m_pPillarMap = 0;
    delete pMap;
  }
}

void CUpscalingTargetPointsetBranch::OnNeighbourDeleted(const CGraphNode& node)
{
  if(&node == m_pPillarMap)
  {
    m_pPillarMap = 0;
    Modified();
  }
}

bool CUpscalingTargetPointsetBranch::HasPointset() const
{
  return (m_pPillarMap != 0);
}

CPillarMap::const_iterator CUpscalingTargetPointsetBranch::begin() const
{
  assert(m_pPillarMap);
  return m_pPillarMap->begin();
}

CPillarMap::const_iterator CUpscalingTargetPointsetBranch::end() const
{
  assert(m_pPillarMap);
  return m_pPillarMap->end();
}

int CUpscalingTargetPointsetBranch::DisplayListSize() const
{
  return m_pPillarMap ? m_pPillarMap->DisplayListSize() : 0;
}

const geo::IObject& CUpscalingTargetPointsetBranch::DisplayList(int nIndex) const
{
  assert(m_pPillarMap);
  return m_pPillarMap->DisplayList(nIndex);
}

CUpscalingTargetPointsetBranch::TColor CUpscalingTargetPointsetBranch::Color() const
{
  assert(m_pPillarMap);
  return m_pPillarMap->Color();
}

void CUpscalingTargetPointsetBranch::CopyPillarMapAsPointset(const CPointSet& set2d, const QString& sName)
{
  assert(set2d.Dimension() == IPointSet::DIM_2D);

  // the number of valuesets in the 2D pointset
  int nValueSetSize = set2d.NodalValueSetSize() - 2;

  // create 3D pointset with the same amount of valuesets
  CPointSet* pPointset = new CPointSet(sName.toStdString().c_str(), Model(), nValueSetSize, IPointSet::DIM_3D);

  std::vector<geo::CValue> vcRow(nValueSetSize + 3);

  // duplicate the valuesets and valuetypes in the 3D pointset
  const CValueTypeFactory *factory = CValueTypeFactory::instance();
  for(int n = 0; n < nValueSetSize; ++n)
  {
    const CNodalValueSet& nvs = set2d.NodalValueSet(n + 2);
    std::set<IValueComponentBase*> stComponents = nvs.Links<IValueComponentBase>();
    assert(stComponents.size() == 1);
    const CValueType* pVT = dynamic_cast<const CValueType*>(&(*stComponents.begin())->Parent());
    assert(pVT);
    pPointset->NodalValueSet(n + 3).LinkTo(factory->BuildValueType(*pPointset, pVT->TypeId(), pVT->Name())->Component());
  }

  CPillarMap::const_iterator it;
  for(it = begin(); it != end(); ++it)
  {
    const CPillar& pillar = it->second;

    // get the index in the 2D pointset
    std::vector<int> vcPoints = set2d.PointAt(pillar.Position());
    assert(vcPoints.size() == 1);

    // get the values at that point
    for(int n = 0; n < nValueSetSize; ++n)
      vcRow[3 + n] = set2d.NodalValueSet(n + 2).Value(vcPoints[0]);

    // iterate over the pillar (Z-direction) and add the points
    CPillar::const_iterator itp;
    for(itp = pillar.begin(); itp != pillar.end(); ++itp)
    {
      CPillar::CPillarPoint pt = (*itp).first;
      vcRow[0] = pt.X();
      vcRow[1] = pt.Y();
      vcRow[2] = pt.Z();
      pPointset->PushBack(vcRow);
    }
  }
}

void CUpscalingTargetPointsetBranch::CreateFromRegular2DGrid(
  const geo::IPoint& corner1, const geo::IPoint& corner2, int pointsNorthing,
  int pointsEasting)
{
    CGridPointsetSource source(corner1, corner2, pointsNorthing, pointsEasting);
    CreateTargetPointset(source);
}

bool CUpscalingTargetPointsetBranch::CanCreateFromRegular2DGrid() const
{
  return !HasPointset();
}

void CUpscalingTargetPointsetBranch::CreateFromExisting2D(const IPointSet& pointSet)
{
    CPointsetSource source(pointSet);
    CreateTargetPointset(source);
}

bool CUpscalingTargetPointsetBranch::CanCreateFromExisting2D() const
{
  if(HasPointset())
    return false;

  const TPointSetEntry* pEntry = static_cast<const TPointSetEntry*>(Model().GraphEntry(MD_BASE_POINTSET));
  assert(pEntry);
  const TPointSetEntry::TNodeSet& stNodes = pEntry->EntryNodes();
  TPointSetEntry::TNodeSet::const_iterator it;
  for(it = stNodes.begin(); it != stNodes.end(); ++it)
  {
    if((*it)->Dimension() == IPointSet::DIM_2D && dynamic_cast<const CPointSet*>(*it) != 0)
      return true;
  }

  return false;
}

void CUpscalingTargetPointsetBranch::CreateFromSurfaceNodes(const IPointSet& pointSet)
{
    CPointsetSource source(pointSet);
    CreateTargetPointset(source);
}

bool CUpscalingTargetPointsetBranch::CanCreateFromSurfaceNodes() const
{
  if(HasPointset())
    return false;

  const CSurfaceEntry* pEntry = static_cast<const CSurfaceEntry*>(Model().GraphEntry(MD_BASE_SURFACE));
  assert(pEntry);
  return !pEntry->EntryNodes().empty();
}

void CUpscalingTargetPointsetBranch::CreateFromHexaMidpoints()
{
  const CModelBase& model = static_cast<const CModelBase&>(Model());
  assert(dynamic_cast<const CHexaMesh*>(&model.Mesh()));
  CGridPointsetSource source(static_cast<const CHexaMesh&>(model.Mesh()));
  CreateTargetPointset(source);
}

bool CUpscalingTargetPointsetBranch::CanCreateFromHexaMidpoints() const
{
  return (!HasPointset() && dynamic_cast<const CHexaModel*>(&Model()) != 0);
}

void CUpscalingTargetPointsetBranch::CreateTargetPointset(const ITargetPointsetSource& source)
{
  std::auto_ptr <IProgressBase> progress(_g->prog()->create(eProgress::Geo, "Generating target pointset"));
  CModelBase& model = static_cast<CModelBase&>(Model());
  CMeshSampler sampler(source, model.Mesh());
  CPillarMap* pPillarMap = sampler.GeneratePillarMap("3D Pointset", model, progress.get());
  assert(!m_pPillarMap);
  LinkTo(*pPillarMap);
  assert(m_pPillarMap == pPillarMap);
}