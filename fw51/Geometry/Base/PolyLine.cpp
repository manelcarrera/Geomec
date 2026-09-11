/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// PolyLine.cpp: implementation of the CPolyLine class.
//
//////////////////////////////////////////////////////////////////////
#include "PolyLine.h"
#include "dimple.h"

#include <list>

namespace geo {

CPolyLine::CSegment::CSegment(IPoint &first, IPoint &second, int firstidx, int secondidx) : m_line(&first, &second) {
  m_NodeIndices.push_back(firstidx);
  m_NodeIndices.push_back(secondidx);
}

const IPoint &CPolyLine::CSegment::Point(int nIndex) const {
  assert(nIndex == 0 || nIndex == 1);
  if (nIndex == 0)
    return *m_line.first;

  return *m_line.second;
}

std::vector<int> CPolyLine::CSegment::Nodes() const { return m_NodeIndices; }

size_t CPolyLine::CSegment::Order() const { return 1; }

CPolyLine::CPolyLine() {}

CPolyLine::~CPolyLine() { RemoveAll(); }

CPolyLine::CPolyLine(const IPolyLine &polyline) {
  for (int i = 0; i < polyline.PointSize(); i++)
    PushBack(polyline.Point(i));

  assert(PointSize() == polyline.PointSize());
}

CPolyLine::CPolyLine(const CPolyLine &polyline) {
  for (int i = 0; i < polyline.PointSize(); i++)
    PushBack(polyline.Point(i));

  assert(PointSize() == polyline.PointSize());
}

CPolyLine::CPolyLine(const CArray<CPoint> &points) {
  for (size_t i = 0; i < points.Size(); i++)
    PushBack(points.Object(i));

  assert(points.Size() == PointSize());
}

CPolyLine::CPolyLine(const CPtrArray<CPoint> &points) {
  for (size_t i = 0; i < points.Size(); i++)
    PushBack(points.Object(i));

  assert(points.Size() == PointSize());
}

CPolyLine::CPolyLine(std::set<CPoint> &Polypts) {
  for (std::set<CPoint>::iterator it = Polypts.begin(); it != Polypts.end(); it++) {
    PushBack(*it);
  }
}

// Create the polyline from a given set of lines
// It will build this polyline from the first set of linked
// lines in the given set
// These lines are then removed from the set (so it's not const)
// This function can be called several times given a set of
// lines, to retrieve all separate polylines, until the set is empty
// The polyline (this) must be empty!
void CPolyLine::CreateFromLines(std::set<const ILine *> &stLines) {
  assert(Empty());
  if (stLines.empty())
    return;

  std::list<const geo::IPoint *> lstPoints;

  // get the first line and erase it from the set
  lstPoints.push_back(&(*stLines.begin())->First());
  lstPoints.push_back(&(*stLines.begin())->Second());
  stLines.erase(stLines.begin());

  bool GoOn;

  do {
    GoOn = false;
    std::set<const ILine *>::iterator it;
    for (it = stLines.begin(); it != stLines.end(); it++) {
      const ILine &l = **it;
      // check connectivity with first and last points
      if (l.First() == *lstPoints.front()) {
        lstPoints.push_front(&l.Second());
        stLines.erase(it);
        GoOn = true;
        break;
      } else if (l.Second() == *lstPoints.front()) {
        lstPoints.push_front(&l.First());
        stLines.erase(it);
        GoOn = true;
        break;
      } else if (l.First() == *lstPoints.back()) {
        lstPoints.push_back(&l.Second());
        stLines.erase(it);
        GoOn = true;
        break;
      } else if (l.Second() == *lstPoints.back()) {
        lstPoints.push_back(&l.First());
        stLines.erase(it);
        GoOn = true;
        break;
      }
    }
  } while (GoOn);

  assert(lstPoints.size() >= 2);
  std::list<const geo::IPoint *>::iterator it;
  for (it = lstPoints.begin(); it != lstPoints.end(); it++) {
    PushBack(**it);
  }
}

int CPolyLine::PointSize() const { return (int)m_vcPoint.size(); }

const IPoint &CPolyLine::Point(int nIndex) const {
  assert(m_vcPoint.size() > nIndex);
  assert(nIndex > -1);
  return *m_vcPoint[nIndex];
}

int CPolyLine::LineSize() const {
  UpdateSegments();
  return (int)m_vcSegment.size();
}

const ILine &CPolyLine::Line(int nIndex) const {
  UpdateSegments();
  assert(m_vcSegment.size() > nIndex);
  assert(nIndex > -1);
  return *m_vcSegment[nIndex];
}

CPolyLine &CPolyLine::operator=(const CPolyLine &rhs) {
  RemoveAll();

  for (int i = 0; i < rhs.PointSize(); i++)
    PushBack(rhs.Point(i));

  return *this;
}

void CPolyLine::UpdateSegments() const {
  if ((m_vcSegment.size() == 0) && (m_vcPoint.size() > 1)) {
    // Update necessary ...
    for (int i = 0; i < m_vcPoint.size() - 1; i++) {
      IPoint *pFirst = m_vcPoint[i];
      IPoint *pSecond = m_vcPoint[i + 1];
      m_vcSegment.push_back(new CSegment(*pFirst, *pSecond, i, i + 1));
    }
  }
}

void CPolyLine::InvalidateCache() {
  for (int i = 0; i < m_vcSegment.size(); i++) {
    delete m_vcSegment[i];
  }

  m_vcSegment.clear();

  IPolyLine::InvalidateCache();
}

bool CPolyLine::PushBack(const IPoint &point) {
  int exst = Exist(point);

  if (exst == -1) {
    InvalidateCache();
    m_vcPoint.push_back(new CPoint(point));
    return true;
  }

  return false;
}

bool CPolyLine::PushFront(const IPoint &point) {
  int exst = Exist(point);

  if (exst == -1) {
    std::vector<IPoint *>::iterator it;
    it = m_vcPoint.begin();
    InvalidateCache();
    m_vcPoint.insert(it, new CPoint(point));
    return true;
  }

  return false;
}

const IPoint &CPolyLine::FirstPoint() const {
  assert(PointSize() > 0);
  return Point(0);
}

const IPoint &CPolyLine::LastPoint() const {
  assert(PointSize() > 0);
  return Point(PointSize() - 1);
}

bool CPolyLine::Remove(const IPoint &point) {
  for (int i = 0; i < PointSize(); i++) {
    if (Point(i) == CPoint(point)) {
      Remove(i);
      return true;
    }
  }
  return false;
}

void CPolyLine::Remove(int nPointIndex) {
  assert(nPointIndex >= 0);
  assert(nPointIndex < PointSize());

  InvalidateCache();

  delete m_vcPoint[nPointIndex];
  m_vcPoint.erase(m_vcPoint.begin() + nPointIndex);
}

void CPolyLine::RemoveAll() {
  InvalidateCache();

  for (int i = 0; i < m_vcPoint.size(); i++)
    delete m_vcPoint[i];

  m_vcPoint.clear();
}

std::vector<int> CPolyLine::Nodes(const IElement &element) const {
  const CSegment *pSegment = dynamic_cast<const CSegment *>(&element);
  assert(pSegment);

  return pSegment->Nodes();
}

void CPolyLine::AssertValid() const { assert(m_vcPoint.size() > 0); }

bool CPolyLine::Empty() const {
  if (PointSize() < 2)
    return true;

  return false;
}

IPoint &CPolyLine::PointAt(int nIndex) { return *m_vcPoint[nIndex]; }

int CPolyLine::Exist(const IPoint &point) const {

  for (int i = 0; i < m_vcPoint.size(); i++) {
    if (*m_vcPoint[i] == point)
      return i;
  }

  // Point does not exist...
  return -1;
}

void CPolyLine::Swap(int nIndex1, int nIndex2) {
  assert(nIndex1 >= 0);
  assert(nIndex1 < PointSize());
  assert(nIndex2 >= 0);
  assert(nIndex2 < PointSize());

  CPoint *p1 = new CPoint(Point(nIndex1));
  CPoint *p2 = new CPoint(Point(nIndex2));
  Remove(nIndex2);
  Remove(nIndex1);
  PushBack(*p2);
  PushBack(*p1);
}

} // namespace geo
