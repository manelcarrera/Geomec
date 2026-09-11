// ISphere.h: interface for the ISphere class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ISPHERE_H__BF0B2D73_FD2D_45A1_ACFC_D08722742549__INCLUDED_)
#define AFX_ISPHERE_H__BF0B2D73_FD2D_45A1_ACFC_D08722742549__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IObject.h"
#include "VectorTempl.h"
#include "DispatchVisitorBase.h"

#include "GeometryExports.h"

namespace geo
{
	class ILine;
// Copyright (c) 2011 TNO DIANA BV                              Confidential
// Description : The ISphere object represents a sphere 3D space 
// Status      : Finished (80%) 
// Remarks     : We need a ICircle as a result from the intersections with a plane
//	           :  	
// Last review : 27-03-2002 
class GEOMETRY_EXPORT  ISphere : public IObject
{
protected:
	ISphere();
public:
	// Data access
	virtual const double& Radius() const = 0;
	virtual void Radius(const double &radius) = 0;
	virtual const IPoint& MidPoint() const = 0;
	virtual void MidPoint(const IPoint& midpoint) = 0;


	// 
	void Rotate(const IVector &vec, const double &dAngleDeg);
	void Move(const IVector &vec);
	void Transform(const IMatrix &matrix);
	void AssertValid() const;
	virtual CPoint Min() const;
	virtual CPoint Max() const;
	virtual bool Empty() const;

	CArray<CPoint> Intersection(const ILine &line) const;

	virtual bool Visit(IVisitor &visitor) { return visitor.HandleSphere(*this); }

};
}

#endif // !defined(AFX_ISPHERE_H__BF0B2D73_FD2D_45A1_ACFC_D08722742549__INCLUDED_)
