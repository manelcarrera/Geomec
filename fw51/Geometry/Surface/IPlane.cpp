/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// IPlane.cpp: implementation of the IPlane class.
//
//////////////////////////////////////////////////////////////////////
#include "IPlane.h"
#include "Line.h"
#include "Point.h"
#include "Vector.h"
#include "dimple.h"
#include <cmath>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace geo {

IPlane::IPlane() {}

// ##ModelId=3B723C7003AB
CPoint IPlane::Project(const IPoint &vertex) const {
  CVector v1;
  CVector v2;
  CVector v3;
  CVector v4;
  CVector sol;

  v3 = Normal();
  v1 = v3.GetNormal();
  v2 = v3.CrossProduct(v1);

  v4 = CVector(vertex - BasePoint());
  //==> v4 = p * v1 + q * v2 + r * v3
  sol = GetSolution(v1, v2, v3, v4);

  if (sol.Empty()) {
    return CPoint();
  }

  double dX = vertex.X() - sol.Z() * v3.X();
  double dY = vertex.Y() - sol.Z() * v3.Y();
  double dZ = vertex.Z() - sol.Z() * v3.Z();

  return CPoint(dX, dY, dZ);
}

double IPlane::SignedDistance(const IPoint &pt, bool bExact) const {
  CVector norm = Normal();
  double A = norm.X();
  double B = norm.Y();
  double C = norm.Z();
  double D = -1 * (A * BasePoint().X() + B * BasePoint().Y() + C * BasePoint().Z());
  double nom = A * pt.X() + B * pt.Y() + C * pt.Z() + D;

  if (bExact)
    return nom / sqrt(pow(A, 2) + pow(B, 2) + pow(C, 2));
  else
    return nom;
}

double IPlane::Distance(const IPoint &vertex) const { return vertex.Distance(Project(vertex)); }

double IPlane::SquareDistance(const IPoint &vertex) const { return vertex.SquareDistance(Project(vertex)); }

bool IPlane::IsParallel(const IVector &vec) const {
  double denom = Normal().DotProduct(vec);
  return (fabs(denom) < EPS);
}

bool IPlane::IsParallel(const ILine &line) const { return IsParallel(CVector(line.Second() - line.First())); }

bool IPlane::IsParallel(const IPlane &plane) const {
  CVector norm = Normal().CrossProduct(plane.Normal());
  return norm.isNullVector();
}

CLine IPlane::Intersection(const IPlane &plane) const {
  if (IsParallel(plane)) {
    return CLine();
  }

  // get a point "p" on both planes
  //(use intersection of line and a plane)
  CVector v1 = Normal().GetNormal(); // a vector parallel to *this

  CLine l(BasePoint(), v1);
  if (plane.IsParallel(l)) {
    return l;
  }

  CPoint p = plane.Intersection(l);

  // the direction of the line is given by
  // the crossproduct of the normals
  CVector n1 = Normal();
  CVector n2 = plane.Normal();
  CVector dir = n1.CrossProduct(n2);
  return CLine(p, dir);
}

CPoint IPlane::Intersection(const ILine &line) const {
  if (IsParallel(line)) {
    return CPoint();
  }

  CVector n = Normal();

  // denom is not zero otherwise parallel
  double denom = n.DotProduct(CVector(line.Second() - line.First()));

  double nom = n.DotProduct(CVector(BasePoint() - line.First()));
  double u = nom / denom;

  return line.First() + CPoint(line.Dir() * u);
}

CPoint IPlane::RealIntersection(const ILine &line, bool bIncP1, bool bIncP2) const {
  CPoint ptRet = Intersection(line);
  if (ptRet.Empty())
    return ptRet; // parallel

  if (ptRet == line.First()) {
    if (bIncP1)
      return ptRet;
    return CPoint();
  }
  if (ptRet == line.Second()) {
    if (bIncP2)
      return ptRet;
    return CPoint();
  }

  if (line.Contains(ptRet, false))
    return ptRet;
  return CPoint();
}

void IPlane::Rotate(const IVector &vec, const double &dAngleDeg) {
  CPoint basepoint = BasePoint();
  basepoint.Rotate(vec, dAngleDeg);
  BasePoint(basepoint);

  CVector normal = Normal();
  normal.Rotate(vec, dAngleDeg);
  Normal(normal);
}

void IPlane::Move(const IVector &vec) {
  CPoint basepoint = BasePoint();
  basepoint.Move(vec);
  BasePoint(basepoint);

  CVector normal = Normal();
  normal.Move(vec);
  Normal(normal);
}

void IPlane::Transform(const IMatrix &matrix) {
  CPoint basepoint = BasePoint();
  basepoint.Transform(matrix);
  BasePoint(basepoint);

  CVector normal = Normal();
  normal.Transform(matrix);
  Normal(normal);
}

void IPlane::AssertValid() const {
  assert(!(BasePoint().Empty()));
  assert(!(Normal().Empty()));
}

bool IPlane::Empty() const {
  if (BasePoint().Empty())
    return true;

  if (Normal().Empty())
    return true;

  return false;
}

CPoint IPlane::Min() const {
  assert(false);
  return CPoint();
}

CPoint IPlane::Max() const {
  assert(false);
  return CPoint();
}

} // namespace geo
