/* Confidential Source Code                 Copyright (c) 2012 TNO DIANA BV */
// BucketKernel.h: interface for the CBucketKernel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BUCKETKERNEL_H__92464FE0_9463_4008_BDFA_4E580DE74795__INCLUDED_)
#define AFX_BUCKETKERNEL_H__92464FE0_9463_4008_BDFA_4E580DE74795__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ILine.h"
#include "Point.h"
#include "dimple.h"
#include <set>
#include <vector>

#include "GeometryExports.h"

namespace geo {
/*!
  \class CExpandCallBack
  \brief The class implements the callback function for the expand function of the bucket kernel.

  Everytime a bucket is evaluated during the expand the AddBucket is called.
*/
template <class T> class CExpandCallBack {
public:
  typedef typename std::set<T> TBucket;
  CExpandCallBack() {}
  virtual ~CExpandCallBack() {}
  virtual void AddBucket(const TBucket &bucket) = 0;
};

/*!
  \class CSetExpandCallBack
  \brief The class colllect all the objects during the expand function of the bucket kernel.

  During construction a bucket can be passed.
*/
template <class T> class CSetExpandCallBack : public CExpandCallBack<T> {
public:
  typedef typename std::set<T> TBucket;

private:
  TBucket m_bucket;

public:
  CSetExpandCallBack(const TBucket &bucket) : m_bucket(bucket) {}
  CSetExpandCallBack() {}
  virtual void AddBucket(const TBucket &bucket) {
    typename TBucket::const_iterator it;
    for (it = bucket.begin(); it != bucket.end(); it++)
      m_bucket.insert(*it);
  }

  const TBucket &Bucket() const { return m_bucket; }
};

template <class T> class CBucketKernelTempl {
public:
  typedef typename std::set<T> TBucket;
  typedef CExpandCallBack<T> TCallBack;

private:
  int m_size[3];                // Size of bucket array
  std::vector<TBucket> m_array; // The bucket array
  double m_dEdgeSize;
  CPoint m_min;
  CPoint m_max;

public:
  // Construction
  CBucketKernelTempl();
  CBucketKernelTempl(const IPoint &min, int size_x, int size_y, int size_z, const double &dEdgeSize);
  CBucketKernelTempl(const IPoint &min, const IPoint &max, int nr_of_buckets);

  // Sizes of the bucket system
  int SizeX() const { return m_size[0]; }
  int SizeY() const { return m_size[1]; }
  int SizeZ() const { return m_size[2]; }
  const double &EdgeSize() const { return m_dEdgeSize; }

  // Resize of the bucket system
  void Resize(const IPoint &orgin, int size_x, int size_y, int size_z, const double &dEdgeSize);
  void Resize(const IPoint &min, const IPoint &max, int nr_of_buckets);

  // Bucket functions
  const TBucket &Bucket(int size_x, int size_y, int size_z) const;
  const TBucket &Bucket(const IPoint &point) const;
  TBucket Buckets(const IPoint &min, const IPoint &max) const;
  TBucket Buckets(const ILine &line) const;
  bool Insert(int size_x, int size_y, int size_z, T &value);
  bool Insert(const IPoint &point, T &value);
  bool Insert(const IPoint &min, const IPoint &max, T &value);
  bool Remove(int size_x, int size_y, int size_z, T &value);
  bool Remove(const IPoint &point, T &value);
  bool Remove(const IPoint &min, const IPoint &max, T &value);
  int BucketX(const double &dX) const;
  int BucketY(const double &dY) const;
  int BucketZ(const double &dZ) const;

  void Expand(TCallBack &ec,      // Call back class
              int size_x,         // X intial bucket x coordinate
              int size_y,         // Y intial bucket y coordinate
              int size_z,         // Z intial bucket z coordinate
              int nExpand) const; // 3D expansion in from x, y, z bucket coordinate

  void ExpandXY(TCallBack &ec,      // Call back class
                int size_x,         // X intial bucket x coordinate
                int size_y,         // Y intial bucket y coordinate
                int nExpand) const; // 2D expansion in from x, y bucket coordinatea

  void Expand(TCallBack &ec,       // Call back class
              const IPoint &point, // Mid point of expansion zone
              int nExpand) const;  // 3D expansion in from x, y, z bucket coordinate

  void ExpandXY(TCallBack &ec,       // Call back class
                const IPoint &point, // Mid point of expansion zone (Z is ignored)
                int nExpand) const;  // 2D expansion in from x, y bucket coordinatea

  // Min and max
  const IPoint &Min() const { return m_min; }
  const IPoint &Max() const { return m_max; }

  // Function keeps an integer value between min and max
  int Limit(int nMin, int nValue, int nMax) const;
};

// To be backward compatible
class CBucketKernel : public CBucketKernelTempl<int> {
public:
  CBucketKernel(const IPoint &min, const IPoint &max, int nr_of_buckets)
      : CBucketKernelTempl<int>(min, max, nr_of_buckets) {}
  CBucketKernel(const IPoint &orgin, int size_x, int size_y, int size_z, const double &dEdgeSize)
      : CBucketKernelTempl<int>(orgin, size_x, size_y, size_z, dEdgeSize) {}
};

template <class T> CBucketKernelTempl<T>::CBucketKernelTempl() : m_dEdgeSize(-1) {
  m_size[0] = 0;
  m_size[1] = 0;
  m_size[2] = 0;
}

template <class T>
CBucketKernelTempl<T>::CBucketKernelTempl(const IPoint &orgin, int size_x, int size_y, int size_z,
                                          const double &dEdgeSize) {
  Resize(orgin, size_x, size_y, size_z, dEdgeSize);
}

template <class T> CBucketKernelTempl<T>::CBucketKernelTempl(const IPoint &min, const IPoint &max, int nr_of_buckets) {
  Resize(min, max, nr_of_buckets);
}

template <class T>
void CBucketKernelTempl<T>::Resize(const IPoint &min, int size_x, int size_y, int size_z, const double &dEdgeSize) {
  assert(size_x > 0);
  assert(size_y > 0);
  assert(size_z > 0);
  assert(dEdgeSize > 0);
  m_min = min;
  m_size[0] = size_x;
  m_size[1] = size_y;
  m_size[2] = size_z;
  m_dEdgeSize = dEdgeSize;
  m_array.clear();
  m_array.resize(size_x * size_y * size_z);
  m_max = CPoint(min.X() + size_x * dEdgeSize, min.Y() + size_y * dEdgeSize, min.Z() + size_z * dEdgeSize);
}

template <class T> void CBucketKernelTempl<T>::Resize(const IPoint &min, const IPoint &max, int nr_of_buckets) {
  m_min = min;
  CPoint range = max - min;
  assert(range.X() >= 0);
  assert(range.Y() >= 0);
  assert(range.Z() >= 0);

  // Determine dimension of coordinate system
  int nDim = 0;
  double dProduct = 1.0;
  if (fabs(range.X()) > IObject::compareTolerance()) {
    nDim++;
    dProduct *= range.X();
  }
  if (fabs(range.Y()) > IObject::compareTolerance()) {
    nDim++;
    dProduct *= range.Y();
  }
  if (fabs(range.Z()) > IObject::compareTolerance()) {
    nDim++;
    dProduct *= range.Z();
  }
  //	assert(nDim > 0);

  // wjrx mantis 2911
  // Avoid division by zero for nDim=0;
  //
  if (nDim == 0) {
    m_size[0] = 1;
    m_size[1] = 1;
    m_size[2] = 1;
  } else {
    // Determine edge size of the bucket system
    m_dEdgeSize = pow(dProduct / nr_of_buckets, 1.0 / nDim);
    m_size[0] = (int)ceil(range.X() / m_dEdgeSize);
    m_size[1] = (int)ceil(range.Y() / m_dEdgeSize);
    m_size[2] = (int)ceil(range.Z() / m_dEdgeSize);
    if (m_size[0] == 0)
      m_size[0] = 1;
    if (m_size[1] == 0)
      m_size[1] = 1;
    if (m_size[2] == 0)
      m_size[2] = 1;
  }
  m_array.clear();
  m_array.resize(m_size[0] * m_size[1] * m_size[2]);
  m_max =
      CPoint(min.X() + m_size[0] * m_dEdgeSize, min.Y() + m_size[1] * m_dEdgeSize, min.Z() + m_size[2] * m_dEdgeSize);
}

template <class T>
const typename CBucketKernelTempl<T>::TBucket &CBucketKernelTempl<T>::Bucket(int size_x, int size_y, int size_z) const {
  assert(size_x >= 0);
  assert(size_y >= 0);
  assert(size_z >= 0);
  assert(size_x < m_size[0]);
  assert(size_y < m_size[1]);
  assert(size_z < m_size[2]);
  int nPos = size_x * m_size[1] * m_size[2] + size_y * m_size[2] + size_z;
  assert(nPos >= 0);
  assert(nPos < m_array.size());
  return m_array[nPos];
}

template <class T>
const typename CBucketKernelTempl<T>::TBucket &CBucketKernelTempl<T>::Bucket(const IPoint &point) const {
  return Bucket(BucketX(point.X()), BucketY(point.Y()), BucketZ(point.Z()));
}

template <class T>
typename CBucketKernelTempl<T>::TBucket CBucketKernelTempl<T>::Buckets(const IPoint &min, const IPoint &max) const {
  CPoint range = max - min;
  assert(range.X() >= 0);
  assert(range.Y() >= 0);
  assert(range.Z() >= 0);
  CSetExpandCallBack<T> call_back;

  int min_x = BucketX(min.X());
  int min_y = BucketY(min.Y());
  int min_z = BucketZ(min.Z());

  int max_x = BucketX(max.X());
  int max_y = BucketY(max.Y());
  int max_z = BucketZ(max.Z());

  for (int x = min_x; x <= max_x; x++)
    for (int y = min_y; y <= max_y; y++)
      for (int z = min_z; z <= max_z; z++)
        call_back.AddBucket(Bucket(x, y, z));
  return call_back.Bucket();
}

template <class T> typename CBucketKernelTempl<T>::TBucket CBucketKernelTempl<T>::Buckets(const ILine &line) const {
  return Buckets(line.Min(), line.Max());
}

template <class T> bool CBucketKernelTempl<T>::Insert(int size_x, int size_y, int size_z, T &value) {
  assert(size_x >= 0);
  assert(size_y >= 0);
  assert(size_z >= 0);
  assert(size_x < m_size[0]);
  assert(size_y < m_size[1]);
  assert(size_z < m_size[2]);
  int nPos = size_x * m_size[1] * m_size[2] + size_y * m_size[2] + size_z;
  assert(nPos >= 0);
  assert(nPos < m_array.size());
  return m_array[nPos].insert(value).second;
}

template <class T> bool CBucketKernelTempl<T>::Insert(const IPoint &point, T &value) {
  return Insert(BucketX(point.X()), BucketY(point.Y()), BucketZ(point.Z()), value);
}

template <class T> bool CBucketKernelTempl<T>::Insert(const IPoint &min, const IPoint &max, T &value) {
  CPoint range = max - min;
  assert(range.X() >= 0);
  assert(range.Y() >= 0);
  assert(range.Z() >= 0);

  bool bRet = false;

  int min_x = BucketX(min.X());
  int min_y = BucketY(min.Y());
  int min_z = BucketZ(min.Z());

  int max_x = BucketX(max.X());
  int max_y = BucketY(max.Y());
  int max_z = BucketZ(max.Z());

  for (int x = min_x; x <= max_x; x++)
    for (int y = min_y; y <= max_y; y++)
      for (int z = min_z; z <= max_z; z++)
        if (Insert(x, y, z, value))
          bRet = true;

  return bRet;
}

template <class T> bool CBucketKernelTempl<T>::Remove(int size_x, int size_y, int size_z, T &value) {
  assert(size_x >= 0);
  assert(size_y >= 0);
  assert(size_z >= 0);
  assert(size_x < m_size[0]);
  assert(size_y < m_size[1]);
  assert(size_z < m_size[2]);
  int nPos = size_x * m_size[1] * m_size[2] + size_y * m_size[2] + size_z;
  assert(nPos >= 0);
  assert(nPos < m_array.size());
  return m_array[nPos].erase(value) == 1;
}

template <class T> bool CBucketKernelTempl<T>::Remove(const IPoint &point, T &value) {
  return Remove(BucketX(point.X()), BucketY(point.Y()), BucketZ(point.Z()), value);
}

template <class T> bool CBucketKernelTempl<T>::Remove(const IPoint &min, const IPoint &max, T &value) {
  CPoint range = max - min;
  assert(range.X() >= 0);
  assert(range.Y() >= 0);
  assert(range.Z() >= 0);

  bool bRet = false;

  int min_x = BucketX(min.X());
  int min_y = BucketY(min.Y());
  int min_z = BucketZ(min.Z());

  int max_x = BucketX(max.X());
  int max_y = BucketY(max.Y());
  int max_z = BucketZ(max.Z());

  for (int x = min_x; x <= max_x; x++)
    for (int y = min_y; y <= max_y; y++)
      for (int z = min_z; z <= max_z; z++)
        if (Remove(x, y, z, value))
          bRet = true;

  return bRet;
}

template <class T> int CBucketKernelTempl<T>::BucketX(const double &dX) const {
  int ret = (int)floor((dX - Min().X()) / m_dEdgeSize);
  return Limit(0, ret, SizeX() - 1);
}

template <class T> int CBucketKernelTempl<T>::BucketY(const double &dY) const {
  int ret = (int)floor((dY - Min().Y()) / m_dEdgeSize);
  return Limit(0, ret, SizeY() - 1);
}

template <class T> int CBucketKernelTempl<T>::BucketZ(const double &dZ) const {
  int ret = (int)floor((dZ - Min().Z()) / m_dEdgeSize);
  return Limit(0, ret, SizeZ() - 1);
}

template <class T> int CBucketKernelTempl<T>::Limit(int nMin, int nValue, int nMax) const {
  if (nValue < nMin)
    return nMin;
  if (nValue > nMax)
    return nMax;
  return nValue;
}

template <class T> void CBucketKernelTempl<T>::Expand(TCallBack &ec, const IPoint &point, int nExpand) const {
  Expand(ec, BucketX(point.X()), BucketY(point.Y()), BucketZ(point.Z()), nExpand);
}

template <class T> void CBucketKernelTempl<T>::ExpandXY(TCallBack &ec, const IPoint &point, int nExpand) const {
  ExpandXY(ec, BucketX(point.X()), BucketY(point.Y()), nExpand);
}

template <class T>
void CBucketKernelTempl<T>::Expand(TCallBack &ec,     // Call back class
                                   int size_x,        // X intial bucket x coordinate
                                   int size_y,        // Y intial bucket y coordinate
                                   int size_z,        // Z intial bucket z coordinate
                                   int nExpand) const // 3D expansion in from x, y, z bucket coordinatea
{
  assert(nExpand >= 0);
  assert(size_x < SizeX());
  assert(size_y < SizeY());
  assert(size_z < SizeZ());

  if (nExpand == 0) {
    ec.AddBucket(Bucket(size_x, size_y, size_z));
    return;
  }

  // Generate the sizes
  int x_min = size_x - nExpand;
  int x_max = size_x + nExpand;
  int y_min = size_y - nExpand;
  int y_max = size_y + nExpand;
  int z_min = size_z - nExpand;
  int z_max = size_z + nExpand;

  // Delimit
  x_min = Limit(0, x_min, SizeX() - 1);
  y_min = Limit(0, y_min, SizeY() - 1);
  z_min = Limit(0, z_min, SizeZ() - 1);
  x_max = Limit(0, x_max, SizeX() - 1);
  y_max = Limit(0, y_max, SizeY() - 1);
  z_max = Limit(0, z_max, SizeZ() - 1);

  // Walk over outer limits of expansion
  for (int iX = x_min; iX <= x_max; iX++) {
    for (int iY = y_min; iY <= y_max; iY++) {
      ec.AddBucket(Bucket(iX, iY, z_min)); // Z_MIN Surface
      ec.AddBucket(Bucket(iX, iY, z_max)); // Z_MAX Surface
    }
  }

  for (int iZ = z_min + 1; iZ < z_max; iZ++) {
    for (int iX = x_min; iX <= x_max; iX++) {
      ec.AddBucket(Bucket(iX, y_min, iZ)); // Y_MIN Surface
      ec.AddBucket(Bucket(iX, y_max, iZ)); // Y_MAX Surface
    }

    for (int iY = y_min + 1; iY < y_max + 1; iY++) {
      ec.AddBucket(Bucket(x_min, iY, iZ)); // X_MIN Surface
      ec.AddBucket(Bucket(x_max, iY, iZ)); // X_MAX Surface
    }
  }
}

template <class T>
void CBucketKernelTempl<T>::ExpandXY(TCallBack &ec,     // Call back class
                                     int size_x,        // X intial bucket x coordinate
                                     int size_y,        // Y intial bucket y coordinate
                                     int nExpand) const // 2D expansion in from x,y bucket coordinate
{
  assert(nExpand >= 0);
  assert(size_x < SizeX());
  assert(size_y < SizeY());

  if (nExpand == 0) {
    for (int size_z = 0; size_z < SizeZ(); size_z++)
      ec.AddBucket(Bucket(size_x, size_y, size_z));
    return;
  }

  // Generate the sizes
  int x_min = size_x - nExpand;
  int x_max = size_x + nExpand;
  int y_min = size_y - nExpand;
  int y_max = size_y + nExpand;

  // Delimit
  x_min = Limit(0, x_min, SizeX() - 1);
  y_min = Limit(0, y_min, SizeY() - 1);
  x_max = Limit(0, x_max, SizeX() - 1);
  y_max = Limit(0, y_max, SizeY() - 1);

  // Walk over outer limits of expansion
  for (int iZ = 0; iZ < SizeZ(); iZ++) {
    for (int iX = x_min; iX <= x_max; iX++) {
      ec.AddBucket(Bucket(iX, y_min, iZ)); // Y_MIN Surface
      ec.AddBucket(Bucket(iX, y_max, iZ)); // Y_MAX Surface
    }

    for (int iY = y_min + 1; iY < y_max + 1; iY++) {
      ec.AddBucket(Bucket(x_min, iY, iZ)); // X_MIN Surface
      ec.AddBucket(Bucket(x_max, iY, iZ)); // X_MAX Surface
    }
  }
}

GEOMETRY_EXPORT_TEMPLATE template class GEOMETRY_EXPORT CSetExpandCallBack<CPoint>;
GEOMETRY_EXPORT_TEMPLATE template class GEOMETRY_EXPORT CBucketKernelTempl<CPoint>;

} // namespace geo

#endif // !defined(AFX_BUCKETKERNEL_H__92464FE0_9463_4008_BDFA_4E580DE74795__INCLUDED_)
