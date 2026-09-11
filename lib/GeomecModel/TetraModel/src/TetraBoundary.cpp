// TetraBoundary.cpp: implementation of the CTetraBoundary class.
//
//////////////////////////////////////////////////////////////////////

#include "TetraBoundary.h"
#include "FemAppModel.h"
#include "FvGocadFile.h"
#include "Global.h"
#include "IProgressFactory.h"
#include "InterfaceElement.h"
#include "ResultRegister.h"
#include "StreamVersion.h"
#include "SurfaceDesc.h"
#include "TSSurface.h"
#include "TSSurfaceProgress.h"
#include "TetSurface.h"
#include "TetraEntryTypes.h"
#include "TetraMesh.h"
#include "TetraModel.h"
#include "TetraSubBoundary.h"
#include "TetraSubHorizon.h"
#include "TetraSuperHorizon.h"
#include "unitnode.h"

const double DEFAULT_SUPER_DISTANCE = 50;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTetraBoundary::CTetraBoundary(CTetraSubBoundary &boundary)
    : CInterfaceBoundary(geo::CPoint(0, 0, 0), geo::CPoint(100, 100, 100), boundary.Model(), DEFAULT_DEFINED),
      m_distance(DEFAULT_SUPER_DISTANCE, 50, 1e5, true, true), m_dLoadedDistance(DEFAULT_SUPER_DISTANCE),
      m_pSubBoundary(&boundary), m_pSuperHorizonEntry(0) {
  // Link to sub boundary and horizon entry
  LinkTo(*m_pSubBoundary);
  m_pSuperHorizonEntry = (CTetraSuperHorizonEntry *)Model().GraphEntry(MD_TETRA_SUPER_HORIZON);
  LinkTo(*m_pSuperHorizonEntry);

  m_side_surface[0] = 0;
  m_side_surface[1] = 0;
  m_side_surface[2] = 0;
  m_side_surface[3] = 0;
}

CTetraBoundary::CTetraBoundary(const CTetraBoundary &rhs)
    : CInterfaceBoundary(rhs), m_distance(rhs.m_distance), m_dLoadedDistance(rhs.m_dLoadedDistance),
      m_pSubBoundary(rhs.m_pSubBoundary), m_pSuperHorizonEntry(rhs.m_pSuperHorizonEntry) {
  m_side_surface[0] = 0;
  m_side_surface[1] = 0;
  m_side_surface[2] = 0;
  m_side_surface[3] = 0;
}

CTetraBoundary::~CTetraBoundary() {}

bool CTetraBoundary::operator==(const CTetraBoundary &rhs) const {
  if (!CInterfaceBoundary::operator==(rhs))
    return false;

  return m_distance == rhs.m_distance;
}

CTetraBoundary &CTetraBoundary::operator=(const CTetraBoundary &rhs) {
  m_distance = rhs.m_distance;
  CInterfaceBoundary::operator=(rhs);
  InvalidateSideSurfaces();
  return *this;
}

CTetraBoundary::TMinMax CTetraBoundary::BestFit() const {
  // Take min and max of the sub boundary first
  TMinMax ret;

  ret.first = m_pSubBoundary->Min();
  ret.second = m_pSubBoundary->Max();

  CTetraSuperHorizonEntry::TNodeSet stNode = m_pSuperHorizonEntry->EntryNodes();

  // Collect points
  for (CTetraSuperHorizonEntry::TNodeSet::const_iterator it = stNode.begin(); it != stNode.end(); it++) {
    const CTetraSuperHorizon &horizon = **it;
    ret.first = ret.first.Min(horizon.Min());
    ret.second = ret.second.Max(horizon.Max());
  }

  if ((State() == BEST_FIT) || ret.first.Empty() || ret.second.Empty())
    return ret;

  return SnapToGrid(
      TMinMax(geo::CPoint(Min().X(), Min().Y(), ret.first.Z()), geo::CPoint(Max().X(), Max().Y(), ret.second.Z())));
}

