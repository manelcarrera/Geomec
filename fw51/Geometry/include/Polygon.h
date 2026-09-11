// PolyBase.h: interface for the CPolyBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POLYBASE_H__D44DA759_47A8_11D6_A244_0060975B1CAF__INCLUDED_)
#define AFX_POLYBASE_H__D44DA759_47A8_11D6_A244_0060975B1CAF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IPolygon.h"

#include "GeometryExports.h"

namespace geo {

// Copyright (c) 2011 TNO DIANA BV                              Confidential
// Description : Polygon object based on:
//         poly_Boolean: a polygon clip library
//         Copyright (C) 1997  Alexey Nikitin, Michael Leonov
//         leonov@propro.iis.nsk.suThe
// Status      : Finished (60%)
// Remarks     : See IFace
// Last review : not yet
class IPolyLine;
class GEOMETRY_EXPORT CPolygon : public IPolygon {
  // internal points
  CArray<CPoint> m_points;

public:
  CPolygon(const CPolygon &poly);
  CPolygon(const IPolyLine &poly_line, bool bIsHole = false);

  // construct from point array
  CPolygon(const CArray<CPoint> &points, bool bIsHole = false);
  CPolygon(const CPtrArray<CPoint> &points, bool bIsHole = false);

  // construct from IFace
  CPolygon(const IFace &face, bool bIsHole = false);
  // Default (Empty) constructor
  CPolygon(bool bIsHole = false);
  virtual ~CPolygon() {}
  CPolygon &operator=(const CPolygon &rhs);

  // IFace interface
  virtual const IPoint &Point(int nIndex) const { return m_points.Object(nIndex); }
  virtual void Point(int nIndex, const IPoint &pt);
  virtual int NrOfPoints() const { return (int)m_points.Size(); }
  void Remove(int nPointIndex);

  // mirror the polyline and reurn a reference to this
  void Mirror(const geo::IPlane &plane);

  // Clear and pushback
  virtual void Clear();
  virtual bool PushBack(const IPoint &point, bool bUnique = false);

  virtual size_t Order() const;
};

} // end namespace geo

#endif // !defined(AFX_POLYBASE_H__D44DA759_47A8_11D6_A244_0060975B1CAF__INCLUDED_)
