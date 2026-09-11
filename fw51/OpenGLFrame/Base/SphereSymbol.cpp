/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "SphereSymbol.h"
#include "Sphere.h"
#include "dimple.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSphereSymbol::CSphereSymbol(const geo::IPoint &location, const double &radius) : m_location(location) {
  m_location.AssertValid();
  assert(radius > 0);

  m_geometry.PushBack(*new geo::CSphere(location, radius));
}

CSphereSymbol::~CSphereSymbol() { m_geometry.ClearAndDelete(); }

int CSphereSymbol::NrOfPositions() const { return 1; }

const geo::IPoint &CSphereSymbol::Position(int Index) const {
  assert(Index == 0);
  return m_location;
}

const geo::IArray &CSphereSymbol::Geometry() const { return m_geometry; }

const geo::IPoint &CSphereSymbol::PivotPoint() const { return geo::CPoint::NullPoint; }

/*virtual*/
void CSphereSymbol::AssertValid() const {
  assert(!m_geometry.Empty());
  assert(!m_location.Empty());
}

/*virtual*/
bool CSphereSymbol::Empty() const { return (m_geometry.Empty() || m_location.Empty()); }

/*virtual*/
geo::CPoint CSphereSymbol::Min() const {
  return m_location; // world point not max of geometry =(screen)
}

/*virtual*/
geo::CPoint CSphereSymbol::Max() const {
  return m_location; // world point
}
