// IPoint.h: interface for the IPoint class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPOINT_H__A587965A_F31F_4FE7_8FD5_9ACF82066F9E__INCLUDED_)
#define AFX_IPOINT_H__A587965A_F31F_4FE7_8FD5_9ACF82066F9E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ICoordinate.h"
#include "DispatchVisitorBase.h"

#include "GeometryExports.h"

namespace geo{


// Copyright (c) 2011 TNO DIANA BV                              Confidential
// Description : The IPoint interface represents a geometrical point in 3D space
// Status      : Finished (100%)
// Remarks     : See IPoint,ICoordinate,IVector
// Last review : 27-03-2002
class CPoint;

class GEOMETRY_EXPORT  IPoint : public ICoordinate
{
protected:
  IPoint();
public:
  
  virtual CPoint Min() const;
  virtual CPoint Max() const;
  
  double SquareDistance(const IPoint &point) const;
  double Distance(const IPoint &point) const;

  // Alias to accommodate divergence in our and Diana's code (CM2-related)
  double DistanceSquared(const IPoint& point) const { return SquareDistance(point); }

  //Determine the minimum values of the coordinates of two 
  //points. Input and return value can be an undefined 
  //point.
  CPoint Min(const IPoint &rhs) const;

  //Determine the maximum values of the coordinates of two 
  //points. Input and return value can be an undefined 
  //point.
  CPoint Max(const IPoint &rhs) const;

  virtual void Mirror(const geo::IPlane& plane);

  //##ModelId=3A90DFD301FE
  CPoint operator+(const ICoordinate &rhs) const;
  CPoint operator-(const ICoordinate &rhs) const;
  CPoint operator*(const double &val) const;
  CPoint operator/(const double &val) const;
  IPoint &operator+=(const ICoordinate &rhs);
  IPoint &operator-=(const ICoordinate &rhs);

  virtual bool Visit(IVisitor &visitor) { return visitor.HandlePoint(*this); }
};

GEOMETRY_EXPORT CPoint operator*(const double &val,const IPoint& rhs);

}

#endif // !defined(AFX_IPOINT_H__A587965A_F31F_4FE7_8FD5_9ACF82066F9E__INCLUDED_)
