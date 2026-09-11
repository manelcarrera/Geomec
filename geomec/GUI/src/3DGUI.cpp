#include "stdafx.h"

#include "3DGUI.h"
#include "3dmodel.h"
#include "FemAppEntryTypes.h"
#include "OIV2DHistoryScene.h"
#include "OIV2DWellPathGraphScene.h"
#include "openglscenenode.h"
#include "resource.h"
#include "resourceIDS.h"

C3DGUI::C3DGUI(CFemAppGUI *parentGUI, C3DModel *a3DModel)
    : CGUIBase(parentGUI, a3DModel), m_3DModel(a3DModel), m_oiv2DWellPathGraphScene(0), m_oiv2DHistoryScene(0) {}

void C3DGUI::OnCloseModel() {
  delete m_oiv2DHistoryScene;
  m_oiv2DHistoryScene = 0;

  delete m_oiv2DWellPathGraphScene;
  m_oiv2DWellPathGraphScene = 0;

  CGUIBase::OnCloseModel();
}

C3DModel *C3DGUI::model() const { return m_3DModel; }

void C3DGUI::OnCreateSceneEntry() {
  if (m_b_GUI_disabled)
    return;

  CGUIBase::OnCreateSceneEntry();

  TSceneNodeEntry *pEntry = (TSceneNodeEntry *)m_3DModel->GraphEntry(MD_BASE_SCENE_NODE);

  m_oiv2DWellPathGraphScene = new OIV2DWellPathGraphScene(*m_3DModel);
  m_oiv2DWellPathGraphScene->LinkTo(*pEntry);

  m_oiv2DHistoryScene = new OIV2DHistoryScene(*m_3DModel);
  m_oiv2DHistoryScene->LinkTo(*pEntry);
}

OIV2DWellPathGraphScene *C3DGUI::getOIV2DWellPathGraphScene() const { return m_oiv2DWellPathGraphScene; }

OIV2DHistoryScene *C3DGUI::getOIV2DHistoryScene() const { return m_oiv2DHistoryScene; }

void C3DGUI::LoadScenes(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &progress,
                        bool bIgnore) {
  if (m_b_GUI_disabled)
    return;

  CGUIBase::LoadScenes(stream, version, progress, bIgnore);

  if (version > CStreamVersion(4, 1, 52)) {
    if (!bIgnore) {
      getOIV2DWellPathGraphScene()->LoadStream(stream, version, progress);
      getOIV2DHistoryScene()->LoadStream(stream, version, progress);
    }
  }
}

void C3DGUI::SaveScenes(CStorageNode::TSTREAM &stream, CStorageNode::TPROGRESS &progress) {
  if (m_b_GUI_disabled)
    return;

  CGUIBase::SaveScenes(stream, progress);
  getOIV2DWellPathGraphScene()->SaveStream(stream, progress);
  getOIV2DHistoryScene()->SaveStream(stream, progress);
}