CTetraBoundary::TMinMax CTetraBoundary::SnapToGrid(const TMinMax &minmax) const {
  // If sub boundary is not defined do nothing
  if (m_pSubBoundary->Min().Empty() || m_pSubBoundary->Max().Empty())
    return minmax;

  TMinMax ret = minmax;
  // Rectify
  if (IsSuperModel()) {
    if (ret.first.X() > (m_pSubBoundary->Min().X() - m_distance.Value()))
      ret.first.X(m_pSubBoundary->Min().X() - m_distance.Value());
    if (ret.first.Y() > (m_pSubBoundary->Min().Y() - m_distance.Value()))
      ret.first.Y(m_pSubBoundary->Min().Y() - m_distance.Value());
    if (ret.second.X() < (m_pSubBoundary->Max().X() + m_distance.Value()))
      ret.second.X(m_pSubBoundary->Max().X() + m_distance.Value());
    if (ret.second.Y() < (m_pSubBoundary->Max().Y() + m_distance.Value()))
      ret.second.Y(m_pSubBoundary->Max().Y() + m_distance.Value());
  }

  return ret;
}

CTetraBoundary::TRotated CTetraBoundary::SnapToGrid(const TRotated & /*rotated*/) const {
  assert(false);
  return TRotated();
}

const geo::IObject &CTetraBoundary::DisplayList(int nIndex) const { return SideSurface(nIndex); }

int CTetraBoundary::DisplayListSize() const { return SideSurfaceSize(); }

void CTetraBoundary::LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress) {
  // Sub Boundary
  CTetraModel *pModel = dynamic_cast<CTetraModel *>(&Model());
  assert(pModel);
  assert(IsLinkedTo(pModel->SubBoundary()));
  assert(&pModel->SubBoundary() == m_pSubBoundary);
  m_pSubBoundary = &pModel->SubBoundary();

  // Tetra super horizon entry
  assert(m_pSuperHorizonEntry == (CTetraSuperHorizonEntry *)Model().GraphEntry(MD_TETRA_SUPER_HORIZON));
  assert(IsLinkedTo(*m_pSuperHorizonEntry));

  if (version < CStreamVersion(4, 1, 2))
    CBoundaryBase::LoadStream(stream, version, progress); // skip CInterfaceBoundary (is done below)
  else
    CInterfaceBoundary::LoadStream(stream, version, progress);

  // From version 3.0.27 we have a minimum distance
  if (CStreamVersion(3, 0, 26) < version) {
    // wedx 03082007:
    // Default changed to DEFAULT_SUPER_DISTANCE, but values have been saved to the file already,
    // even if the model was not a super model.
    // Since the super horizon entry is loaded (and filled) after the tetra boundary, we only
    // load the distance into a member variable.
    // In OnNeighbourModified this distance is actually used when the super horizon entry is filled
    // (so this is actually a super model).
    stream >> m_dLoadedDistance;
  }

  if (version < CStreamVersion(4, 1, 2)) {
    LoadSupportNodes(stream, version, progress);
  }

  // version 3.0.67 and higher save and load the settings for the interface on the boundary
  if (CStreamVersion(3, 0, 66) < version && version < CStreamVersion(4, 1, 2)) {
    // load CInterfaceBoundary properties now
    LoadPre412Stream(stream, version, progress);
  }

  if (CStreamVersion(3, 0, 67) < version && version < CStreamVersion(3, 0, 76)) {
    // Is not nescessary anymore. Just read and forget
    // The elements are added in the CTetraMesh
    int nIndx, nIndxSize, i;
    stream >> nIndxSize;
    for (i = 0; i < nIndxSize; i++)
      stream >> nIndx;

    int nIntfElSize;
    stream >> nIntfElSize;

    int nIdx;
    for (i = 0; i < nIntfElSize; i++)
      stream >> nIdx;
  }
}

