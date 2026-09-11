//*******************************************************************
//
//  FILE      : ICircle.h
//  AUTHOR    : htg
//  PROJECT   : Geometry
//  DATE      : 22.04.2003
//  COMMENTS  : -
// Copyright (c) 2011 TNO DIANA BV                              Confidential
//  STATUS    :
//
//*******************************************************************

#if !defined(AFX_ICIRCLE_H__460777C5_349A_44AB_9E63_F8D3B8A9326D__INCLUDED_)
#define AFX_ICIRCLE_H__460777C5_349A_44AB_9E63_F8D3B8A9326D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DispatchVisitorBase.h"
#include "IObject.h"
#include "VectorTempl.h"

#include "GeometryExports.h"

namespace geo {
class IPoint;
class ILine;
//*******************************************************************
//
//  AUTHOR    : htg
//  CLASS     : ICircle
//  COMMENTS  : -
//
//*******************************************************************
class GEOMETRY_EXPORT ICircle : public IObject {
public:
  ICircle &operator=(const ICircle &rhs);

  ICircle();
  virtual ~ICircle();

public:
  // IObject interface
  virtual void Rotate(const IVector &vec, const double &dAngleDeg);
  virtual void Move(const IVector &vec);
  virtual void Transform(const IMatrix &matrix);
  virtual void AssertValid() const;
  virtual bool Empty() const;
  virtual CPoint Min() const;
  virtual CPoint Max() const;

  // ICircle interface

  virtual const double &Radius() const = 0;
  virtual void Radius(const double &radius) = 0;

  virtual const IPoint &MidPoint() const = 0;
  virtual void MidPoint(const IPoint &midpoint) = 0;

  virtual const IVector &Normal() const = 0;
  virtual void Normal(const IVector &normal) = 0;

  virtual const IVector &StartVector() const = 0;
  virtual void StartVector(const IVector &startvector) = 0;

  virtual const IVector &EndVector() const = 0;
  virtual void EndVector(const IVector &endvector) = 0;

  CPoint StartPoint() const;
  CPoint EndPoint() const;
  double CircleAngleDeg() const;

  // Get points on Circumference
  void GetPointsOnCircumference(CPtrArray<IPoint> &points, long iNumberOfPoints = 10) const;
  void GetPointsOnCircumference(CArray<CPoint> &points, long iNumberOfPoints = 10) const;

  // returning 0,1 or 2 intersections
  void IntersectionWithLineInPlane(const ILine &lines, CPtrArray<IPoint> &points) const;

  virtual bool Visit(IVisitor &visitor) { return visitor.HandleCircle(*this); }
};

} // end namespace geo

#endif // !defined(AFX_ICIRCLE_H__460777C5_349A_44AB_9E63_F8D3B8A9326D__INCLUDED_)
