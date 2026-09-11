 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// IPoint.cpp: implementation of the IPoint class.
//
//////////////////////////////////////////////////////////////////////
#include "dimple.h"
#include <cassert>
#include <algorithm>
#include <cmath>
#include "Point.h"
#include "IPlane.h"
#include "Vector.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



namespace geo {
IPoint::IPoint()
{

}




CPoint IPoint::Min() const
{
  return ICoordinate::Min();
}

CPoint IPoint::Max() const
{
  return ICoordinate::Max();
}
  
double IPoint::SquareDistance(const IPoint &pt) const
{
  assert(!Empty());
  assert(!pt.Empty());
  CPoint diff = *this - pt;
  return diff.X() * diff.X() + diff.Y() * diff.Y() + diff.Z() * diff.Z();
}


//##ModelId=3B4C71CB0301
double IPoint::Distance(const IPoint &pt) const
{
  return sqrt(SquareDistance(pt));
}


//##ModelId=3A90DFD301F0
CPoint IPoint::operator*(const double &val) const
{
  AssertValid();
  return CPoint( X() * val,
               Y() * val,
           Z() * val);
}


CPoint IPoint::operator/(const double &val) const
{
  AssertValid();
  assert(val != 0.0);
  return CPoint(X() / val,
          Y() / val,
          Z() / val);
}


//##ModelId=3A90DFD301FE
CPoint IPoint::operator +(const ICoordinate &rhs) const
{
  AssertValid();
  rhs.AssertValid();
  return CPoint(X() + rhs.X(),
          Y() + rhs.Y(),
              Z() + rhs.Z());
}


//##ModelId=3A90DFD30201
CPoint IPoint::operator -(const ICoordinate &rhs) const
{
  AssertValid();
  rhs.AssertValid();
  return CPoint(X() - rhs.X(),
          Y() - rhs.Y(),
              Z() - rhs.Z());
}


//##ModelId=3A90DFD301A0
CPoint IPoint::Max(const IPoint &rhs) const
{
  if(Empty())
    return CPoint(rhs);
  
  if(rhs.Empty())
    return CPoint(*this);
  

  // construct without factory
  return CPoint( std::max(X(), rhs.X()),				
           std::max(Y(), rhs.Y()),				
           std::max(Z(), rhs.Z()));
}

//##ModelId=3A90DFD301A3
CPoint IPoint::Min(const IPoint &rhs) const
{
  if(Empty())
    return CPoint(rhs);
  
  if(rhs.Empty())
    return CPoint(*this);
  
  // construct without factory
  return CPoint( std::min(X(), rhs.X()),				
           std::min(Y(), rhs.Y()),				
           std::min(Z(), rhs.Z()));
}

IPoint &IPoint::operator+=(const ICoordinate &rhs)
{
  // Assignment ...
  X(X() + rhs.X());
  Y(Y() + rhs.Y());
  Z(Z() + rhs.Z());

  return *this;
}

IPoint &IPoint::operator-=(const ICoordinate &rhs)
{
  // Assignment ...
  X(X() - rhs.X());
  Y(Y() - rhs.Y());
  Z(Z() - rhs.Z());

  return *this;
}


CPoint operator*(const double &val,const IPoint& rhs)
{
  return rhs*val;
}


/*virtual*/ void IPoint::Mirror(const geo::IPlane& plane) 
{
  geo::CPoint p =plane.Project(*this);
  geo::CVector v(*this,p);
  p.Move(v);
  *this=p;
}

}
