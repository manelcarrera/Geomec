 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// FaceGroup.cpp: implementation of the CFaceGroup class.
//
//////////////////////////////////////////////////////////////////////
#include "dimple.h"
#include "IMesh.h"
#include "IFace.h"
#include "FaceGroup.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace geo {

/////////////////////////////////////////////////////////////////////
// Implementation for CBodyGroup::CSideSurface
//////////////////////////////////////////////////////////////////////
CFaceGroup::CFaceGroup(IMesh& mesh)
: m_mesh(mesh)
{
	m_mesh.AddElementGroup(*this);
}

CFaceGroup::~CFaceGroup()
{
	m_mesh.RemoveElementGroup(*this);
}

// Construction
bool CFaceGroup::AddFace(const IFace& face)
{
	// Add points
	int nFaceIndex = (int)m_vcFace.size();
	for(int i = 0; i < face.NrOfPoints(); i++)
	{
		// Add to map
		int nIndex = (int)m_vcPoint.size();
		std::pair<TPointMap::iterator, bool> ret = m_mpPoint.insert(TPointMap::value_type(&face.Point(i), nIndex));
		if(ret.second)
		{
			TFaceIndexVec vcFaceIndex;
			vcFaceIndex.push_back(nFaceIndex);
			m_vcPoint.push_back(TSurfacePoint(&face.Point(i), vcFaceIndex));
		}
		else
		{
			int nNodeIndex = ret.first->second;
			TFaceIndexVec& vcFaceIndex = m_vcPoint[nNodeIndex].second;
			vcFaceIndex.push_back(nFaceIndex);
		}
	}

	m_vcFace.push_back(&face);

	InvalidateCache();

	return true;
}

const IMesh& CFaceGroup::Mesh() const
{
	return m_mesh;
}

IMesh& CFaceGroup::Mesh()
{
	return m_mesh;
}

IPoint &CFaceGroup::PointAt(int /*nIndex*/)
{
	assert(false);
	IPoint *pBogus = 0;
	return *pBogus;
}

const IPoint& CFaceGroup::Point(int nIndex) const
{
	assert(nIndex >= 0);
	assert(nIndex < PointSize());
	return *m_vcPoint[nIndex].first;
}

int CFaceGroup::PointSize() const
{
	 return (int)m_vcPoint.size();
}

std::vector<int> CFaceGroup::Nodes(const IElement &/*element*/) const
{
	assert(false);
	return std::vector<int>();
}

const CPtrArray <IFace> CFaceGroup::FacesAt(const IPoint &/*p*/) const
{
	assert(false);
	return CPtrArray <IFace>();
}

const CPtrArray <IFace> CFaceGroup::FacesAtNode(const IPoint &p) const
{
	TPointMap::const_iterator it = m_mpPoint.find(&p);
	assert(it != m_mpPoint.end());	// Point is not member of surface
	const TFaceIndexVec& vcFaceIndex = m_vcPoint[it->second].second;
	CPtrArray <IFace> vcRet;
	for(int i = 0; i < vcFaceIndex.size(); i++)
		vcRet.PushBack((IFace&)*m_vcFace[i]);
	return vcRet;
}


const IFace &CFaceGroup::Face(int nIndex) const
{ 
	assert(nIndex >= 0);
	assert(nIndex < m_vcFace.size());
	return *m_vcFace[nIndex];
}

int CFaceGroup::FaceSize() const
{
	return (int)m_vcFace.size();
}

}

