// IObject.h: interface for the IObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IOBJECT_H__1414E04D_D50F_4AFE_AE3F_83A3CDACFB46__INCLUDED_)
#define AFX_IOBJECT_H__1414E04D_D50F_4AFE_AE3F_83A3CDACFB46__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "dimple.h"
#include "DispatchVisitorBase.h"

#include "GeometryExports.h"

#ifndef VERIFY
#ifdef NDEBUG
#define VERIFY(a)				\
{								\
	((a) != 0);		\
}
#else // !NDEBUG
#define VERIFY(a)				\
{								\
	bool chk = ((a) != 0);		\
	assert(chk);				\
}
#endif  // NDEBUG
#endif

namespace geo {

// Constant ...
#define PI 3.1415926535897932384626433832795
#define TWOPI 6.283185307179586476925286766559
#define DEG_TO_RAD 0.017453292519943295769236907684886
#define RAD_TO_DEG 57.295779513082320876798154814105
#define DEG2RAD(angle) ((angle) * DEG_TO_RAD)
#define RAD2DEG(angle) ((angle) * RAD_TO_DEG)
#define EPS 1e-4
#define DBL_UNDEFINED -99999999.999


class IVector;
class IMatrix;
class ILine;
class IPoint;
class CPoint;
class IPlane;

// Copyright (c) 2011 TNO DIANA BV                              Confidential
// Description : The abstract base class "IObject" is the ancestor of all 
//			 	 geometrycal objects and provides a set of standard function 
//			 	 which aply to all objects
// Status      : Finished (100%)
// Last review : 27-03-2002 
class GEOMETRY_EXPORT  IObject  
{
protected:
	IObject(){}
public:
	// needed for polymorphic destruction of IObject derived classes
	// do not remove
	virtual ~IObject(){}

	//Rotates the object arround the origin 
	//along a vector with a specified angle
	//##ModelId=3BC2A45E0081
	virtual void RotateLine(const ILine &line, const double &dAngleDeg);
	virtual void Rotate(const IVector &vec, const double &dAngleDeg) = 0;
	

	//The move function translates the object along the 
	//vector.
	//##ModelId=3BC2A45E0085
	virtual void Move(const IVector &vec) = 0;

	//##ModelId=3BC2A45E0088
	virtual void Transform(const IMatrix &matrix) = 0;

	//##ModelId=3BC2A45E0091
	virtual void AssertValid() const = 0;

	//##ModelId=3BC2A45E0093
	virtual bool Empty() const = 0;

	//Returns the minimun value of the bounding box. If 
	//the object is undefined (Empty() == true) the function 
	//returns an undefined (Empty() == true) vertex.
	//##ModelId=3BC2A45E0095
	virtual CPoint Min() const = 0;

	//Return the maximum value of the bouding box. If the 
	//object is undefined (Empty() == true) the function returns 
	//an undefined (Empty() == true) vertex.
	//##ModelId=3BC2A45E0097
	virtual CPoint Max() const = 0;

	// checks whether the bounding boxes intersect
	// will return bIncludeEdges if the boxes only "touch"
	virtual bool BoundingBoxIntersect(const geo::IObject &rhs, bool bIncludeEdges = true) const;



	//check if boxes are exclusive outside each other
	//touching returns false (htg)
	bool BoundingBoxOutside(const geo::IObject &rhs) const;

	//check if rhs is strictly inside this box
	//touching returns false (htg)
	bool BBRhsInside(const geo::IObject &rhs) const;

	//check if point is exlusive in this box
	//boundary return false (htg)
	bool PointInsideBoundingBox(const geo::IPoint &point) const;

	//check if point is exlusive outside this box
	//boundary return false (htg)
	bool PointOutsideBoundingBox(const geo::IPoint &point) const;

	//check if point is exlusive on the boundary of this box
	bool PointOnBoundaryOfBoundingBox(const geo::IPoint &point) const;

	virtual void Mirror(const geo::IPlane& plane);

	virtual bool Visit(IVisitor &visitor) { return visitor.HandleObject(*this); }

  virtual bool IsElement() const;
  virtual bool IsInterfaceElement() const;
  virtual bool IsBody() const;

  static ftn_double_t compareTolerance();
  static ftn_double_t setCompareTolerance( ftn_double_t tolerance );

private:
  static ftn_double_t m_compareTolerance;
};

}

#endif // !defined(AFX_IOBJECT_H__1414E04D_D50F_4AFE_AE3F_83A3CDACFB46__INCLUDED_)
