#include "stdafx.h"

#include "ModelProxyGUI.h"
#include "IWellGUI.h"
#include "FemAppEntryTypes.h"
#include "FemAppDoc.h"

CModelProxyGUI::CModelProxyGUI(CFemAppDoc& document, CFemAppModel& model,
  CFemAppGUI& gui)
: CModelProxy()
, m_document(document)
, m_model(model)
, m_gui(gui)
{
}

CModelProxyGUI::~CModelProxyGUI()
{
}

void CModelProxyGUI::createContainers()
{
  m_gui.OnCreateSceneEntry();
  assert(m_model.GraphEntry(MD_BASE_SCENE_NODE));
}

void CModelProxyGUI::detachModel()
{
  m_document.OnDetachModel(m_model, m_gui);
}

void CModelProxyGUI::onCloseModel()
{
  m_gui.OnCloseModel();
}

void CModelProxyGUI::switchToParent()
{
  dynamic_cast <IWellGUI&> (m_gui).SwitchToParent();
}
