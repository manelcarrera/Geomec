/*
 */
 /* Copyright (c) 2012 TNO DIANA BV                              Confidential */
// BodyGroup.cpp: implementation of the CBodyGroup class.
//
//////////////////////////////////////////////////////////////////////
#include "dimple.h"
#include "IFace.h"
#include "Mesh.h"
#include "IBody.h"
#include "BodyGroup.h"

#include <algorithm>

namespace geo {
//////////////////////////////////////////////////////////////////////
// Implementation for CBodyGroup::CSideSurface
//////////////////////////////////////////////////////////////////////
CBodyGroup::CSideSurface::CSideSurface(CBodyGroup& body_group)
: m_body_group(body_group)
{
	body_group.m_vcSurface.push_back(this);
}

CBodyGroup::CSideSurface::~CSideSurface()
{
	for(int i = 0; i < m_body_group.m_vcSurface.size(); i++)
	{
		if(m_body_group.m_vcSurface[i] == this)
		{
			m_body_group.m_vcSurface.erase(m_body_group.m_vcSurface.begin() + i);
			break;
		}
	}
}

// Construction
bool CBodyGroup::CSideSurface::AddFace(const IFace& face)
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
      TFaceIndexVec vcFaceIndex; vcFaceIndex.reserve(1);
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

void CBodyGroup::CSideSurface::reserveFaceSize(size_t size)
{
  m_vcFace.reserve(size);
  m_vcPoint.reserve(size);
}

IPoint &CBodyGroup::CSideSurface::PointAt(int /*nIndex*/)
{
	assert(false);
	IPoint *pBogus = 0;
	return *pBogus;
}

const CBodyGroup& CBodyGroup::CSideSurface::BodyGroup() const
{
	 return m_body_group;
}

const IPoint& CBodyGroup::CSideSurface::Point(int nIndex) const
{
	assert(nIndex >= 0);
	assert(nIndex < PointSize());
	return *m_vcPoint[nIndex].first;
}

int CBodyGroup::CSideSurface::PointSize() const
{
	 return (int)m_vcPoint.size();
}

std::vector<int> CBodyGroup::CSideSurface::Nodes(const IElement &/*element*/) const
{
	assert(false);
	return std::vector<int>();
}

const CPtrArray <IFace> CBodyGroup::CSideSurface::FacesAt(const IPoint &/*p*/) const
{
	assert(false);
	return CPtrArray <IFace>();
}

const CPtrArray <IFace> CBodyGroup::CSideSurface::FacesAtNode(const IPoint &p) const
{
	TPointMap::const_iterator it = m_mpPoint.find(&p);
	assert(it != m_mpPoint.end());	// Point is not member of surface
	const TFaceIndexVec& vcFaceIndex = m_vcPoint[it->second].second;
	CPtrArray <IFace> vcRet;
	for(int i = 0; i < vcFaceIndex.size(); i++)
		vcRet.PushBack((IFace&)*m_vcFace[vcFaceIndex[i]]);
	return vcRet;
}


const IFace &CBodyGroup::CSideSurface::Face(int nIndex) const
{ 
	assert(nIndex >= 0);
	assert(nIndex < m_vcFace.size());
	return *m_vcFace[nIndex];
}

int CBodyGroup::CSideSurface::FaceSize() const
{
	return (int)m_vcFace.size();
}

void CBodyGroup::CSideSurface::ReplaceNode( const INode& oldNode,
                                            const INode& newNode)
{
  TPointMap::iterator it = m_mpPoint.find( &oldNode );
  if ( it != m_mpPoint.end() ) {
    // the map uses ICoordinate::CCoordinateLess so make sure we're
    // actually using the oldNode's pointer value...
    if ( it->first == &oldNode) {
      // get the index of the node in the m_vcPoint vector
      int nPointIndex = it->second;

      // get the entry in the m_vcPoint vector
      std::pair<const IPoint*, TFaceIndexVec>& prPoint = m_vcPoint[nPointIndex];

      // check that this is actually the old node's entry
      assert( prPoint.first == &oldNode );

      m_mpPoint.erase(it);
      m_mpPoint.insert(TPointMap::value_type( &newNode, nPointIndex) );
      prPoint.first = &newNode;
    }
  }
}

//////////////////////////////////////////////////////////////////////
// Implementation for CBodyGroup
//////////////////////////////////////////////////////////////////////

CBodyGroup::CBodyGroup(IMesh& mesh)
: m_mesh(mesh)
{
	mesh.AddElementGroup(*this);
}

CBodyGroup::~CBodyGroup()
{
	m_mesh.RemoveElementGroup(*this);
	Clear();
}

bool CBodyGroup::AddBody(const IBody& body)
{
	assert(body.IndexingElementSet() == &m_mesh);
	m_vcElement.push_back(body.Index());
	for(int i = 0; i < body.NrOfPoints(); i++)
	{
		std::pair<TPointMap::iterator, bool> ret = m_mpPoint.insert(TPointMap::value_type(&body.Point(i), TBodyVec()));

		if(ret.second)
			m_vcNode.push_back(body.PointIndex(i));

		ret.first->second.push_back(&body);
	}

	InvalidateCache();

	return true;
}

void CBodyGroup::reserveBodySize(size_t size)
{
  m_vcElement.reserve(size);
}

IPoint &CBodyGroup::PointAt(int /*nIndex*/)
{
	assert(false);	// Cannot alter groups
	IPoint *pBogus = 0;
	return *pBogus;
}

const IMesh& CBodyGroup::Mesh() const
{
	return m_mesh;
}

IMesh& CBodyGroup::Mesh()
{ 
	return m_mesh;
}

const IBody &CBodyGroup::Body(int nIndex) const
{
	assert(nIndex >= 0 && nIndex < m_vcElement.size());
	const IBody* pBody = dynamic_cast<const IBody *> (&m_mesh.Element(m_vcElement[nIndex]));
	assert(pBody);
	return *pBody;
}

int CBodyGroup::BodySize() const
{
	return (int)m_vcElement.size();
}

const IPoint &CBodyGroup::Point(int nIndex) const
{
	return m_mesh.Point(m_vcNode[nIndex]);
}

int CBodyGroup::PointSize() const
{
	return (int)m_vcNode.size();
}

void CBodyGroup::AssertValid() const
{
}


