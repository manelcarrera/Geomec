 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// Mesh.cpp: implementation of the CMesh class.
//
//////////////////////////////////////////////////////////////////////

#include "dimple.h"
#include "Mesh.h"

#include "IElement.h"
#include "ISupport.h"
#include "MeshNodeGroup.h"
#include "IElementSet.h"
#include "ICacheInterface.h"
#include "TiedBodyTriangle.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace geo {

//// CMesh::CNode implementation

CMesh::CNode::CNode(const IMesh &mesh, int nIndex, const IPoint& point, int nElements)
: m_mesh(mesh), m_nIndex(nIndex)
{
  m_vcElement.reserve(nElements);
  m_coor[0] = point.X();
  m_coor[1] = point.Y();
  m_coor[2] = point.Z();
}

CMesh::CNode::~CNode()
{
}

const IMesh &CMesh::CNode::Mesh() const
{
  return m_mesh;
}

int CMesh::CNode::Index() const
{
  return m_nIndex;
}

void CMesh::CNode::Index(int nIndex)
{
  m_nIndex = nIndex;
}

// Coordinate access
const double &CMesh::CNode::X() const
{
  return m_coor[0];
}

const double &CMesh::CNode::Y() const
{
  return m_coor[1];
}

const double &CMesh::CNode::Z() const
{
  return m_coor[2];
}

// Coordinate modification
void CMesh::CNode::X(const double &dX)
{
  m_coor[0] = dX;
}

void CMesh::CNode::Y(const double &dY)
{
  m_coor[1] = dY;
}

void CMesh::CNode::Z(const double &dZ)
{
  m_coor[2] = dZ;
}

void CMesh::CNode::AssertValid() const
{
}

bool CMesh::CNode::Empty() const
{
  return false;
}

// Looping over elements
int CMesh::CNode::AttachedElementSize() const
{
  return (int)m_vcElement.size();
}

const geo::IElement& CMesh::CNode::AttachedElement(int nIndex) const
{
  assert(nIndex >= 0 && nIndex < AttachedElementSize());
  return *m_vcElement[nIndex];
}

// Register element
int CMesh::CNode::RegisterElement(geo::IElement& element)
{
  int i;
  for(i = 0; i < m_vcElement.size(); i++)
  {
    if(m_vcElement[i] == &element) return i;
  }

  int nIndex = (int)m_vcElement.size();
  m_vcElement.push_back(&element);

  return nIndex;
}

bool CMesh::CNode::UnregisterElement(geo::IElement& element)
{
  int i;
  for(i = 0; i < m_vcElement.size(); i++)
  {
    if(&element == m_vcElement[i])
    {
      m_vcElement.erase(m_vcElement.begin() + i);
      return true;
    }
  }

  return false;
}


//// CMesh implementation

CMesh::CMesh() : m_vcPoint(1)
{
}

CMesh::~CMesh()
{
}

CMesh::CNode* CMesh::CreateMeshNode(int nIndex, const IPoint& point, int nElements)
{
  return new CNode(*this, nIndex, point, nElements);
}

// Registering elements and points in the mesher
int CMesh::RegisterElement(IElement& element)
{
  assert(element.IndexingElementSet() == this);

  InvalidateCache();
  
  // Register element on nodes ....
  for(int i = 0; i < element.NrOfPoints(); i++)
  {
    // Register element on node
    int nNodeIndex = element.PointIndex(i);
    assert(nNodeIndex >= 0);
    assert(nNodeIndex < PointSize());
    INode *pNode = (INode*)(&m_vcPoint.ObjectBase(nNodeIndex));
    pNode->RegisterElement(element);
  }

  // Add element
  int nIndex = (int)m_vcElement.size();
  m_vcElement.push_back(&element);

  return nIndex;
}

void CMesh::reserveElementSize(size_t size)
{
  m_vcElement.reserve(size);
}

// Clear the mesh
void CMesh::Clear()
{
  InvalidateCache();

  ICacheInterface &ci = CacheInterface();
  ci.Invalidate();

  // Destroy all elements
  for(size_t i = 0; i < m_vcElement.size(); i++)
    delete m_vcElement[i];

  m_vcElement.clear();

  // Destroy all tied body triangles
  for (size_t i = 0; i < m_vcTiedBodyTriangle.size(); ++i)
  delete m_vcTiedBodyTriangle[i];
  m_vcTiedBodyTriangle.clear();

  // Destroy all nodes
  m_mpPoint.clear();
  for(size_t i = 0; i < m_vcPoint.Size(); i++)
    delete &m_vcPoint.ObjectBase(i);

  m_vcPoint.Clear();

  IMesh::Clear();
}

