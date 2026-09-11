/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// IElementSet.cpp: implementation of the IElementSet class.
//
//////////////////////////////////////////////////////////////////////
#include "IElementSet.h"
#include "BucketKernel.h"
#include "CacheInterfaceDefault.h"
#include "ElementPoint.h"
#include "IElement.h"
#include "IPlane.h"
#include "IVector.h"
#include "Line.h"
#include "dimple.h"
#include <cassert>
#include <cmath>

#include <tbb/spin_rw_mutex.h>

namespace {

// See comments in IFace

tbb::spin_rw_mutex myGlobalIElementSetMutex[16];

} // namespace

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace geo {

IElementSet::IElementSet() : m_pKernel(0), m_cache_interface(&CCacheInterfaceDefault::Instance()) {}

IElementSet::~IElementSet() {}

void IElementSet::CreateBuckets() const {
  {
    int index = (intptr_t)this >> 6 & 0xf;

    tbb::spin_rw_mutex::scoped_lock lock(myGlobalIElementSetMutex[index], false);

    if (m_pKernel == 0) {
      lock.upgrade_to_writer();

      if (m_pKernel != 0)
        return;

      m_pKernel = new CBucketKernel(Min(), Max(), ElementSize() / 2);

      for (int nElement = 0; nElement < ElementSize(); nElement++) {
        const IElement &element = Element(nElement);
        int min_x = m_pKernel->BucketX(element.Min().X() - EPS);
        int min_y = m_pKernel->BucketY(element.Min().Y() - EPS);
        int min_z = m_pKernel->BucketZ(element.Min().Z() - EPS);
        int max_x = m_pKernel->BucketX(element.Max().X() + EPS);
        int max_y = m_pKernel->BucketY(element.Max().Y() + EPS);
        int max_z = m_pKernel->BucketZ(element.Max().Z() + EPS);
        for (int x = min_x; x <= max_x; x++)
          for (int y = min_y; y <= max_y; y++)
            for (int z = min_z; z <= max_z; z++)
              m_pKernel->Insert(x, y, z, nElement);
      }
    }
  }
}

void IElementSet::InvalidateCache() {
  m_min = geo::CPoint();
  m_max = geo::CPoint();
  delete m_pKernel;
  m_pKernel = 0;
  m_cache_interface->Invalidate();
}

int min_index(int n1, int n2) {
  if ((n1 == -1) || ((n2 < n1) && (n2 != -1)))
    return n2;
  return n1;
}

int max_index(int n1, int n2) {
  if ((n1 == -1) || ((n2 > n1) && (n2 != -1)))
    return n2;
  return n1;
}

void IElementSet::XDir(const IPlane &Plane, std::set<int> &stElementIndex) const {
  // Calculate postions
  std::vector<std::vector<int>> vcPos(BucketKernel().SizeY() + 1);
  double dEdgeSize = BucketKernel().EdgeSize();
  int y;
  for (y = 0; y <= BucketKernel().SizeY(); y++) {
    vcPos[y].resize(BucketKernel().SizeZ() + 1);
    for (int z = 0; z <= BucketKernel().SizeZ(); z++) {
      double dY = BucketKernel().Min().Y() + y * dEdgeSize;
      double dZ = BucketKernel().Min().Z() + z * dEdgeSize;
      CPoint pt = Plane.Intersection(
          geo::CLine(geo::CPoint(BucketKernel().Min().X(), dY, dZ), geo::CPoint(BucketKernel().Max().X(), dY, dZ)));

      if (pt.Empty())
        vcPos[y][z] = -1;
      else
        vcPos[y][z] = BucketKernel().BucketX(pt.X());
    }
  }

  // Determine boxes
  for (y = 0; y < BucketKernel().SizeY(); y++) {
    for (int z = 0; z < BucketKernel().SizeZ(); z++) {
      int x_min = min_index(min_index(vcPos[y + 1][z], vcPos[y][z + 1]), min_index(vcPos[y][z], vcPos[y + 1][z + 1]));
      int x_max = max_index(max_index(vcPos[y + 1][z], vcPos[y][z + 1]), max_index(vcPos[y][z], vcPos[y + 1][z + 1]));
      if (x_min != -1) {
        assert(x_max != -1);
        for (int x = x_min; x <= x_max; x++) {
          const CBucketKernel::TBucket &bucket = BucketKernel().Bucket(x, y, z);
          for (CBucketKernel::TBucket::const_iterator it = bucket.begin(); it != bucket.end(); it++)
            stElementIndex.insert(*it);
        }
      }
    }
  }
}

void IElementSet::YDir(const IPlane &Plane, std::set<int> &stElementIndex) const {
  // Calculate postions
  std::vector<std::vector<int>> vcPos(BucketKernel().SizeX() + 1);
  double dEdgeSize = BucketKernel().EdgeSize();
  int x;
  for (x = 0; x <= BucketKernel().SizeX(); x++) {
    vcPos[x].resize(BucketKernel().SizeZ() + 1);
    for (int z = 0; z <= BucketKernel().SizeZ(); z++) {
      double dX = BucketKernel().Min().X() + x * dEdgeSize;
      double dZ = BucketKernel().Min().Z() + z * dEdgeSize;
      CPoint pt = Plane.Intersection(
          geo::CLine(geo::CPoint(dX, BucketKernel().Min().Y(), dZ), geo::CPoint(dX, BucketKernel().Max().Y(), dZ)));

      if (pt.Empty())
        vcPos[x][z] = -1;
      else
        vcPos[x][z] = BucketKernel().BucketY(pt.Y());
    }
  }

  // Determine boxes
  for (x = 0; x < BucketKernel().SizeX(); x++) {
    for (int z = 0; z < BucketKernel().SizeZ(); z++) {
      int y_min = min_index(min_index(vcPos[x + 1][z], vcPos[x][z + 1]), min_index(vcPos[x][z], vcPos[x + 1][z + 1]));
      int y_max = max_index(max_index(vcPos[x + 1][z], vcPos[x][z + 1]), max_index(vcPos[x][z], vcPos[x + 1][z + 1]));
      if (y_min != -1) {
        assert(y_max != -1);
        for (int y = y_min; y <= y_max; y++) {
          const CBucketKernel::TBucket &bucket = BucketKernel().Bucket(x, y, z);
          for (CBucketKernel::TBucket::const_iterator it = bucket.begin(); it != bucket.end(); it++)
            stElementIndex.insert(*it);
        }
      }
    }
  }
}

void IElementSet::ZDir(const IPlane &Plane, std::set<int> &stElementIndex) const {
  // Calculate postions
  std::vector<std::vector<int>> vcPos(BucketKernel().SizeX() + 1);
  double dEdgeSize = BucketKernel().EdgeSize();
  int x;
  for (x = 0; x <= BucketKernel().SizeX(); x++) {
    vcPos[x].resize(BucketKernel().SizeY() + 1);
    for (int y = 0; y <= BucketKernel().SizeY(); y++) {
      double dX = BucketKernel().Min().X() + x * dEdgeSize;
      double dY = BucketKernel().Min().Y() + y * dEdgeSize;
      CPoint pt = Plane.Intersection(
          geo::CLine(geo::CPoint(dX, dY, BucketKernel().Min().Z()), geo::CPoint(dX, dY, BucketKernel().Max().Z())));

      if (pt.Empty())
        vcPos[x][y] = -1;
      else
        vcPos[x][y] = BucketKernel().BucketZ(pt.Z());
    }
  }

  // Determine boxes
  for (x = 0; x < BucketKernel().SizeX(); x++) {
    for (int y = 0; y < BucketKernel().SizeY(); y++) {
      int z_min = min_index(min_index(vcPos[x + 1][y], vcPos[x][y + 1]), min_index(vcPos[x][y], vcPos[x + 1][y + 1]));
      int z_max = max_index(max_index(vcPos[x + 1][y], vcPos[x][y + 1]), max_index(vcPos[x][y], vcPos[x + 1][y + 1]));
      if (z_min != -1) {
        assert(z_max != -1);
        for (int z = z_min; z <= z_max; z++) {
          const CBucketKernel::TBucket &bucket = BucketKernel().Bucket(x, y, z);
          for (CBucketKernel::TBucket::const_iterator it = bucket.begin(); it != bucket.end(); it++)
            stElementIndex.insert(*it);
        }
      }
    }
  }
}

/*!
  This function return indices of the elements which are possibly intersected by the
  plane.
*/
std::set<int> IElementSet::Candidates(const IPlane &Plane) const {
  std::set<int> stRet;
  if (fabs(Plane.Normal().X()) > fabs(Plane.Normal().Y())) {
    if (fabs(Plane.Normal().X()) > fabs(Plane.Normal().Z()))
      XDir(Plane, stRet);
    else
      ZDir(Plane, stRet);
  } else {
    if (fabs(Plane.Normal().Y()) > fabs(Plane.Normal().Z()))
      YDir(Plane, stRet);
    else
      ZDir(Plane, stRet);
  }

  return stRet;
}

/*!
  This function return indices of the elements which are possibly contained by the
  bounding box defined by ptMin and ptMax.
*/
std::set<int> IElementSet::Candidates(const IPoint &ptMin, const IPoint &ptMax) const {
  if (m_pKernel == 0)
    CreateBuckets();
  assert(m_pKernel);

  std::set<int> ret;
  int min_x = m_pKernel->BucketX(ptMin.X());
  int min_y = m_pKernel->BucketY(ptMin.Y());
  int min_z = m_pKernel->BucketZ(ptMin.Z());
  int max_x = m_pKernel->BucketX(ptMax.X());
  int max_y = m_pKernel->BucketY(ptMax.Y());
  int max_z = m_pKernel->BucketZ(ptMax.Z());
  for (int x = min_x; x <= max_x; x++)
    for (int y = min_y; y <= max_y; y++)
      for (int z = min_z; z <= max_z; z++) {
        const CBucketKernel::TBucket &bucket = m_pKernel->Bucket(x, y, z);
        for (CBucketKernel::TBucket::const_iterator it = bucket.begin(); it != bucket.end(); it++) {
          ret.insert(*it);
        }
      }
  return ret;
}

/*!
  This function return indices of the elements which can possibly contains the point.
*/
const std::set<int> &IElementSet::Candidates(const IPoint &point) const {
  if (m_pKernel == 0)
    CreateBuckets();
  assert(m_pKernel);

  int x = m_pKernel->BucketX(point.X());
  int y = m_pKernel->BucketY(point.Y());
  int z = m_pKernel->BucketZ(point.Z());

  return m_pKernel->Bucket(x, y, z);
}

void IElementSet::Rotate(const IVector &vec, const double &dAngleDeg) {
  double dSinAng = sin(DEG_TO_RAD * dAngleDeg);
  double dCosAng = cos(DEG_TO_RAD * dAngleDeg);

  for (int i = 0; i < PointSize(); i++) {
    PointAt(i).Rotate(vec, dSinAng, dCosAng);
  }

  InvalidateCache();
}

void IElementSet::Move(const IVector &vec) {
  for (int i = 0; i < PointSize(); i++) {
    PointAt(i).Move(vec);
  }

  InvalidateCache();
}

void IElementSet::Transform(const IMatrix &matrix) {
  for (int i = 0; i < PointSize(); i++) {
    PointAt(i).Transform(matrix);
  }

  InvalidateCache();
}

CPoint IElementSet::Max() const {
  AssertValid();

  if (PointSize() == 0)
    return CPoint();

  if (m_min.Empty() || m_max.Empty())
    CalcMinMax();

  return m_max;
}

CPoint IElementSet::Min() const {
  AssertValid();

  if (PointSize() == 0)
    return CPoint();

  if (m_min.Empty() || m_max.Empty())
    CalcMinMax();

  return m_min;
}

bool IElementSet::Empty() const { return (ElementSize() == 0); }

std::vector<int> IElementSet::ElementsAt(const IPoint &point) const {
  std::vector<int> vcRet;

  ICacheInterface &ci = const_cast<IElementSet &>(*this).CacheInterface();

  if (ci.PointMap(point, vcRet))
    return vcRet;

  // do we know the elements up-front?
  const CElementPoint *pElementPoint = dynamic_cast<const CElementPoint *>(&point);
  if (pElementPoint) {
    const std::set<const IElement *> &stElements = pElementPoint->Elements();

    // wjrx mantis 3480
    //
    std::set<const IElement *>::const_iterator it;
    for (it = stElements.begin(); it != stElements.end(); ++it) {
      if ((*it)->IndexingElementSet() == this)
        vcRet.push_back((*it)->Index());
    }
    if (!vcRet.empty()) {
      ci.SetPointMap(point, vcRet);
      return vcRet;
    }
  }

  // Contains function with edge faces
  const IPoint &min = Min();
  const IPoint &max = Max();
  if ((point.X() < min.X() - EPS && point.Y() < min.Y() - EPS && point.Z() < min.Z() - EPS) ||
      (point.X() > max.X() + EPS && point.Y() > max.Y() + EPS && point.Z() > max.Z() + EPS)) {
    ci.SetPointMap(point, vcRet);
    return vcRet;
  }

  const std::set<int> &bucket = Candidates(point);
  for (std::set<int>::const_iterator it = bucket.begin(); it != bucket.end(); ++it) {
    if (Element(*it).Contains(point, true))
      vcRet.push_back(*it);
  }

  ci.SetPointMap(point, vcRet);
  return vcRet;
}

bool IElementSet::InBoundingBox(const IPoint &point, double eps) const {
  if (m_min.Empty() || m_max.Empty())
    CalcMinMax();

  if (point.X() < m_min.X() - eps)
    return false;
  if (point.Y() < m_min.Y() - eps)
    return false;
  if (point.Z() < m_min.Z() - eps)
    return false;
  if (point.X() > m_max.X() + eps)
    return false;
  if (point.Y() > m_max.Y() + eps)
    return false;
  if (point.Z() > m_max.Z() + eps)
    return false;

  return true;
}

std::vector<const IPoint *> IElementSet::NearestPoint(const IPoint &point) const {
  assert(ElementSize() > 0);
  std::vector<const IPoint *> vcRet;

  if (m_pKernel == 0)
    CreateBuckets();
  assert(m_pKernel);

  int x = m_pKernel->BucketX(point.X());
  int y = m_pKernel->BucketY(point.Y());
  int z = m_pKernel->BucketZ(point.Z());

  // Extend bucket till we find elements. Note that this can never
  // result in an infinite loop because there are elements in the
  // elementset.
  int nExpand = 0;
  CSetExpandCallBack<int> call_back;
  while (call_back.Bucket().size() == 0) {
    m_pKernel->Expand(call_back, x, y, z, nExpand);
    nExpand++;
  }

  // Get all the nodes in a set
  typedef std::set<const IPoint *, ICoordinate::CCoordinateLess> TNodeSet;
  TNodeSet stPoint;
  for (CBucketKernel::TBucket::const_iterator it = call_back.Bucket().begin(); it != call_back.Bucket().end(); it++) {
    for (int nNode = 0; nNode < Element(*it).NrOfPoints(); nNode++)
      stPoint.insert(&Element(*it).Point(nNode));
  }

  assert(stPoint.size() > 0);

  // Determine the minimum distance
  TNodeSet::iterator it_point = stPoint.begin();
  double dMinimumDistance = DBL_MAX;
  while (it_point != stPoint.end()) {
    double dDistance = (*it_point)->SquareDistance(point);
    if (dDistance < dMinimumDistance) {
      vcRet.clear();
      dMinimumDistance = dDistance;
    }
    if (dDistance <= dMinimumDistance)
      vcRet.push_back(*it_point);
    it_point++;
  }

  return vcRet;
}

void IElementSet::CalcMinMax() const {
  assert(PointSize() > 0);
  assert(m_min.Empty() && m_max.Empty());
  m_min = Point(0);
  m_max = Point(0);

  for (int i = 1; i < PointSize(); i++) {
    m_min = m_min.Min(Point(i));
    m_max = m_max.Max(Point(i));
  }
}

IElementSet::TElementNodeVec IElementSet::ElementsAt(int nNodeIndex) const {
  std::vector<int> vcElements = ElementsAt(Point(nNodeIndex));
  assert(vcElements.size() > 0);

  TElementNodeVec ret;

  for (size_t i = 0; i < vcElements.size(); i++) {
    const IElement &el = Element(vcElements[i]);
    int idx = -1;
    for (int j = 0; j < el.NrOfPoints(); j++) {
      if (el.Point(j) == Point(nNodeIndex)) {
        idx = j;
        break;
      }
    }
    assert(idx >= 0);

    ret.push_back(std::make_pair(&Element(vcElements[i]), idx));
  }

  return ret;
}

/*!
  The generic function Contains is only implemented for the case that the edge is included.
  Derived classes can implement the contain function excluding the edge. When a derived class
  does implement the contain function excludiing edge an assert will fail.
*/
bool IElementSet::Contains(const IPoint &point, bool bIncludeEdge) const {
  assert(bIncludeEdge);

  const std::set<int> &bucket = Candidates(point);

  for (std::set<int>::const_iterator it = bucket.begin(); it != bucket.end(); it++) {
    if (Element(*it).Contains(point, true))
      return true;
  }

  return false;
}

ICacheInterface &IElementSet::CacheInterface() { return *m_cache_interface; }

void IElementSet::CacheInterface(ICacheInterface &cache_interface) { m_cache_interface = &cache_interface; }

const CBucketKernel &IElementSet::BucketKernel() const {
  if (m_pKernel == 0)
    CreateBuckets();
  assert(m_pKernel);
  return *m_pKernel;
}

// returns whether the point is one of the nodes in the set
bool IElementSet::KnownPoint(const IPoint &point) const {
  std::vector<int> vcElements = ElementsAt(point);

  for (size_t i = 0; i < vcElements.size(); i++) {
    const IElement &element = Element(vcElements[i]);
    int j;
    for (j = 0; j < element.NrOfPoints(); j++) {
      if (&element.Point(j) == &point)
        return true;
    }
  }

  return false;
}

} // namespace geo
