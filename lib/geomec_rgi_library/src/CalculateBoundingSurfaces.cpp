#include "CalculateBoundingSurfaces.h"
#include "IElementSet.h"
#include "Tetrahedron.h"

namespace {

void addTrianglesCount(GeomecRGI::TSharedBodyTriangleMap &triangleMap,
                       GeomecRGI::TSharedBodyTriangle &sharedBodyTriangle, int *nodes, size_t i0, size_t i1,
                       size_t i2) {
  QString hash = QString("%1.%2.%3").arg(nodes[i0]).arg(nodes[i1]).arg(nodes[i2]);
  std::pair<GeomecRGI::TSharedBodyTriangleMap::iterator, bool> result =
      triangleMap.insert(std::make_pair(hash, sharedBodyTriangle));

  if (!result.second) {
    size_t count = result.first->second->getCount();

    result.first->second->setCount(++count);
  }
}

const int NUMBER_OF_NODES = 3;

GeomecRGI::TSharedBodyTriangleMap collectTriangles(geo::IElementSet &elementSet) {
  GeomecRGI::TSharedBodyTriangleMap triangleMap;

  for (int element = 0; element < elementSet.ElementSize(); ++element) {
    const geo::CTetrahedron &tetrahedron = static_cast<const geo::CTetrahedron &>(elementSet.Element(element));

    for (int face = 0; face < tetrahedron.NrOfFaces(); ++face) {
      const geo::CBodyTriangle *bodyTriangle = static_cast<const geo::CBodyTriangle *>(&tetrahedron.Face(face));
      GeomecRGI::TSharedBodyTriangle bodyTriangleCountPair = QSharedPointer<GeomecRGI::CSharedBodyTriangle>(
          new GeomecRGI::CSharedBodyTriangle(elementSet, bodyTriangle, 0));
      int nodes[NUMBER_OF_NODES];

      assert(NUMBER_OF_NODES == bodyTriangle->NrOfNodes());

      for (int n = 0; n < bodyTriangle->NrOfNodes(); ++n) {
        nodes[n] = bodyTriangle->Node(n).Index();
      }

      addTrianglesCount(triangleMap, bodyTriangleCountPair, nodes, 0, 1, 2);
      addTrianglesCount(triangleMap, bodyTriangleCountPair, nodes, 0, 2, 1);
      addTrianglesCount(triangleMap, bodyTriangleCountPair, nodes, 1, 0, 2);
      addTrianglesCount(triangleMap, bodyTriangleCountPair, nodes, 1, 2, 0);
      addTrianglesCount(triangleMap, bodyTriangleCountPair, nodes, 2, 0, 1);
      addTrianglesCount(triangleMap, bodyTriangleCountPair, nodes, 2, 1, 0);
    }
  }

  return triangleMap;
}

} // anonymous namespace

namespace GeomecRGI {

CCalculateBoundingSurfaces::CCalculateBoundingSurfaces(geo::IElementSet &elementSet)
    : m_SharedBodyTriangleMap(collectTriangles(elementSet)) {}

const TSharedBodyTriangleMap &CCalculateBoundingSurfaces::getSharedBodyTriangleMap() const {
  return m_SharedBodyTriangleMap;
}

} // namespace GeomecRGI
