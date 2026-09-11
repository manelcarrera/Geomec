 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// ringfactory.h: interface for the CRingFactory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RINGFACTORY_H__97E3D42F_A823_4D43_BC7F_C8902F9CDF4F__INCLUDED_)
#define AFX_RINGFACTORY_H__97E3D42F_A823_4D43_BC7F_C8902F9CDF4F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IPoint.h"
#include "ILine.h"
#include <set>

#include "GeometryExports.h"

namespace geo
{
class CDirectedSegment;

class GEOMETRY_EXPORT  CRingFactory  
{
  friend class CDirectedSegment;
public:
  typedef std::vector<int> TRing;
  typedef std::pair<int, int> TSegment;	
private:
  // Segment points
  typedef std::vector<int> TSegmentIndexVec;
  typedef std::pair<const IPoint*, TSegmentIndexVec> TSegmentPoint;
  typedef std::vector<TSegmentPoint> TSegmentPointVec;
  TSegmentPointVec m_vcPoint;

  // Segments
  typedef std::vector<TSegment> TSegmentVec;
  TSegmentVec m_vcSegment;
  
  typedef std::set<int> TRingSet;
  typedef std::pair<TRing, TRingSet> TRingPair;
  std::vector<TRingPair>	m_vcRing;
  std::vector<TRingPair>	m_vcBoundary;
  void*					m_pSegment;
  TRingPair				m_prRing;

  bool IsRingBoundarySegment(const CDirectedSegment& segment) const;
  bool Contains(const TRingPair &ring_pair, const CDirectedSegment& segment) const;
  bool Walk(const CDirectedSegment& start);
  bool Valid(const CDirectedSegment& edge) const;
  bool Boundary(const TRing& ring) const;
public:
  CRingFactory();
  virtual ~CRingFactory();

  int AddPoint(const IPoint& point);
  bool AddSegment(int nFirstPoint, int nSecondPoint);

  int PointSize() const;
  const IPoint& Point(int nIndex) const;

  size_t SegmentSize() const;
  TSegment Segment(size_t nIndex) const;

  bool CreateRings();

  int RingSize() const;
  const TRing& Ring(int nIndex) const;

  int BoundarySize() const;
  const TRing& Boundary(int nIndex) const;

  void Clear();
};

}

#endif // !defined(AFX_RINGFACTORY_H__97E3D42F_A823_4D43_BC7F_C8902F9CDF4F__INCLUDED_)
