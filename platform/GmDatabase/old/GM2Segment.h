// GM2CompartmentCreator.h: interface for the C2DCompartmentCreator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GM2COMPARTMENTCREATOR_H__6266A4DC_0A4F_4AF2_B483_39DB29D58A0E__INCLUDED_)
#define AFX_GM2COMPARTMENTCREATOR_H__6266A4DC_0A4F_4AF2_B483_39DB29D58A0E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GM2Vertex.h"
#include "ipolygon.h"
#include <deque>

namespace gm {
class C2DSegment : public geo::ISegment {
  std::pair<const C2DVertex *, const C2DVertex *> m_prPoint;

public:
  C2DSegment(C2DVertex &first, C2DVertex &second);
  virtual const geo::ISegmentPoint &FirstSegmentPoint() const;
  virtual const geo::ISegmentPoint &SecondSegmentPoint() const;
  const C2DVertex &FirstVertex() const;
  const C2DVertex &SecondVertex() const;
  virtual void First(const geo::IPoint &point);
  virtual void Second(const geo::IPoint &point);
  virtual int Order() const { return 1; }
};

class C2DRingPolygon : public geo::IPolygon {
  const geo::CRingFactory::TRing &m_ring;

public:
  C2DRingPolygon(const geo::CRingFactory::TRing &ring);
  virtual const geo::IPoint &Point(const int nIndex) const;
  virtual void Point(const int nIndex, const geo::IPoint &pt);
  virtual int NrOfPoints() const;
};

} // namespace gm

#endif // !defined(AFX_GM2COMPARTMENTCREATOR_H__6266A4DC_0A4F_4AF2_B483_39DB29D58A0E__INCLUDED_)
