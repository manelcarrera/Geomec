/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// NearestNeighbour.cpp: implementation of the CNearestNeighbour class.
//
//////////////////////////////////////////////////////////////////////
#include "NearestNeighbour.h"
#include "BucketKernel.h"
#include "dimple.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace geo {
class CCallBack : public CExpandCallBack<int> {
  double m_dMinDistance;
  CNearestNeighbour::CBucket m_bucket;
  bool m_bXY;
  const IPoint &m_point;
  const CPtrArray<IPoint> &m_array;

public:
  // Construction
  CCallBack(const CPtrArray<IPoint> &array, const IPoint &point, bool bXY)
      : m_dMinDistance(DBL_MAX), m_bXY(bXY), m_point(point), m_array(array) {}
  const double &Distance() const { return m_dMinDistance; }
  const CNearestNeighbour::CBucket &Bucket() const { return m_bucket; }

  virtual void AddBucket(const TBucket &bucket) {
    for (TBucket::const_iterator it = bucket.begin(); it != bucket.end(); it++) {
      double dDistance;
      if (m_bXY) {
        double dDeltaX = m_point.X() - m_array.Object(*it).X();
        double dDeltaY = m_point.Y() - m_array.Object(*it).Y();
        dDistance = dDeltaX * dDeltaX + dDeltaY * dDeltaY;
      } else {
        dDistance = m_point.SquareDistance(m_array.Object(*it));
      }

      if (dDistance == m_dMinDistance)
        m_bucket.push_back(*it);
      if (dDistance < m_dMinDistance) {
        m_bucket.clear();
        m_dMinDistance = dDistance;
        m_bucket.push_back(*it);
      }
    }
  }
};

void CNearestNeighbour::MapPointOnBucket(int nPointIndex) const {
  assert(m_pKernel);
  IPoint &point = (IPoint &)ObjectBase(nPointIndex);
  int x = m_pKernel->BucketX(point.X());
  int y = m_pKernel->BucketY(point.Y());
  int z = m_pKernel->BucketZ(point.Z());
  m_pKernel->Insert(x, y, z, nPointIndex);
}

bool CNearestNeighbour::BucketsCreated() const { return m_pKernel != 0; }

void CNearestNeighbour::CreateBuckets() const {
  assert(!BucketsCreated());

  assert(m_dPointDensity > EPS);
  m_pKernel = new CBucketKernel(Min(), Max(), int(Size() / m_dPointDensity));

  // Add the nodes to the buckets
  for (int i = 0; i < Size(); i++)
    MapPointOnBucket(i);
}

void CNearestNeighbour::DestroyBuckets() const {
  delete m_pKernel;
  m_pKernel = 0;
}

CNearestNeighbour::CNearestNeighbour(const double &dPointDensity, int nSize)
    : CPtrArray<IPoint>(nSize), m_pKernel(0), m_dPointDensity(dPointDensity) {
  assert(dPointDensity > 0);
}

CNearestNeighbour::CNearestNeighbour(const CNearestNeighbour &rhs)
    : CPtrArray<IPoint>(rhs), m_pKernel(0), m_dPointDensity(rhs.m_dPointDensity) {}

CNearestNeighbour::~CNearestNeighbour() {
  if (m_pKernel)
    delete m_pKernel;
}

bool CNearestNeighbour::PrepareMapping() {
  if (!BucketsCreated())
    CreateBuckets();

  return true;
}

void CNearestNeighbour::Clear() {
  if (BucketsCreated())
    DestroyBuckets();

  CPtrArray<IPoint>::Clear();
}

void CNearestNeighbour::PushBack(IPoint &pt) {
  if (BucketsCreated()) {
    if ((Max() < pt) || (pt < Min()))
      DestroyBuckets();
    else {
      int nIndex = Size();
      CPtrArray<IPoint>::PushBack(pt);
      MapPointOnBucket(nIndex);
    }
  }

  CPtrArray<IPoint>::PushBack(pt);
}

void CNearestNeighbour::Object(int nIndex, IPoint &object) {
  if (BucketsCreated())
    DestroyBuckets();

  CPtrArray<IPoint>::Object(nIndex, object);
}

void CNearestNeighbour::Invalidate() { DestroyBuckets(); }

int CNearestNeighbour::Dimension() const {
  return 3;
  /*
  int nRet = 0;

  assert(m_vcBucketCoorEnable.size() == 3);

  for(int i = 0; i < 3; i++)
  {
    if(m_vcBucketCoorEnable[i])
      nRet++;
  }

  return nRet;*/
}

const double &CNearestNeighbour::PointDensity() const { return m_dPointDensity; }

int CNearestNeighbour::BucketCount() const {
  if (!BucketsCreated())
    CreateBuckets();

  return m_pKernel->SizeX() * m_pKernel->SizeY() * m_pKernel->SizeZ();
}