void CTetraBoundary::SaveStream(TSTREAM &stream, TPROGRESS &progress) {
  CInterfaceBoundary::SaveStream(stream, progress);

  stream << m_distance.Value();
}

const CTetraHorizonBase *CTetraBoundary::TopHorizon() const {
  if (State() == DEFAULT_DEFINED)
    return m_pSubBoundary->TopHorizon().Horizon();

  // Get top horizon from entry
  assert(Model().GraphEntry(MD_TETRA_SUPER_HORIZON));
  const CTetraSuperHorizonEntry *pEntry =
      dynamic_cast<const CTetraSuperHorizonEntry *>(Model().GraphEntry(MD_TETRA_SUPER_HORIZON));
  assert(pEntry);
  CTetraSuperHorizonEntry::TSortedNodeSet stHorizon = pEntry->SortedEntryNodes();
  assert(stHorizon.size() > 0);
  return *stHorizon.begin();
}

CTetraHorizonBase *CTetraBoundary::TopHorizon() {
  if (State() == DEFAULT_DEFINED)
    return m_pSubBoundary->TopHorizon().Horizon();

  // Get top horizon from entry
  assert(Model().GraphEntry(MD_TETRA_SUPER_HORIZON));
  const CTetraSuperHorizonEntry *pEntry =
      dynamic_cast<const CTetraSuperHorizonEntry *>(Model().GraphEntry(MD_TETRA_SUPER_HORIZON));
  assert(pEntry);
  CTetraSuperHorizonEntry::TSortedNodeSet stHorizon = pEntry->SortedEntryNodes();
  assert(stHorizon.size() > 0);
  return *stHorizon.begin();
}

const CTetraHorizonBase *CTetraBoundary::BottomHorizon() const {
  if (State() == DEFAULT_DEFINED)
    return m_pSubBoundary->BottomHorizon().Horizon();

  assert(Model().GraphEntry(MD_TETRA_SUPER_HORIZON));
  const CTetraSuperHorizonEntry *pEntry =
      dynamic_cast<const CTetraSuperHorizonEntry *>(Model().GraphEntry(MD_TETRA_SUPER_HORIZON));
  assert(pEntry);
  CTetraSuperHorizonEntry::TSortedNodeSet stHorizon = pEntry->SortedEntryNodes();
  assert(stHorizon.size() > 0);
  CTetraSuperHorizonEntry::TSortedNodeSet::iterator it = stHorizon.end();
  it--;
  return *it;
}

CTetraHorizonBase *CTetraBoundary::BottomHorizon() {
  if (State() == DEFAULT_DEFINED)
    return m_pSubBoundary->BottomHorizon().Horizon();

  assert(Model().GraphEntry(MD_TETRA_SUPER_HORIZON));
  const CTetraSuperHorizonEntry *pEntry =
      dynamic_cast<const CTetraSuperHorizonEntry *>(Model().GraphEntry(MD_TETRA_SUPER_HORIZON));
  assert(pEntry);
  CTetraSuperHorizonEntry::TSortedNodeSet stHorizon = pEntry->SortedEntryNodes();
  assert(stHorizon.size() > 0);
  CTetraSuperHorizonEntry::TSortedNodeSet::const_iterator it = stHorizon.end();
  it--;
  return *it;
}

bool CTetraBoundary::IsSuperModel() const {
  assert(Model().GraphEntry(MD_TETRA_SUPER_HORIZON));
  const CTetraSuperHorizonEntry *pEntry =
      dynamic_cast<const CTetraSuperHorizonEntry *>(Model().GraphEntry(MD_TETRA_SUPER_HORIZON));
  if (pEntry) {
    CTetraSuperHorizonEntry::TSortedNodeSet stHorizon = pEntry->SortedEntryNodes();
    return stHorizon.size() > 0;
  }

  return false;
}

