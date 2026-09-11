// FormationVolume.cpp: implementation of the FormationVolume class.
//
//////////////////////////////////////////////////////////////////////

#include "FormationVolume.h"
#include "3DFormation.h"
#include "BodyGroup.h"
#include "MeshBase.h"
#include "ResultTree.h"

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
// #define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Implementation of CFormationVolume
//////////////////////////////////////////////////////////////////////
CFormationVolume::CFormationVolume(C3DFormation &formation, geo::CBodyGroup &body_group, bool bDoLink)
    : IFormationElementSet(formation, bDoLink), m_BodyGroup(&body_group) {}

CFormationVolume::CFormationVolume(C3DFormation &formation) : IFormationElementSet(formation), m_BodyGroup(0) {}

CFormationVolume::CFormationVolume(const CFormationVolume &rhs)
    : IFormationElementSet(rhs), m_BodyGroup(rhs.m_BodyGroup) {}

bool CFormationVolume::operator==(const CFormationVolume &rhs) const { return IElementSet::operator==(rhs); }

CFormationVolume &CFormationVolume::operator=(const CFormationVolume &rhs) {
  IElementSet::operator=(rhs);
  m_BodyGroup = rhs.m_BodyGroup;
  return *this;
}

unsigned int CFormationVolume::IconId() const { return IDI_BODY; }

unsigned int CFormationVolume::TypeId() const { return 0; }

QString CFormationVolume::TypeName() const { return "Formation Bodies"; }

int CFormationVolume::BodyGroupIndex(const geo::IMesh &mesh, const geo::CBodyGroup &group) const {
  for (int i = 0; i < mesh.ElementGroupSize(); i++) {
    if (&group == &mesh.ElementGroup(i))
      return i;
  }

  return -1;
}

bool CFormationVolume::Valid() const { return (m_BodyGroup != 0); }

void CFormationVolume::OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint) {
  CModelBase &model = dynamic_cast<CModelBase &>(Model());
  if (&model.Mesh() == &node) {
    // Reset group index if there is no mesh ...
    if (!model.Mesh().IsMesh())
      m_BodyGroup = 0;
    Modified(uHint);
  }
}

bool CFormationVolume::Volume(const geo::CBodyGroup &group) {
  m_BodyGroup = const_cast<geo::CBodyGroup *>(&group);
  return (m_BodyGroup != 0);
}

const geo::CBodyGroup &CFormationVolume::Volume() const { return *m_BodyGroup; }

geo::CBodyGroup &CFormationVolume::Volume() { return *m_BodyGroup; }

const geo::IElementSet &CFormationVolume::ElementSet() const { return Volume(); }

geo::IElementSet &CFormationVolume::ElementSet() { return Volume(); }

CFormationVolume::DIMENSION CFormationVolume::Dimension() const { return DIM_3D; }

bool CFormationVolume::PointInConvexHull(const geo::IPoint & /*pt*/) const {
  assert(false);
  return false;
}

int CFormationVolume::DisplayListSize() const {
  CModelBase &model = (CModelBase &)(Model());
  // Do we have a mesh?
  if (model.Mesh().IsMesh() && (m_BodyGroup != 0))
    return IElementSet::DisplayListSize();
  return 0; // No mesh, nothing to display
}

namespace {

class CZLess {
public:
  bool operator()(const geo::CPoint &lhs, const geo::CPoint &rhs) { return lhs.Z() < rhs.Z(); }
};

} // anonymous namespace

geo::CValue CFormationVolume::ThicknessAt(const geo::IPoint &pt, geo::IPoint &ptTop, geo::IPoint &ptBottom,
                                          geo::IParallelInitializationCallback *cb) const {
  geo::CPoint ptKey(pt.X(), pt.Y(), 0);

  TThicknessCache *cache = &m_mpThicknessCache;

  TThicknessCache::iterator it1 = cache->find(ptKey); // first try our sequential cache
  if (it1 != cache->end()) {
    ptTop = it1->second.ptTop;
    ptBottom = it1->second.ptBottom;
    return it1->second.value;
  }

  if (dynamic_cast<CThicknessParallelInitializationCallback *>(cb)) // else, our thread-local cache
  {
    static_cast<CThicknessParallelInitializationCallback *>(cb)->GetCache(this, &cache);

    it1 = cache->find(ptKey);
    if (it1 != cache->end()) {
      ptTop = it1->second.ptTop;
      ptBottom = it1->second.ptBottom;
      return it1->second.value;
    }
  }

  geo::CPoint ptMin = Min();
  geo::CPoint ptMax = Max();
  ptMin.X(pt.X());
  ptMin.Y(pt.Y());
  ptMax.X(pt.X());
  ptMax.Y(pt.Y());

  geo::CLine l(ptMin, ptMax);

  std::set<geo::CPoint, CZLess> stPoints;

  int i;
  for (i = 0; i < Volume().SideSurfaceSize(); ++i) {
    const geo::CBodyGroup::CSideSurface &surf = Volume().SideSurface(i);
    if (surf.PointSize() == 0)
      continue;

    // check for intersections
    std::set<int> stCandidates = surf.Candidates(ptMin, ptMax);
    for (std::set<int>::iterator it2 = stCandidates.begin(); it2 != stCandidates.end(); ++it2) {
      const geo::IFace &face = surf.Face(*it2);
      geo::CPoint ptIntersect = face.Intersection(l);
      if (!ptIntersect.Empty())
        stPoints.insert(ptIntersect);
    }
  }

  geo::CValue valRet;

  if (stPoints.size() ==
      0) // in case of wellpaths and multiple volumes in a formation, we don't need to intersect at all
  {
    ptTop = pt;
    ptBottom = pt;
    return valRet;
  }

  ptTop = *stPoints.begin();
  ptBottom = *stPoints.rbegin();

  valRet = ptBottom.Z() - ptTop.Z();

  assert(stPoints.size() > 1 || valRet == 0); // 0 if line only touched the volume

  TThicknessValue thickval;
  thickval.value = valRet;
  thickval.ptTop = ptTop;
  thickval.ptBottom = ptBottom;
  cache->insert(TThicknessCache::value_type(ptKey, thickval));

  return valRet;
}

void CFormationVolume::clearThicknessCache() { m_mpThicknessCache.clear(); }

void CFormationVolume::AddToCache(TThicknessCache *cache) const {
  for (TThicknessCache::iterator it = cache->begin(); it != cache->end(); ++it) {
    m_mpThicknessCache.insert(*it);
  }
}
