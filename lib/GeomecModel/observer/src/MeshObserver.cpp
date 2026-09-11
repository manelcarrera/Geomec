
#include "MeshObserver.h"
#include "ModifiedHint.h"
#include "ValueCacheBase.h"

CMeshObserver::CMeshObserver(CMeshBase *mesh, CValueCacheBase &valueCache)
    : CGraphNode(""), m_Mesh(mesh), m_ValueCache(valueCache) {
  /*
   * The parameter mesh is not fully defined at this point.
   * The initialization is deferred to the CValueCache parent.
   * Initialization will take place at an opportune moment (when the
   * CValueCache instance is accessed).
   */
}

CMeshObserver::CMeshObserver(const CMeshObserver &rhs)
    : CGraphNode(rhs), m_Mesh(rhs.m_Mesh), m_ValueCache(rhs.m_ValueCache) {}

CMeshObserver &CMeshObserver::operator=(const CMeshObserver &rhs) {
  CMeshObserver temporaryMeshObserver(rhs);

  temporaryMeshObserver.swap(*this);

  return *this;
}

CMeshObserver::~CMeshObserver() {
  /*
   * The destructor of the base class CGraphNode will take care of un-linking
   * this observer from the subject m_Mesh.
   */
}

unsigned int CMeshObserver::IconId() const { return 0; }

unsigned int CMeshObserver::TypeId() const { return 0; }

void CMeshObserver::OnNewNeighbour(const CGraphNode &node) { CGraphNode::OnNewNeighbour(node); }

void CMeshObserver::OnNeighbourModified(const CGraphNode &node, enum ModifiedHint hint) {
  if (hint == MeshCleared) {
    m_ValueCache.clearCache();
  }

  CGraphNode::OnNeighbourModified(node, hint);
}

void CMeshObserver::OnNeighbourDeleted(const CGraphNode &node) { CGraphNode::OnNeighbourDeleted(node); }

CMeshBase *CMeshObserver::getMesh() const { return m_Mesh; }

// private

void CMeshObserver::swap(CMeshObserver &aMeshObserver) {
  CMeshBase *temporaryMesh = aMeshObserver.m_Mesh;
  CValueCacheBase &temporaryValueCache = aMeshObserver.m_ValueCache;

  aMeshObserver.m_Mesh = m_Mesh;
  aMeshObserver.m_ValueCache = m_ValueCache;

  m_Mesh = temporaryMesh;
  m_ValueCache = temporaryValueCache;
}
