#include "stdafx.h"

#include "GUIBase.h"
#include "Geomec.h" // Mandatory order, before the files ModelGeneralPg.h,
#include "ModelBase.h"
// ModelTimestepPg.h, ModelGeologyPg.h!
#include "ArchiveCArchive.h"
#include "ArchiveQDataStream.h"
#include "AttriWellZoomInParentResults.h"
#include "BinaryResultsImport.h"
#include "ConsistencyGuard.h"
#include "DocumentProxy.h"
#include "FemAppMainWindow.h"
#include "GUIFactory.h"
#include "GeoProgress.h"
#include "GeomecStringTable.h"
#include "GeomecUtils.h"
#include "Global.h"
#include "HDF5File.h"
#include "IColorScaleEntry.h"
#include "IProgressBase.h"
#include "IProgressFactory.h"
#include "ISceneWrapper.h"
#include "ImportFormat.h"
#include "ImportPetrel.h"
#include "ModelGeneralPg.h"
#include "ModelGeologyPg.h"
#include "ModelTimestepPg.h"
#include "NewWellPath.h" // wjrx mantis 3185
#include "OpenInventorSceneNode.h"
#include "PetrelFileUnitDialog.h"
#include "ProgressDlg_MFC.h"
#include "PropertyFile.h"
#include "ReadOnlyScene.h"
#include "TnoFileDialog.h"
#include "WellPathFileUnitDlg.h"

#include "GlobalMessage.h"
#include "meshconverterdlg.h"

CGUIBase::CGUIBase(CFemAppGUI *parentGUI, CModelBase *modelBase)
    : CFemAppGUI(parentGUI, modelBase), m_modelBase(modelBase), m_pOpenInventorScene(0) {
  // make sure child models are created throught their associated GUI objects
  CModelBase *pParentModel = modelBase->parentModel();
  if (pParentModel)
    pParentModel->getNewModelObject().setModel(this);
}

void CGUIBase::CollectPropertyPages(CPropertySheet &sheet) {
  CPropertyPage *pPage = new CModelGeneralPg((CGeomecDoc &)(*GetGeomecDoc()));

  sheet.AddPage(pPage);
  pPage = new CModelTimestepPg(*m_modelBase);
  sheet.AddPage(pPage);
  pPage = new CModelGeologyPg(*m_modelBase);
  sheet.AddPage(pPage);
}

void CGUIBase::ModelProperties() {
  CPropertySheet sheet(IDS_MODEL_PROPERTIES);

  CollectPropertyPages(sheet);
  sheet.DoModal();
}

void CGUIBase::ModelTree() { MeshConverterDlg::instance()->show(); }

void CGUIBase::MeshRefinement() { m_pOpenInventorScene->show_refinement_box_dlg(); }

bool CGUIBase::Can_MeshRefinement() const {
  if (m_b_GUI_disabled)
    return false;

  return m_pOpenInventorScene->can_mesh_refinement();
}

namespace {

bool insertOneDeviatedWellPath(CModelBase *modelBase, QSharedPointer<IProgressBase> &progress, const QString &pathName,
                               bool insertByPointSet = false) {
  if (insertByPointSet) {
    if (!CNewWellPathEntry::ImportFile(pathName, *progress, *modelBase)) {
      return false;
    }
  } else {
    QString sPath, sFile;

    SplitPathAndFileName(pathName, sPath, sFile);

    CQuantity::UNIT unit;

    if (sFile.right(4).compare(".MWR", Qt::CaseInsensitive) == 0) {
      // get units in file from user

      CWellPathFileUnitDlg UnitDlg;

      UnitDlg.DoModal();
      unit = UnitDlg.Unit();
    }

    if (!CNewWellPathEntry::ImportFile(pathName, *progress, *modelBase, unit)) {
      return false;
    }
  }

  return true;
}

bool insertOneDeviatedWellPathFromPetrel(CModelBase *modelBase, QSharedPointer<IProgressBase> &progress,
                                         const QString &pathName) {
  QString sPath, sFile;

  SplitPathAndFileName(pathName, sPath, sFile);

  CQuantity::UNIT defaultLateralUnit = CQuantity::SI_UNIT, defaultDepthUnit = CQuantity::SI_UNIT;

  {
    IProgressBase progress2;
    CImportPetrel importPetrel(sFile.toStdString().c_str(), progress2, defaultLateralUnit, defaultDepthUnit);

    if (importPetrel.Import()) {
      defaultLateralUnit =
          importPetrel.getLateralUnit() == CImportPetrel::e_si_unit ? CQuantity::SI_UNIT : CQuantity::FIELD_UNIT;
      defaultDepthUnit =
          importPetrel.getDepthUnit() == CImportPetrel::e_si_unit ? CQuantity::SI_UNIT : CQuantity::FIELD_UNIT;
    }
  }

  CPetrelFileUnitDialog petrelFileUnitDialog(sFile.toStdString().c_str(), defaultLateralUnit, defaultDepthUnit);

  petrelFileUnitDialog.DoModal();

  if (!CNewWellPathEntry::ImportPetrelFile(pathName, *progress, *modelBase, petrelFileUnitDialog.getLateralUnit(),
                                           petrelFileUnitDialog.getDepthUnit())) {
    return false;
  }

  return true;
}

bool isPetrelFile(const QString &fileName) {
  std::ifstream stream(fileName.toStdString(), std::ios::in);
  const size_t bufferSize = 1000;
  std::vector<char> buffer(bufferSize, 0);

  stream.getline(&buffer[0], bufferSize);

  return CImportPetrel::hasPetrelHeader(&buffer[0]);
}

} // anonymous namespace

