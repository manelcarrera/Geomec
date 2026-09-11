#ifndef _ElementVertex_h_
#define _ElementVertex_h_

#include <set>
#include <vector>

#include <QSharedPointer>

namespace geo {

class IElement;

} // namespace geo

#include "ElementVertex.h"

namespace WellPath {

class CElementVertex;

typedef QSharedPointer<CElementVertex> TElementVertex;

struct COrderByVertexIndices {
  bool operator()(const TElementVertex &lhs, const TElementVertex &rhs);
};

typedef std::set<TElementVertex, COrderByVertexIndices> TElementVertices;

class CElementVertex {
public:
  typedef std::vector<int> TIndices;

  CElementVertex(const geo::IElement &element, int index0, int index1);
  ~CElementVertex();

  const geo::IElement &element() const;

  TIndices vertexIndices() const;

  TElementVertices &otherElementVertices();
  const TElementVertices &otherElementVertices() const;
  void otherElementVertex(const TElementVertex &otherElementVertex);

  void removeLinkWithOtherElementVertices();

private:
  CElementVertex(const CElementVertex &rhs);
  CElementVertex &operator=(CElementVertex rhs);

  static TIndices createVertexIndices(int index0, int index1);

  const geo::IElement &m_element;
  TIndices m_vertexIndices;
  TElementVertices m_otherElementVertices;
};

} // namespace WellPath

#endif // _ElementVertex_h_