int CMesh::RegisterNode(const IPoint& point, bool bUnique, int nElements)
{
  InvalidateCache();

  if(bUnique)
  {
    // Point has to be unique so do we have it already?
    TPointMap::const_iterator it = m_mpPoint.find(&point);
    if(it != m_mpPoint.end())
    {
      assert(it->second.size() > 0);
      return it->second[0];
    }
  }

  // Register the node
  int nIndex = m_vcPoint.Size();
  CNode *pNode = CreateMeshNode(nIndex, point, nElements);
  assert(pNode->Index() == nIndex);
  m_vcPoint.PushBack(*pNode);
  std::pair<TPointMap::iterator, bool> ret = m_mpPoint.insert(TPointMap::value_type(pNode, TNodeVec()));
  ret.first->second.push_back(nIndex);

  return nIndex;
}

void CMesh::RemoveElement(int nIndex)
{
  InvalidateCache();

  IElement* pElm = m_vcElement[nIndex];

  // unregister from nodes
  for(int i = 0; i < pElm->NrOfNodes(); ++i)
  {
  INode& node = const_cast<INode&>(pElm->Node(i));
  node.UnregisterElement(*pElm);
  }

  m_vcElement.erase(m_vcElement.begin() + nIndex);
}

int CMesh::RegisterTiedBodyTriangle(int nIndex, const CTiedBodyTriangle& tiedBodyTriangle)
{
  if (nIndex < 0)
  {
  nIndex = m_vcTiedBodyTriangle.size();
  m_vcTiedBodyTriangle.push_back(&tiedBodyTriangle);
  }
  else
  {
  if (nIndex >= m_vcTiedBodyTriangle.size())
      m_vcTiedBodyTriangle.resize(nIndex + 1);
  m_vcTiedBodyTriangle[nIndex] = &tiedBodyTriangle;
  }
  return nIndex;
}

const CTiedBodyTriangle& CMesh::TiedBodyTriangle(int nIndex)
{
  assert(nIndex >= 0 && nIndex < m_vcTiedBodyTriangle.size());
  return *m_vcTiedBodyTriangle[nIndex];
}

void CMesh::RemoveNode(int nIndex)
{
  InvalidateCache();

  CNode* pNode = static_cast<CNode*>(&m_vcPoint.Object(nIndex));
  assert(pNode->AttachedElementSize() == 0); // only unused nodes may be removed

  m_vcPoint.Remove(nIndex);
  m_vcPoint.Invalidate();
  for(int i = nIndex; i < m_vcPoint.Size(); ++i)
  {
  CNode* n = static_cast<CNode*>(&m_vcPoint.Object(i));
  n->Index(i);
  }

  TPointMap::iterator it = m_mpPoint.find(pNode);
  assert(it != m_mpPoint.end());
  for(int i = 0; i < it->second.size(); ++i)
  {
  if(it->second[i] == nIndex)
  {
      it->second.erase(it->second.begin() + i);
      break;
  }
  }
}

const INode& CMesh::Node(int nIndex) const
{
  assert(nIndex >= 0);
  assert(nIndex < PointSize());
  const IObject& object = m_vcPoint.ObjectBase(nIndex);
  const INode& node = dynamic_cast<const INode&>(object);
  return node;
}

int CMesh::NodeSize() const
{
  return m_vcPoint.Size();
}

const IElement &CMesh::Element(int nIndex) const
{
  return *m_vcElement[nIndex];
}

int CMesh::ElementSize() const
{
  return (int)m_vcElement.size();
}

void CMesh::Node(int nIndex, const IPoint& point)
{
  // Get node vector and remove it from the map. 
  INode& p = dynamic_cast<INode&> (m_vcPoint.ObjectBase(nIndex));
  TPointMap::iterator it = m_mpPoint.find(&p);
  assert(it != m_mpPoint.end());
  TNodeVec vcNode = it->second;
  m_mpPoint.erase(it);

  // Remove current point from node vector
  for(int i = 0; i < vcNode.size(); i++)
  {
    if(vcNode[i] == nIndex)
      vcNode.erase(vcNode.begin() + i);
  }
  
  // Reinsert node vector when size > 0
  if(vcNode.size() > 0)
  {
    const INode& ref_node = Node(vcNode[0]);
    m_mpPoint.insert(TPointMap::value_type(&ref_node, vcNode));
  }

  // Change node position
  p.X(point.X());
  p.Y(point.Y());
  p.Z(point.Z());

  // Insert node in 
  std::pair<TPointMap::iterator, bool> ret = m_mpPoint.insert(TPointMap::value_type(&p, TNodeVec()));
  ret.first->second.push_back(nIndex);
}

CMesh::TNodeVec CMesh::NodeIndex(const IPoint& point)
{
  // Find point in map
  TPointMap::const_iterator it = m_mpPoint.find(&point);
  if(it != m_mpPoint.end())
  {
    assert(it->second.size() > 0);
    return it->second;
  }

  return TNodeVec();
}

IPoint &CMesh::PointAt(int nIndex)
{
  assert(nIndex >= 0);
  assert(nIndex < PointSize());
  return (IPoint&)m_vcPoint.ObjectBase(nIndex);
}

CMesh::TNodeVec CMesh::NearestPoint(const IPoint& point) const
{
  return m_vcPoint.NearestNeighbourIndex(point);
}

}
