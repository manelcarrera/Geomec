// PolyLine.h: interface for the CPolyLine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POLYLINE_H__FBA98FCE_E342_416F_B9FA_746A24E6B587__INCLUDED_)
#define AFX_POLYLINE_H__FBA98FCE_E342_416F_B9FA_746A24E6B587__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IPolyLine.h"
#include "ILine.h"

#include "GeometryExports.h"

namespace geo {

// Copyright (c) 2011 TNO DIANA BV                              Confidential
// Description : Implementation of a 3D polyline
// Status      : Finished (80%)
// Remarks     : 
// Last review : 27-03-2002 
class GEOMETRY_EXPORT  CPolyLine : public IPolyLine
{
	class GEOMETRY_EXPORT CSegment : public ILine
	{
		typedef std::pair<IPoint*, IPoint*> TLine;
		TLine m_line;
		std::vector<int> m_NodeIndices;
	public:
		CSegment(IPoint& first, IPoint& second, int firstidx, int secondidx);
		virtual const IPoint& Point(int nIndex) const;
		virtual void Point(int /*nIndex*/, const IPoint& /*point*/) {assert(false);}
		virtual std::vector<int> Nodes() const;
		virtual size_t Order() const;
	};

	typedef std::vector<CSegment*> TPolyLine;
	typedef std::vector<IPoint*> TPointVec;
	TPointVec m_vcPoint;
	mutable TPolyLine m_vcSegment;
	void UpdateSegments() const;
protected:
	virtual IPoint &PointAt(int nIndex);
	virtual void InvalidateCache();
public:
	CPolyLine();
	CPolyLine(const CArray<CPoint> &points);
	CPolyLine(const CPtrArray<CPoint> &points);
	CPolyLine(const IPolyLine &polyline);// Copy construction
	CPolyLine(const CPolyLine &polyline);
	CPolyLine(std::set<CPoint> &Polypts);

	virtual ~CPolyLine();

	CPolyLine& operator=(const CPolyLine& rhs);

	const IPoint &FirstPoint() const;
	const IPoint &LastPoint() const;

		
	//IPolyLine Interface
	virtual int PointSize() const;
	virtual const IPoint &Point(int nIndex) const;

	virtual int LineSize() const;
	virtual const ILine& Line(int Index) const;

	// Create the polyline from a given set of lines
	// It will build this polyline from the first set of linked
	// lines in the given set
	// These lines are then removed from the set (so it's not const)
	// This function can be called several times given a set of
	// lines, to retrieve all separate polylines, until the set is empty
	// The polyline (this) must be empty!
	void CreateFromLines(std::set<const ILine*>& stLines);

	//adding/removing points
	virtual bool PushBack(const IPoint &point); // returns true when the point is succesfully added to the line, 
												// false if the point was allready present.
	bool PushFront(const IPoint &point); // returns true when the point is succesfully added to the line,
										 // false if the point was allready present.
	virtual bool Remove(const IPoint &point);
	virtual void Remove(int nPointIndex);
	virtual void RemoveAll();

	// IElementSet interface
	virtual std::vector<int> Nodes(const IElement &element) const;

	//IObject Interface
	virtual void AssertValid() const;
	virtual bool Empty() const;

	// Returns the index of the point, -1 if point does not exist in polyline
	int Exist(const IPoint &point) const;

	virtual void Swap(int nIndex1, int nIndex2);
};

}

#endif // !defined(AFX_POLYLINE_H__FBA98FCE_E342_416F_B9FA_746A24E6B587__INCLUDED_)
