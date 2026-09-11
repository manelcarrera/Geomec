#include "stdafx.h"

#include "DocumentProxyGUI.h"
#include "FemAppEntryTypes.h"
#include "FemAppGUI.h"
#include "GUIBase.h"
#include "GUIFactory.h"
#include "GeomecDoc.h"
#include "ModelProxyGUI.h"
#include "openglscenebase.h"
#include "openglscenenode.h"

#include "ISettings.h"

namespace {

const QString INSTANCE_NAME = "CFemAppGUI";

} // anonymous namespace

// CFemAppGUI

CFemAppGUI::CFemAppGUI(CFemAppGUI *parentGUI, CFemAppModel *femAppModel)
    : CDocumentGUI(femAppModel), m_femAppModel(femAppModel), m_observer(0), m_childGUI(), m_parentGUI(parentGUI),
      m_b_GUI_disabled(false) {
  m_observer = new Observer(*this, INSTANCE_NAME);
  m_femAppModel->LinkTo(*m_observer);
  m_femAppModel->setModelProxy(new CModelProxyGUI(*GetGeomecDoc(), *m_femAppModel, *this));
  m_femAppModel->setDocumentProxy(new CDocumentProxyGUI(*GetGeomecDoc()));

  QString val = ISettings::instance()->getProfileString("Debug", "DISABLE_GUI").toUpper();
  if (val == "Y")
    m_b_GUI_disabled = true;
}

CFemAppGUI::~CFemAppGUI() {
  while (!m_childGUI.empty()) {
    CFemAppGUI *femAppGUI = m_childGUI.back();

    m_childGUI.pop_back();
    delete femAppGUI;
  }

  delete m_observer;
}

void CFemAppGUI::ModelProperties() {
  // Default nothing implemented ...
}

int CFemAppGUI::childGUISize() const { return m_childGUI.size(); }

CFemAppGUI &CFemAppGUI::childGUI(int index) { return *m_childGUI[index]; }

CFemAppGUI *CFemAppGUI::parentGUI() { return m_parentGUI; }

void CFemAppGUI::modelDeleted() {
  if (m_parentGUI) {
    m_parentGUI->removeChildGUI(this);
  }

  delete this;
}

void CFemAppGUI::removeChildGUI(CFemAppGUI *femAppGUI) {
  for (size_t i = 0; i < m_childGUI.size(); ++i) {
    if (m_childGUI[i] == femAppGUI) {
      m_childGUI.erase(m_childGUI.begin() + i);
      break;
    }
  }
}

bool CFemAppGUI::NewModel(bool bAttachToDocument) {
  m_femAppModel->NewModel(bAttachToDocument);

  // The creation of the default scene depends on actions executed in the call
  // to NewModel (OnCreateSceneEntry())!

  createDefaults();

  // Fire an update to all views ...

  if (bAttachToDocument) {
    GetGeomecDoc()->OnAttachModel(*m_femAppModel, *this);
  }

  return true;
}

void CFemAppGUI::OnCreateSceneEntry() { new TSceneNodeEntry(MD_BASE_SCENE_NODE, 0, 0, *m_femAppModel); }

void CFemAppGUI::createDefaults() {}

void CFemAppGUI::OnCloseModel() {
  DeleteScenes();

  m_femAppModel->OnCloseModel();
}

void CFemAppGUI::DeleteScenes() {
  if (m_b_GUI_disabled)
    return;

  // Delete all scenes so no drawing information is gathered while closing

  assert(m_femAppModel->GraphEntry(MD_BASE_SCENE_NODE));

  TSceneNodeEntry::TNodeSet scenes;

  scenes = ((TSceneNodeEntry *)m_femAppModel->GraphEntry(MD_BASE_SCENE_NODE))->EntryNodes();

  for (TSceneNodeEntry::TNodeSet::iterator iter = scenes.begin(); iter != scenes.end(); iter++) {
    delete *iter;
  }
}

CFemAppModel *CFemAppGUI::model() const { return m_femAppModel; }

// CFemAppGUI::Observer

CFemAppGUI::Observer::Observer(CFemAppGUI &femAppGUI, const QString &strInstanceName)
    : CGraphNode(strInstanceName), m_femAppGUI(femAppGUI) {}

CFemAppGUI::Observer::Observer(CFemAppGUI &femAppGUI, unsigned int uInstanceName)
    : CGraphNode(uInstanceName), m_femAppGUI(femAppGUI) {}

void CFemAppGUI::Observer::OnNewNeighbour(const CGraphNode &node) {
  // This function serves no purpose yet.
}

void CFemAppGUI::Observer::OnNeighbourModified(const CGraphNode &node, enum ModifiedHint uHint) {
  CFemAppModel *femAppModel = const_cast<CFemAppModel *>(dynamic_cast<const CFemAppModel *>(&node));

  if (femAppModel) {
    int femAppModelSize = femAppModel->childModelSize();
    int femAppGUISize = m_femAppGUI.childGUISize();

    if (femAppModelSize > femAppGUISize) {
      int i = 0;

      for (; i < femAppModelSize; ++i) {
        int j = 0;

        for (; j < femAppGUISize; ++j) {
          if (&femAppModel->childModel(i) == m_femAppGUI.childGUI(j).model()) {
            break;
          }
        }

        if (j == femAppGUISize) {
          CFemAppGUI *femAppGUI = CGUIFactory::createGUI(&m_femAppGUI, &femAppModel->childModel(i));

          m_femAppGUI.m_childGUI.push_back(femAppGUI);
        }
      }
    }
  }
}

void CFemAppGUI::Observer::OnNeighbourDeleted(const CGraphNode &node) {
  if (&node == m_femAppGUI.model()) {
    m_femAppGUI.modelDeleted();
  }
}

unsigned int CFemAppGUI::Observer::IconId() const { return 0; }

unsigned int CFemAppGUI::Observer::TypeId() const { return 0; }
