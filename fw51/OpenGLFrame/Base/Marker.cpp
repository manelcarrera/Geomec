 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include <cmath>
#include "OpenGLStar.h"
#include "Marker.h"
#include "Polygon.h"

//////////////////////////////////////////////////////////////////////
// Implementation of CMarker
//////////////////////////////////////////////////////////////////////

void CMarker::CreateGeometry()
{
  m_geometry.ClearAndDelete();
  double unit=m_dSize/6;

  geo::CArray<geo::CPoint> points;
  switch(Type())
  {
  case CMarker::MT_STAR :
  {
    COpenGLStar star;
    geo::IPolygon* pPolygon = star.CreateStar(m_dSize*1.4);
    m_geometry.PushBack(*pPolygon);
    m_geometry.Move(geo::CVector(PivotPoint()));
    break;
  }
  case CMarker::MT_POINT :
  {
    geo::CPoint p0(-3*unit,3*unit);
    points.PushBack(p0);
    geo::CPoint p1(3*unit,3*unit);
    points.PushBack(p1);
    geo::CPoint p2(3*unit,-3*unit);
    points.PushBack(p2);
    geo::CPoint p3(-3*unit,-3*unit);
    points.PushBack(p3);
    m_geometry.PushBack(*(new geo::CPolygon(points)));
    m_geometry.Move(geo::CVector(PivotPoint()));
    break;
  }
  case CMarker::MT_TRIANGLE :
  {
    geo::CPoint p0(-4*unit,-2*unit);
    points.PushBack(p0);
    geo::CPoint p1(0,4*unit);
    points.PushBack(p1);
    geo::CPoint p2(4*unit,-2*unit);
    points.PushBack(p2);
    m_geometry.PushBack(*(new geo::CPolygon(points)));
    m_geometry.Move(geo::CVector(PivotPoint()));
    break;
  }
  case CMarker::MT_CIRCLE :
    double rad =3*unit;
    int NrOfSegments=10;
    double delta=2*PI/double(NrOfSegments-1);
    for(int i=0;i<NrOfSegments-1;i++)
    {
      double angle=double(i)*delta;
      double x=rad*cos(angle);
      double y=rad*sin(angle);
      geo::CPoint p0(x,y);
      points.PushBack(p0);
    }
    m_geometry.PushBack(*(new geo::CPolygon(points)));
    m_geometry.Move(geo::CVector(PivotPoint()));
    break;
  }


}

//////////////////////////////////////////////////////////////////////
// Implementation of CMarker
//////////////////////////////////////////////////////////////////////
CMarker::CMarker(const geo::IPoint& location, 
         TColor color, 
         TMarkerType type,
         const QString& sLabel,
         bool bFill, 
         const double& dSize)
: m_bFill(bFill),
  m_location(location),
  m_sLabel(sLabel),
  m_color(color),
  m_type(type),
  m_dSize(dSize)
{
  CreateGeometry();
}

CMarker::CMarker(const CMarker& rhs)
: m_bFill(rhs.m_bFill),
  m_location(rhs.m_location), 
  m_sLabel(rhs.m_sLabel), 
  m_color(rhs.m_color), 
  m_type(rhs.m_type), 
  m_dSize(rhs.m_dSize)
{
  CreateGeometry();
}

const QString& CMarker::Label() const 
{
  return m_sLabel;
}

void CMarker::Label(const QString& sLabel)
{
  m_sLabel = sLabel;
}

CMarker::TColor CMarker::Color() const 
{
  return m_color;
}

CMarker::TMarkerType CMarker::Type() const 
{
  return m_type;
}

bool CMarker::Fill() const 
{
  return m_bFill;
}

void CMarker::Fill(bool bFill) 
{
  m_bFill = bFill;
}

void CMarker::Rotate(const geo::IVector &vec, const double &dAngleDeg)
{
  m_location.Rotate(vec, dAngleDeg);
}

void CMarker::Move(const geo::IVector &vec)
{
  m_location.Move(vec);
}

void CMarker::Transform(const geo::IMatrix &matrix)
{
  m_location.Transform(matrix);
}


CMarker::~CMarker()
{
  m_geometry.ClearAndDelete();
}

int CMarker::NrOfPositions() const
{
  return 1;
}

const geo::IPoint& CMarker::Position(int Index) const
{
  assert(Index == 0);
  return m_location;
}

const geo::IArray& CMarker::Geometry() const
{
  return m_geometry;
}

const geo::IPoint& CMarker::PivotPoint() const
{
  return geo::CPoint::NullPoint;
}

/*virtual*/ 
void CMarker::AssertValid() const
{
  assert(!m_geometry.Empty());
  assert(!m_location.Empty());
}

/*virtual*/ 
bool CMarker::Empty() const
{
  return 	(m_geometry.Empty() || m_location.Empty());
}

/*virtual*/ 
geo::CPoint CMarker::Min() const
{
  return m_location; //world point not max of geometry =(screen)
}

/*virtual*/ 
geo::CPoint CMarker::Max() const
{
  return m_location; //world point
}

void CMarker::Position(const geo::IPoint& point)
{
  m_location = point;
}
