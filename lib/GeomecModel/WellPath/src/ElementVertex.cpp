#include <cassert>

#include "ElementVertex.h"

namespace WellPath
{

// COrderByVertexIndices

bool COrderByVertexIndices::operator () (const TElementVertex& lhs,
  const TElementVertex& rhs)
{
  return (lhs->vertexIndices() < rhs->vertexIndices());
}

// CElementVertex

CElementVertex::CElementVertex(const geo::IElement& element, int index0,
  int index1)
: m_element(element)
, m_vertexIndices(createVertexIndices(index0, index1))
, m_otherElementVertices()
{
}

CElementVertex::~CElementVertex()
{
  removeLinkWithOtherElementVertices();
}

const geo::IElement& CElementVertex::element() const
{
  return m_element;
}

CElementVertex::TIndices CElementVertex::vertexIndices() const
{
  return m_vertexIndices;
}

TElementVertices& CElementVertex::otherElementVertices()
{
  return m_otherElementVertices;
}

const TElementVertices& CElementVertex::otherElementVertices() const
{
  return m_otherElementVertices;
}

void CElementVertex::otherElementVertex(
  const TElementVertex& otherElementVertex)
{
  m_otherElementVertices.insert(otherElementVertex);
}

void CElementVertex::removeLinkWithOtherElementVertices()
{
  for (TElementVertices::const_iterator
    otherElementVertex = m_otherElementVertices.begin();
    otherElementVertex != m_otherElementVertices.end(); ++otherElementVertex)
  {
    for (TElementVertices::const_iterator
      elementVertex = (*otherElementVertex)->otherElementVertices().begin();
      elementVertex != (*otherElementVertex)->otherElementVertices().end();
      ++elementVertex)
    {
      if (m_vertexIndices == (*elementVertex)->vertexIndices())
      {
        (*otherElementVertex)->otherElementVertices().erase(elementVertex);
        break;
      }
    }
  }

  m_otherElementVertices.clear();
}

// private

// static

CElementVertex::TIndices
  CElementVertex::createVertexIndices(int index0, int index1)
{
  TIndices vertexIndices;

  vertexIndices.push_back(index0);
  vertexIndices.push_back(index1);

  return vertexIndices;
}

} // namespace WellPath
