#ifndef _MainMeshNodeAssistant_h_
#define _MainMeshNodeAssistant_h_

class COpenInventorEventsHandler;
class COpenInventorSceneNode;

class CMainMeshNodeAssistant : public CGraphNode
{
public:
    CMainMeshNodeAssistant(COpenInventorEventsHandler * eventsHandler, COpenInventorSceneNode * sceneNode);

    virtual bool CanConnectItem(const CGraphNode& item) const;  
    virtual bool ConnectItem(const CGraphNode& node);    
    virtual void OnNewNeighbour(const CGraphNode & node);   
    virtual void OnNeighbourModified(const CGraphNode& node, enum ModifiedHint uHint);   
    virtual void OnNeighbourDeleted(const CGraphNode& node);
    virtual void LinkTo(CGraphNode &rhs);
    virtual unsigned int IconId() const;  
    virtual unsigned int TypeId() const;

    const COpenGLNode * getMeshNode() const;
    bool canBeDisplayed() const;

private:
    const COpenGLNode * m_meshNode;
    COpenInventorEventsHandler * m_eventsHandler;
    COpenInventorSceneNode     * m_sceneNode;
    bool m_canBeDisplayed;
};

#endif  // _MainMeshNodeAssistant_h_
