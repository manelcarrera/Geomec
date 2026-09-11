 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// ISphere.cpp: implementation of the ISphere class.
//
//////////////////////////////////////////////////////////////////////

#include "dimple.h"
#include <cmath>
#include "Vector.h"
#include "ILine.h"
#include "ISphere.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace geo
{

ISphere::ISphere()
{

}

void ISphere::Rotate(const IVector &vec, const double &dAngleDeg)
{
  assert(dAngleDeg >= 0);
  assert(dAngleDeg <= 360);

  CPoint p = MidPoint();
  p.Rotate(vec, dAngleDeg);
  MidPoint(p);
}

void ISphere::Move(const IVector &vec)
{
  CPoint p = MidPoint();
  p.Move(vec);
  MidPoint(p);
}

void ISphere::Transform(const IMatrix &matrix)
{
  CPoint p = MidPoint();
  p.Transform(matrix);
  MidPoint(p);
}

void ISphere::AssertValid() const
{
  assert(!MidPoint().Empty());
  assert(Radius() > 0);
}

CPoint ISphere::Min() const
{
  return CPoint(MidPoint().X() - Radius(), 
          MidPoint().Y() - Radius(),
          MidPoint().Z() - Radius());
}

CPoint ISphere::Max() const
{
  return CPoint(MidPoint().X() + Radius(), 
          MidPoint().Y() + Radius(),
          MidPoint().Z() + Radius());
}

CArray<CPoint> ISphere::Intersection(const ILine &line) const
{

  CArray<CPoint> ret;
  CVector sv = line.First() - MidPoint();
  CVector rv = line.Dir();
  
  double a = rv.SquareLength();
  double b = 2.0 * rv.DotProduct(sv);
  double c = sv.SquareLength() - Radius() * Radius();
  double eps4 = EPS * pow(Radius(), 4);
  
  sv = line.First();

  double D = b * b - 4.0 * a * c;
  
  if(D < -eps4)
  {
    //No intersection points
    return ret;
  }
  else
  {
    // at least one intersection or tangens line
  double sqrtD = (D > 0.0 ? sqrt(D) : 0.0);
    double labda = (-b + sqrtD) / (2.0 * a);
    CPoint p;
    p = CPoint(sv + rv * labda);
    ret.PushBack(p);
    if(D > eps4)
    {
      // two intersection points
      labda = (-b - sqrt(D)) / (2.0 * a);
      p = CPoint(sv + rv * labda);
      ret.PushBack(p);
    }
  }
  return ret;
}

bool ISphere::Empty() const
{
  if(MidPoint().Empty())
    return true;

  if(Radius()<EPS)
    return true;

  return false;
}

}