// wjrx mantis 2976
// wjrx mantis 3185: added bInsertByPointSet
void CGUIBase::InsertDeviatedWellPath(bool bInsertByPointSet /*=false*/) {
  QString sFilter;
  sFilter = getStringTableEntry(IDS_WELLPATH_IMPORT);

  CTnoFileDialog dlg(TRUE, NULL, NULL, OFN_ALLOWMULTISELECT, sFilter.toStdString().c_str());

  if (dlg.DoModal() == IDOK) {
    // Collect path name

    std::vector<QString> vcPathName;
    POSITION pos = dlg.GetStartPosition();

    while (pos) {
      vcPathName.push_back((LPCSTR)dlg.GetNextPathName(pos));
    }

    // Create stupid dialog

    QSharedPointer<IProgressBase> progress(_g->prog()->create(eProgress::Dual, "Import", true, vcPathName.size()));

    for (size_t i = 0; i < vcPathName.size(); i++) {
      progress->NextJob(vcPathName[i]);

      if (isPetrelFile(vcPathName[i])) {
        if (!insertOneDeviatedWellPathFromPetrel(m_modelBase, progress, vcPathName[i])) {
          break;
        }
      } else {
        if (!insertOneDeviatedWellPath(m_modelBase, progress, vcPathName[i], bInsertByPointSet)) {
          break;
        }
      }
    }

    FemAppGetMainWnd()->SetForegroundWindow();
  }
}

// wjrx mantis 2976
void CGUIBase::ImportResults() {
  std::vector<QString> vcPath;

  CTnoFileDialog file_dlg(TRUE, NULL, NULL, OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY,
                          "All Result Files (*.str;*.bin;*.ff)|"
                          "*.str;*.bin;*.ff|ASCII Result Files (*.str)|*.str|Binary Result Files (*.bin)|"
                          "*.bin|Filos Files (*.ff)|*.ff|All Files (*.*)|*.*||");

  if (file_dlg.DoModal() == IDOK) {
    POSITION pos = file_dlg.GetStartPosition();

    while (pos) {
      vcPath.push_back((LPCSTR)file_dlg.GetNextPathName(pos));
    }
  }

  if (vcPath.empty()) {
    return;
  }

  bool bSuccess = true;
  QString errmsg;
  QString strTitle = "Reading result files";

  std::auto_ptr<IProgressBase> progdlg(_g->prog()->create(eProgress::Dual, strTitle, true, vcPath.size()));

  for (size_t i = 0; i < vcPath.size() && bSuccess; ++i) {
    if (i) {
      progdlg->NextJob("");
    }

    if (vcPath[i].right(4).compare(".bin", Qt::CaseInsensitive) == 0) {
      CBinaryResultsImport bin_import(*m_modelBase);

      try {
        bSuccess = bin_import.Open(vcPath[i].toStdString().c_str(), *progdlg);

        if (!bSuccess) {
          errmsg = bin_import.ErrorMessage();
        }
      }

      catch (CProgressCancel *e) {
        delete e;
        bin_import.Close();
        bSuccess = false;
      }
    } else if (vcPath[i].right(3).compare(".ff", Qt::CaseInsensitive) == 0) {
      m_modelBase->ResultRegister().ReadFilosResults(vcPath[i]);
    } else {
      CResultImportFormat format(*m_modelBase);
      CElementSetFile file(format);

      try {
        bSuccess = file.Open(vcPath[i], *progdlg);
        if (!bSuccess) {
          errmsg = file.ErrorMessage();
        }
      }

      catch (CProgressCancel *e) {
        delete e;
        file.Close();
        bSuccess = false;
      }
    }
  }

  if (bSuccess) {
    m_modelBase->ResultRegister().Modified();

    // At this point we don't know what kind of results were imported, and therefore we propagate CLEAR_ALL
    m_modelBase->GetConsistencyGuard()->ResultClearingPropagate(*m_modelBase, CConsistencyGuard::CLEAR_ALL, true);
  } else {
    m_modelBase->ResultRegister().ClearAll();

    if (!errmsg.isEmpty()) {
      _m()->msg(errmsg);
    }

    return;
  }
}

