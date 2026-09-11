 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// ringfactory.cpp: implementation of the CRingFactory class.
//
//////////////////////////////////////////////////////////////////////
#include "dimple.h"
#include <stack>
#include "DirectedSegment.h"
#include "RingPolygon.h"

namespace geo {

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
typedef std::set<CDirectedSegment> TSegmentSet;
typedef std::stack<CDirectedSegment> TSegmentStack;
CRingFactory::CRingFactory()
{
	m_pSegment = new TSegmentSet();
}

CRingFactory::~CRingFactory()
{
	delete (TSegmentSet *)m_pSegment;
}

bool CRingFactory::Valid(const CDirectedSegment& edge) const
{
	TSegmentSet* pSegmentSet = (TSegmentSet*)(m_pSegment);
	return pSegmentSet->find(edge) != pSegmentSet->end();
}

/*!
	A point is added and the index is returned.
*/
int CRingFactory::AddPoint(const IPoint& point)
{
	int nIndex = (int)m_vcPoint.size();

	TSegmentPoint segment_point(&point, TSegmentIndexVec());
	m_vcPoint.push_back(segment_point);

	return nIndex;
}

/*!
	A segment is added by two points indices. The function returns false when the segment
	is already defined.
*/
bool CRingFactory::AddSegment(int nFirstPoint, int nSecondPoint)
{
	assert((nFirstPoint >= 0) && (nSecondPoint >= 0));
	assert((nFirstPoint < PointSize()) && (nSecondPoint < PointSize()));

	// Check uniqueness of segments
	TSegmentSet* pSegmentSet = (TSegmentSet*)(m_pSegment);
	if(!pSegmentSet->insert(CDirectedSegment(*this, nFirstPoint, nSecondPoint)).second)
		return false;
	VERIFY(pSegmentSet->insert(CDirectedSegment(*this, nSecondPoint, nFirstPoint)).second);

	int nSegmentIndex = (int)m_vcSegment.size();	
	m_vcSegment.push_back(TSegment(nFirstPoint, nSecondPoint));
	assert(pSegmentSet->size() == (2 * m_vcSegment.size()));
	m_vcPoint[nFirstPoint].second.push_back(nSegmentIndex);
	m_vcPoint[nSecondPoint].second.push_back(nSegmentIndex);
	
	return true;
}

int CRingFactory::PointSize() const
{
	return (int)m_vcPoint.size();
}

const IPoint& CRingFactory::Point(int nIndex) const
{
	assert(nIndex >= 0);
	assert(nIndex < PointSize());
	const TSegmentPoint& segment_point = m_vcPoint[nIndex];
	return *segment_point.first;
}

size_t CRingFactory::SegmentSize() const
{
  	return m_vcSegment.size();
}

CRingFactory::TSegment CRingFactory::Segment(size_t nIndex) const
{
	assert(nIndex < SegmentSize());
	return m_vcSegment[nIndex];
}

bool CRingFactory::CreateRings()
{
	// Do we have segments?
	TSegmentSet* pSegmentSet = (TSegmentSet*)(m_pSegment);
	if(pSegmentSet->size() == 0)
		return false;

	assert(pSegmentSet->size() == (2 * m_vcSegment.size()));

	// Flush all existing ring and data
	m_prRing.first.clear();
	m_prRing.second.clear();
	m_vcRing.clear();
	m_vcBoundary.clear();

	while(pSegmentSet->size())
	{
		// Take the first segment and start
		Walk(CDirectedSegment(*pSegmentSet->begin()));
		// NB. We copy the segment because the segment in the set
		// is erased from the set in the walk function
	}

	// Restore segmentset
	for(size_t i = 0; i < SegmentSize(); i++)
	{
		TSegment segment = Segment(i);
		VERIFY(pSegmentSet->insert(CDirectedSegment(*this, segment.first, segment.second)).second)
		VERIFY(pSegmentSet->insert(CDirectedSegment(*this, segment.second, segment.first)).second);
	}

	return m_vcRing.size() > 0;
}

/*!
	Return true when the segment is on a ring or boundary and returns false when not so
*/
bool CRingFactory::IsRingBoundarySegment(const CDirectedSegment& segment) const
{
	int i;
	for(i = 0; i < m_vcRing.size(); i++)
	{
		if(Contains(m_vcRing[i], segment))
			return true;
	}

	for(i = 0; i < m_vcBoundary.size(); i++)
	{
		if(Contains(m_vcBoundary[i], segment))
			return true;
	}

	return false;
}

/*!
	Return true when the segment is in the given ring and returns false when not so
*/
bool CRingFactory::Contains(const TRingPair &ring_pair, const CDirectedSegment& segment) const
{
	if( (ring_pair.second.find(segment.First()) != ring_pair.second.end() ) &&
		(ring_pair.second.find(segment.Second()) != ring_pair.second.end() ) )
	{
		// When the two vertices are in the ring, we check ring for the segment
		const TRing& ring = ring_pair.first;
		for(int j = 0; j < ring.size(); j++)
		{
			int nFirst = ring[j];
			int nSecond = ring[(j + 1) % ring.size()];
			if((nFirst == segment.First()) && (nSecond == segment.Second()))
				return true;
		}
	}

	return false;
}

bool CRingFactory::Boundary(const TRing& ring) const
{
	// We are on a boundary when all the crossing segments are inside the polygon of the boundary
	assert(ring.size() > 2);
	// Take the first directed segment of the ring
	CRingPolygon polygon(*this, ring);
	for(int i = 0; i < (ring.size() - 1); i++)
	{
		int nFirst = ring[i];
		int nSecond = ring[(i + 1) % ring.size()];
		CDirectedSegment current(*this, nFirst, nSecond);
		std::vector<CDirectedSegment> vcSegment = current.NextSegments();
		// Evaluate the next segments
		for(int s = 0; s < vcSegment.size(); s++)
		{
			assert(vcSegment[s].First() == nSecond);
			if(vcSegment[s].Second() != ring[i + 1])
			{
				// Evalate the crossing segment
				if( Valid( vcSegment[s] ) || IsRingBoundarySegment( vcSegment[s] ) )
				{
					if(!polygon.Contains(Point(vcSegment[s].Second()), true))
						return false;
				}
			}
		}
	}

	return true;
}

bool CRingFactory::Walk(const CDirectedSegment& start)
{
	// Is the segment valid?
	if(!Valid(start))
		return false;

	// We are now walking over a valid segment so remove it from the set
	TSegmentSet* pSegmentSet = (TSegmentSet*)(m_pSegment);
	VERIFY(pSegmentSet->erase(start) == 1);

	// Add the first point to the ring
	m_prRing.first.push_back(start.First());
	VERIFY(m_prRing.second.insert(start.First()).second);
	
	// Is the current segment completing the ring?
	if(m_prRing.second.find(start.Second()) != m_prRing.second.end())
	{
		assert(m_prRing.first.size() == m_prRing.second.size());
		// Finalize the compartment
		TRingPair prRing;
        int       i;
		for(i = 0; i < m_prRing.first.size(); i++)
		{
			if((start.Second() == m_prRing.first[i]) || (prRing.first.size() > 0))
			{
				prRing.first.push_back(m_prRing.first[i]);
				VERIFY(prRing.second.insert(m_prRing.first[i]).second);
			} else {
				// Place back segments
				if( i > 0 ) {
					pSegmentSet->insert( CDirectedSegment( *this, m_prRing.first[i-1], m_prRing.first[i] ) );
				}
			}
		}

		m_prRing.first.clear();
		m_prRing.second.clear();

		assert(prRing.first.size() == prRing.second.size());

		// Is the ring already a boundary?
		for(i = 0; i < m_vcBoundary.size(); i++)
		{
			if(m_vcBoundary[i].second == prRing.second)
			{
				m_vcRing.push_back(prRing);
				return true;
			}
		}

		// Is the ring the boundary?
		if(Boundary(prRing.first))
			m_vcBoundary.push_back(prRing);
		else
			m_vcRing.push_back(prRing);

		return true;
	}

	// What are the options from here
	std::vector<CDirectedSegment> vcSegment = start.NextSegments();
	for(int i = 0; i < vcSegment.size(); i++)
	{
		if(Walk(vcSegment[i]))
			return true;
	}
	
	// Unroll adding to ring
	if(m_prRing.first.size() > 0)
	{
		assert(m_prRing.first[m_prRing.first.size() - 1] == start.First());
		m_prRing.first.resize(m_prRing.first.size() - 1);
		VERIFY(m_prRing.second.erase(start.First()) == 1);
	}

	assert(m_prRing.first.size() == m_prRing.second.size());

	return false;
}

int CRingFactory::RingSize() const
{
	return (int)m_vcRing.size();
}

const CRingFactory::TRing& CRingFactory::Ring(int nIndex) const
{
	return m_vcRing[nIndex].first;
}

int CRingFactory::BoundarySize() const
{
	return (int)m_vcBoundary.size();
}

const CRingFactory::TRing& CRingFactory::Boundary(int nIndex) const
{
	return m_vcBoundary[nIndex].first;
}

void CRingFactory::Clear()
{
	// Flush all existing ring and data
	TSegmentSet* pSegmentSet = (TSegmentSet*)(m_pSegment);
	pSegmentSet->clear();

	m_prRing.first.clear();
	m_prRing.second.clear();
	m_vcRing.clear();
	m_vcBoundary.clear();
	m_vcPoint.clear();
	m_vcSegment.clear();
}

}