bool CTetraBoundary::IsSuperModelValid() const {
  assert(IsSuperModel());
  const CTetraModel &model = dynamic_cast<const CTetraModel &>(Model());
  if (BottomHorizon() && TopHorizon() && !model.SubBoundary().Empty() && !model.SubBoundary().Empty()) {
    return (model.SubBoundary().Max().Z() < BottomHorizon()->Max().Z()) &&
           (model.SubBoundary().Min().Z() > TopHorizon()->Min().Z());
  }

  return false;
}

bool CTetraBoundary::DistanceToSubBoundary(const double &dDistance, CQuantity::UNIT unit) {
  // The distance
  m_distance.Value(dDistance, unit);

  // Rectify model
  switch (State()) {
  case DEFAULT_DEFINED:
    // Do nothing, the boundary size only depends on the model
    return false;
  case BEST_FIT:
    // Calculate best fit state and the set will rectify the distance
    return Set(BestFit().first, BestFit().second);
  case USER_DEFINED:
    // Rectify current user settings
    return Set(Min(), Max());
  default:
    assert(false);
  };
  assert(false);
  return false;
}

const CLengthQuantity &CTetraBoundary::DistanceToSubBoundary() const { return m_distance; }

const geo::CSurfaceDesc &CTetraBoundary::SideSurfaceDesc(int nIndex) const {
  if (State() == DEFAULT_DEFINED)
    return m_pSubBoundary->SideSurfaceDesc(nIndex);

  const CModelBase *pModel = dynamic_cast<const CModelBase *>(&Model());
  assert(pModel);
  const CTetraMesh *pMesh = dynamic_cast<const CTetraMesh *>(&pModel->Mesh());
  assert(pMesh && pMesh->IsMesh());

  // Search for side surface
  for (int i = 0; i < pMesh->InputSurfaceSize(); i++) {
    if (!pMesh->InputSurface(i).first->Slip()) {
      if (&pMesh->InputSurface(i).first->Surface() == &SideSurface(nIndex))
        return *pMesh->InputSurface(i).first;
    }
  }

  geo::CSurfaceDesc *pCrap = 0;
  return *pCrap;
}

void CTetraBoundary::ExportSurfaces(const QString &fileName, const CUnitNode &unitNode) {
  // Create file object
  geo::CMesh mesh;
  CGoCadFile file(mesh);

  if (unitNode.Unit() == IQuantityDouble::SI_UNIT)
    file.setUnitType(SI_UNIT);
  else if (unitNode.Unit() == IQuantityDouble::FIELD_UNIT)
    file.setUnitType(FIELD_UNIT);
  else
    file.setUnitType(OTHER_UNIT);

  // Insert surfaces in file object
  for (int i = 0; i < SideSurfaceSize(); i++) {
    QString sName;
    sName = QString("Side%1").arg(i + 1);
    file.AppendSurface(SideSurface(i), sName);
  }

  // Set up progress dialog and save surfaces
  std::auto_ptr<IProgressBase> prog;
  try {
    prog.reset(_g->prog()->create(eProgress::Geo, ""));
    file.Save(fileName, *prog);
  } catch (CProgressCancel *p) {
    delete p;
    file.Close();
  }
}

bool CTetraBoundary::CanExportSurface() const { return IsSuperModel() && IsSuperModelValid(); }

const geo::ISurface &CTetraBoundary::SideSurface(int nIndex) const {
  if (State() == DEFAULT_DEFINED)
    return m_pSubBoundary->SideSurface(nIndex).Surface();

  assert(nIndex >= 0);
  assert(nIndex < 4);

  if (m_side_surface[nIndex] == 0)
    GenerateSurface(nIndex);

  assert(m_side_surface[nIndex]);

  return *m_side_surface[nIndex];
}