void CGUIBase::OnCreateSceneEntry() {
  if (m_b_GUI_disabled)
    return;

  TSceneNodeEntry *pEntry = new TSceneNodeEntry(MD_BASE_SCENE_NODE, IDI_VIEW, IDS_TREE_VIEWS, *m_modelBase);

  m_pOpenInventorScene = new COpenInventorSceneNode(*m_modelBase);
  m_pOpenInventorScene->LinkTo(*pEntry);
}

void CGUIBase::OnCloseModel() {
  delete m_pOpenInventorScene;
  m_pOpenInventorScene = 0;

  CFemAppGUI::OnCloseModel();
}

void CGUIBase::AllowRedrawing() {
  if (m_modelBase->RedrawingDisabled()) { // hajx: not sure why this is done; there seems to be no "return to automatic
                                          // redrawing"
    m_modelBase->setAllowRedrawing(true);
  }

  // back to automatic redrawing, draw current scene so we're up to date

  COpenGLSceneNode *pScene = GetGeomecDoc()->CurrentScene()->OpenGLScene();

  if (pScene) {
    COpenGLSceneNode::TOpenGLNodeSet stNodes = pScene->NodesInScene();
    COpenGLSceneNode::TOpenGLNodeSet::iterator it;

    m_modelBase->setForcingRedraw(true);

    {
      COperation oper(*m_modelBase);

      for (it = stNodes.begin(); it != stNodes.end(); ++it) {
        pScene->UnLink(const_cast<COpenGLNode &>(**it));
        pScene->LinkTo(const_cast<COpenGLNode &>(**it));
      }
    }

    m_modelBase->setForcingRedraw(false);
  }

  COpenInventorSceneNode *pOIVScene = GetGeomecDoc()->CurrentScene()->OpenInventorScene();

  if (pOIVScene) {

    COpenInventorSceneNode::TOpenGLNodeSet stNodes = pOIVScene->NodesInScene();
    COpenInventorSceneNode::TOpenGLNodeSet::iterator it;

    m_modelBase->setForcingRedraw(true);
    pOIVScene->SetRedrawing(true);

    {
      COperation oper(*m_modelBase);

      for (it = stNodes.begin(); it != stNodes.end(); ++it) {
        pOIVScene->UnLink(const_cast<COpenGLNode &>(**it));
        pOIVScene->LinkTo(const_cast<COpenGLNode &>(**it));
      }
    }

    pOIVScene->SetRedrawing(false);
    m_modelBase->setForcingRedraw(false);

    pOIVScene->ForceValueRedraw();
  }
}

CModelBase *CGUIBase::model() const { return m_modelBase; }

bool CGUIBase::Load(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &prog,
                    bool bLoadHeader) {
  bool bRet;

  m_modelBase->SetChildModelLoadSave(new CChildModelLoadSave(*this));

  if (version < CStreamVersion(3, 8, 1)) {
    // old loading code
    bRet = LoadPre381Stream(stream, version, prog);
  } else {
    // new file format
    bRet = m_modelBase->Load(stream, version, prog, bLoadHeader);

    if (bRet) {
      int sceneMarker = m_modelBase->LoadSceneMarker(stream, version);

      stream.PushDataSet(CHDF5File::ModelGroup(m_modelBase->Index()) + "Scenes", CHDF5File::TINY);

      if (sceneMarker > 1) {
        LoadScenes(stream, version, prog);
      } else if (sceneMarker == 1 && model()->parentModel() != 0) {
        LoadScenes(stream, version, prog, true);
      }

      stream.PopDataSet();
    }
  }

  return bRet;
}

bool CGUIBase::Save(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &prog, bool bSaveHeader) {
  m_modelBase->SetChildModelLoadSave(new CChildModelLoadSave(*this));

  if (!m_modelBase->Save(stream, prog, bSaveHeader))
    return false;

  m_modelBase->SaveSceneMarker(stream, true);

  stream.PushDataSet(CHDF5File::ModelGroup(m_modelBase->Index()) + "Scenes", CHDF5File::TINY);
  SaveScenes(stream, prog);
  stream.PopDataSet();

  return true;
}

