// Conus.h: interface for the CConus class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONUS_H__B13544C2_A081_4BAB_BEE2_ABFEBBAAA9DE__INCLUDED_)
#define AFX_CONUS_H__B13544C2_A081_4BAB_BEE2_ABFEBBAAA9DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IConus.h"
#include "Point.h"
#include "Vector.h"

#include "GeometryExports.h"

//*******************************************************************
//              
//  FILE:       Conus.h
//  AUTHOR:     htg
//  PROJECT:    Geometry
//  COMPONENT:  CConus
//  DATE:       08.10.2003
// Copyright (c) 2011 TNO DIANA BV                              Confidential
//  COMMENTS:   The ratio is the ratio of the upper radius and lower radius
//				if the ratio is 1 the it is a cilinder		
//              
//*******************************************************************
namespace geo {
class GEOMETRY_EXPORT  CConus : public IConus  
{
public:
	CConus(	const geo::IPoint& basepoint,
			const geo::IVector& direction,
			const double& radius,
			const double& height,
			const double& ratio);
	
	CConus(const IConus& right);
	CConus&		operator=(const IConus& right);

	void Change(	const geo::IPoint& basepoint,
			const geo::IVector& direction,
			const double& radius,
			const double& height,
			const double& ratio);

	virtual ~CConus();

	//IObject interface
	virtual void Rotate(const IVector &vec, const double &dAngleDeg);
	virtual void Move(const IVector &vec);
	virtual void Transform(const IMatrix &matrix);
	virtual void AssertValid() const;
	virtual bool Empty() const;
	virtual CPoint Min() const;
	virtual CPoint Max() const;

	//IConus interface
	virtual const geo::IPoint&  BasePoint() const;
	virtual const geo::IVector& Direction() const;
	virtual const double& Radius() const;
	virtual const double& Height() const;
	virtual const double& Ratio() const;

private:
	geo::CPoint m_BasePoint;
	geo::CVector m_Direction;
	double m_Radius;
	double m_Height;
	double m_Ratio;

};

}

#endif // !defined(AFX_CONUS_H__B13544C2_A081_4BAB_BEE2_ABFEBBAAA9DE__INCLUDED_)