QString CTetraBoundary::SideSurfaceName(int nIndex) const {
  if (State() == DEFAULT_DEFINED)
    return m_pSubBoundary->SideSurface(nIndex).Name().toStdString().c_str();

  assert(nIndex >= 0);
  assert(nIndex < 4);

  QString sName;
  sName = QString("Super boundary surface %1").arg(nIndex);
  return sName;
}

int CTetraBoundary::SideSurfaceSize() const {
  if (State() == DEFAULT_DEFINED)
    return m_pSubBoundary->SideSurfaceSize();

  return 4;
}

void CTetraBoundary::InvalidateSideSurfaces() {
  // Delete side surfaces
  delete m_side_surface[0];
  delete m_side_surface[1];
  delete m_side_surface[2];
  delete m_side_surface[3];
  // Set back to null
  m_side_surface[0] = 0;
  m_side_surface[1] = 0;
  m_side_surface[2] = 0;
  m_side_surface[3] = 0;
}

void CTetraBoundary::AddPointsFromSurface(const geo::ISurface &surface, geo::CArray<geo::CPoint> &arPoint,
                                          int nIndex) const {
  for (int nPoint = 0; nPoint < surface.PointSize(); nPoint++) {
    const geo::IPoint &point = surface.Point(nPoint);
    switch (nIndex) {
    case 0:
      if (fabs(Min().X() - point.X()) < EPS) {
        geo::CPoint geoPoint(point);
        arPoint.PushBack(geoPoint);
      }
      break;
    case 1:
      if (fabs(Max().X() - point.X()) < EPS) {
        geo::CPoint geoPoint(point);
        arPoint.PushBack(geoPoint);
      }
      break;
    case 2:
      if (fabs(Min().Y() - point.Y()) < EPS) {
        geo::CPoint geoPoint(point);
        arPoint.PushBack(geoPoint);
      }
      break;
    case 3:
      if (fabs(Max().Y() - point.Y()) < EPS) {
        geo::CPoint geoPoint(point);
        arPoint.PushBack(geoPoint);
      }
      break;
    default:
      assert(false);
      break;
    }
  }
}

void CTetraBoundary::GenerateSurface(int nIndex) const {
  assert(State() != DEFAULT_DEFINED);

  std::vector<const CTetraHorizonBase *> vcHorizon;

  // The top- and bottom horizons of the sub boundary are not streched to super boundary anymore

  // Get the sub horizon
  CTetraSuperHorizonEntry::TNodeSet stNode = m_pSuperHorizonEntry->EntryNodes();
  assert(stNode.size() > 1);

  for (CTetraSuperHorizonEntry::TNodeSet::const_iterator it = stNode.begin(); it != stNode.end(); it++)
    vcHorizon.push_back(*it);

  // Collect points
  geo::CArray<geo::CPoint> arPoint;
  for (size_t i = 0; i < vcHorizon.size(); i++) {
    const CTetraHorizonBase &horizon = *vcHorizon[i];

    if (horizon.ConstantDepth())
      AddPointsFromSurface(horizon.GeneratedConstantDepthSurface(), arPoint, nIndex);
    else {
      assert(horizon.SurfaceSize() == 1);
      if (!((fabs(horizon.Min().X() - Min().X()) < EPS) && (fabs(horizon.Min().Y() - Min().Y()) < EPS) &&
            (fabs(horizon.Max().X() - Max().X()) < EPS) && (fabs(horizon.Max().Y() - Max().Y()) < EPS))) {
        const_cast<CSurfaceBase &>(horizon.Surface(0)).Increase(Box());
      }

      AddPointsFromSurface(horizon.Surface(0).Surface(), arPoint, nIndex);
    }
  }

  assert(arPoint.Size() > 0);

  if ((nIndex == 0) || (nIndex == 1))
    arPoint.Rotate(geo::CVector::Yaxis, 90);
  else
    arPoint.Rotate(geo::CVector::Xaxis, 90);

  CTSSurfaceProgress SurfaceProgress;
  m_side_surface[nIndex] = new CTSSurface(arPoint, SurfaceProgress);

  if ((nIndex == 0) || (nIndex == 1))
    m_side_surface[nIndex]->Rotate(geo::CVector::Yaxis, -90);
  else
    m_side_surface[nIndex]->Rotate(geo::CVector::Xaxis, -90);

  //	assert(m_side_surface[nIndex]->Rotate(FaceSize() > 2);
}

