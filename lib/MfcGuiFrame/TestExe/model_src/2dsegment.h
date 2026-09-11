// 2DSegment.h: interface for the C2DSegment class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_2DSEGMENT_H__A7E7D094_7A8E_42C4_B614_38F653D7E7A5__INCLUDED_)
#define AFX_2DSEGMENT_H__A7E7D094_7A8E_42C4_B614_38F653D7E7A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ModelContainer.h"
#include "ringfactory.h"

class C2DPolyLine;
class C2DVertex;

class C2DSegment : public IModelObject  
{
	class CSegment : public geo::ISegment
	{
		const C2DSegment &m_segment;
	public:
		CSegment(const C2DSegment &segment);
		virtual ~CSegment();

		CSegment &operator=(const CSegment &rhs);

		const C2DSegment& Segment() const;

		// ILine interface
		virtual const geo::ISegmentPoint& FirstSegmentPoint() const;
		virtual	const geo::ISegmentPoint& SecondSegmentPoint() const;
		virtual void First(const geo::IPoint& point);
		virtual void Second(const geo::IPoint& point);
		virtual int Order() const { return 1; }
	};

	std::pair<C2DVertex *, C2DVertex *> m_prVertices;
	CSegment m_segment;
	std::vector<C2DPolyLine *> m_vc2DPolyLines;
	std::vector<int> m_vcEdge;
	void InsertSegment(C2DPolyLine& poly_line, C2DSegment& old_segment, C2DSegment& new_segment);
public:
	C2DSegment(IModelObject& parent);
	C2DSegment(IModelObject& parent, C2DVertex &first, C2DVertex &second);
	virtual ~C2DSegment();

	const geo::ISegment &Segment() const;

	const C2DVertex &FirstVertex() const;
	C2DVertex &FirstVertex();
	const C2DVertex &SecondVertex() const;
	C2DVertex &SecondVertex();
	FirstVertex(C2DVertex& vertex);
	SecondVertex(C2DVertex& vertex);

	const C2DVertex &OtherVertex(const C2DVertex &vertex) const;
	C2DVertex &OtherVertex(const C2DVertex &vertex);
/*
	virtual UINT IconId() const;
	virtual UINT TypeId() const;

	virtual void OnNewNeighbour(const CGraphNode &node);
	virtual void OnNeighbourModified(const CGraphNode& node, UINT uHint);
	virtual void OnNeighbourDeleted(const CGraphNode &node);
*/
	int PolyLineSize() const;
	const C2DPolyLine& PolyLine(int nIndex) const;
	C2DPolyLine& PolyLine(int nIndex);

	// Save and load
/*	virtual BOOL Empty() const;
	virtual long SavedItems() const;
	virtual void LoadStream(TSTREAM& stream, CVersion& version, TPROGRESS& progress);
	virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);
*/
	bool CanSplit(const geo::IPoint& point) const;
	void Split(const geo::IPoint& point);
};

class C2DDocument;
class C2DSegmentContainer : public CModelContainer<C2DSegment>
{
public:
	C2DSegmentContainer(C2DDocument& document);
	typedef std::pair<const C2DSegment*, geo::CPoint> TIntersection;
	std::vector<TIntersection> Intersection(geo::ILine& line) const;
	
};

#endif // !defined(AFX_2DSEGMENT_H__A7E7D094_7A8E_42C4_B614_38F653D7E7A5__INCLUDED_)
