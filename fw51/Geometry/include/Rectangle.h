// Rectangle.h: interface for the CRectangle class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RECTANGLE_H__D0B2ABC2_64FF_45F7_BE5A_D84FB1CFE061__INCLUDED_)
#define AFX_RECTANGLE_H__D0B2ABC2_64FF_45F7_BE5A_D84FB1CFE061__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IRectangle.h"
#include "Vector.h"
#include "Node.h"

#include "GeometryExports.h"

namespace geo{
// Copyright (c) 2011 TNO DIANA BV                              Confidential
// Description	: The CRectangle object represents a rectangle in 3D
// Status		: Finished (50%) 
// Remarks		: 
//				:  	
// Last review	: 27-03-2002 
class GEOMETRY_EXPORT  CRectangle : public IRectangle
{
	//std::vector<CPoint> m_vcPoint;

public:
	CRectangle();
	CRectangle(const IPoint &RefPoint, const IVector &Dir1, const IVector &Dir2);

	static bool ValidRectangleData(const IPoint &Point1, const IPoint &Point2);
	// Point1 and Point2 have to be corner points!!
	CRectangle(const IPoint &Point1, const IPoint &Point2);  // TNMA: constructor does not work properly! (try making a rectangle in XY-plane)
	
	
	CRectangle(const IRectangle &rhs);
	virtual ~CRectangle();

	virtual const IPoint& Point(int nIndex) const;
	virtual void Point(int nIndex, const IPoint& point);
	virtual size_t Order() const;

	virtual const IPoint& Refpoint() const;
	virtual const IVector& Direction1() const;
	virtual const IVector& Direction2() const;

	//IObject impl.
	virtual void Rotate(const IVector &vec, const double &dAngleDeg);
	virtual void Move(const IVector &vec);
	virtual void Transform(const IMatrix &matrix);

	virtual size_t NrOfNodes() const;
	virtual const INode& Node(int nIndex) const;
	virtual void Node(int nIndex, const IPoint& point);

private:
	CPoint m_RefPoint;
	CVector m_vcDirection1;
	CVector m_vcDirection2;
	std::vector<CNode> m_vcCornerPoints;
	void CreateCornerPoints();
};

}
#endif // !defined(AFX_RECTANGLE_H__D0B2ABC2_64FF_45F7_BE5A_D84FB1CFE061__INCLUDED_)