	// Point is member of the mesh
	// Return value : Bodies adjacent to the point
const std::vector<const IBody*>& CBodyGroup::PointToBody(const IPoint &point) const
{
	TPointMap::const_iterator it = m_mpPoint.find(&point);
	assert(it != m_mpPoint.end());
	return it->second;
}

std::vector<int> CBodyGroup::Nodes(const IElement &/*element*/) const
{
	return std::vector<int>();
}

void CBodyGroup::Clear()
{
	m_mpPoint.clear();
	m_vcElement.clear();
	m_vcNode.clear();

	// Destroy all surfaces
	while(SideSurfaceSize() != 0)
		delete &SideSurface(0);

	InvalidateCache();
}

int CBodyGroup::SideSurfaceSize() const
{
	 return (int)m_vcSurface.size();
}

const CBodyGroup::CSideSurface& CBodyGroup::SideSurface(int nIndex) const
{
	return *m_vcSurface[nIndex];
}

CBodyGroup::CSideSurface& CBodyGroup::SideSurface(int nIndex)
{
	return *m_vcSurface[nIndex];
}

const IFace & CBodyGroup::EdgeFace(int nIndex) const
{
	int nFloor = 0;
	for(int i = 0; i < SideSurfaceSize(); i++)
	{
		if(nIndex < (nFloor + SideSurface(i).FaceSize()))
			return SideSurface(i).Face(nIndex - nFloor);

		nFloor += SideSurface(i).FaceSize();
	}

	assert(false);	// Out of range
	const IFace *pFace = 0;
	return *pFace;
}

int CBodyGroup::EdgeFaceSize() const
{
	// Take the sum of all side surface faces
	int lRet = 0;
	for(int i = 0; i < SideSurfaceSize(); i++)
		lRet += SideSurface(i).FaceSize();
	return lRet;
}

void CBodyGroup::ReplaceNode(IBody& body, int nOldNodeIndex, int nNewNodeIndex)
{
  const INode& oldNode = Mesh().Node(nOldNodeIndex);
  const INode& newNode = Mesh().Node(nNewNodeIndex);

  TPointMap::iterator it = m_mpPoint.find( &oldNode );
  if ( it != m_mpPoint.end() ) {
    TBodyVec& vcBody = it->second;
    TBodyVec::iterator itv = std::find ( vcBody.begin(), vcBody.end(), &body );
    if ( itv != vcBody.end() ) {
      vcBody.erase(itv);
      if ( vcBody.empty() ) {
        // erase the node from the point map and vector
        m_mpPoint.erase(it);
        std::vector<int>::iterator itvn =
          std::find( m_vcNode.begin(), m_vcNode.end(), oldNode.Index() );
        if ( itvn != m_vcNode.end() )
          m_vcNode.erase(itvn);
      }
    }

    std::pair<TPointMap::iterator, bool> prInsert =
      m_mpPoint.insert( TPointMap::value_type( &newNode, TBodyVec()) );
    prInsert.first->second.push_back( &body ) ;

    if ( prInsert.second )
      m_vcNode.push_back( nNewNodeIndex );
  }

  int i;
  for( i = 0; i < m_vcSurface.size(); ++i )
    m_vcSurface[i]->ReplaceNode(oldNode, newNode);
}

} // namespace geo
