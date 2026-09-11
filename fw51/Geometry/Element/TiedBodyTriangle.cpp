#include "TiedBodyTriangle.h"
#include "BodyTriangle.h"
#include "IMesh.h"

namespace geo {

CTiedBodyTriangle::CTiedBodyTriangle(IMesh &mesh, std::vector<int> vcNodes,
                                     std::vector<const CBodyTriangle *> vcTriangles)
    : m_mesh(mesh), m_nIndex(-1) {
  for (size_t i = 0; i < 3; ++i) {
    m_nNodes[i] = vcNodes[i];
    m_pTriangles[i] = vcTriangles[i];
  }

  m_nIndex = m_mesh.RegisterTiedBodyTriangle(-1, *this);
}

CTiedBodyTriangle::CTiedBodyTriangle(IMesh &mesh, int nIndex, std::vector<int> vcNodes,
                                     std::vector<const CBodyTriangle *> vcTriangles)
    : m_mesh(mesh), m_nIndex(-1) {
  for (size_t i = 0; i < 3; ++i) {
    m_nNodes[i] = vcNodes[i];
    m_pTriangles[i] = vcTriangles[i];
  }

  m_mesh.RegisterTiedBodyTriangle(nIndex, *this);
}

CTiedBodyTriangle::~CTiedBodyTriangle() {}

const IBody *CTiedBodyTriangle::Parent() const {
  assert(false);
  return 0; // m_pTriangles[0]->Parent();
}

const CBodyTriangle *CTiedBodyTriangle::BodyTriangle(int nIndex) const { return m_pTriangles[nIndex]; }

int CTiedBodyTriangle::PointIndex(int nIndex) const { return m_nNodes[nIndex]; }

size_t CTiedBodyTriangle::NrOfNodes() const { return Order() * 3; }

const INode &CTiedBodyTriangle::Node(int nIndex) const {
  assert(nIndex >= 0 && nIndex < NrOfNodes());
  return m_mesh.Node(PointIndex(nIndex));
}

void CTiedBodyTriangle::Node(int nIndex, const IPoint &node) { assert(false); }

size_t CTiedBodyTriangle::Order() const { return 1; }

} // namespace geo
