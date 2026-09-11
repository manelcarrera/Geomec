/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// ringpolygon.cpp: implementation of the CRingPolygon class.
//
//////////////////////////////////////////////////////////////////////
#include "RingPolygon.h"
#include "RingFactory.h"
#include "dimple.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace geo {

CRingPolygon::CRingPolygon(const CRingFactory &factory, const CRingFactory::TRing &ring)
    : IPolygon(false), m_ring(ring), m_factory(factory) {}

const IPoint &CRingPolygon::Point(int nIndex) const { return m_factory.Point(m_ring[nIndex]); }
void CRingPolygon::Point(int /*nIndex*/, const IPoint & /*pt*/) { assert(false); }

int CRingPolygon::NrOfPoints() const { return (int)m_ring.size(); }

size_t CRingPolygon::Order() const { return 1; }

} // namespace geo