CNearestNeighbour::CPointVec CNearestNeighbour::NearestNeighbour(const IPoint &pt, int count /* = 0 */) const {

  CBucket bucket = NearestNeighbourIndex(pt, count);
  CPointVec ret(bucket.size());
  for (int i = 0; i < bucket.size(); i++)
    ret[i] = (IPoint *)(&ObjectBase(bucket[i]));

  return ret;
}

CNearestNeighbour::CBucket CNearestNeighbour::NearestNeighbourIndex(const IPoint &pt, int count /* = 0 */) const {
  CPoint point(pt);
  point = point.Max(Min());
  point = point.Min(Max());

  if (!BucketsCreated())
    CreateBuckets();

  assert(m_pKernel);

  int x = m_pKernel->BucketX(point.X());
  int y = m_pKernel->BucketY(point.Y());
  int z = m_pKernel->BucketZ(point.Z());

  if (count == 0) {
    int nExpand = 0;
    CCallBack cb(*this, point, false);
    while ((cb.Distance() != 0) && ((cb.Bucket().size() == 0) || (nExpand <= 1))) {
      m_pKernel->Expand(cb, x, y, z, nExpand);
      nExpand++;
    }

    return cb.Bucket();
  }

  // Fill the map with square distances
  int nExpand = 0;
  CSetExpandCallBack<int> cb;
  while ((count > cb.Bucket().size()) || (nExpand <= 1)) {
    // Expand in
    m_pKernel->Expand(cb, x, y, z, nExpand);
    nExpand++;
  }

  TDistanceMap mpRet;
  double eps_square = EPS * EPS;
  CBucket ret;

  {
    for (CBucketKernel::TBucket::const_iterator it = cb.Bucket().begin(); it != cb.Bucket().end(); it++) {
      double dsquaredist = pt.SquareDistance(CPtrArray<IPoint>::Object(*it));
      if (!count && dsquaredist < eps_square) // no fabs needed because it's a square
      {
        ret.push_back(*it);
        return ret;
      }
      mpRet.insert(TDistanceMap::value_type(dsquaredist, *it));
    }
  }

  TDistanceMap::iterator it = mpRet.begin();
  assert(count < mpRet.size());

  int i = 0;
  while (i < count) {
    ret.push_back(it->second);
    it++;
    i++;
  }

  assert(ret.size() > 0);
  return ret;
}

CNearestNeighbour::CPointVec CNearestNeighbour::NearestNeighbourXY(const IPoint &pt, int count /* = 0 */) const {

  CBucket bucket = NearestNeighbourXYIndex(pt, count);
  CPointVec ret(bucket.size());
  for (int i = 0; i < bucket.size(); i++)
    ret[i] = (IPoint *)(&ObjectBase(bucket[i]));

  return ret;
}

CNearestNeighbour::CBucket CNearestNeighbour::NearestNeighbourXYIndex(const IPoint &pt, int count /* = 0 */) const {
  CPoint point(pt);
  point = point.Max(Min());
  point = point.Min(Max());

  if (!BucketsCreated())
    CreateBuckets();

  assert(m_pKernel);

  int x = m_pKernel->BucketX(point.X());
  int y = m_pKernel->BucketY(point.Y());

  if (count == 0) {
    int nExpand = 0;
    CCallBack cb(*this, point, true);
    while ((cb.Distance() != 0) && ((cb.Bucket().size() == 0) || (nExpand <= 1))) {
      m_pKernel->ExpandXY(cb, x, y, nExpand);
      nExpand++;
    }

    return cb.Bucket();
  }

  // Fill the map with square distances
  int nExpand = 0;
  CSetExpandCallBack<int> cb;
  while ((count > cb.Bucket().size()) || (nExpand <= 1)) {
    // Expand in
    m_pKernel->ExpandXY(cb, x, y, nExpand);
    nExpand++;
  }

  TDistanceMap mpRet;
  CBucket ret;

  {
    for (CBucketKernel::TBucket::const_iterator it = cb.Bucket().begin(); it != cb.Bucket().end(); it++) {
      // Determine the square distance between X and Y
      double dDeltaX = pt.X() - CPtrArray<IPoint>::Object(*it).X();
      double dDeltaY = pt.Y() - CPtrArray<IPoint>::Object(*it).Y();
      double dDistance = dDeltaX * dDeltaX + dDeltaY * dDeltaY;
      mpRet.insert(TDistanceMap::value_type(dDistance, *it));
    }
  }

  TDistanceMap::iterator it = mpRet.begin();
  if (count == 0) {
    double dDistance = it->first;
    while ((dDistance == it->first) && (it != mpRet.end())) {
      ret.push_back(it->second);
      it++;
    }
  } else {
    assert(count <= mpRet.size());
    int i = 0;
    while (i < count) {
      ret.push_back(it->second);
      it++;
      i++;
    }
  }

  assert(ret.size() > 0);
  return ret;
}

} // namespace geo
