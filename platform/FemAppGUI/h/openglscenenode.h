// OpenGLSceneBase.h: interface for the COpenGLSceneBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPENGLSCENENODE_H__551C94FF_E6A3_4B7C_9846_7061028F297C__INCLUDED_)
#define AFX_OPENGLSCENENODE_H__551C94FF_E6A3_4B7C_9846_7061028F297C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class COpenGLNode_Delegate;

#include "OpenGLScene.h"
#include "OperationReceiver.h"
#include <map>
#include <set>

class COpenGLNode;
class COpenGLNodeBase;
class CLegendFrame;
class COpenGLSceneNode : public CStorageNode, public COpenGLScene {
public:
  // The default listener
  class CDefaultListener : public CMouseListener {
  public:
    // Prototype of function, which execute the handler of opengl node
    typedef std::vector<const geo::IObject *> TObjectVec;
    typedef bool (COpenGLNode_Delegate::*pfHandler)(TFrame &frame, IOpenGLFrame::TKeyboardModifiers state,
                                                    IOpenGLFrame::TMouseButton button, const TScreenPoint &point,
                                                    const TObjectVec &vcHit);

  private:
    COpenGLSceneNode &m_scene;
    bool m_bDestroy;
    // Given a point the selection function returns a vector of object
    // that are hit. (Only for use in the message functions)
    bool DispatchSelection(TKeyboardModifiers state, TMouseButton button, const TScreenPoint &point, pfHandler handler);

  public:
    CDefaultListener(COpenGLSceneNode &scene);

    virtual bool MousePress(TKeyboardModifiers state, TMouseButton button, const TScreenPoint &point);
    virtual bool MouseRelease(TKeyboardModifiers state, TMouseButton button, const TScreenPoint &point);
    virtual bool MouseDblClk(TKeyboardModifiers state, TMouseButton button, const TScreenPoint &point);
    virtual bool MouseMove(TKeyboardModifiers state, TMouseButton button, const TScreenPoint &point);

    virtual void DrawScene() {}
    void Destroy();
    virtual bool OnOK();
    virtual bool OnCancel();
    virtual bool OnDelete();
  };

  class CSceneOperationReceiver : public COperationReceiver {
    COpenGLSceneNode &m_scene;

  protected:
    virtual void OnBeginOperation();
    virtual void OnEndOperation();

  public:
    CSceneOperationReceiver(COpenGLSceneNode &scene);
  };

  friend class CSceneOperationReceiver;

private:
  class COpenGLNodePriorityLess {
  public:
    bool operator()(const COpenGLNode *p1, const COpenGLNode *p2) const;
  };

  typedef std::vector<const geo::IObject *> TDisplayVec;
  typedef std::map<const COpenGLNode *, TDisplayVec, CGraphNode::CPtrLess> TViewMap;
  TViewMap m_mpViewMap;
  CGraphNode *m_pSelected;
  bool m_bFill;
  void Render(const COpenGLNodeBase &node);
  CSceneOperationReceiver *m_pReceiver;
  bool m_bDirty;
  bool m_bProgress;

protected:
  // Progress indicator
  virtual void OnBeginDraw(int nSteps);
  virtual void OnDrawStep(int nSteps);
  virtual void OnEndDraw();

  // Synchronisation of the scene
  virtual void OnBeginOperation();
  virtual void OnEndOperation();
  virtual void UpdateScene();
  bool IsOperation() const;
  virtual void OnUpdateLegend();
  virtual bool OnInsertNodeInLegend(const COpenGLNode &node) const;
  int InsertLinkedNodesInLegend();
  bool IsLegendFrame() const;
  CLegendFrame &LegendFrame();
  CDefaultListener *m_pDefaultListener; // Enables the user to install its own default listener
public:
  typedef std::set<const COpenGLNode *, COpenGLNodePriorityLess> TOpenGLNodeSet;
  // Construction
  COpenGLSceneNode(CFemAppModel &model);
  COpenGLSceneNode(const QString &strName, CFemAppModel &model);
  COpenGLSceneNode(unsigned int uName, CFemAppModel &model);
  COpenGLSceneNode(const COpenGLSceneNode &rhs);
  virtual ~COpenGLSceneNode();

  // Assignment
  bool operator==(const COpenGLSceneNode &rhs) const;
  COpenGLSceneNode &operator=(const COpenGLSceneNode &rhs);

  // Overrides ...
  virtual IDrawDef *OnCreateDrawDef(const COpenGLNode &node, const geo::IObject &object);
  virtual std::vector<TColor> OnColor(const COpenGLNode &node, const geo::IObject &object) const;
  virtual void OnOpenGLNodeDeleted(COpenGLNode &node);
  virtual void OnOpenGLNodeModified(COpenGLNode &node, enum ModifiedHint uHint);
  virtual void OnNewOpenGLNode(COpenGLNode &node);
  virtual void Clear();
  virtual void OnSelect(CGraphNode &node);
  TOpenGLNodeSet NodesInScene() const;
  virtual TOpenGLNodeSet NodesInSceneForLegend() const;

  void UpdateLegend();

  // Activation and deactivation
  virtual bool ReParent(IOpenGLFrame *pParent);

  // GraphNode overides
  virtual void OnNewNeighbour(const CGraphNode &node);
  virtual void OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint);
  virtual void OnNeighbourDeleted(const CGraphNode &node);
  virtual bool ConnectItem(const CGraphNode &item);
  virtual bool CanConnectItem(const CGraphNode &item) const;
  virtual unsigned int IconId() const;
  virtual unsigned int TypeId() const;

  // Selection mode
  enum eSelectMode { SM_SELECT, SM_RADIO, SM_CHECK };
  virtual eSelectMode SelectMode() const;

  // Fill
  bool Fill() const;
  void Fill(bool bFill, bool bUpdate = true);

  // Invalidate
  void InvalidateAll();

  // Save and load
  virtual bool Empty() const;
  virtual long SavedItems() const;

  virtual void LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress);
  virtual void SaveStream(TSTREAM &stream, TPROGRESS &progress);
};

// ##ModelId=3BC55D600380
typedef CGraphEntryTemp<COpenGLSceneNode> TSceneNodeEntry;

#endif // !defined(AFX_OPENGLSCENENODE_H__551C94FF_E6A3_4B7C_9846_7061028F297C__INCLUDED_)
