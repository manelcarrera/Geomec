#ifndef _OIDI_SET_H__
#define _OIDI_SET_H__

#include <Inventor/SbVec.h>
#include <cassert>
#include <map>
#include <vector>

class COpenGLNode;

class OIDIMesh;

namespace geo {
class IMesh;
}

class OIDISet {

public:
  OIDISet();
  ~OIDISet();

  enum OIType { VOLUME_UNSTRUCTURED = 0, SURFACE_UNSTRUCTURED, LINE_UNSTRUCTURED, UNUSED };

  struct Handle {
    OIType type;
    std::size_t index;

    Handle() : type(UNUSED), index((unsigned int)-1) {}
    Handle(OIType type, std::size_t index) : type(type), index(index) {}
  };

  class Node {
  public:
    Node(const OIDISet *parent) : m_Parent(parent) {}

    std::size_t NrOfHandles() { return m_Handles.size(); }

    Handle getHandle(std::size_t index) {
      assert(index >= 0 && index < m_Handles.size());
      return m_Handles[index];
    }
    void addHandle(Handle handle) { m_Handles.push_back(handle); }

  private:
    const OIDISet *m_Parent;
    std::vector<Handle> m_Handles;
  };

  typedef std::map<const COpenGLNode *, Node *> TNodeMap;
  typedef std::vector<const OIDIMesh *> TMeshVector;

  bool handleNodeNew(const COpenGLNode &node);
  bool handleNodeDeleted(const COpenGLNode &node);

  bool isEmpty() const;
  const OIDIMesh *getFirstMesh();

  void clear();
  void clearIfEmpty();

  void collectMeshes(const COpenGLNode &node, std::vector<const OIDIMesh *> &meshes);

  void deleteMeshAndAssociatedMeshes(const OIDIMesh *mesh);

  std::size_t NrOfMesh() const { return m_MeshVector.size(); }

  const OIDIMesh *getMesh(std::size_t index) const {
    assert(index < m_MeshVector.size());
    return m_MeshVector[index];
  }

  void addMesh(OIDIMesh *wrapper);
  void Translate(const SbVec3d &translation);

  TMeshVector::iterator begin() { return m_MeshVector.begin(); }

  TMeshVector::iterator end() { return m_MeshVector.end(); }

  void deleteOIDISetNode(OIDISet::Node *node);

private:
  void deleteMesh(std::size_t index);

  TNodeMap m_NodeMap;

  TMeshVector m_MeshVector;

  SbVec3d m_translation;
};
#endif