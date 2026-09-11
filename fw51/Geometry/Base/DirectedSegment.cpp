 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// directedsegment.cpp: implementation of the CAttri2DHorizonDlg class.
//
//////////////////////////////////////////////////////////////////////
#include "dimple.h"
#include <cassert>
#include <map>
#include "Vector.h"
#include "RingFactory.h"
#include "DirectedSegment.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace geo {

CDirectedSegment::CDirectedSegment(const CRingFactory& factory, int nFirst, int nSecond)
: m_segment(nFirst, nSecond), m_factory(factory)
{
  assert((nFirst >= 0) && (nSecond >= 0));
  assert((nFirst < m_factory.PointSize()) && (nSecond < m_factory.PointSize()));
  assert( nFirst != nSecond);
}

CDirectedSegment::CDirectedSegment(const CRingFactory& factory, int nFirst, const TSegment& segment)
: m_segment(nFirst, -1), m_factory(factory)
{
  assert((nFirst >= 0) && (segment.first >= 0) && (segment.second >= 0));
  assert((nFirst < m_factory.PointSize()) && (segment.first < m_factory.PointSize()) && (segment.second < m_factory.PointSize()));
  assert( segment.first != segment.second);
  if(nFirst == segment.first)
  {
    m_segment.second = segment.second;
  }
  else
  {
    m_segment.second = segment.first;
    assert(nFirst == segment.second);
  }
}

CDirectedSegment::CDirectedSegment(const CDirectedSegment& rhs)
: m_segment(rhs.m_segment), m_factory(rhs.m_factory)
{
}

int CDirectedSegment::First() const
{
  return m_segment.first;
}

int CDirectedSegment::Second() const
{
  return m_segment.second;
}

std::vector<CDirectedSegment> CDirectedSegment::NextSegments() const
{
  return AdjacentSegments(Second());
}

std::vector<CDirectedSegment> CDirectedSegment::PrevSegments() const
{
  return AdjacentSegments(First());
}

/*!
  Returns the adjacent segments given the first or second vertex. The vertices are
  ordered by the angle to current segment.
*/
std::vector<CDirectedSegment> CDirectedSegment::AdjacentSegments(int nVertex) const
{
  assert((nVertex == First()) || (nVertex == Second()));
  typedef std::map<double, CDirectedSegment> TSegmentMap;
  TSegmentMap mpSegment;

  // Determine other vertex
  assert((First() == nVertex) || (Second() == nVertex));
  int nOtherVertex = First();
  if(nOtherVertex == nVertex)
    nOtherVertex = Second();

  // Get the
  const std::vector<int>& vcSegments = m_factory.m_vcPoint[nVertex].second;

  for(size_t i = 0; i < vcSegments.size(); i++)
  {
    const TSegment segment = m_factory.Segment( vcSegments[i] );;
    assert((segment.first == nVertex) || (segment.second == nVertex));
    int nOtherNextPoint = segment.first;
    if(segment.first == nVertex)
      nOtherNextPoint = segment.second;
  
    if(nOtherVertex != nOtherNextPoint)
    {
      if(nVertex == First())
      {
        CDirectedSegment directed_segment(m_factory, nOtherNextPoint, segment);
        VERIFY(mpSegment.insert(TSegmentMap::value_type(Angle(directed_segment), directed_segment)).second);
      }
      else
      {
        CDirectedSegment directed_segment(m_factory, nVertex, segment);
        VERIFY(mpSegment.insert(TSegmentMap::value_type(Angle(directed_segment), directed_segment)).second);
      }
    }
  }

  // Put in the
  std::vector<CDirectedSegment> vcRet;
  for(TSegmentMap::const_iterator it = mpSegment.begin(); it != mpSegment.end(); it++)
    vcRet.push_back(it->second);
  return vcRet;
}

bool CDirectedSegment::operator ==(const CDirectedSegment &rhs) const
{
  return m_segment == rhs.m_segment;
}

CDirectedSegment& CDirectedSegment::operator=(const CDirectedSegment &rhs)
{
  assert(&m_factory == &rhs.m_factory);
  m_segment = rhs.m_segment;
  return *this;
}

double CDirectedSegment::Angle(const CDirectedSegment &rhs) const
{
  assert((First() == rhs.First()) || (First() == rhs.Second()) || (Second() == rhs.First()) || (Second() == rhs.Second()));
  
  // Determ shared vertex
  int nSharedVertex = Second();
  if((First() == rhs.First()) || (First() == rhs.Second()))
    nSharedVertex = First();

  // We need two vector
  geo::CVector v1, v2;
  if(nSharedVertex == First())
    v1 = geo::CVector(m_factory.Point(Second()) - m_factory.Point(nSharedVertex)).UnitVector();
  else
    v1 = geo::CVector(m_factory.Point(First()) - m_factory.Point(nSharedVertex)).UnitVector();

  if(nSharedVertex == rhs.First())
    v2 = geo::CVector(m_factory.Point(rhs.Second()) - m_factory.Point(nSharedVertex)).UnitVector();
  else
    v2 = geo::CVector(m_factory.Point(rhs.First()) - m_factory.Point(nSharedVertex)).UnitVector();

  double dTheta = v1.AngleRad(v2);

  // Take the cross product
  if(v1.CrossProduct(v2).Z() < 0)
    // Take cosine inv. from inproduct
    return dTheta;
  
  return 2*PI - dTheta;
}

bool CDirectedSegment::operator <(const CDirectedSegment& rhs) const
{
  return m_segment < rhs.m_segment;
}

}
