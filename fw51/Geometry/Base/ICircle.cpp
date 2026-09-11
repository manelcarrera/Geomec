//*******************************************************************
//              
//  FILE      : ICircle.cpp
//  AUTHOR    : htg
//  PROJECT   : Geometry.lib
//  DATE      : 22.04.2003
//  COMMENTS  : -
// Copyright (c) 2011 TNO DIANA BV                              Confidential
//  STATUS    :
//
//*******************************************************************
#include "dimple.h"
#include <cmath>
#include "ILine.h"
#include "ICircle.h"
#include "Vector.h"
namespace geo {

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


ICircle& ICircle::operator=(const ICircle& rhs)
{
	if(rhs.Empty())
	{
		MidPoint(CPoint());
		Radius(0);
		Normal(CVector());
		StartVector(CVector());
		EndVector(CVector());
		assert(Empty());
	}
	else
	{
		MidPoint(rhs.MidPoint());
		Radius(rhs.Radius());
		Normal(rhs.Normal());
		StartVector(rhs.StartVector());
		EndVector(rhs.EndVector());
		assert(!Empty());
	}
	return *this;
}


ICircle::ICircle()
{
	return;
}

ICircle::~ICircle()
{
	return;
}


CPoint ICircle::StartPoint() const
{
	return (MidPoint() + StartVector().UnitVector()*Radius());
}

CPoint ICircle::EndPoint() const
{
	return (MidPoint() + EndVector().UnitVector()*Radius());
}

double ICircle::CircleAngleDeg() const
{
	if(StartVector().IsParallel(EndVector()))
		return 360;

	double angle = StartVector().AngleDeg(EndVector());

	if(fabs(angle) < EPS)
		return 360;

	if(!(StartVector().CrossProduct(EndVector()).UnitVector() == Normal().UnitVector()))
	{
		angle = 360.0 - angle;
	}

	return angle;
}

//returning 0,1 or 2 intersections
void ICircle::IntersectionWithLineInPlane(const ILine& line, CPtrArray<IPoint>& points) const
{
	assert(Normal().IsPerpendicular(line.Dir()));



	CArray<CPoint> ret;
	CVector sv = line.First() - MidPoint();
	CVector rv = line.Dir();

	double a = rv.SquareLength();
	double b = 2.0 * rv.DotProduct(sv);
	double c = sv.SquareLength() - Radius() * Radius();

	double D;
	

	if(fabs((b * b)/( 4.0 * a * c)) - 1 < EPS)
		D=0;
	else
		D = b * b - 4.0 * a * c;

	
	
	if ( D < -EPS )	{
		//No intersection points
		return ;
	} else {
		double sqrtD=sqrt(fabs(D));
		// at least one intersection or tangens line
		double labda = (-b + sqrtD) / (2.0 * a);
		CPoint p;
		p = CPoint(line.First() + rv * labda);
		points.PushBack(*new CPoint(p));
		if(fabs(D) > EPS)
		{
			// two intersection points
			labda = (-b - sqrtD) / (2.0 * a);
			p = CPoint(line.First() + rv * labda);
			points.PushBack(*new CPoint(p));
		}
		return;
	}
	

}

void ICircle::GetPointsOnCircumference(CArray<CPoint>& points,long iNumberOfPoints /*=10*/) const
{
	CPtrArray<IPoint> tmp_points;
	GetPointsOnCircumference(tmp_points,iNumberOfPoints);
	for(size_t i=0;i<tmp_points.Size();i++)
	{
		CPoint p(tmp_points.Object(i));
//		points.PushBack(CPoint(tmp_points.Object(i)));
		points.PushBack(p);
	}
	tmp_points.ClearAndDelete();
}

void ICircle::GetPointsOnCircumference(CPtrArray<IPoint>& points,long iNumberOfPoints /*=10*/) const
{
	assert(iNumberOfPoints >= 4);

	double angle = CircleAngleDeg();
	
	double delta = angle / double(iNumberOfPoints);
	CPoint start = StartPoint();

	for(long i=0;i<iNumberOfPoints;i++)
	{
		points.PushBack(*new CPoint(start));
		start.Move(CVector(MidPoint()*-1));
		start.Rotate(Normal(),delta);
		start.Move(CVector(MidPoint()));
	}

}



void ICircle::Rotate(const IVector &vec, const double &dAngleDeg)
{
	CPoint mid(MidPoint());
	mid.Rotate(vec,dAngleDeg);
	MidPoint(mid);

	CVector norm(Normal());
	norm.Rotate(vec,dAngleDeg);
	Normal(norm);

	CVector start(StartVector());
	start.Rotate(vec,dAngleDeg);
	StartVector(start);

	CVector end(EndVector());
	end.Rotate(vec,dAngleDeg);
	EndVector(end);
	
}

void ICircle::Move(const IVector &vec)
{
	CPoint mid(MidPoint());
	mid.Move(vec);
	MidPoint(mid);
}

void ICircle::Transform(const IMatrix &matrix)
{
	CPoint mid(MidPoint());
	mid.Transform(matrix);
	MidPoint(mid);

	CVector norm(Normal());
	norm.Transform(matrix);
	Normal(norm);

	CVector start(StartVector());
	start.Transform(matrix);
	StartVector(start);

	CVector end(EndVector());
	end.Transform(matrix);
	EndVector(end);

}

void ICircle::AssertValid() const
{
	if(Radius() < EPS)
	{
		assert(false);
	}
}

bool ICircle::Empty() const
{
	if(MidPoint().Empty())
		return true;

	if(Normal().Empty())
		return true;

	return false;

}

CPoint ICircle::Min() const
{
	CPtrArray<IPoint> points;
	GetPointsOnCircumference(points);
	CPoint Min = points.Min();
	points.ClearAndDelete();
	return Min;
}

CPoint ICircle::Max() const
{
	CPtrArray<IPoint> points;
	GetPointsOnCircumference(points);
	CPoint Max = points.Max();
	points.ClearAndDelete();
	return Max;
}



}//end namespace geo
