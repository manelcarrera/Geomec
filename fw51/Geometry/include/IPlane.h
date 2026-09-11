// IPlane.h: interface for the IPlane class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPLANE_H__3AB62E7D_6A37_4A4C_8D10_DF63F1326965__INCLUDED_)
#define AFX_IPLANE_H__3AB62E7D_6A37_4A4C_8D10_DF63F1326965__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DispatchVisitorBase.h"
#include "IObject.h"

#include "GeometryExports.h"

namespace geo {
class ILine;
class CLine;
class IPoint;
class CPoint;
// Copyright (c) 2011 TNO DIANA BV                              Confidential
// Description : Interface for a 3D plane without borders
// Status      : Finished (100%)
// Remarks     : Dont use max/min for a plane because its
//				 theoretical infinite.
// Last review : 27-03-2002
class GEOMETRY_EXPORT IPlane : public IObject {
public:
  IPlane();

  virtual const IPoint &BasePoint() const = 0;
  virtual void BasePoint(const IPoint &base) = 0;
  virtual const IVector &Normal() const = 0;
  virtual void Normal(const IVector &normal) = 0;

  // returns the line of intersection between two planes
  virtual CLine Intersection(const IPlane &plane) const;

  // returns the point of intersection between a plane and a line.
  // line and plane are theoretiacl infinite,and therfore have an intersection
  // unless they are parallel
  virtual CPoint Intersection(const ILine &line) const;

  // returns the point of the intersection between a plane and a line.
  // This only returns a non-empty point if the line's segment really crosses
  // the plane.
  // It is possible to tell for each of line's endpoint whether it should be
  // included in the test or not
  virtual CPoint RealIntersection(const ILine &line, bool bIncP1, bool bIncP2) const;

  // returns the projected point of a point and the plane
  virtual CPoint Project(const IPoint &pt) const;

  // returns the smallest (e.i. normal) distance of a point and the plane
  virtual double Distance(const IPoint &pt) const;

  // return the smallest (e.i. normal) signed distance of a point and the plane,
  // sign depends on the direction of the normal. (same == +,  other side == -)
  // IMPORTANT: the "bExact" boolean tells the function if the exact distance should be returned.
  // If it is false, the function will not perform a "sqrt" operation in order to get the exact result.
  // since this function is mostly used to see on which side of the plane a point is, the final "sqrt" is not
  // always necessary.
  virtual double SignedDistance(const IPoint &pt, bool bExact = false) const;

  // returns the smallest (e.i. normal) square distance of a point and the plane
  virtual double SquareDistance(const IPoint &pt) const;

  // see if planes are parallel (no intersections)
  virtual bool IsParallel(const IPlane &plane) const;

  // see if a plane and a line are parallel (no intersections)
  virtual bool IsParallel(const ILine &line) const;

  // see if a plane and a vector are parallel
  virtual bool IsParallel(const IVector &vec) const;

  // Interface of IObject ....
  virtual void Rotate(const IVector &vec, const double &dAngleDeg);
  virtual void Move(const IVector &vec);
  virtual void Transform(const IMatrix &matrix);
  virtual void AssertValid() const;
  virtual bool Empty() const;
  virtual CPoint Min() const;
  virtual CPoint Max() const;

  virtual bool Visit(IVisitor &visitor) { return visitor.HandlePlane(*this); }
};

} // namespace geo

#endif // !defined(AFX_IPLANE_H__3AB62E7D_6A37_4A4C_8D10_DF63F1326965__INCLUDED_)
