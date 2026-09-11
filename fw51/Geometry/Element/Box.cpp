/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// Box.cpp: implementation of the CBox class.
//
//////////////////////////////////////////////////////////////////////
#include "Box.h"
#include "Point.h"
#include "Vector.h"
#include "dimple.h"
#include <cmath>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace geo {

// ##ModelId=3BC55D8D0039
CBox::CBox() {}

// ##ModelId=3BC55D8D0057
CBox::~CBox() {}

// ##ModelId=3C0B808E01A9
CBox::CBox(const IPoint &Pos, const IVector &Dir1, const IVector &Dir2, const IVector &Dir3) {
  // directions must be perpendiculare
  assert(fabs(Dir1.DotProduct(Dir2)) < EPS);
  assert(fabs(Dir1.DotProduct(Dir3)) < EPS);
  assert(fabs(Dir3.DotProduct(Dir2)) < EPS);

  CNode p(Pos);
  m_Points.PushBack(p);
  p += Dir1;
  m_Points.PushBack(p);
  p += Dir2;
  m_Points.PushBack(p);
  p = Pos + Dir2;
  m_Points.PushBack(p);

  p = Pos + Dir3;
  m_Points.PushBack(p);
  p += Dir1;
  m_Points.PushBack(p);
  p += Dir2;
  m_Points.PushBack(p);
  p = Pos + Dir3 + Dir2;
  m_Points.PushBack(p);
}

// ##ModelId=3C0B808E01D7
CBox::CBox(const IPoint &MinPoint, const IPoint &MaxPoint) {
  CVector Dir1(CVector::NullVector);
  CVector Dir2(CVector::NullVector);
  CVector Dir3(CVector::NullVector);

  Dir1.X(MaxPoint.X() - MinPoint.X());
  Dir2.Y(MaxPoint.Y() - MinPoint.Y());
  Dir3.Z(MaxPoint.Z() - MinPoint.Z());

  CNode p(MinPoint);
  m_Points.PushBack(p);
  p += Dir1;
  m_Points.PushBack(p);
  p += Dir2;
  m_Points.PushBack(p);
  p = MinPoint + Dir2;
  m_Points.PushBack(p);

  p = MinPoint + Dir3;
  m_Points.PushBack(p);
  p += Dir1;
  m_Points.PushBack(p);
  p += Dir2;
  m_Points.PushBack(p);
  p = MinPoint + Dir3 + Dir2;
  m_Points.PushBack(p);
}

// ##ModelId=3C0B808E01F8
CBox &CBox::operator=(const CBox &rhs) {
  m_Points = rhs.m_Points;

  return *this;
}

// ##ModelId=3C0B808E01E8
CBox::CBox(const CBox &rhs) : IBox(rhs), m_Points(rhs.m_Points) {}

const IPoint &CBox::Point(int nIndex) const { return Node(nIndex); }

size_t CBox::NrOfNodes() const {
  // comparison of unsigned expression >= 0 is always true

  /*assert(m_Points.Size() >= 0);*/

  return m_Points.Size();
}

const INode &CBox::Node(int nIndex) const {
  assert(nIndex >= 0 && nIndex < NrOfNodes());
  return m_Points.Object(nIndex);
}

void CBox::Node(int nIndex, const IPoint &point) { m_Points.Object(nIndex) = point; }

// No modification of the box trough a single point
void CBox::Point(int nIndex, const IPoint &pt) { Node(nIndex, pt); }

// ##ModelId=3C0B808E0236
int CBox::NrOfPoints() const { return NrOfNodes(); }

void CBox::AssertValid() const { assert(m_Points.Size() == 8); }

double CBox::Width() const { return Point(1).Distance(Point(0)); }

double CBox::Depth() const { return Point(3).Distance(Point(0)); }

double CBox::Height() const { return Point(4).Distance(Point(0)); }

// ##ModelId=3C0B808E0256
double CBox::Size() const { return Width() * Depth() * Height(); }

// ##ModelId=3BC55D8D0064
CArray<CNode> CBox::GeneratePoints() const { return m_Points; }

// ##ModelId=3BC55D8D0066
CArray<CLine> CBox::GenerateLines() const {
  AssertValid();
  CArray<CNode> PointColl = GeneratePoints();
  CArray<CLine> LineColl;
  CLine l;

  l = CLine(PointColl.Object(0), PointColl.Object(1));
  LineColl.PushBack(l);

  l = CLine(PointColl.Object(1), PointColl.Object(2));
  LineColl.PushBack(l);

  l = CLine(PointColl.Object(2), PointColl.Object(3));
  LineColl.PushBack(l);

  l = CLine(PointColl.Object(3), PointColl.Object(0));
  LineColl.PushBack(l);

  l = CLine(PointColl.Object(4), PointColl.Object(5));
  LineColl.PushBack(l);

  l = CLine(PointColl.Object(5), PointColl.Object(6));
  LineColl.PushBack(l);

  l = CLine(PointColl.Object(6), PointColl.Object(7));
  LineColl.PushBack(l);

  l = CLine(PointColl.Object(7), PointColl.Object(4));
  LineColl.PushBack(l);

  l = CLine(PointColl.Object(0), PointColl.Object(4));
  LineColl.PushBack(l);

  l = CLine(PointColl.Object(1), PointColl.Object(5));
  LineColl.PushBack(l);

  l = CLine(PointColl.Object(2), PointColl.Object(6));
  LineColl.PushBack(l);

  l = CLine(PointColl.Object(3), PointColl.Object(7));
  LineColl.PushBack(l);

  return LineColl;
}

// ##ModelId=3BC55D8D0088
CPoint CBox::Min() const { return m_Points.Min(); }

// ##ModelId=3BC55D8D0093
CPoint CBox::Max() const { return m_Points.Max(); }

bool CBox::Contains(const IPoint &point, bool bIncludeEdge) const {
  bool ret = false;
  CPoint ptMax = Max();
  CPoint ptMin = Min();

  if (bIncludeEdge) {
    if ((ptMin.X() <= point.X()) && (point.X() <= ptMax.X()) && (ptMin.Y() <= point.Y()) && (point.Y() <= ptMax.Y()) &&
        (ptMin.Z() <= point.Z()) && (point.Z() <= ptMax.Z()))
      ret = true;
  } else {
    if ((ptMin.X() < point.X()) && (point.X() < ptMax.X()) && (ptMin.Y() < point.Y()) && (point.Y() < ptMax.Y()) &&
        (ptMin.Z() < point.Z()) && (point.Z() < ptMax.Z()))
      ret = true;
  }

  return ret;
}

size_t CBox::Order() const { return 1; }

} // end namespace geo
