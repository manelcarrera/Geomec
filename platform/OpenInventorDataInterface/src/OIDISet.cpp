#include "OIDISet.h"
#include "IMesh.h"
#include "OIDIMesh.h"
#include "OIDIVisitor.h"
#include "openglnode.h"

OIDISet::OIDISet() : m_translation(0, 0, 0) {}

OIDISet::~OIDISet() { clear(); }

void OIDISet::clear() {
  for (TNodeMap::iterator it = m_NodeMap.begin(); it != m_NodeMap.end(); ++it)
    delete it->second;
  m_NodeMap.clear();

  for (std::size_t i = 0; i < m_MeshVector.size(); ++i)
    if (m_MeshVector[i])
      delete m_MeshVector[i];
  m_MeshVector.clear();
}

void OIDISet::clearIfEmpty() {
  if (isEmpty()) {
    m_MeshVector.clear();
  }
}

void OIDISet::collectMeshes(const COpenGLNode &node, std::vector<const OIDIMesh *> &meshes) {
  TNodeMap::iterator it = m_NodeMap.find(&node);
  if (it != m_NodeMap.end()) {
    for (int i = 0; i < it->second->NrOfHandles(); ++i) {
      Handle handle = it->second->getHandle(i);
      meshes.push_back(m_MeshVector[handle.index]);
    }
  }
}

bool OIDISet::handleNodeNew(const COpenGLNode &node) {
  TNodeMap::iterator it = m_NodeMap.find(&node);
  if (it != m_NodeMap.end())
    return true;

  Node *ifNode = new Node(this);

  OIDIVisitor visitor(&node, this, ifNode);

  for (int i = 0; i < node.DisplayListSize(); ++i) {
    geo::IObject *object = const_cast<geo::IObject *>(&node.DisplayList(i));

    object->Visit(visitor);
  }

  if (ifNode->NrOfHandles() > 0) {
    m_NodeMap.insert(std::make_pair(&node, ifNode));
    return true;
  } else {
    delete ifNode;
    return false;
  }
}

bool OIDISet::handleNodeDeleted(const COpenGLNode &node) {
  // the tno diana mesh may have been destroyed
  TNodeMap::iterator it = m_NodeMap.find(&node);
  if (it != m_NodeMap.end()) {
    OIDISet::Node *pNode = it->second;
    deleteOIDISetNode(pNode);

    m_NodeMap.erase(it);

    return true;
  }

  return false;
}

void OIDISet::deleteMeshAndAssociatedMeshes(const OIDIMesh *mesh) {
  for (TNodeMap::iterator it = m_NodeMap.begin(); it != m_NodeMap.end(); ++it) {
    OIDISet::Node *node = it->second;
    bool meshWasFound = false;
    for (std::size_t i = 0; i < node->NrOfHandles(); ++i) {
      Handle handle = node->getHandle(i);
      if (m_MeshVector[handle.index] == mesh) {
        meshWasFound = true;
        break;
      }
    }

    if (meshWasFound) {
      deleteOIDISetNode(node);
      m_NodeMap.erase(it);
      return;
    }
  }
}

void OIDISet::deleteMesh(std::size_t index) {
  assert(index >= 0 && index < m_MeshVector.size());
  delete (m_MeshVector[index]);
  m_MeshVector[index] = 0;
  clearIfEmpty();
}

bool OIDISet::isEmpty() const {
  for (std::size_t i = 0; i < m_MeshVector.size(); ++i)
    if (m_MeshVector[i])
      return false;
  return true;
}

const OIDIMesh *OIDISet::getFirstMesh() {
  for (std::size_t i = 0; i < m_MeshVector.size(); ++i)
    if (m_MeshVector[i])
      return m_MeshVector[i];

  return 0;
}

void OIDISet::deleteOIDISetNode(OIDISet::Node *node) {
  for (std::size_t i = 0; i < node->NrOfHandles(); ++i) {
    Handle handle = node->getHandle(i);
    deleteMesh(handle.index);
  }
  delete node;
}

void OIDISet::addMesh(OIDIMesh *wrapper) {
  const_cast<OIDIGeometryI &>(wrapper->getGeometry()).Translate(m_translation);
  m_MeshVector.push_back(wrapper);
}

void OIDISet::Translate(const SbVec3d &translation) {
  m_translation.setValue(translation[0], translation[1], translation[2]);
  for (size_t i = 0; i < m_MeshVector.size(); ++i) {
    if (m_MeshVector[i])
      const_cast<OIDIGeometryI &>(m_MeshVector[i]->getGeometry()).Translate(m_translation);
  }
}
