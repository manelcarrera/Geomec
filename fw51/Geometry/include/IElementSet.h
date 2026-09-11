/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// IElementSet.h: interface for the IElementSet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IELEMENTSET_H__DD555C88_8F5F_41CC_BE85_8E48C23E73DB__INCLUDED_)
#define AFX_IELEMENTSET_H__DD555C88_8F5F_41CC_BE85_8E48C23E73DB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DispatchVisitorBase.h"
#include "ICacheInterface.h"
#include "Point.h"
#include <set>
#include <vector>

#include "GeometryExports.h"

namespace geo {
class IValueSet;
class IElement;
class CBucketKernel;
class IPlane;
class ICacheInterface;

class GEOMETRY_EXPORT IElementSet : public IObject {
protected:
  IElementSet();
  virtual IPoint &PointAt(int nIndex) = 0;
  virtual void InvalidateCache();
  const CBucketKernel &BucketKernel() const;

private:
  mutable CPoint m_min;             // Cached min point
  mutable CPoint m_max;             // Cached max point
  mutable CBucketKernel *m_pKernel; // Cached kernel
  ICacheInterface *m_cache_interface;
  void CalcMinMax() const;
  void CreateBuckets() const;
  void XDir(const IPlane &Plane, std::set<int> &stElementIndex) const;
  void YDir(const IPlane &Plane, std::set<int> &stElementIndex) const;
  void ZDir(const IPlane &Plane, std::set<int> &stElementIndex) const;

public:
  typedef std::pair<const IElement *, int> TElementNode;
  typedef std::vector<TElementNode> TElementNodeVec;

  virtual ~IElementSet();

  virtual bool IsMeshForEntry() const { return false; }

  virtual const IPoint &Point(int nIndex) const = 0;
  virtual int PointSize() const = 0;
  std::vector<const IPoint *> NearestPoint(const IPoint &point) const;

  virtual const IElement &Element(int nIndex) const = 0;
  virtual int ElementSize() const = 0;
  virtual std::vector<int> ElementsAt(const IPoint &point) const;
  bool InBoundingBox(const IPoint &point, double eps = 1E-6) const;
  virtual bool Contains(const IPoint &point, bool bIncludeEdge = true) const;

  virtual ICacheInterface &CacheInterface();
  virtual void CacheInterface(ICacheInterface &cache_interface);

  // returns the candidate elements on certain location
  const std::set<int> &Candidates(const IPoint &point) const;
  std::set<int> Candidates(const IPoint &ptMin, const IPoint &ptMax) const;
  std::set<int> Candidates(const IPlane &Plane) const;

  // returns whether the point is one of the nodes in the set
  virtual bool KnownPoint(const IPoint &point) const;

  // get the node indices connected to the given element
  virtual std::vector<int> Nodes(const IElement &element) const = 0;

  // get the elements and local node indices for the given node index
  virtual TElementNodeVec ElementsAt(int nNodeIndex) const;

  // virtual function implementations from IObject
  virtual void Rotate(const IVector &vec, const double &dAngleDeg);
  virtual void Move(const IVector &vec);
  virtual void Transform(const IMatrix &matrix);
  virtual bool Empty() const;
  virtual CPoint Min() const;
  virtual CPoint Max() const;

  virtual bool Visit(IVisitor &visitor) { return visitor.HandleElementSet(*this); }
};

} // namespace geo

#endif // !defined(AFX_IELEMENTSET_H__DD555C88_8F5F_41CC_BE85_8E48C23E73DB__INCLUDED_)
