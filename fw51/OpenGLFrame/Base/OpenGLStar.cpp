 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "dimple.h"
#include "OpenGLStar.h"
#include "Line.h"
#include "Polygon.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



COpenGLStar::COpenGLStar(long NrOfStarPoints,double StarPointAngle)
:m_NrOfStarPoints(NrOfStarPoints),m_StarPointAngle(StarPointAngle)
{

}



geo::IPolygon* COpenGLStar::CreateStar(double Size)
{
  geo::CArray<geo::CPoint> points;

  double dAngle = 360.0/m_NrOfStarPoints;

  double dRad=Size/2.0;

  geo::CPoint p1(0,dRad);
  geo::CLine l1(geo::CPoint::NullPoint,p1);
  geo::CLine l2(geo::CPoint::NullPoint,p1);
  l1.Rotate(geo::CVector::Zaxis,dAngle/2.0);

  geo::CVector trans=l2.Second();
  l2.Move(trans*-1);
  l2.Rotate(geo::CVector::Zaxis,-m_StarPointAngle/2.0);
  l2.Move(trans);
  
  geo::CPoint p2 = l1.Intersection(l2);
  for(int i=0 ;i<m_NrOfStarPoints;i++)
  {
    double angle = double(i)*dAngle;
    geo::CPoint pt1(p1);
    geo::CPoint pt2(p2);
    pt1.Rotate(geo::CVector::Zaxis,angle);
    pt2.Rotate(geo::CVector::Zaxis,angle);
    points.PushBack(pt1);
    points.PushBack(pt2);
  }

  geo::CPolygon* ret=new geo::CPolygon(points);
  return ret;
}

COpenGLStar::~COpenGLStar()
{

}
