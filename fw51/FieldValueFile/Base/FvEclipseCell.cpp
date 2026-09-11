/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// Voxet.cpp: implementation of the CVoxet class.
//
//////////////////////////////////////////////////////////////////////
#include "FvEclipseCell.h"
#include "dimple.h"
#include <cmath>
#include <cstdlib> // for size_t

// bodyface names
static const char XI1[] = "XI1";
static const char XI2[] = "XI2";
static const char ETA1[] = "ETA1";
static const char ETA2[] = "ETA2";
static const char ZETA1[] = "ZETA1";
static const char ZETA2[] = "ZETA2";

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/*
void CEclipseCell::CreateFaces() const
{
  CEclipseCell *pCell = const_cast<CEclipseCell*>(this);

  // front
  m_vcFace.push_back(new CEclipseCellFace(*pCell,
                      0, 0, 0,
                      1, 0, 0,
                      1, 0, 1,
                      0, 0, 1,
                      0, 1, 5, 4, ETA1));
  // right
  m_vcFace.push_back(new CEclipseCellFace(*pCell,
                      1, 0, 0,
                      1, 1, 0,
                      1, 1, 1,
                      1, 0, 1,
                      1, 2, 6, 5, XI2));
  // back
  m_vcFace.push_back(new CEclipseCellFace(*pCell,
                      1, 1, 0,
                      0, 1, 0,
                      0, 1, 1,
                      1, 1, 1,
                      2, 3, 7, 6, ETA2));
  // left
  m_vcFace.push_back(new CEclipseCellFace(*pCell,
                      0, 1, 0,
                      0, 0, 0,
                      0, 0, 1,
                      0, 1, 1,
                      3, 0, 4, 7, XI1));
  // top
  m_vcFace.push_back(new CEclipseCellFace(*pCell,
                      0, 0, 1,
                      1, 0, 1,
                      1, 1, 1,
                      0, 1, 1,
                      4, 5, 6, 7, ZETA2));
  // bottom
  m_vcFace.push_back(new CEclipseCellFace(*pCell,
                      0, 0, 0,
                      0, 1, 0,
                      1, 1, 0,
                      1, 0, 0,
                      0, 3, 2, 1, ZETA1));
}
*/

CEclipseCell::CEclipseCell() : m_array(2, 2, 2) {}

CEclipseCell::CEclipseCell(const CEclipseCell &rhs) : m_array(rhs.m_array) {
  //	assert(m_vcFace.size() == 0);
}

CEclipseCell::~CEclipseCell() {
  //	for(int i = 0; i < m_vcFace.size(); i++)
  //		delete m_vcFace[i];
}

CEclipseCell &CEclipseCell::operator=(const CEclipseCell &rhs) {
  m_array = rhs.m_array;
  return *this;
}

// This function returns the points of the cell in a vector but in ordered  fashion:
// The four lower points first (smalles Z value) and the four upper points next. The points are also put in the
// vector counter clock wise. (for non negative volumes and such)
std::vector<const geo::IPoint *> CEclipseCell::Points() const {
  std::vector<const geo::IPoint *> vcRet;
  const geo::IPoint &Pt0 = InternalPoint(0);
  const geo::IPoint &Pt1 = InternalPoint(1);
  const geo::IPoint &Pt2 = InternalPoint(2);
  const geo::IPoint &Pt3 = InternalPoint(3);
  const geo::IPoint &Pt4 = InternalPoint(4);

  bool bCounterClock = false;
  int i;

  if (Pt0.X() < Pt1.X() && Pt0.Y() < Pt3.Y() && Pt0.X() < Pt2.X() && Pt0.Y() < Pt2.Y())
    bCounterClock = true;
  else if (Pt0.X() > Pt3.X() && Pt0.Y() < Pt1.Y() && Pt0.X() > Pt2.X() && Pt0.Y() < Pt2.Y())
    bCounterClock = true;
  else if (Pt0.X() > Pt1.X() && Pt0.Y() > Pt3.Y() && Pt0.X() > Pt2.X() && Pt0.Y() > Pt2.Y())
    bCounterClock = true;
  else if (Pt0.X() < Pt3.X() && Pt0.Y() > Pt1.Y() && Pt0.X() < Pt2.X() && Pt0.Y() > Pt2.Y())
    bCounterClock = true;

  if (Pt0.Z() < Pt4.Z()) {
    if (bCounterClock) {
      for (i = 0; i < 4; i++) {
        vcRet.push_back(&InternalPoint(i));
      }
      for (i = 4; i < 8; i++) {
        vcRet.push_back(&InternalPoint(i));
      }
    } else {
      for (i = 3; i > -1; i--) {
        vcRet.push_back(&InternalPoint(i));
      }
      for (i = 7; i > 3; i--) {
        vcRet.push_back(&InternalPoint(i));
      }
    }
  } else if (Pt0.Z() > Pt4.Z()) {
    if (bCounterClock) {
      for (i = 4; i < 8; i++) {
        vcRet.push_back(&InternalPoint(i));
      }
      for (i = 0; i < 4; i++) {
        vcRet.push_back(&InternalPoint(i));
      }
    } else {
      for (i = 7; i > 3; i--) {
        vcRet.push_back(&InternalPoint(i));
      }
      for (i = 3; i > -1; i--) {
        vcRet.push_back(&InternalPoint(i));
      }
    }
  }

  return vcRet;
}

