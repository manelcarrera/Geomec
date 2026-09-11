 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// IVolume.h: interface for the IVolume class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IVOLUME_H__6ABA132D_9D86_47D7_A32F_3441D4FE16BB__INCLUDED_)
#define AFX_IVOLUME_H__6ABA132D_9D86_47D7_A32F_3441D4FE16BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IElementSet.h"
#include "VectorTempl.h"
#include "DispatchVisitorBase.h"

#include "GeometryExports.h"

class IProgressBase;

namespace geo {
class CLine;
class IPolyLine;
class CPolyLine;
class CBucketKernel;
class IFace;
class IPlane;
class IBody;
class CPolygon;
class ILine;
class CLine;

class GEOMETRY_EXPORT  IVolume : public IElementSet
{
	// Mapping van elementen op punten
	CArray<CPolyLine> ExtractPolyLine(CArray<CLine> &LineArr) const;
	void GenerateEdgeFaces() const;
	mutable std::vector<const IFace*> m_vcEdgeFace;
public:
	IVolume();
	virtual ~IVolume();

// Cool !!! Interface dual to the ISurface interface ...
	virtual const IBody &Body(int nIndex) const = 0;
	virtual int BodySize() const = 0;

	virtual const IPoint &Point(int nIndex) const = 0;
	virtual int PointSize() const = 0;

	virtual const IElement &Element(int nIndex) const;
	virtual int ElementSize() const;

	const IFace &EdgeFace(int nIndex) const;
	int EdgeFaceSize() const;
	
	virtual bool Contains(const IPoint &point, bool bIncludeEdge = true) const;
	
	// Pre	: The volume consists of convex bodies
	virtual std::vector<CPolygon> Intersection(const IPlane &Plane, IProgressBase* pProgress = 0) const;
	virtual void Intersection(const ILine &Line, std::set<CPoint> &PointSet) const;
	virtual void Intersection(const IPolyLine &Line, std::set<CPoint> &PointSet) const;
	virtual void Intersection(const IPlane &Plane, CArray<CPolygon> &PolArr, std::set<CPoint> *PointSet = 0, IProgressBase* pProgress = 0) const;
	virtual void IntersectionWithEdgeFaces(const ILine &Line, std::set<CPoint> &PointSet) const;
	virtual std::vector<const IBody*> PointInVolume(const IPoint &point) const;

	virtual bool Visit(IVisitor &visitor) { return visitor.HandleVolume(*this); }

protected:
	virtual void InvalidateCache();
};

}

#endif // !defined(AFX_IVOLUME_H__6ABA132D_9D86_47D7_A32F_3441D4FE16BB__INCLUDED_)
