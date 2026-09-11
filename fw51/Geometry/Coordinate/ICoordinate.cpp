/* Copyright (c) 2011 TNO DIANA BV                              Confidential */

#include "ICoordinate.h"
#include "Matrix.h"
#include "Point.h"
#include "Vector.h"
#include "dimple.h"
#include <cmath>

namespace geo {

typedef long long TLongLong;

void ICoordinate::Set(const double &dX, const double &dY, const double &dZ) {
  X(dX);
  Y(dY);
  Z(dZ);
}

bool ICoordinate::CloseTo(const ICoordinate &coord, double eps) const {
  if (fabs(coord.X() - X()) > eps)
    return false;
  if (fabs(coord.Y() - Y()) > eps)
    return false;
  if (fabs(coord.Z() - Z()) > eps)
    return false;
  return true;
}

void ICoordinate::Rotate(const IVector &vec, const double &dAngleDeg) {
  if (fabs(dAngleDeg) < EPS) {
    return;
  }

  double AngleRad = dAngleDeg * DEG_TO_RAD;
  double cosAngle = cos(AngleRad);
  double sinAngle = sin(AngleRad);

  Rotate(vec, sinAngle, cosAngle);
}

void ICoordinate::Rotate(const IVector &vec, const double &dSinAngle, const double &dCosAngle) {

  CVector r(vec.UnitVector());

  double dX = 0.0;
  double dY = 0.0;
  double dZ = 0.0;

  dX += (dCosAngle + (1 - dCosAngle) * r.X() * r.X()) * X();
  dX += ((1 - dCosAngle) * r.X() * r.Y() - r.Z() * dSinAngle) * Y();
  dX += ((1 - dCosAngle) * r.X() * r.Z() + r.Y() * dSinAngle) * Z();

  dY += ((1 - dCosAngle) * r.X() * r.Y() + r.Z() * dSinAngle) * X();
  dY += (dCosAngle + (1 - dCosAngle) * r.Y() * r.Y()) * Y();
  dY += ((1 - dCosAngle) * r.Y() * r.Z() - r.X() * dSinAngle) * Z();

  dZ += ((1 - dCosAngle) * r.X() * r.Z() - r.Y() * dSinAngle) * X();
  dZ += ((1 - dCosAngle) * r.Y() * r.Z() + r.X() * dSinAngle) * Y();
  dZ += (dCosAngle + (1 - dCosAngle) * r.Z() * r.Z()) * Z();

  X(dX);
  Y(dY);
  Z(dZ);
}

void ICoordinate::Move(const IVector &vec) {
  X(X() + vec.X());
  Y(Y() + vec.Y());
  Z(Z() + vec.Z());
}

void ICoordinate::Transform(const IMatrix &matrix) {
  double dX = X() * matrix.Value(0, 0) + Y() * matrix.Value(0, 1) + Z() * matrix.Value(0, 2) + matrix.Value(0, 3);
  double dY = X() * matrix.Value(1, 0) + Y() * matrix.Value(1, 1) + Z() * matrix.Value(1, 2) + matrix.Value(1, 3);
  double dZ = X() * matrix.Value(2, 0) + Y() * matrix.Value(2, 1) + Z() * matrix.Value(2, 2) + matrix.Value(2, 3);
  X(dX);
  Y(dY);
  Z(dZ);
}

CPoint ICoordinate::Min() const {
  if (Empty())
    return CPoint();

  return CPoint(X(), Y(), Z());
}

CPoint ICoordinate::Max() const {
  if (Empty())
    return CPoint();

  return CPoint(X(), Y(), Z());
}

void ICoordinate::Divide(const ICoordinate &coor) {
  X(X() / coor.X());
  Y(Y() / coor.Y());
  Z(Z() / coor.Z());
}

void ICoordinate::Multiply(const ICoordinate &coor) {
  X(X() * coor.X());
  Y(Y() * coor.Y());
  Z(Z() * coor.Z());
}

ICoordinate &ICoordinate::operator=(const ICoordinate &rhs) {
  // Assignment ...
  Set(rhs.X(), rhs.Y(), rhs.Z());

  return *this;
}

bool ICoordinate::operator>(const ICoordinate &rhs) const {
  if (*this < rhs) {
    return false;
  }

  if (*this == rhs) {
    return false;
  }

  return true;
}

ICoordinate::CCoordinateXYLess::CCoordinateXYLess(const double &dEps) : m_dEps(dEps) {}

ICoordinate::CCoordinateXYLess::CCoordinateXYLess() : m_dEps(compareTolerance()) {}

bool ICoordinate::CCoordinateXYLess::operator()(const geo::ICoordinate *p1, const geo::ICoordinate *p2) const {
  p1->AssertValid();
  p2->AssertValid();
  assert(m_dEps >= 0);

  if (m_dEps > 0) {
    TLongLong p1X = TLongLong(p1->X() / m_dEps);
    TLongLong p1Y = TLongLong(p1->Y() / m_dEps);
    TLongLong p2X = TLongLong(p2->X() / m_dEps);
    TLongLong p2Y = TLongLong(p2->Y() / m_dEps);

    if (p1X < p2X)
      return true;

    if (p1X > p2X)
      return false;

    return p1Y < p2Y;
  } else {
    if (p1->X() < p2->X())
      return true;

    if (p1->X() > p2->X())
      return false;
  }

  return p1->Y() < p2->Y();
}

ICoordinate::CCoordinateLess::CCoordinateLess(const double &dEps) : m_dEps(dEps) {}

ICoordinate::CCoordinateLess::CCoordinateLess() : m_dEps(compareTolerance()) {}

bool ICoordinate::CCoordinateLess::operator()(const geo::ICoordinate *p1, const geo::ICoordinate *p2) const {
  p1->AssertValid();
  p2->AssertValid();
  assert(m_dEps >= 0);

  if (m_dEps > 0) {
    TLongLong p1X = TLongLong(p1->X() / m_dEps);
    TLongLong p1Y = TLongLong(p1->Y() / m_dEps);
    TLongLong p1Z = TLongLong(p1->Z() / m_dEps);
    TLongLong p2X = TLongLong(p2->X() / m_dEps);
    TLongLong p2Y = TLongLong(p2->Y() / m_dEps);
    TLongLong p2Z = TLongLong(p2->Z() / m_dEps);

    if (p1X < p2X)
      return true;

    if (p1X > p2X)
      return false;

    if (p1Y < p2Y)
      return true;

    if (p1Y > p2Y)
      return false;

    return p1Z < p2Z;
  } else {
    if (p1->X() < p2->X())
      return true;

    if (p1->X() > p2->X())
      return false;

    if (p1->Y() < p2->Y())
      return true;

    if (p1->Y() > p2->Y())
      return false;
  }

  return p1->Z() < p2->Z();
}

bool ICoordinate::operator<(const ICoordinate &rhs) const {
  AssertValid();
  rhs.AssertValid();

  return CCoordinateLess()(this, &rhs);
}

bool ICoordinate::operator==(const ICoordinate &rhs) const {
  AssertValid();
  rhs.AssertValid();

  if (fabs(rhs.X() - X()) > compareTolerance())
    return false;
  if (fabs(rhs.Y() - Y()) > compareTolerance())
    return false;
  if (fabs(rhs.Z() - Z()) > compareTolerance())
    return false;

  return true;
}

bool ICoordinate::operator!=(const ICoordinate &rhs) const { return !(*this == rhs); }

} // namespace geo
