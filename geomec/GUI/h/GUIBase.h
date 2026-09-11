#ifndef _GUIBase_h_
#define _GUIBase_h_

class CPropertySheet;

#include "FemAppGUI.h"
#include "ModelBase.h"

class COpenInventorSceneNode;

class CGUIBase : public CFemAppGUI {
public:
  CGUIBase(CFemAppGUI *parentGUI, CModelBase *modelBase);

  virtual void CollectPropertyPages(CPropertySheet &sheet);
  virtual void ModelProperties();
  virtual void ModelTree();
  virtual void MeshRefinement();
  bool Can_MeshRefinement() const;

  void InsertDeviatedWellPath(bool bInsertByPointSet = false); // wjrx mantis 3185

  void ImportResults();

  virtual void OnCreateSceneEntry();
  virtual void OnCloseModel();

  // allow redrawing after it was automatically disabled

  void AllowRedrawing();

  CModelBase *model() const;

  bool Load(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &prog, bool bLoadHeader);

  void LinkModelToOpenInventor();

  bool Save(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &prog, bool bSaveHeader);
  void SaveFileHeader(CStorageNode::TSTREAM &stream, const QString &currentAppVersion);
  long SavedItems() const;

  virtual void LoadScenes(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &progress,
                          bool bIgnore = false);
  virtual void SaveScenes(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &progress);
  long SavedItemsScenes() const;

  virtual bool CanSwitchTo() const;
  virtual void SwitchTo();

  void SwitchToParent();

  void SetParentModelResults();

  COpenInventorSceneNode &OpenInventorScene();
  COpenInventorSceneNode *getDefaultOpenInventorSceneNode() const;

protected:
  virtual bool LoadPre381Stream(CStorageNode::TSTREAM &stream, CStreamVersion &version,
                                CStorageNode::TPROGRESS &prog) = 0;

private:
  class CChildModelLoadSave : public CModelBase::IModelLoadSave {
  public:
    CChildModelLoadSave(CGUIBase &gui);
    virtual bool LoadModel(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &prog,
                           CModelBase &childModel);
    virtual bool SaveModel(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &prog, CModelBase &childModel);
    virtual long SavedItems(CModelBase &childModel) const;

    virtual bool SupportGMP() const;
    virtual CArchiveInterface *createCArchive(const QString &filename) const;

  private:
    CGUIBase *getChildGUI(CModelBase &childModel);
    const CGUIBase *getChildGUI(CModelBase &childModel) const;

  private:
    CGUIBase &m_gui;
  };

public:
  class CMainModelLoadSave : public CModelBase::IModelLoadSave {
  public:
    CMainModelLoadSave(CGUIBase *pGUI = 0);
    CMainModelLoadSave(const QString &appVersion, CGUIBase *pGUI = 0);
    virtual bool LoadModel(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &prog,
                           CModelBase &model);
    virtual bool SaveModel(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &prog, CModelBase &model);
    virtual long SavedItems(CModelBase &model) const;

    virtual void SetVersionManager(CVersionManager &versionManager);

    virtual bool SupportGMP() const;
    virtual CArchiveInterface *createCArchive(const QString &filename) const;

    CGUIBase *GetGUI() const;

  private:
    CGUIBase *m_gui;
    CVersionManager *m_pVersionManager;
  };

private:
  CGUIBase(const CGUIBase &rhs);
  CGUIBase &operator=(const CGUIBase &rhs);

  CModelBase *m_modelBase;
  COpenInventorSceneNode *m_pOpenInventorScene;
};

#endif // _GUIBase_h_
