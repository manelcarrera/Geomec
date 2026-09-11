// IVector.h: interface for the IVector class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IVECTOR_H__A4667E13_AFB0_4289_BF82_7B365EE0562C__INCLUDED_)
#define AFX_IVECTOR_H__A4667E13_AFB0_4289_BF82_7B365EE0562C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ICoordinate.h"
#include "DispatchVisitorBase.h"

#include "GeometryExports.h"

namespace geo {
class CVector;


// Copyright (c) 2011 TNO DIANA BV                              Confidential
// Description : Interface for a 3D geometrical vector
// Status      : Finished (100%)
// Remarks     : See CVector for default implementation
// Last review : 27-03-2002 
class GEOMETRY_EXPORT  IVector : public ICoordinate
{
protected:
	IVector();
public:
        virtual double operator[]( int i ) const = 0;
	double SquareLength();
	CVector GetNormal() const;
	double Length() const;
	//the angle between two vectors in degrees
	double AngleDeg(const IVector &vector) const;
	//the angle between two vectors in radians
	double AngleRad(const IVector &vector) const;

	CVector operator+(const IVector &vector) const;
	CVector operator-(const IVector &vector) const;
	CVector operator*(const double &d) const;
	CVector operator/(const double &val) const;
	CVector operator-() const;
	IVector& operator+=(const IVector& rhs);
	IVector& operator-=(const IVector& rhs);

	bool operator==(const IVector &rhs) const;
  bool isNullVector() const;

	//returns the crossproduct of two vectors (that is the 
	//normal vector of the two vectors)

	CVector CrossProduct(const IVector &vector) const;
	double DotProduct(const IVector &vector) const;

	//Returns the vector with length 1 (normal)
	CVector UnitVector() const;

	//see if two vectors are parallel
	bool IsParallel(const IVector &v) const;

	// see if two vectors are perpendicular
	bool IsPerpendicular(const IVector &v) const;
	
	//see if two vectors point in opposite direction
	bool IsOppositeDirection(const IVector &v) const;

	//flips the vector (inverts the sign of x,y,z)
	void Flip();

	// returns a flipped vector
	CVector Flipped() const;

	// returns the in-plane component of this vector, where the plane is
	// represented by the given normal. The normal must be a unit vector
	CVector InPlaneComponent(const IVector &normal) const;

	//if you want to rotate multiple items with the same angle around the same vector
	//use this function
	//this function returns the complete transformation-matrix to rotate objects with
	//the given angle arround this vector
	void GetRotationMatrix(const double &AngleDeg, IMatrix& matrix ) const;

	//CMatrix GetRotationMatrix(const double &AngleDeg) const;

	virtual bool Visit(IVisitor &visitor) { return visitor.HandleVector(*this); }

};

}

#endif // !defined(AFX_IVECTOR_H__A4667E13_AFB0_4289_BF82_7B365EE0562C__INCLUDED_)
