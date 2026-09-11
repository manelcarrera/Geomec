 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// ringpolygon.h: interface for the CRingPolygon class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RINGPOLYGON_H__5FDF9A9D_1B18_456B_A531_EA9D079ABFE7__INCLUDED_)
#define AFX_RINGPOLYGON_H__5FDF9A9D_1B18_456B_A531_EA9D079ABFE7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IPolygon.h"
#include "RingFactory.h"

namespace geo {
class CRingFactory;
class CRingPolygon : public IPolygon
{
  const CRingFactory::TRing& m_ring;
  const CRingFactory& m_factory;
public:
  CRingPolygon(const CRingFactory& factory, const CRingFactory::TRing& ring);
  virtual const IPoint &Point(int nIndex) const;
  virtual void Point(int nIndex, const IPoint &pt);
  virtual int NrOfPoints() const;
  virtual size_t Order() const;
};

}

#endif // !defined(AFX_RINGPOLYGON_H__5FDF9A9D_1B18_456B_A531_EA9D079ABFE7__INCLUDED_)
