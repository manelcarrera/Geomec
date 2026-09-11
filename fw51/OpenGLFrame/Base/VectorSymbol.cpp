 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "dimple.h"
#include "Conus.h"
#include "Line.h"
#include "VectorSymbol.h"
#include "OpenGLArrow.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define CONESIZE 3
#define CONEHEIGHT 3.5*CONESIZE

CVectorSymbolBase::CVectorSymbolBase(const geo::IPoint &location, const geo::IVector &dir, const double &length, bool TailIsPivot /*=true*/)
: m_location(location)
{
  assert(dir.Length() > EPS);
  assert(length > EPS);

  

  if(TailIsPivot)
    m_pivot=geo::CPoint::NullPoint;
  else
    m_pivot=geo::CPoint(dir.UnitVector() * length);

}

CVectorSymbolBase::~CVectorSymbolBase()
{
  m_geometry.ClearAndDelete();
}


int CVectorSymbolBase::NrOfPositions() const
{
  return 1;
}

const geo::IPoint& CVectorSymbolBase::Position(int Index) const
{
  assert(Index == 0);
  return m_location;
}

const geo::IArray& CVectorSymbolBase::Geometry() const
{
  return m_geometry;
}

const geo::IPoint& CVectorSymbolBase::PivotPoint() const
{
  return m_pivot;
}

/*virtual*/ 
void CVectorSymbolBase::AssertValid() const
{
  assert(!m_pivot.Empty());
  assert(!m_geometry.Empty());
  assert(!m_location.Empty());
}

/*virtual*/ 
bool CVectorSymbolBase::Empty() const
{
  return 	(m_pivot.Empty() || m_geometry.Empty() || m_location.Empty());
}

/*virtual*/ 
geo::CPoint CVectorSymbolBase::Min() const
{
  return m_location; //world point not max of geometry =(screen)
}

/*virtual*/ 
geo::CPoint CVectorSymbolBase::Max() const
{
  return m_location; //world point
}

/*virtual*/ void CVectorSymbolBase::Rotate(const geo::IVector &vec,const double &dAngleDeg)
{
  m_location.Rotate(vec,dAngleDeg);
}

/*virtual*/ void CVectorSymbolBase::Move(const geo::IVector &vec)
{
  m_location.Move(vec);
}

/*virtual*/ void CVectorSymbolBase::Transform(const geo::IMatrix &matrix)
{
  m_location.Transform(matrix);
}




void CVectorSymbol::CreateGeometry(const geo::IVector &vector)
{
  geo::CLine *line = new geo::CLine(geo::CPoint::NullPoint, vector);
  geo::CConus *conus = new geo::CConus(line->Point(1)-vector.UnitVector()*CONEHEIGHT , vector, CONESIZE, CONEHEIGHT, 0);
  m_geometry.PushBack(*line);
  m_geometry.PushBack(*conus);
}


void CArrowSymbol::CreateGeometry(const geo::IVector &vector)
{
  COpenGLArrow* arrow = new COpenGLArrow(geo::CPoint::NullPoint,vector);
  m_geometry.PushBack(*arrow);
}


CVectorSymbol::CVectorSymbol(const geo::IPoint &location, const geo::IVector &dir, const double &length,bool TailIsPivot)
:CVectorSymbolBase(location,dir,length,TailIsPivot)
{
  CreateGeometry(geo::CVector(dir.UnitVector() * length));
}

CArrowSymbol::CArrowSymbol(const geo::IPoint &location, const geo::IVector &dir, const double &length,bool TailIsPivot)
:CVectorSymbolBase(location,dir,length,TailIsPivot)
{
  CreateGeometry(geo::CVector(dir.UnitVector() * length));
}
