 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// Conus.cpp: implementation of the CConus class.
//
//////////////////////////////////////////////////////////////////////
#include "dimple.h"
#include <cassert>
#include "Conus.h"
#include "Circle.h"
#include <cmath>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace geo {

CConus::CConus(	const geo::IPoint& basepoint,
				const geo::IVector& direction,
				const double& radius,
				const double& height,
				const double& ratio)
:
m_BasePoint(basepoint),
m_Direction(direction),
m_Radius(radius),
m_Height(height),
m_Ratio(ratio)
{

}


CConus::CConus(const IConus& right)
:
m_BasePoint(right.BasePoint()),
m_Direction(right.Direction()),
m_Radius(right.Radius()),
m_Height(right.Height()),
m_Ratio(right.Ratio())
{
	
}

CConus& CConus::operator=(const IConus& right)
{
	m_BasePoint = right.BasePoint();
	m_Direction = right.Direction();
	m_Radius = right.Radius();
	m_Height= right.Height();
	m_Ratio = right.Ratio();
	return *this;
}

CConus::~CConus()
{

}

void CConus::Change(	const geo::IPoint& basepoint,
			const geo::IVector& direction,
			const double& radius,
			const double& height,
			const double& ratio)
{
	m_BasePoint = basepoint;
	m_Direction = direction;
	m_Radius = radius;
	m_Height=height;
	m_Ratio=ratio;
}


/*virtual*/ void CConus::Rotate(const IVector &vec,const double &dAngleDeg)
{
	m_BasePoint.Rotate(vec,dAngleDeg);
	m_Direction.Rotate(vec,dAngleDeg);

}

/*virtual*/ void CConus::Move(const IVector &vec)
{
	m_BasePoint.Move(vec);
}

/*virtual*/ void CConus::Transform(const IMatrix &matrix)
{
	m_BasePoint.Transform(matrix);
	m_Direction.Transform(matrix);
	
}

/*virtual*/ void CConus::AssertValid() const
{
	assert(!m_BasePoint.Empty());
	assert(!m_Direction.Empty());
	assert(m_Radius > EPS);
	assert(m_Height > EPS);
}

/*virtual*/ bool CConus::Empty() const
{
	if(m_BasePoint.Empty())
		return true;

	if(m_Direction.Empty())
		return true;

	return false;
}

/*virtual*/ CPoint CConus::Min() const
{
	CCircle c1(m_BasePoint,m_Radius,m_Direction);
	CPoint ret = c1.Min();
	
	if(fabs(m_Ratio) > EPS)
	{
		CCircle c2(m_BasePoint+m_Direction.UnitVector()*m_Height,m_Radius*m_Ratio,m_Direction);
		ret= ret.Min(c2.Min());
	}
	return ret; 
}

/*virtual*/ CPoint CConus::Max() const
{
	CCircle c1(m_BasePoint,m_Radius,m_Direction);
	CPoint ret = c1.Max();
	
	if(fabs(m_Ratio) > EPS)
	{
		CCircle c2(m_BasePoint+m_Direction.UnitVector()*m_Height,m_Radius*m_Ratio,m_Direction);
		ret= ret.Max(c2.Max());
	}
	return ret; 

}

/*virtual*/ const geo::IPoint& CConus::BasePoint() const
{
	return m_BasePoint;
}

/*virtual*/ const geo::IVector& CConus::Direction() const
{
	return m_Direction;
}

/*virtual*/ const double& CConus::Radius() const
{
	return m_Radius;
}

/*virtual*/ const double& CConus::Height() const
{
	return m_Height;
}

/*virtual*/ const double& CConus::Ratio() const
{
	return m_Ratio;
}

}//end namespace
