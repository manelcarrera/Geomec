//*******************************************************************
//              
//  FILE      : Circle.cpp
//  AUTHOR    : htg
//  PROJECT   : Geometry
//  DATE      : 22.04.2003
//  COMMENTS  : -
// Copyright (c) 2011 TNO DIANA BV                              Confidential
//  STATUS    :
//
//*******************************************************************
#include "dimple.h"
#include "Circle.h"
#include "Line.h"
namespace geo {


//*******************************************************************
//              
//  AUTHOR    : htg
//  CLASS     : CCircle
//  COMMENTS  : -
//
//*******************************************************************
CCircle::CCircle(
		const IPoint& midpoint,
		const double& radius,
		const IVector& normal,       /* = geo::CVector::Zaxis*/
		const IVector& startvector,  /* = geo::CVector() */
		const IVector& endvector     /* = geo::CVector() */
		):
	m_MidPoint(midpoint),
	m_Radius(radius),
	m_Normal(normal)
{
	
	if(startvector.Empty())
	{
		m_StartVector= m_Normal.GetNormal();
	}
	else
	{
		m_StartVector=startvector;
	}

	if(endvector.Empty())
	{
		m_EndVector= m_Normal.GetNormal();
	}
	else
	{
		m_EndVector= endvector;
	}

}

void CCircle::Rotate(const IVector &vec, const double &dAngleDeg)
{
	ICircle::Rotate(vec,dAngleDeg);
	m_Max=CPoint();
	m_Min=CPoint();
}

void CCircle::Move(const IVector &vec)
{
	ICircle::Move(vec);
	m_Max=CPoint();
	m_Min=CPoint();
}

void CCircle::Transform(const IMatrix &matrix)
{
	ICircle::Transform(matrix);
	m_Max=CPoint();
	m_Min=CPoint();
}


CCircle::CCircle()
{
}

CCircle::CCircle(const CCircle& rhs)
{
	*this = rhs;
	m_Max=CPoint();
	m_Min=CPoint();
}

CCircle::CCircle(const ICircle& rhs)
{
	*this = rhs;
	m_Max=CPoint();
	m_Min=CPoint();
}

CCircle& CCircle::operator=(const ICircle& rhs)
{
	ICircle::operator =(rhs);
	m_Max=CPoint();
	m_Min=CPoint();
	return *this;
}

CCircle& CCircle::operator=(const CCircle& rhs)
{
	ICircle::operator =(rhs);
	m_Max=CPoint();
	m_Min=CPoint();
	return *this;
}



CCircle::~CCircle()
{

}
CPoint CCircle::Min() const
{
	if(m_Min.Empty())
		m_Min=ICircle::Min();

	return m_Min;
}

CPoint CCircle::Max() const
{
	if(m_Max.Empty())
		m_Max=ICircle::Max();

	return m_Max;
}


void CCircle::TrimLinesToCircleCorner(ILine& l1,ILine& l2,CCircle& circle ,const double& radius)
{
	if(radius < EPS)
		return;

	CPoint intersect = l1.Intersection(l2);

	if(intersect.Empty())
		return;

	
	IPoint *p1,*p2;

	if(intersect==l1.First())
		p1=const_cast<IPoint*>(&l1.First());
	else if(intersect==l1.Second())
		p1=const_cast<IPoint*>(&l1.Second());
	else
	{
		//intersection should be the first or second point of the line
		assert(false);
		return;
	}

	if(intersect==l2.First())
		p2 = const_cast<IPoint*>(&l2.First());
	else if(intersect==l2.Second())
		p2=const_cast<IPoint*>(&l2.Second());
	else
	{
		//intersection should be the first or second point of the line
		assert(false);
		return;
	}

	CPtrArray<CCircle> circles;
	LineCorner(l1,l2,radius,circles);

	assert(circles.Size()==4);

	for(size_t i =0; i<circles.Size();i++)
	{
		CCircle* cir = &circles.Object(i);

		if(l1.Contains(cir->StartPoint()))
		{
			if(l2.Contains(cir->EndPoint()))
			{
				circle = *cir;
				*p1=cir->StartPoint();
				*p2=cir->EndPoint();
				circles.ClearAndDelete();
				return;
			}
		}
		else if(l1.Contains(cir->EndPoint()))
		{
			if(l2.Contains(cir->StartPoint()))
			{
				circle=*cir;
				*p1=cir->EndPoint();
				*p2=cir->StartPoint();
				circles.ClearAndDelete();
				return;

			}
		}

	}
	circles.ClearAndDelete();
}

void CCircle::LineCorner(const ILine& l1,const ILine& l2,const double& radius,  CPtrArray<CCircle>& circles)
{
	if(radius < EPS)
		return;

	assert(circles.Size() == 0 );

	CPoint pi = l1.Intersection(l2);
	if(pi.Empty())
		return;

	CVector normal = l1.Dir().CrossProduct(l2.Dir());
	CVector n1 = normal.CrossProduct(l1.Dir());
	CVector n2 = normal.CrossProduct(l2.Dir());

	n1 = n1.UnitVector();
	n2 = n2.UnitVector();


	CLine l1a=l1;
	CLine l1b=l1;

	CLine l2a=l2;
	CLine l2b=l2;

	l1a.Move(n1*radius);
	l1b.Move(n1*(radius*-1.0));

	l2a.Move(n2*radius);
	l2b.Move(n2*(radius*-1.0));

	CPoint p1a2a = l1a.Intersection(l2a);
	CPoint p1a2b = l1a.Intersection(l2b);
	CPoint p1b2a = l1b.Intersection(l2a);
	CPoint p1b2b = l1b.Intersection(l2b);

	circles.PushBack( *new CCircle(p1a2a,radius,normal) );
	circles.PushBack( *new CCircle(p1a2b,radius,normal) );
	circles.PushBack( *new CCircle(p1b2a,radius,normal) );
	circles.PushBack( *new CCircle(p1b2b,radius,normal) );

	assert(circles.Size()==4);
	for(size_t i =0; i<circles.Size();i++)
	{
		CCircle& circle = circles.Object(i);
		CPtrArray<IPoint> points;

		circle.IntersectionWithLineInPlane(l1,points);
		assert(points.Size()==1);
		CPoint p1 = points.Object(0);

		points.ClearAndDelete();

		circle.IntersectionWithLineInPlane(l2,points);
		assert(points.Size()==1);
		CPoint p2 = points.Object(0);
		points.ClearAndDelete();

		circle.StartVector(CVector(p1-circle.MidPoint()));
		circle.EndVector(CVector(p2-circle.MidPoint()));
		if(circle.CircleAngleDeg() > 180.0)
		{
			circle.Normal(circle.Normal()*-1);
		}
	}




}



}//end namespace geo