bool CTetraBoundary::OnSet(const TMinMax &minmax) {
  // If the min max changed we invalidate the surfaces
  TMinMax minmax_snapped = SnapToGrid(minmax);

  if (!((minmax_snapped.first == Min()) && (minmax_snapped.second == Max())))
    InvalidateSideSurfaces();

  return CInterfaceBoundary::OnSet(minmax_snapped);
}

void CTetraBoundary::OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint) {
  CTetraModel *pModel = dynamic_cast<CTetraModel *>(&Model());
  assert(pModel);

  // wedx 03082007:
  // If the model is being loaded and it appears to be a super model (super horizons
  // are added to the super horizon entry), the loaded m_dLoadedDistance (distance
  // between the sub- and super model) needs to be installed.
  if (&node == m_pSuperHorizonEntry && pModel->Loading()) {
    if (m_pSuperHorizonEntry->EntryNodes().size() > 1)
      m_distance.Value(m_dLoadedDistance, CQuantity::SI_UNIT);
  }

  // The super horizon entry can be decoupled and the pointer is then zero. This is case when
  // messaging is done during destruction.
  if (m_pSuperHorizonEntry && m_pSubBoundary && ((&node == m_pSubBoundary) || (&node == m_pSuperHorizonEntry)) &&
      !pModel->Mesh().IsMesh()) {
    // The sub boundary or the horizon entry is changed
    //		bool bModified = false;
    InvalidateSideSurfaces();
    if (m_pSuperHorizonEntry->EntryNodes().size() > 1) {
      // If default switch directly to best fit
      if (State() == DEFAULT_DEFINED)
        State(BEST_FIT);
    }

    if (m_pSuperHorizonEntry->EntryNodes().size() < 2) {
      // If not default switch directly to it
      if ((State() == BEST_FIT) || (State() == USER_DEFINED))
        State(DEFAULT_DEFINED);
    }

    // Calculate the rectifier

    bool bModified = false;
    if ((State() == DEFAULT_DEFINED) && (m_pSubBoundary->SideSurfaceSize() > 0))
      bModified = OnSet(TMinMax(m_pSubBoundary->Min(), m_pSubBoundary->Max()));

    if (!bModified)
      Modified();
  }

  // Is one of the super horizons changed?
  if (m_pSuperHorizonEntry) {
    CTetraSuperHorizonEntry::TNodeSet stNode = m_pSuperHorizonEntry->EntryNodes();

    if (stNode.find((CTetraSuperHorizon *)(&node)) != stNode.end()) {
      if ((State() != DEFAULT_DEFINED) && (!pModel->Mesh().IsMesh())) {
        InvalidateSideSurfaces();
        TMinMax bestfit = BestFit();
        if (!OnSet(bestfit))
          Modified();
      }
    }
  }

  if (&node == &pModel->Mesh() && uHint != MESH_LOADED && pModel->Mesh().IsMesh() && CreateInterfaces())
    CreateInterfaceElements();

  CInterfaceBoundary::OnNeighbourModified(node, uHint);
}

void CTetraBoundary::OnNeighbourDeleted(const CGraphNode &node) {
  // Boundary deleted
  if (m_pSubBoundary == &node)
    m_pSubBoundary = 0;

  // Super horizons entry
  if (m_pSuperHorizonEntry == &node)
    m_pSuperHorizonEntry = 0;

  CInterfaceBoundary::OnNeighbourDeleted(node);
}

// Top and bottom horizon
const CTetraHorizonBase &CTetraBoundary::GetTopHorizon() const { return *TopHorizon(); }

