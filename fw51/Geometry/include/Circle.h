//*******************************************************************
//
//  FILE      : Circle.h
//  AUTHOR    : htg
//  PROJECT   : Geometry
//  DATE      : 22.04.2003
//  COMMENTS  : -
// Copyright (c) 2011 TNO DIANA BV                              Confidential
//  STATUS    :
//
//*******************************************************************

// Includes

#if !defined(AFX_CIRCLE_H__98A44FA0_F418_4C5D_9DC8_EED1CF77374F__INCLUDED_)
#define AFX_CIRCLE_H__98A44FA0_F418_4C5D_9DC8_EED1CF77374F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ICircle.h"
#include "Vector.h"

#include "GeometryExports.h"

namespace geo {

//*******************************************************************
//
//  AUTHOR    : htg
//  CLASS     : CCircle
//  COMMENTS  : -
//
//*******************************************************************
class GEOMETRY_EXPORT CCircle : public ICircle {
private:
  CPoint m_MidPoint;
  double m_Radius;
  CVector m_Normal;
  CVector m_StartVector;
  CVector m_EndVector;
  mutable CPoint m_Max, m_Min;

public:
  virtual CPoint Min() const;
  virtual CPoint Max() const;

  CCircle(const ICircle &rhs);
  CCircle(const CCircle &rhs);

  CCircle &operator=(const ICircle &rhs);
  CCircle &operator=(const CCircle &rhs);

  virtual void Rotate(const IVector &vec, const double &dAngleDeg);
  virtual void Move(const IVector &vec);
  virtual void Transform(const IMatrix &matrix);

  // ICircle interface

  CCircle(); // default "empty" constructor

  CCircle(const IPoint &midpoint, const double &radius, const IVector &normal = CVector::Zaxis,
          const IVector &startvector = CVector(), const IVector &endvector = CVector());

  virtual ~CCircle();

  // ICircle interface
  virtual const double &Radius() const { return m_Radius; }
  virtual void Radius(const double &radius) { m_Radius = radius; }

  virtual const IPoint &MidPoint() const { return m_MidPoint; }
  virtual void MidPoint(const IPoint &midpoint) { m_MidPoint = midpoint; }

  virtual const IVector &Normal() const { return m_Normal; }
  virtual void Normal(const IVector &normal) { m_Normal = normal; }

  virtual const IVector &StartVector() const { return m_StartVector; }
  virtual void StartVector(const IVector &startvector) { m_StartVector = startvector; }

  virtual const IVector &EndVector() const { return m_EndVector; }
  virtual void EndVector(const IVector &endvector) { m_EndVector = endvector; }

  static void LineCorner(const ILine &l1, const ILine &l2, const double &radius, CPtrArray<CCircle> &circles);
  static void TrimLinesToCircleCorner(ILine &l1, ILine &l2, CCircle &circle, const double &radius);
};

} // end namespace geo

#endif // !defined(AFX_CIRCLE_H__98A44FA0_F418_4C5D_9DC8_EED1CF77374F__INCLUDED_)
