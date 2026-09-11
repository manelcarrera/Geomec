/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// plane.cpp: implementation of the plane class.
//
//////////////////////////////////////////////////////////////////////
#include "Plane.h"
#include "dimple.h"
#include <cassert>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace geo {

// the 3 cartisian plane definitions
const CPlane CPlane::PlaneXY(CPoint(0, 0, 0), CVector(0, 0, 1));
const CPlane CPlane::PlaneYZ(CPoint(0, 0, 0), CVector(1, 0, 0));
const CPlane CPlane::PlaneZX(CPoint(0, 0, 0), CVector(0, 1, 0));

// ##ModelId=3B4C71CE0237
CPlane::CPlane() {}

// ##ModelId=3B83AE7101A4
CPlane::CPlane(const CPlane &rhs) {
  if (rhs.Empty()) {
    m_BasePoint = CPoint();
    m_NormVector = CVector();
    assert(Empty());
  } else {
    rhs.AssertValid();
    m_BasePoint = rhs.BasePoint();
    m_NormVector = rhs.Normal();
    assert(!Empty());
  }
}

CPlane::CPlane(const IPlane &rhs) {
  if (rhs.Empty()) {
    m_BasePoint = CPoint();
    m_NormVector = CVector();
    assert(Empty());
  } else {
    rhs.AssertValid();
    m_BasePoint = rhs.BasePoint();
    m_NormVector = rhs.Normal();
    assert(!Empty());
  }
}

CPlane::CPlane(const IPoint &p1, const IPoint &p2, const IPoint &p3) {
  m_BasePoint = p1;
  CVector dir1(p2 - p1);
  CVector dir2(p3 - p2);
  m_NormVector = dir1.CrossProduct(dir2).UnitVector();

  AssertValid();
}

CPlane::CPlane(const IPoint &p1, const IVector &normal) : m_BasePoint(p1), m_NormVector(normal.UnitVector()) {
  AssertValid();
}

// ##ModelId=3B723C7002EF
CPlane::CPlane(const IPoint &base, const IVector &dir1, const IVector &dir2) {
  m_BasePoint = base;
  m_NormVector = dir1.CrossProduct(dir2).UnitVector();
  AssertValid();
}

CPlane &CPlane::operator=(const CPlane &rhs) {
  if (rhs.Empty()) {
    m_BasePoint = CPoint();
    m_NormVector = CPoint();
    assert(Empty());
  } else {
    m_BasePoint = rhs.BasePoint();
    m_NormVector = rhs.Normal();
    assert(!Empty());
  }
  return *this;
}

CPlane &CPlane::operator=(const IPlane &rhs) {
  if (rhs.Empty()) {
    m_BasePoint = CPoint();
    m_NormVector = CPoint();
    assert(Empty());
  } else {
    m_BasePoint = rhs.BasePoint();
    m_NormVector = rhs.Normal();
    assert(!Empty());
  }
  return *this;
}

const IPoint &CPlane::BasePoint() const { return m_BasePoint; }

void CPlane::BasePoint(const IPoint &base) {
  m_BasePoint = base;
  AssertValid();
}

const IVector &CPlane::Normal() const {
  // Supplies unit vector
  return m_NormVector;
}

void CPlane::Normal(const IVector &normal) {
  m_NormVector = normal.UnitVector();
  AssertValid();
}

// ##ModelId=3B723C710040
void CPlane::AssertValid() const {
  assert(!m_BasePoint.Empty());
  assert(!m_NormVector.Empty());
  // assert(!(m_NormVector == CVector(0,0,0)));
}

} // namespace geo