void CGUIBase::SaveFileHeader(CStorageNode::TSTREAM &stream, const QString &currentAppVersion) {
  m_modelBase->SaveFileHeader(stream, currentAppVersion);
}

long CGUIBase::SavedItems() const {
  m_modelBase->SetChildModelLoadSave(new CChildModelLoadSave(const_cast<CGUIBase &>(*this)));

  return m_modelBase->SavedItems() + SavedItemsScenes();
}

void CGUIBase::LoadScenes(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &progress,
                          bool bIgnore) {
  if (m_b_GUI_disabled)
    return;

  IColorScaleEntry *m_pColorScaleEntry = dynamic_cast<IColorScaleEntry *>(model()->GraphEntry(MD_BASE_COLOR_SCALE));
  // There may already be scenes. Search and Destroy. But not the default scene...
  TSceneNodeEntry *pSceneEntry = (TSceneNodeEntry *)model()->GraphEntry(MD_BASE_SCENE_NODE);
  TSceneNodeEntry::TNodeSet scenes = pSceneEntry->EntryNodes();

  assert(scenes.size() == 0);

  // Load Scenes
  int iNrOfScenes;
  stream >> iNrOfScenes;
  for (int iCount = 0; iCount < iNrOfScenes; iCount++) {
    QString type;
    stream >> type;

    CReadOnlyScene readOnlyScene(type, *model());

    readOnlyScene.LoadStream(stream, version, progress);
  }

  // Insert unloaded scenes
  if (!bIgnore) {
    getDefaultOpenInventorSceneNode()->LoadStream(stream, version, progress);
  }
}

void CGUIBase::SaveScenes(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &progress) {
  if (m_b_GUI_disabled)
    return;

  // Save views
  assert(model()->GraphEntry(MD_BASE_SCENE_NODE));
  TSceneNodeEntry::TNodeSet stNode = ((TSceneNodeEntry *)model()->GraphEntry(MD_BASE_SCENE_NODE))->EntryNodes();
  int nCount = stNode.size();
  // wjrx mantis 2550 stream << nCount-1; // because we don't save the defaultscene
  //  because we DO save the defaultscene
  //  wjrx mantis 2550, save background color
  stream << nCount;

  assert(nCount == 0);

  for (TSceneNodeEntry::TNodeSet::iterator it = stNode.begin(); it != stNode.end(); it++, nCount++) {
    {
      COpenGLSceneBase *pBase;
      pBase = dynamic_cast<COpenGLSceneBase *>(*it);
      stream << pBase->GetTypeString();
      (*it)->SaveStream(stream, progress);
    }
  }
  getDefaultOpenInventorSceneNode()->SaveStream(stream, progress);
}

long CGUIBase::SavedItemsScenes() const {
  if (m_b_GUI_disabled)
    return 0;

  long nRet = 0;

  // Save views
  assert(model()->GraphEntry(MD_BASE_SCENE_NODE));
  TSceneNodeEntry::TNodeSet stNode = ((TSceneNodeEntry *)model()->GraphEntry(MD_BASE_SCENE_NODE))->EntryNodes();
  for (TSceneNodeEntry::TNodeSet::iterator it = stNode.begin(); it != stNode.end(); it++) {
    nRet += (*it)->SavedItems();
  }

  return nRet;
}

bool CGUIBase::CanSwitchTo() const { return m_modelBase->CanSwitchTo(); }

void CGUIBase::SwitchTo() {
  assert(GetGeomecDoc()->Model() != m_modelBase);
  assert(GetGeomecDoc()->GUI() != this);

  /*if( CMeshRefinementBoxDlg::instance()->is_show() )
    CMeshRefinementBoxDlg::instance()->reject();*/

  GetGeomecDoc()->OnSwitchModel(*m_modelBase, *this);
  m_modelBase->SwitchTo(GetGeomecDoc()->Model());
}

void CGUIBase::SwitchToParent() {
  assert(m_modelBase->parentModel());
  assert(GetGeomecDoc()->Model() == m_modelBase);
  assert(GetGeomecDoc()->GUI() == this);

  GetGeomecDoc()->OnSwitchModel(*m_modelBase->parentModel(), *parentGUI());
}

void CGUIBase::SetParentModelResults() {
  CAttriWellZoomInParentResultsDlg dlg(*m_modelBase, FemAppGetMainWnd());

  dlg.DoModal();
}

