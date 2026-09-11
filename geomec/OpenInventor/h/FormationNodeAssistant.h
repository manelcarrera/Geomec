#ifndef _FormationNodeAssistant_h_
#define _FormationNodeAssistant_h_

class COpenInventorEventsHandler;

class CFormationNodeAssistant : public CGraphNode {
public:
  CFormationNodeAssistant(COpenInventorEventsHandler *eventsHandler);

  virtual bool CanConnectItem(const CGraphNode &item) const;
  virtual bool ConnectItem(const CGraphNode &node);
  virtual void OnNewNeighbour(const CGraphNode &node);
  virtual void OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint);
  virtual void OnNeighbourDeleted(const CGraphNode &node);
  virtual unsigned int IconId() const;
  virtual unsigned int TypeId() const;

private:
  COpenInventorEventsHandler *m_eventsHandler;
};

#endif // _FormationNodeAssistant_h_
