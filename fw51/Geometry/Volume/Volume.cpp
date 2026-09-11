 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// Volume.cpp: implementation of the CVolume class.
//
//////////////////////////////////////////////////////////////////////

#include "dimple.h"
#include "IBody.h"
#include "Volume.h"

#include <tbb/spin_rw_mutex.h>

namespace {
tbb::spin_rw_mutex myGlobalVolumeMutex[16];
}

namespace geo
{

//////////////////////////////////////////////////////////////////////
// This is the implemention of an empty object to mark the end of the 
// iteration loop
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

	
CVolume::CVolume(): m_NearestNeighbour(1)
{
}

CVolume::CVolume(const IVolume &/*rhs*/): m_NearestNeighbour(1)
{
}

CVolume::~CVolume()
{
	Clear();
}

void CVolume::PushBack(const IBody &body)
{
	m_vcBody.push_back(&body);
	InvalidateCache();
}

void CVolume::Clear()
{
	// Invalidate first
	InvalidateCache();
	m_vcBody.clear();
}

//=============================================================================
const IBody &CVolume::Body(int nIndex) const
{
	assert(nIndex > -1);
	assert(nIndex < m_vcBody.size());
	return *m_vcBody[nIndex];
}

int CVolume::BodySize() const
{
	return (int)m_vcBody.size();
}

const IPoint &CVolume::Point(int nIndex) const
{
	if(m_mpPointMap.size() == 0) UpdatePointMap();
	return (const IPoint&) (m_NearestNeighbour.ObjectBase(nIndex));
}

IPoint& CVolume::PointAt(int nIndex)
{
	if(m_mpPointMap.size() == 0) UpdatePointMap();
	return (IPoint&) (m_NearestNeighbour.ObjectBase(nIndex));
}

int CVolume::PointSize() const
{
	if(m_mpPointMap.size() == 0) UpdatePointMap();
	return (int)m_mpPointMap.size();
}

void CVolume::AssertValid() const
{
	// TODO : HTG Implement CVolume verification code
}

// Create a map which hold the connectivity between the points and the elements.
void CVolume::UpdatePointMap() const
{
  int index = (intptr_t)this >> 6 & 0xf;

  tbb::spin_rw_mutex::scoped_lock lock(myGlobalVolumeMutex[index]);

  if (m_mpPointMap.size() != 0)
    return; // some other tread beat us to it

	assert(m_mpPointMap.size() == 0);
	assert(m_NearestNeighbour.Size() == 0);

	for(int nBody = 0; nBody < BodySize(); nBody++)
	{
		for (int j = 0; j < Body(nBody).NrOfPoints(); j++)
		{
			const IPoint *pPoint = &Body(nBody).Point(j);
			TPointMap::iterator it = m_mpPointMap.find(pPoint);

			if (it != m_mpPointMap.end())
			{
				// Point allready in the list so now only add the element.
				it->second.push_back(&Body(nBody));
			}
			else
			{
				// Point is not defined yet so add it.
				it = m_mpPointMap.insert(TPointMap::value_type(pPoint,TBodyVec())).first;
				it->second.push_back(&Body(nBody));
			}
		}
	}
	// Put all the points in the NearestNeighbour for fast searching.
	for(TPointMap::const_iterator it = m_mpPointMap.begin(); it != m_mpPointMap.end(); it++)
		m_NearestNeighbour.PushBack((IPoint&)(*it->first));

	assert(m_mpPointMap.size() == m_NearestNeighbour.Size());
}

void CVolume::InvalidateCache()
{
	m_NearestNeighbour.Clear();
	m_mpPointMap.clear();
	IVolume::InvalidateCache();
}

std::vector<int> CVolume::Nodes(const IElement &/*element*/) const
{
	std::vector<int> ret;

	assert(false);
	return ret;
}

const std::vector<const IBody*>& CVolume::PointToBody(const IPoint &point) const
{
	if (m_mpPointMap.size() == 0) UpdatePointMap(); 

	TPointMap::const_iterator it = m_mpPointMap.find(&point);
	assert(it != m_mpPointMap.end());

	return it->second;
}

}	// namespace
