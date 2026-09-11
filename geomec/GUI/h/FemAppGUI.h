#ifndef _FemAppGUI_h_
#define _FemAppGUI_h_

class CFemAppModel;

#include "DocumentGUI.h"
#include "GraphNode.h"

class CFemAppGUI : public CDocumentGUI {
public:
  CFemAppGUI(CFemAppGUI *parentGUI, CFemAppModel *femAppModel);
  virtual ~CFemAppGUI();

  virtual void ModelProperties();

  int childGUISize() const;
  CFemAppGUI &childGUI(int index);
  CFemAppGUI *parentGUI();

  void modelDeleted();
  void removeChildGUI(CFemAppGUI *femAppGUI);

  virtual bool NewModel(bool bAttachToDocument = true);

  virtual void OnCreateSceneEntry();
  virtual void createDefaults();
  virtual void OnCloseModel();

  void DeleteScenes();

  CFemAppModel *model() const;

private:
  class Observer : public CGraphNode {
  public:
    Observer(CFemAppGUI &femAppGUI, const QString &strInstanceName);
    Observer(CFemAppGUI &femAppGUI, unsigned int uInstanceName);

    virtual void OnNewNeighbour(const CGraphNode &node);
    virtual void OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint);
    virtual void OnNeighbourDeleted(const CGraphNode &node);

    virtual unsigned int IconId() const;
    virtual unsigned int TypeId() const;

  private:
    Observer(const Observer &rhs);
    Observer &operator=(const Observer &rhs);

    CFemAppGUI &m_femAppGUI;
  };

  CFemAppGUI(const CFemAppGUI &rhs);
  CFemAppGUI &operator=(const CFemAppGUI &rhs);

  CFemAppModel *m_femAppModel;
  Observer *m_observer;
  std::vector<CFemAppGUI *> m_childGUI;
  CFemAppGUI *m_parentGUI;

protected:
  bool m_b_GUI_disabled;

public:
  bool disabled() { return m_b_GUI_disabled; }
};

#endif // _FemAppGUI_h_
