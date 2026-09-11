/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// Point.cpp: implementation of the CPoint class.
//
//////////////////////////////////////////////////////////////////////
#include "Point.h"
#include "Vector.h"
#include "dimple.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace geo {

const CPoint CPoint::NullPoint(0.0, 0.0, 0.0);

/*!
  Creates an empty point
*/
CPoint::CPoint() : m_empty(true) {}

/*!
  Creates a point from a vector
*/
CPoint::CPoint(const IVector &vec) : m_empty(vec.Empty()) {
  if (!vec.Empty()) {
    vec.AssertValid();
    m_coor[0] = vec.X();
    m_coor[1] = vec.Y();
    m_coor[2] = vec.Z();
    assert(!Empty());
  }
}

/*!
  Creates a point from a point interface
*/
CPoint::CPoint(const IPoint &pt) : m_empty(pt.Empty()) {
  if (!pt.Empty()) {
    pt.AssertValid();
    m_coor[0] = pt.X();
    m_coor[1] = pt.Y();
    m_coor[2] = pt.Z();
    assert(!Empty());
  }
}

/*!
  Copy constructor
*/
CPoint::CPoint(const CPoint &pt) : m_empty(pt.Empty()) {
  if (!pt.Empty()) {
    pt.AssertValid();
    m_coor[0] = pt.X();
    m_coor[1] = pt.Y();
    m_coor[2] = pt.Z();
    assert(!Empty());
  }
}

/*!
  Initializes with x,y and z
*/
CPoint::CPoint(const double &dX, const double &dY, const double &dZ) : m_empty(false) {
  m_coor[0] = dX;
  m_coor[1] = dY;
  m_coor[2] = dZ;
  assert(!Empty());
}

/*!
  Assigns a point
*/
CPoint &CPoint::operator=(const CPoint &rhs) {
  m_empty = rhs.Empty();
  if (!rhs.Empty()) {
    m_coor[0] = rhs.X();
    m_coor[1] = rhs.Y();
    m_coor[2] = rhs.Z();
  }

  return *this;
}

bool CPoint::Empty() const { return m_empty; }

void CPoint::AssertValid() const { assert(!Empty()); }

const double &CPoint::X() const {
  assert(!Empty());
  return m_coor[0];
}

const double &CPoint::Y() const {
  assert(!Empty());
  return m_coor[1];
}

const double &CPoint::Z() const {
  assert(!Empty());
  return m_coor[2];
}

void CPoint::X(const double &dX) {
  assert(!Empty());
  m_coor[0] = dX;
}

void CPoint::Y(const double &dY) {
  assert(!Empty());
  m_coor[1] = dY;
}

void CPoint::Z(const double &dZ) {
  assert(!Empty());
  m_coor[2] = dZ;
}

void CPoint::Set(const double &dX, const double &dY, const double &dZ) {
  m_empty = false;
  m_coor[0] = dX;
  m_coor[1] = dY;
  m_coor[2] = dZ;
}

bool CPoint::CoLinear(const CPoint &p1, const CPoint &p2, const CPoint &p3) {
  return CVector(p2 - p1).CrossProduct(CVector(p3 - p1)).isNullVector();
}

} // namespace geo
