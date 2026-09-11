 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// TriangleEdge.cpp: implementation of the CTriangleEdge class.
//
//////////////////////////////////////////////////////////////////////
typedef double REAL;
#include "tri_triangle.h"
#include "TriangleSurface.h"
#include "TriangleEdge.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTriangleEdge::	CTriangleEdge(const CTriangleSurface& surface, int nIndex)
: m_surface(surface),
  m_nIndex(nIndex),
  m_prTriangle( (const geo::ITriangle*)0, (const geo::ITriangle*)0 )
{
}

CTriangleEdge::~CTriangleEdge()
{
}

size_t CTriangleEdge::TriangleSize() const
{
	if(m_prTriangle.first == 0)	BuildTrianglePair();
	if(m_prTriangle.second == 0)
		return 1;
	 return 2;
}

const geo::ITriangle& CTriangleEdge::Triangle(size_t nIndex) const
{
	if(m_prTriangle.first == 0)	BuildTrianglePair();
	assert((nIndex == 0) || (nIndex == 1));
	assert((nIndex == 0) || m_prTriangle.first);
	if(nIndex == 0)
		return *m_prTriangle.first;
	return *m_prTriangle.second;
}

const geo::IPoint &CTriangleEdge::First() const
{
	return m_surface.Point(FirstIndex());
}

void CTriangleEdge::First(const geo::IPoint& /*point*/)
{
	assert(false);
}

void CTriangleEdge::Second(const geo::IPoint& /*point*/)
{
	assert(false);
}

const geo::IPoint &CTriangleEdge::Second() const
{
	return m_surface.Point(SecondIndex());
}

int CTriangleEdge::SecondIndex() const
{
	return m_surface.m_mesh->edgelist[2*m_nIndex + 1];
}

int CTriangleEdge::FirstIndex() const
{
	return m_surface.m_mesh->edgelist[2*m_nIndex];
}

void CTriangleEdge::BuildTrianglePair() const
{
	assert(m_prTriangle.first == 0);
	assert(m_prTriangle.second == 0);

	// Get the element at this point
	typedef std::vector<int> TElementVec;
	TElementVec vcElement = m_surface.ElementsAt(First());

	for(size_t i = 0; i < vcElement.size(); i++)
	{
		const geo::ITriangle& element = m_surface.Triangle(vcElement[i]);
		assert(element.IndexingElementSet() == &m_surface);
		for(int p = 0; p < element.NrOfPoints(); p++)
		{
			if(element.PointIndex(p) == SecondIndex())
			{
				assert(m_prTriangle.second == 0);
				if(m_prTriangle.first == 0)
					m_prTriangle.first = &element;
				else
					m_prTriangle.second = &element;
			}
		}
	}

	assert(m_prTriangle.first);
}

int CTriangleEdge::SegmentIndex() const
{
	return m_surface.m_mesh->edgemarkerlist[m_nIndex] - 1;
}

CTriangleEdge::TSegment CTriangleEdge::Segment() const
{
	return TSegment(m_surface.m_input->segmentlist[SegmentIndex() * 2],
					m_surface.m_input->segmentlist[SegmentIndex() * 2 + 1]);
}
