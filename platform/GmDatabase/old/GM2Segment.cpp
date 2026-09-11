// GM2CompartmentCreator.cpp: implementation of the C2DCompartmentCreator class.
//
//////////////////////////////////////////////////////////////////////
#include "GM2Segment.h"
#include "stdafx.h"
#include "vector.h"

namespace gm {
//////////////////////////////////////////////////////////////////////
// Implementation of directed edge
//////////////////////////////////////////////////////////////////////
C2DSegment::C2DSegment(C2DVertex &first, C2DVertex &second) : m_prPoint(&first, &second) {
  first.AddSegment(*this);
  second.AddSegment(*this);
}

const geo::ISegmentPoint &C2DSegment::FirstSegmentPoint() const { return *m_prPoint.first; }

const geo::ISegmentPoint &C2DSegment::SecondSegmentPoint() const { return *m_prPoint.second; }

void C2DSegment::First(const geo::IPoint &point) { assert(false); }

void C2DSegment::Second(const geo::IPoint &point) { assert(false); }

const C2DVertex &C2DSegment::FirstVertex() const { return *m_prPoint.first; }

const C2DVertex &C2DSegment::SecondVertex() const { return *m_prPoint.second; }

C2DRingPolygon::C2DRingPolygon(const geo::CRingFactory::TRing &ring) : geo::IPolygon(false), m_ring(ring) {}

const geo::IPoint &C2DRingPolygon::Point(const int nIndex) const { return *m_ring[nIndex]; }
void C2DRingPolygon::Point(const int nIndex, const geo::IPoint &pt) { assert(false); }

int C2DRingPolygon::NrOfPoints() const { return m_ring.size(); }
} // namespace gm