const geo::IPoint &CEclipseCell::InternalPoint(int nIndex) const {
  const geo::IPoint *ret = 0;

  switch (nIndex) {
  case 0:
    ret = &GetAt(0, 0, 0);
    break;
  case 1:
    ret = &GetAt(1, 0, 0);
    break;
  case 2:
    ret = &GetAt(1, 1, 0);
    break;
  case 3:
    ret = &GetAt(0, 1, 0);
    break;
  case 4:
    ret = &GetAt(0, 0, 1);
    break;
  case 5:
    ret = &GetAt(1, 0, 1);
    break;
  case 6:
    ret = &GetAt(1, 1, 1);
    break;
  case 7:
    ret = &GetAt(0, 1, 1);
    break;
  }

  assert(ret != 0);

  return *ret;
}
const geo::IPoint &CEclipseCell::Point(int nIndex) const {
  std::vector<const geo::IPoint *> points = Points();
  return *points[nIndex];
}

void CEclipseCell::Point(int /*nIndex*/, const geo::IPoint & /*pt*/) { assert(false); }

/*
int CEclipseCell::NrOfPoints() const
{
  return 8;
}

const geo::IFace& CEclipseCell::Face(int nIndex) const
{
  if(m_vcFace.size() == 0)
    CreateFaces();
  return *m_vcFace[nIndex];
}

int CEclipseCell::NrOfFaces() const
{
  if(m_vcFace.size() == 0)
    CreateFaces();
  return m_vcFace.size();
}
*/
double CEclipseCell::Volume() const {
  return (GetAt(0, 0, 0).X() - GetAt(1, 1, 1).X()) * (GetAt(0, 0, 0).Y() - GetAt(1, 1, 1).Y()) *
         (GetAt(0, 0, 0).Z() - GetAt(1, 1, 1).Z());
}

const geo::IPoint &CEclipseCell::GetAt(int x, int y, int z) const {
  assert((x > -1) && (x < 2));
  assert((y > -1) && (y < 2));
  assert((z > -1) && (z < 2));

  return *(m_array.get_at(x, y, z));
}

geo::IPoint &CEclipseCell::GetAt(int x, int y, int z) {
  assert((x > -1) && (x < 2));
  assert((y > -1) && (y < 2));
  assert((z > -1) && (z < 2));

  return *(m_array.get_at(x, y, z));
}

void CEclipseCell::SetAt(int x, int y, int z, const geo::IPoint *point) {
  assert((x > -1) && (x < 2));
  assert((y > -1) && (y < 2));
  assert((z > -1) && (z < 2));

  m_array.set_at(x, y, z, (geo::CPoint *)point);
}

geo::CPoint CEclipseCell::CentreOfGravity() const {
  // Middle Z = 0 of
  return geo::CPoint((GetAt(0, 0, 0).X() + GetAt(1, 1, 1).X()) / 2, (GetAt(0, 0, 0).Y() + GetAt(1, 1, 1).Y()) / 2,
                     (GetAt(0, 0, 0).Z() + GetAt(1, 1, 1).Z()) / 2);
}

bool CEclipseCell::IsPinchOut() const {
  const geo::IPoint &pt1 = InternalPoint(0);
  const geo::IPoint &pt2 = InternalPoint(1);
  const geo::IPoint &pt3 = InternalPoint(2);
  const geo::IPoint &pt4 = InternalPoint(3);
  const geo::IPoint &pt5 = InternalPoint(4);
  const geo::IPoint &pt6 = InternalPoint(5);
  const geo::IPoint &pt7 = InternalPoint(6);
  const geo::IPoint &pt8 = InternalPoint(7);

  if (fabs(pt1.Z() - pt5.Z()) < EPS)
    return true;
  if (fabs(pt2.Z() - pt6.Z()) < EPS)
    return true;
  if (fabs(pt3.Z() - pt7.Z()) < EPS)
    return true;
  if (fabs(pt4.Z() - pt8.Z()) < EPS)
    return true;

  return false;
}

bool CEclipseCell::Contains(const geo::IPoint & /*point*/, bool /*bIncludeEdge*/) const {
  assert(false); // NOT IMPLEMENTED YET...
  return false;
}

geo::CValue CEclipseCell::InterpolateValue(const geo::IPoint & /*point*/,
                                           const std::vector<geo::CValue> &values) const {
  if (ContainsInvalidValue(values))
    return geo::CValue();

  assert(false);
  return geo::CValue();
}

std::set<geo::CPoint> CEclipseCell::Intersection(const geo::IPlane & /*plane*/) const {
  assert(false);
  return std::set<geo::CPoint>();
}
