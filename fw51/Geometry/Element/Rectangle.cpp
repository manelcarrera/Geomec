/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// Rectangle.cpp: implementation of the CRectangle class.
//
//////////////////////////////////////////////////////////////////////
#include "Rectangle.h"
#include "dimple.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace geo {

CRectangle::CRectangle() {}

CRectangle::CRectangle(const IPoint &RefPoint, const IVector &Dir1, const IVector &Dir2) {
  // directions should be perpendicular

  assert(Dir1.IsPerpendicular(Dir2));

  m_RefPoint = RefPoint;
  m_vcDirection1 = Dir1;
  m_vcDirection2 = Dir2;

  CreateCornerPoints();
}

bool CRectangle::ValidRectangleData(const IPoint &Point1, const IPoint &Point2) {
  if (Point1.Empty())
    return false;
  if (Point2.Empty())
    return false;
  if (Point1 == Point2)
    return false;

  CPoint min = Point1.Min(Point2);
  CPoint max = Point1.Max(Point2);

  CVector dir1(max.X() - min.X(), 0, max.Z() - min.Z());
  CVector dir2(0, max.Y() - min.Y(), 0);

  // directions should be perpendicular
  if (!dir1.IsPerpendicular(dir2))
    return false;
  return true;
}

// TNMA: constructor does not work properly! (try making a rectangle in XY-plane)
CRectangle::CRectangle(const IPoint &Point1, const IPoint &Point2) {
  assert(CRectangle::ValidRectangleData(Point1, Point2));

  CPoint min = Point1.Min(Point2);
  CPoint max = Point1.Max(Point2);

  CVector dir1(max.X() - min.X(), 0, max.Z() - min.Z());
  CVector dir2(0, max.Y() - min.Y(), 0);

  m_RefPoint = min;
  m_vcDirection1 = dir1;
  m_vcDirection2 = dir2;

  CreateCornerPoints();
}

void CRectangle::CreateCornerPoints() {
  // directions should be perpendicular
  assert(m_vcDirection1.IsPerpendicular(m_vcDirection2));

  m_vcCornerPoints.clear();
  m_vcCornerPoints.push_back(m_RefPoint);
  m_vcCornerPoints.push_back(m_RefPoint + m_vcDirection1);
  m_vcCornerPoints.push_back(m_RefPoint + m_vcDirection1 + m_vcDirection2);
  m_vcCornerPoints.push_back(m_RefPoint + m_vcDirection2);
}

CRectangle::CRectangle(const IRectangle &rhs) : IRectangle(rhs) {}

CRectangle::~CRectangle() {}

const IPoint &CRectangle::Refpoint() const { return m_RefPoint; }

const IVector &CRectangle::Direction1() const { return m_vcDirection1; }

const IVector &CRectangle::Direction2() const { return m_vcDirection2; }

const IPoint &CRectangle::Point(int nIndex) const { return Node(nIndex); }

void CRectangle::Point(int nIndex, const IPoint &point) { Node(nIndex, point); }

size_t CRectangle::Order() const { return 1; }

void CRectangle::Rotate(const IVector &vec, const double &dAngleDeg) {
  m_RefPoint.Rotate(vec, dAngleDeg);
  m_vcDirection1.Rotate(vec, dAngleDeg);
  m_vcDirection2.Rotate(vec, dAngleDeg);
  CreateCornerPoints();
}

void CRectangle::Move(const IVector &vec) {
  m_RefPoint.Move(vec);
  CreateCornerPoints();
}

void CRectangle::Transform(const IMatrix &matrix) {
  geo::CPoint p1 = m_RefPoint + m_vcDirection1;
  geo::CPoint p2 = m_RefPoint + m_vcDirection2;
  p1.Transform(matrix);
  p2.Transform(matrix);
  m_RefPoint.Transform(matrix);
  m_vcDirection1 = CVector(m_RefPoint, p1);
  m_vcDirection2 = CVector(m_RefPoint, p2);
  CreateCornerPoints();
}

size_t CRectangle::NrOfNodes() const { return 4; }

const INode &CRectangle::Node(int nIndex) const {
  assert(nIndex <= 3 && nIndex >= 0);
  return m_vcCornerPoints[nIndex];
}

void CRectangle::Node(int /*nIndex*/, const IPoint & /*point*/) { assert(false); }

} // namespace geo