COpenInventorSceneNode &CGUIBase::OpenInventorScene() { return *getDefaultOpenInventorSceneNode(); }

///// CGUIBase::CChildModelLoadSave
CGUIBase::CChildModelLoadSave::CChildModelLoadSave(CGUIBase &gui) : m_gui(gui) {}

bool CGUIBase::CChildModelLoadSave::LoadModel(CStorageNode::TSTREAM &stream, CStreamVersion &version,
                                              CStorageNode::TPROGRESS &prog, CModelBase &childModel) {
  CGUIBase *pChildGUI = getChildGUI(childModel);

  if (pChildGUI)
    return pChildGUI->Load(stream, version, prog, false);

  // childModel not found
  assert(false);
  return false;
}

bool CGUIBase::CChildModelLoadSave::SaveModel(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &prog,
                                              CModelBase &childModel) {
  CGUIBase *pChildGUI = getChildGUI(childModel);

  if (pChildGUI)
    return pChildGUI->Save(stream, prog, false);

  // childModel not found
  assert(false);
  return false;
}

long CGUIBase::CChildModelLoadSave::SavedItems(CModelBase &childModel) const {
  const CGUIBase *pChildGUI = getChildGUI(childModel);

  if (pChildGUI)
    return pChildGUI->SavedItems();

  // childModel not found
  assert(false);
  return false;
}

bool CGUIBase::CChildModelLoadSave::SupportGMP() const { return true; }

CArchiveInterface *CGUIBase::CChildModelLoadSave::createCArchive(const QString &filename) const {
  return new CArchiveCArchive(filename); // caller owns the pointer
}

CGUIBase *CGUIBase::CChildModelLoadSave::getChildGUI(CModelBase &childModel) {
  int i;
  for (i = 0; i < m_gui.childGUISize(); ++i) {
    if (m_gui.childGUI(i).model() == &childModel) {
      CGUIBase &childGUI = static_cast<CGUIBase &>(m_gui.childGUI(i));
      return &childGUI;
    }
  }

  // not found
  return 0;
}

const CGUIBase *CGUIBase::CChildModelLoadSave::getChildGUI(CModelBase &childModel) const {
  return (const_cast<CChildModelLoadSave *>(this))->getChildGUI(childModel);
}

///// CGUIBase::CMainModelLoadSave
CGUIBase::CMainModelLoadSave::CMainModelLoadSave(CGUIBase *pGUI) : m_gui(pGUI), m_pVersionManager(0) {}

CGUIBase::CMainModelLoadSave::CMainModelLoadSave(const QString &appVersion, CGUIBase *pGUI)
    : m_gui(pGUI), m_pVersionManager(0) {
  SetAppVersion(appVersion);
}

bool CGUIBase::CMainModelLoadSave::LoadModel(CStorageNode::TSTREAM &stream, CStreamVersion &version,
                                             CStorageNode::TPROGRESS &prog, CModelBase &model) {
  assert(!m_gui);
  assert(m_pVersionManager);

  m_gui = CGUIFactory::createGUI(0, &model);

  m_gui->NewModel(false);
  m_gui->LinkModelToOpenInventor();

  return m_gui->Load(stream, m_pVersionManager->FileVersion(), prog, true);
}

bool CGUIBase::CMainModelLoadSave::SaveModel(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &prog,
                                             CModelBase &model) {
  assert(m_gui);

  stream.PushDataSet(CHDF5File::ModelGroup(model.Index()) + "ModelStream");

  m_gui->SaveFileHeader(stream, GetAppVersion());
  bool retval = m_gui->Save(stream, prog, true);

  stream.PopDataSet();

  return retval;
}

long CGUIBase::CMainModelLoadSave::SavedItems(CModelBase &model) const {
  assert(m_gui);
  return m_gui->SavedItems();
}

void CGUIBase::CMainModelLoadSave::SetVersionManager(CVersionManager &versionManager) {
  m_pVersionManager = &versionManager;
}

bool CGUIBase::CMainModelLoadSave::SupportGMP() const { return true; }

CArchiveInterface *CGUIBase::CMainModelLoadSave::createCArchive(const QString &filename) const {
  return new CArchiveCArchive(filename); // caller owns the pointer
}

CGUIBase *CGUIBase::CMainModelLoadSave::GetGUI() const { return m_gui; }

COpenInventorSceneNode *CGUIBase::getDefaultOpenInventorSceneNode() const { return m_pOpenInventorScene; }

void CGUIBase::LinkModelToOpenInventor() {
  if (m_b_GUI_disabled)
    return;

  getDefaultOpenInventorSceneNode()->SetMainMeshNode();
}
