#include "stdafx.h"

#include "FormationBase.h"
#include "FormationNodeAssistant.h"
#include "OpenInventorEventsHandler.h"

CFormationNodeAssistant::CFormationNodeAssistant(COpenInventorEventsHandler *eventsHandler)
    : CGraphNode("CFormationNodeAssistant"), m_eventsHandler(eventsHandler) {}

bool CFormationNodeAssistant::CanConnectItem(const CGraphNode &item) const { return true; }

bool CFormationNodeAssistant::ConnectItem(const CGraphNode &node) { return true; }

void CFormationNodeAssistant::OnNewNeighbour(const CGraphNode &node) {
  // Only called for formations, so static_cast is OK
  m_eventsHandler->OnNewFormationNode(static_cast<const CFormationBase &>(node));
}

void CFormationNodeAssistant::OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint) {
  if (uHint == MeshCleared)
    return;

  if (uHint == GEOMETRY_CHANGED) {
    OnNeighbourDeleted(node);
    OnNewNeighbour(node);
  } else
    m_eventsHandler->OnFormationNodeModified(static_cast<const CFormationBase &>(node), uHint);
}

void CFormationNodeAssistant::OnNeighbourDeleted(const CGraphNode &node) {
  m_eventsHandler->OnFormationNodeDeleted(static_cast<const CFormationBase &>(node));
}

unsigned int CFormationNodeAssistant::IconId() const { return 0; }

unsigned int CFormationNodeAssistant::TypeId() const { return 0; }
