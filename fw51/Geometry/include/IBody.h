// IBody.h: interface for the IBody class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IBODY_H__66D8404B_41CC_4B8B_9CB8_30FFD918DA42__INCLUDED_)
#define AFX_IBODY_H__66D8404B_41CC_4B8B_9CB8_30FFD918DA42__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <set>
#include "Line.h"
#include "DispatchVisitorBase.h"

#include "GeometryExports.h"

namespace geo {

class IFace;
class IPlane;
class CPolygon;
// Copyright (c) 2011 TNO DIANA BV                              Confidential
// Description : The IBody object represents an closed 3D body 
// Status      : Finished (100%)
// Remarks     : 
// Last review : 27-03-2002 
class GEOMETRY_EXPORT  IBody  : public IElement
{
public:
	typedef std::vector<int> TIndexVec;
	IBody();

	// the number of faces surrounding this body
	virtual int NrOfFaces() const = 0;
	// face retrieval
	virtual const IFace &Face(int nIndex) const = 0;
	IFace& Face(int nIndex);

	// return the name of the face given the index
	virtual const char* FaceName(int nIndex) const = 0;
	virtual const char* SideName(int nIndex) const;

	// the number of (unique) lines that this body contains
	virtual int NrOfLines() const = 0;
	// line retrieval
	virtual const ILine &Line(int nIndex) const = 0;
	virtual const TIndexVec& LinePointIndices(int nIndex) const = 0;
	virtual const TIndexVec& LinePointIndices(const ILine& line) const;
	virtual const TIndexVec& LineNodeIndices(int nIndex) const = 0;
	virtual const TIndexVec& LineNodeIndices(const ILine &line) const;
	
	// returns a vector of this body's local point indices that the face with the
	// given index is connected to (this is without higher order intermediate nodes)
	virtual const TIndexVec& FacePointIndices(int nIndex) const = 0;
	// same as above, given the face instead of the index
	virtual const TIndexVec& FacePointIndices(const IFace &face) const;

	// returns a vector of this body's local node indices that the face with the
	// given index is connected to (this includes higher order intermediate nodes)
	virtual const TIndexVec& FaceNodeIndices(int nIndex) const = 0;
	// same as above, given the face instead of the index
	virtual const TIndexVec& FaceNodeIndices(const IFace &face) const;

	double Volume() const { return Size(); }

	virtual bool Intersects(const IPlane &plane, bool bIncludeEdge = true) const; 
	// PRE	: Body is convex
	// POST	: Return polygon is not Empty() (At least three points from the polygon)
	//		  Return polygon is empty (There is no intersection or the intersection
	//		  is a single line or a single point).
	virtual CPolygon IntersectionConv(const IPlane &plane) const;
	virtual void Intersection(const IPlane &plane, std::set<CLine, ILine::CLineLess> &LineSet, std::set<CPoint> *PointSet = 0) const;
	virtual void Intersection(const ILine &line, std::set<CPoint> &PointSet) const;
	virtual bool Contains(const IPoint &point, bool bIncludeEdge) const;

	virtual int FaceIndex(const IFace &face) const;

	typedef std::pair<double, std::vector<CPoint> > TIntersection;
	TIntersection Intersection(const IBody &rhs) const;

	// return a representative length for this element
	// length of a line
	// square root of area for a face
	// 3rd square root of volume for a body
	virtual double RepresentativeLength() const;

  virtual bool IsBody() const;

	virtual bool Visit(IVisitor &visitor) { return visitor.HandleBody(*this); }
};
}

#endif // !defined(AFX_IBODY_H__66D8404B_41CC_4B8B_9CB8_30FFD918DA42__INCLUDED_)
