// plane.h: interface for the plane class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLANE_H__4C0AE824_A0FD_477F_877D_D36986B70E61__INCLUDED_)
#define AFX_PLANE_H__4C0AE824_A0FD_477F_877D_D36986B70E61__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IPlane.h"
#include "Point.h"
#include "Vector.h"

#include "GeometryExports.h"

namespace geo {

// Copyright (c) 2011 TNO DIANA BV                              Confidential
// Description : The default implementation of the IPlane interface
// Status      : Finished (100%)
// Remarks     : See IPlane
// Last review : 27-03-2002
class GEOMETRY_EXPORT CPlane : public IPlane {

private:
  CPoint m_BasePoint;
  CVector m_NormVector;

public:
  CPlane();

  // Construction with one base point and two vectors
  // v2 and v3 are direction vectors
  CPlane(const IPoint &basepoint, const IVector &dir1, const IVector &dir2);

  // Construction with 3 points
  CPlane(const IPoint &p1, const IPoint &p2, const IPoint &p3);

  // construction with a point and a normal vector
  CPlane(const IPoint &basepoint, const IVector &normal);
  CPlane(const CPlane &rhs);
  CPlane(const IPlane &rhs);
  CPlane &operator=(const CPlane &rhs);
  CPlane &operator=(const IPlane &rhs);

  // IPlane Interface
  virtual const IPoint &BasePoint() const;
  virtual void BasePoint(const IPoint &base);
  virtual const IVector &Normal() const;
  virtual void Normal(const IVector &normal);

  virtual void AssertValid() const;

  // the 3 cartisian plane definitions
  static const CPlane PlaneXY;
  static const CPlane PlaneYZ;
  static const CPlane PlaneZX;
};

} // namespace geo

#endif // !defined(AFX_PLANE_H__4C0AE824_A0FD_477F_877D_D36986B70E61__INCLUDED_)
