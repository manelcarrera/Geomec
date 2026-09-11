/* Confidential Source Code Copyright (c) 2011 TNO DIANA BV                              Confidential */
// ISurface.h: interface for the ISurface class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ISURFACE_H__9B9CCA93_D91E_4ACE_AA56_B5E091385C7F__INCLUDED_)
#define AFX_ISURFACE_H__9B9CCA93_D91E_4ACE_AA56_B5E091385C7F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include "IElementSet.h"
//#include "VectorTempl.h"
#include "PolyLine.h"
#include "DispatchVisitorBase.h"

class IProgressBase;

namespace geo {
class CPolyLine;
class CVector;
class CValue;
class IFace;
class CLine;
class CNearestNeighbour;

class GEOMETRY_EXPORT  ISurface : public IElementSet
{
private:
	class GEOMETRY_EXPORT CPointPair
	{
	public:
		CPointPair(const IPoint *p1, const IPoint *p2);
		~CPointPair();
		bool operator<(const CPointPair &rhs) const;
		const IPoint &First() const;
		const IPoint &Second() const;

	private:
		const IPoint *m_p1;
		const IPoint *m_p2;
	};

	typedef std::set<CPointPair> TPointPairSet;
	typedef std::map<CPointPair*, std::vector<const IFace*> > TPointMap;

	mutable std::vector<CNearestNeighbour *> m_vcEdgeNearestNeighbour;

	void InsertPointsIntoMapAndSet(TPointPairSet &stPointPairs, TPointMap &mpFaces, const IPoint &pt1, const IPoint &pt2, const IFace &face) const;
	void CreateConnectivityPointMapAndSet(TPointPairSet &stPointPairs, TPointMap &mpFaces) const;
	std::multimap<double, const IPoint*> NearestXYPoints(int size_x, int size_y, const double& dX, const double& dY) const;
	void ClearEdgeNearestNeighbours() const;
	void CreateEdgeNearestNeighbours(std::vector<std::vector<const IPoint *> > &vcEdgePoints) const;
  bool LocateRingIndex(const IPoint& point, std::set<const IPoint*>& stEncountered, std::map<const IPoint*, int>& ringmap) const;

protected:
	ISurface();
	virtual void InvalidateCache();
public:
	//##ModelId=3BC2A45C0052
	virtual ~ISurface();
	virtual const CPtrArray <IFace> FacesAt(const IPoint &p) const = 0;
	virtual const CPtrArray <IFace> FacesAtNode(const IPoint &p) const =0;
	virtual const IFace &Face(int nIndex) const = 0;
	virtual int FaceSize() const = 0;
  virtual void reserveFaceSize(size_t size) {}

	virtual const IElement &Element(int nIndex) const;
	virtual int ElementSize() const;

	virtual const IPoint& Point(int nIndex) const = 0;

	virtual const CVector AverageNormal() const;
	virtual const CVector AverageNormalInPoint(const IPoint &p) const;

	virtual bool Planar() const {return false;} // should be defined in derivative classes
	//the intersection of a surface and a line is defined
	//as an array of CVertex
	//##ModelId=3BC2A45C0086
	//THE FOLLOWING FUNCTION IS COMMENTED OUT SINCE A GOOD IMPLEMENTATION IS NOT YET AVAILABLE.
/*	virtual const CArray<CPoint> Intersection(const ILine &line) const;	*/

	//the intersection of two surfaces is defined
	//as an array(std::vector) of polylines
	//##ModelId=3BC2A45C0090
	//THE FOLLOWING FUNCTION IS COMMENTED OUT SINCE A GOOD IMPLEMENTATION IS NOT YET AVAILABLE.
/*	virtual const CArray<CLine> &Intersection(const ISurface &surface) const;	*/

	typedef std::vector<CPolyLine> TPolyLineVec;
	TPolyLineVec Intersection(const IPlane& plane, IProgressBase* pProgress = 0 ) const;
	
	//returns the edge of the surface (closed polyline)
	//##ModelId=3BC2A45C0093
	// the array is NOT sorted (suitable for drawing)
	virtual const CArray<CLine> Edge() const;
	// Not that XY-coordinate must be on the surface
	CValue InterpolateZ(const IPoint& point, bool bExtraPolate = false) const;
	// Return the nearest point in the xy plane
	const IPoint& NearestXYPoint(const IPoint& point) const;
	virtual CPolyLine SortedEdge() const;

	// sorted edge nodes, actual pointers to the IPoints in the surface
	virtual std::vector<std::vector<const IPoint *> > SortedEdgePoints(bool bRetry = false, double dEpsilon = 0.0001) const;

	// if a surface consists of several sub-surfaces (i.e. separate parts)
	// you can get a map saying which point lies in which part
	// first call SortedEdgePoints() and give the result of that to this function
	// the returned map contains the full set of points mapped to the index
	// of the outer vector returned by SortedEdgePoints()
  std::map<const IPoint *, int> SubSurfaceIndices(std::vector<std::vector<const IPoint *> > &edgepoints) const;

	typedef std::set<const IElement*> TElementSet;



	//get the elements that are on the side of the box (touch_point touch_edge touche_side or intersect)
	void FilterElementsInOrthoBox(	const geo::IPoint& ptMin,
									const geo::IPoint& ptMax,
									TElementSet& elements_on_the_side ,
									TElementSet& elements_strict_inside,
									TElementSet& elements_strict_outside) const;


	
	//returns a plane through the origin,
	//normal to the normal-vector of the surface
	//("parallel to the surface")
	//THE FOLLOWING FUNCTION IS COMMENTED OUT SINCE A GOOD IMPLEMENTATION IS NOT YET AVAILABLE.
/*	CPlane &ProjectionPlane() const;	*/

	//See if two surfaces share the same points
	//THE FOLLOWING FUNCTION IS COMMENTED OUT SINCE A GOOD IMPLEMENTATION IS NOT YET AVAILABLE.
/*	bool SharePoints(const ISurface &surface, CArray<CPoint> ShareEdges) const;	*/


	virtual void AssertValid() const;

	virtual bool Visit(IVisitor &visitor) { return visitor.HandleSurface(*this); }

};

}
#endif // !defined(AFX_ISURFACE_H__9B9CCA93_D91E_4ACE_AA56_B5E091385C7F__INCLUDED_)
