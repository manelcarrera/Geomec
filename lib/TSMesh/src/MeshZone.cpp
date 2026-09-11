#include <assert.h>

#include "SurfaceDetail.h"
#include "MeshZone.h"

CMeshZone::CMeshZone():m_pMateRegion(0), m_dEdgeSquared(0)
{
	assert(m_vcElementPoints.size() == 0);
}
	
CMeshZone::CMeshZone(const geo::CPtrArray<geo::IPoint> &points):m_pMateRegion(0), m_dEdgeSquared(0)
{
	m_vcElementPoints.resize(points.Size());
	geo::CPoint point;

	for(size_t i = 0; i < points.Size(); i++)
	{
		point = points.Object(i); 
		m_vcElementPoints[i] = point;
	}
}

CMeshZone::CMeshZone(const geo::IElement &element):m_pMateRegion(0), m_dEdgeSquared(0)
{
	assert(!element.Empty());
	m_vcElementPoints.resize(element.NrOfPoints());

	int i;
	geo::CPoint point;

	for(i = 0; i < element.NrOfPoints(); i++)
	{
		point = element.Point(i); 
		m_vcElementPoints[i] = point;
	}
}
	
CMeshZone::CMeshZone(const CMeshZone &meshzone):m_pMateRegion(0)
{
	m_vcElementPoints.resize(meshzone.m_vcElementPoints.size());

	for(size_t i = 0; i < meshzone.m_vcElementPoints.size(); i++)
	{
		m_vcElementPoints[i] = meshzone.m_vcElementPoints[i];
	}

	m_dEdgeSquared = meshzone.m_dEdgeSquared;
}
	
CMeshZone::~CMeshZone()
{
	m_vcElementPoints.clear();
}

const geo::IPoint &CMeshZone::Point(const int nIndex) const
{
	return m_vcElementPoints[nIndex];
}
	
void CMeshZone::Point(const int nIndex, const geo::IPoint &pt)
{
	geo::CPoint point(pt.X(), pt.Y(), pt.Z());
	m_vcElementPoints[nIndex] = point;
}
	
int CMeshZone::NrOfPoints() const
{
	return m_vcElementPoints.size();
}

/*
bool CMeshZone::operator < (const CMeshZone &rhs) const
{
	std::set<geo::CPoint> set1;
	std::set<geo::CPoint> set2;

	if(NrOfPoints() < rhs.NrOfPoints())
		return true;

	if(NrOfPoints() > rhs.NrOfPoints())
		return false;

	for(int i = 0; i < NrOfPoints(); i++)
	{
		set1.insert(Point(i));
		set2.insert(rhs.Point(i));
	}

	return std::lexicographical_compare(set1.begin(), set1.end(), set2.begin(), set2.end());
}
*/

geo::CValue CMeshZone::InterpolateValue(const geo::IPoint &/*point*/, const std::vector<geo::CValue> &values) const
{
	if(ContainsInvalidValue(values)) return geo::CValue();

	assert(false);
	return geo::CValue();
}
