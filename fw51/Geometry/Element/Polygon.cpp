 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// PolyBase.cpp: implementation of the CPolyBase class.
//
//////////////////////////////////////////////////////////////////////
#include "dimple.h"

#include "Polygon.h"
#include "IPolyLine.h"
#include "IPlane.h"

namespace geo {
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CPolygon::CPolygon(const CPtrArray<CPoint>& points, bool bIsHole)
: IPolygon(bIsHole)
{
  for(size_t i=0;i< points.Size();i++)
  {
    CPoint p(points.Object(i));
  m_points.PushBack(p);
  }
}


CPolygon::CPolygon(const CArray<CPoint>& points, bool bIsHole)
: IPolygon(bIsHole)
{

#ifdef _DEBUG
  std::set<CPoint> temp;
  for(CArray<CPoint>::const_iterator it = points.begin(); it != points.end(); it++)
  {
  temp.insert(*it);
  }

  assert(temp.size() == points.Size());
#endif  
  
  m_points=points;
}


CPolygon::CPolygon(const IFace& face, bool bIsHole)
: IPolygon(bIsHole)
{
  CPoint p;
  for(int i =0 ;i<face.NrOfPoints();i++) {
    p = face.Point(i);
  m_points.PushBack(p);
  }
}

CPolygon::CPolygon(bool bIsHole)
: IPolygon(bIsHole)
{

}

CPolygon& CPolygon::operator=(const CPolygon& rhs)
{
  InvalidateCache();
  m_points=rhs.m_points;
  IsHole(rhs.IsHole());
  return *this;
}

CPolygon::CPolygon(const CPolygon& rhs)
: IPolygon(rhs.IsHole()), 
  m_points(rhs.m_points)
{
}

CPolygon::CPolygon(const IPolyLine& poly_line, bool bIsHole)
: IPolygon(bIsHole)
{
  CPoint p;
  for(int i = 0; i < poly_line.PointSize(); i++)
  {
    p = poly_line.Point(i);
  m_points.PushBack(p);
  }

}

void CPolygon::Clear()
{
  m_points.Clear();
  InvalidateCache();
}

bool CPolygon::PushBack( const IPoint& point, bool bUnique )
{
  // The point must not be equal to the last or first point
  if ( !bUnique && NrOfPoints() > 0 )
  {
  if((Point(0) == point) || (Point(NrOfPoints() - 1) == point))
      return false;
  }

  CPoint p(point);
  m_points.PushBack(p);
  InvalidateCache();
  return true;
}

void CPolygon::Remove(int nPointIndex)
{
  m_points.Remove(nPointIndex);
  InvalidateCache(); // polygon has changed, any available normal needs to be recalculated  
}

void CPolygon::Point(int nIndex, const IPoint &pt)
{
  m_points.Object(nIndex) = pt;
  InvalidateCache(); // polygon has changed, any available normal needs to be recalculated
}

size_t CPolygon::Order() const
{
  return 1;
}


void CPolygon::Mirror(const geo::IPlane& plane)
{
  m_points.Mirror(plane);
}


}//end namespace geo