const CTetraHorizonBase &CTetraBoundary::GetBottomHorizon() const { return *BottomHorizon(); }

std::vector<const geo::ISurface *> CTetraBoundary::GetSideMeshSurfaces() const {
  std::vector<const geo::ISurface *> vcSurface;

  int i;
  for (i = 0; i < SideSurfaceSize(); ++i) {
    const geo::CSurfaceDesc &surfdesc = SideSurfaceDesc(i);
    int j;
    for (j = 0; j < surfdesc.TetSurfaceSize(); ++j)
      vcSurface.push_back(&surfdesc.TetSurface(j));
  }

  return vcSurface;
}

/*
void CTetraBoundary::GfromKrad(const double &krad, CDoubleQuantity::UNIT unit)
{
  double g;
  g = (R().Value(unit) * krad) / (4.0 * 0.76);
  m_ShearQuantity.Value(g, unit);
}

void CTetraBoundary::GfromKtan(const double &ktan, CDoubleQuantity::UNIT unit)
{
  double g;
  g = (R().Value(unit) * ktan) / (3.0 * 0.52);
  m_ShearQuantity.Value(g, unit);
}
*/
void CTetraBoundary::CreateInterfaceElements() {
  if (InterfaceElements())
    return; // they were created earlier

  CModelBase &model = dynamic_cast<CModelBase &>(Model());
  assert(&model);

  int i;
  for (i = 0; i < BottomHorizon()->OutputSurfaceSize(); i++) {
    ElementsFromSurfDesc(BottomHorizon()->OutputSurface(i));
  }

  for (i = 0; i < TopHorizon()->OutputSurfaceSize(); i++) {
    ElementsFromSurfDesc(TopHorizon()->OutputSurface(i));
  }

  for (i = 0; i < SideSurfaceSize(); i++) {
    ElementsFromSurfDesc(SideSurfaceDesc(i));
  }

  model.ResultRegister().ClearLinear(false);
  model.ResultRegister().ClearNonLinear(false);
  model.ResultRegister().ClearMixture();
}

void CTetraBoundary::ElementsFromSurfDesc(const geo::CSurfaceDesc &sd) {
  CModelBase &model = dynamic_cast<CModelBase &>(Model());
  assert(&model);
  CMeshBase &mesh = model.Mesh();

  // A pool for generated interface nodes.
  typedef std::set<const geo::INode *, geo::ICoordinate::CCoordinateLess> TNodeSet;
  TNodeSet stInterfaceNodes;

  for (int k = 0; k < sd.TetSurfaceSize(); k++) {

    const geo::CTetSurface &s = sd.TetSurface(k);
    for (int j = 0; j < s.FaceSize(); j++) {
      const geo::IFace &front = s.Face(j);
      assert(front.NrOfPoints() == 3);
      std::vector<int> vcNode(front.NrOfPoints() * 2);
      for (int i = 0; i < front.NrOfPoints(); i++) {
        vcNode[i] = front.Node(i).Index();
        TNodeSet::iterator it = stInterfaceNodes.find(&front.Node(i));
        if (it != stInterfaceNodes.end()) {
          // Node exists
          vcNode[i + front.NrOfPoints()] = (*it)->Index();
        } else {
          // Generate an extra node
          int nNodeIdx = mesh.Mesh().RegisterNode(front.Node(i), false);
          const geo::INode &node = mesh.Mesh().Node(nNodeIdx);
          stInterfaceNodes.insert(&node);
          vcNode[i + front.NrOfPoints()] = nNodeIdx;
        }
      }

      // wedx 14092007:
      // geo::CInterfaceElement doesn't like NULL faces, so provide the front twice
      geo::CInterfaceElement *pElement = new geo::CInterfaceElement(mesh.Mesh(), &front, &front, vcNode);
      AddInterfaceElement(*pElement);
    }
  }
}
