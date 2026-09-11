 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// ipolygon.h: interface for the IPolygon class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPOLYGON_H__99ADAD58_9CC6_4205_8A7F_78C683211932__INCLUDED_)
#define AFX_IPOLYGON_H__99ADAD58_9CC6_4205_8A7F_78C683211932__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IFace.h"
#include "Vector.h"
#include "DispatchVisitorBase.h"

#include "GeometryExports.h"

namespace geo { 

class IPolygon;
class CPolygonPtrArray : public CPtrArray<IPolygon>
{
public:
	virtual bool Visit(IVisitor &visitor)
	{
		assert(dynamic_cast<CVisitorBase *>(&visitor));
		return static_cast<CVisitorBase &>(visitor).HandlePolygonPtrArray(*this);
	}
};


class GEOMETRY_EXPORT  IPolygon : public IFace
{
	bool m_bIsHole;
	mutable CVector m_Normal;
	mutable bool m_bNormalCalculated;
protected:
	virtual void InvalidateCache();
public:
	IPolygon(bool bIsHole);
	virtual ~IPolygon();

	bool IsHole() const;
	void IsHole(bool bIsHole);
	//TODO not implemented yet (htg)
	virtual CVector Normal() const;

	//See if point is on the boundaray
	bool PointOnBoundary(const IPoint& p);

	virtual bool Visit(IVisitor &visitor) { return visitor.HandlePolygon(*this); }

	//the clip options for polyclipping
	enum ClipOperation {
		UNITE,
		ISECT,
		SUB,
		XOR
	};
	
	//the poly1 and poly2 should be in the sane plane !!!!
	static CPolygonPtrArray PolyClip(ClipOperation ClipType,CPolygonPtrArray& Poly1,CPolygonPtrArray& Poly2);

};

}

#endif // !defined(AFX_IPOLYGON_H__99ADAD58_9CC6_4205_8A7F_78C683211932__INCLUDED_)
