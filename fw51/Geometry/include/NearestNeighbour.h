 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// NearestNeighbour.h: interface for the CNearestNeighbour class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NEARESTNEIGHBOUR_H__8BC07977_A850_4924_8BF3_83ADB3D0AD51__INCLUDED_)
#define AFX_NEARESTNEIGHBOUR_H__8BC07977_A850_4924_8BF3_83ADB3D0AD51__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "VectorTempl.h"
#include <map>

#include "GeometryExports.h"

namespace geo {
class CBucketKernel;
class GEOMETRY_EXPORT  CNearestNeighbour : public CPtrArray<IPoint>
{
public:
	typedef std::vector<int> CBucket;					// Bucket
	typedef std::vector<const IPoint*> CPointVec;		// PointVec
private:
	mutable CBucketKernel *m_pKernel;
	double m_dPointDensity;
	void MapPointOnBucket(int nPointIndex) const;

	// Internal bucket management
	bool BucketsCreated() const;
	void CreateBuckets() const;
	void DestroyBuckets() const;
	typedef std::multimap<double, int> TDistanceMap;
public:
	// Construction
	CNearestNeighbour(const double &dPointDensity, int nSize = 0);
	CNearestNeighbour(const CNearestNeighbour &rhs);
	virtual ~CNearestNeighbour();

  virtual bool PrepareMapping();

	// Assignment
	using CPtrArray<IPoint>::Object;
	virtual void PushBack(IPoint &pt);
	virtual void Object(int nIndex, IPoint& pt);
	virtual void Clear();
	void Invalidate();

	// Statics
	int Dimension() const;
	const double& PointDensity() const;
	int BucketCount() const;

	// Nearest neighbour in 3D space
	CBucket NearestNeighbourIndex(const IPoint& pt, int count = 0) const;
	CPointVec NearestNeighbour(const IPoint &pt, int count = 0) const;

	// Nearest neighbour in XY-plane space
	CBucket NearestNeighbourXYIndex(const IPoint& pt, int count = 0) const;
	CPointVec NearestNeighbourXY(const IPoint &pt, int count = 0) const;

};

}

#endif // !defined(AFX_NEARESTNEIGHBOUR_H__8BC07977_A850_4924_8BF3_83ADB3D0AD51__INCLUDED_)
