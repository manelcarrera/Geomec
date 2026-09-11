 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// TriangleEdge.h: interface for the CTriangleEdge class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRIANGLEEDGE_H__F478D94D_5B82_42D8_B7ED_976FE40703AC__INCLUDED_)
#define AFX_TRIANGLEEDGE_H__F478D94D_5B82_42D8_B7ED_976FE40703AC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ILine.h"

#include "TriangleExports.h"

class CTriangleSurface;
class TRIANGLE_EXPORT CTriangleEdge : public geo::ILine  
{
  const CTriangleSurface& m_surface;
  int m_nIndex; // Index of the segment on the surface
  mutable std::pair<const geo::ITriangle*, const geo::ITriangle*> m_prTriangle;
  void BuildTrianglePair() const;
public:
  // Construction
  CTriangleEdge(const CTriangleSurface& m_surface, int nIndex);
  virtual ~CTriangleEdge();

  // Triangle size is 1 or 2
  size_t TriangleSize() const;
  const geo::ITriangle& Triangle(size_t nIndex) const;

  // ILine interface
  virtual const geo::IPoint &First() const;
  virtual void First(const geo::IPoint& point);
  virtual void Second(const geo::IPoint& point);
  virtual const geo::IPoint &Second() const;
  int SecondIndex() const;
  int FirstIndex() const;
  virtual size_t Order() const { return 1; }

  // Segment index
  int SegmentIndex() const;
  typedef std::pair<int, int> TSegment;
  TSegment Segment() const;
};

#endif // !defined(AFX_TRIANGLEEDGE_H__F478D94D_5B82_42D8_B7ED_976FE40703AC__INCLUDED_)
