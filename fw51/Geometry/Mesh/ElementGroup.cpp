 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// ElementGroup.cpp: implementation of the CElementGroup class.
//
//////////////////////////////////////////////////////////////////////
#include "dimple.h"
#include "Mesh.h"
#include "IElement.h"
#include "ElementGroup.h"

#include <tbb/spin_rw_mutex.h>

namespace {
tbb::spin_rw_mutex myGlobalMutex[16];
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace geo {
CElementGroup::CElementGroup(IMesh &mesh, bool bVisible)
: m_mesh(mesh),
  m_bVisible(bVisible)
{
  m_mesh.AddElementGroup(*this);
}

CElementGroup::~CElementGroup()
{
  m_mesh.RemoveElementGroup(*this);
}

//Construction
bool CElementGroup::AddMeshElement(IElement& elem)
{
  assert(elem.IndexingElementSet() == &m_mesh);

  m_vcElement.push_back(&elem);
  m_vcNode.clear();

  return true;
}

void CElementGroup::createPointBuffer() const
{
  int index = (intptr_t)this >> 6 & 0xf;

  tbb::spin_rw_mutex::scoped_lock lock(myGlobalMutex[index], false);

  if( m_vcNode.size() == 0 ) {

  lock.upgrade_to_writer();

  if (m_vcNode.size() != 0)
      return;

  std::set<int> stNode;
  int i;
  for(i = 0; i < ElementSize(); i++) {
      for(int j = 0; j < Element(i).NrOfNodes(); j++) {
    stNode.insert( Element(i).Node(j).Index() );
      }
  }
  i = 0;
  m_vcNode.resize( stNode.size(), 0 );
  for( std::set<int>::iterator it = stNode.begin(); it != stNode.end(); it++) {
      m_vcNode[i] = *it;
      i++;
  }
  }
}

IPoint& CElementGroup::PointAt(int nIndex)
{
  assert( nIndex >= 0 );
  assert( nIndex < PointSize() );
  createPointBuffer();
  return const_cast<IPoint&>(m_mesh.Point( m_vcNode[nIndex] ));
}

const IPoint& CElementGroup::Point(int nIndex) const
{
  assert( nIndex >= 0 );
  assert( nIndex < PointSize() );
  createPointBuffer();
  return m_mesh.Point( m_vcNode[nIndex] );

}

int CElementGroup::PointSize() const
{
  createPointBuffer();
  return (int)m_vcNode.size();
}

const IElement& CElementGroup::Element(int nIndex) const
{
  assert(nIndex>=0);
  assert(nIndex<m_vcElement.size());
  return *m_vcElement[nIndex];
}

IElement& CElementGroup::Element(int nIndex)
{
  assert(nIndex>=0);
  assert(nIndex<m_vcElement.size());
  return *m_vcElement[nIndex];
}

int CElementGroup::ElementSize() const
{
  return (int)m_vcElement.size();
}

std::vector<int> CElementGroup::ElementsAt(const IPoint &/*point*/) const
{
  assert(false);
  return std::vector<int>();
}

// get the node indices connected to the given element
std::vector<int> CElementGroup::Nodes(const IElement &/*element*/) const
{
  assert(false);
  return std::vector<int>();
}

void CElementGroup::AssertValid() const
{
}

bool CElementGroup::isVisible() const
{
  return m_bVisible;
}

void CElementGroup::setVisible(bool bVisible)
{
  m_bVisible = bVisible;
}

}

