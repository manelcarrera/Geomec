// FemAppDoc.h : interface of the CFemAppDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_FEMAPPDOC_H__E321AF99_DA35_4988_AC51_3F4AAC08E45C__INCLUDED_)
#define AFX_FEMAPPDOC_H__E321AF99_DA35_4988_AC51_3F4AAC08E45C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define NEW_MODEL 1
#define NEW_CURRENT_SCENE 1034
#define ON_ZOOMWINDOW_BUTTON 1035
#define CURRENT_SCENE_MODIFIED 1036

class CFemAppGUI;

#include "GraphPtr.h"
#include "legendframebase.h"
#include "unitnode.h"

class COpenGLSceneBase;
class CFemAppModel;
class ICtrlObject;
class ISceneWrapper;
class COpenInventorSceneNode;

class CFemAppDoc : public CDocument {
  ICtrlObject *m_pContext;
  CGraphPtr<CGraphNode> m_pSelection;
  CFemAppModel *m_pModel;
  CFemAppGUI *m_pGUI;
  bool m_bIsDeleting;
  friend class CFemAppModel;
  CUnitNode m_unit;
  ISceneWrapper *m_pCurrent;
  bool m_bStickToScene;

protected: // create from serialization only
  // ##ModelId=3B653D030178
  CFemAppDoc();
  DECLARE_DYNCREATE(CFemAppDoc)
public:
  bool IsDeleting() { return m_bIsDeleting; }
  // ##ModelId=3B653D030179
  virtual void OnModelProperties();
  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CFemAppDoc)
public:
  virtual void DeleteContents();
  //}}AFX_VIRTUAL

  virtual void OnCloseDocument();
  virtual void OnInvalidateStorage();
  //	virtual void OnInvalidateCalculation(int nType = RECALCULATE_ALL);

  // These function are called by the model to attach and detach to the document
  virtual void OnAttachModel(CFemAppModel &model, CFemAppGUI &gui);
  virtual void OnDetachModel(CFemAppModel &model, CFemAppGUI &gui);

  // Switch between existing models (don't delete anything, just switch the current context)
  virtual void OnSwitchModel(CFemAppModel &model, CFemAppGUI &gui);

  CString &PathName() { return m_strPathName; }

  virtual void OnSelect(CGraphNode *pNode);
  void OnContextMenu(ICtrlObject &ctrl_object);
  CGraphNode &ContextMenuNode();
  const ICtrlObject &ContextMenuCtrlObject() const;
  ICtrlObject &ContextMenuCtrlObject();
  CGraphNode *SelectedNode();
  const CGraphNode *SelectedNode() const;

  ISceneWrapper *CurrentScene();
  const ISceneWrapper *CurrentScene() const;
  void CurrentScene(ISceneWrapper *pScene);

  bool StickToScene() const;
  void StickToScene(bool bStick);

  // Access to unit node
  CUnitNode &UnitNode();
  const CUnitNode &UnitNode() const;

  // ##ModelId=3B653D030198
  CFemAppModel *Model();
  const CFemAppModel *Model() const;
  CFemAppGUI *GUI();
  const CFemAppGUI *GUI() const;

  CFemAppGUI *retrieveMatchingGUI(CFemAppGUI *femAppGUI, CFemAppModel *femAppModel) const;

  // ##ModelId=3B653D030199
  virtual ~CFemAppDoc();
  void OnContextDelete();
  bool OnContextCanDelete() const;
#ifdef _DEBUG
  // ##ModelId=3B653D03019B
  virtual void AssertValid() const;
  // ##ModelId=3B653D03019D
  virtual void Dump(CDumpContext &dc) const;
#endif

  // Generated message map functions
protected:
  //{{AFX_MSG(CFemAppDoc)
  afx_msg void OnUpdateZoomWindow(CCmdUI *pCmdUI);
  afx_msg void OnZoomWindow();
  afx_msg void OnViewFill();
  afx_msg void OnViewOptions();
  afx_msg void OnViewPerspective();
  afx_msg void OnViewLighting();
  afx_msg void OnViewRotate();
  afx_msg void OnViewAxis();
  afx_msg void OnZoomIn();
  afx_msg void OnZoomOut();
  afx_msg void OnZoomReset();
  afx_msg void OnLockX();
  afx_msg void OnLockY();
  afx_msg void OnLockZ();
  afx_msg void OnUnlockAxes();
  //}}AFX_MSG

  DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FEMAPPDOC_H__E321AF99_DA35_4988_AC51_3F4AAC08E45C__INCLUDED_)
