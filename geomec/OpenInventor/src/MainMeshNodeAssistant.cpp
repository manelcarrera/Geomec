#include "stdafx.h"

#include "MainMeshNodeAssistant.h"
#include "OpenInventorSceneNode.h"
#include "openglnode.h"

CMainMeshNodeAssistant::CMainMeshNodeAssistant(COpenInventorEventsHandler *eventsHandler,
                                               COpenInventorSceneNode *sceneNode)
    : CGraphNode("CMainMeshNodeAssistant"), m_meshNode(0), m_eventsHandler(eventsHandler), m_sceneNode(sceneNode),
      m_canBeDisplayed(false) {}

bool CMainMeshNodeAssistant::CanConnectItem(const CGraphNode &item) const { return m_meshNode != 0; }

bool CMainMeshNodeAssistant::ConnectItem(const CGraphNode &node) {
  m_meshNode = &dynamic_cast<const COpenGLNode &>(node);
  return true;
}

void CMainMeshNodeAssistant::OnNewNeighbour(const CGraphNode &node) {
  m_meshNode = &dynamic_cast<const COpenGLNode &>(node);
  m_sceneNode->ConnectFormationNodeAssistant();
  m_canBeDisplayed = m_sceneNode->NodeCanBeDisplayed(m_meshNode);
}

void CMainMeshNodeAssistant::OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint) {
  // We get numerous MeshCleared messages when loading a (hexa)mesh, and then a last modified with uHint == 0 (default)
  // So it seems the meaning of MeshCleared is either "I'm still building this thing" (see Bug 116912.2)
  // or "I actually have no mesh" (see Bug 113964)
  //
  // Fixing it here, as that seems to be in line with how the calling routines want it

  if (uHint == MeshCleared) {
    m_canBeDisplayed = false;
    m_sceneNode->DisconnectFormationNodeAssistant(uHint);
    m_sceneNode->ClearAll();

    m_meshNode = 0;
  } else {
    if (!m_meshNode) {
      assert(dynamic_cast<const CMeshBase *>(&node));
      m_meshNode = dynamic_cast<const COpenGLNode *>(&node);
    }

    m_canBeDisplayed = m_sceneNode->NodeCanBeDisplayed(m_meshNode);
    m_sceneNode->DisconnectFormationNodeAssistant();
    m_sceneNode->DisconnectUnconnectables();

    m_sceneNode->ConnectFormationNodeAssistant();
  }
}

void CMainMeshNodeAssistant::OnNeighbourDeleted(const CGraphNode &node) {
  m_canBeDisplayed = false;
  m_sceneNode->DisconnectFormationNodeAssistant();
  m_sceneNode->ClearAll();

  m_meshNode = 0;
}

void CMainMeshNodeAssistant::LinkTo(CGraphNode &rhs) {
  while (referenceSize() > 0)
    UnLink(referenceAt(0));
  CGraphNode::LinkTo(rhs);
}

unsigned int CMainMeshNodeAssistant::IconId() const { return 0; }

unsigned int CMainMeshNodeAssistant::TypeId() const { return 0; }

const COpenGLNode *CMainMeshNodeAssistant::getMeshNode() const { return m_meshNode; }

bool CMainMeshNodeAssistant::canBeDisplayed() const { return m_canBeDisplayed; }
