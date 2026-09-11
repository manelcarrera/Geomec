#include "stdafx.h"

#include "GeomecDoc.h"
#include "IWellGUI.h"
#include "IWellModel.h"
#include "ModelProxyGUI.h"
#include "resource.h"
#include "resourceIDS.h"

IWellGUI::IWellGUI(CFemAppGUI *parentGUI, IWellModel *wellModel)
    : CGUIBase(parentGUI, wellModel), m_wellModel(wellModel) {
  m_wellModel->setModelProxy(new CModelProxyGUI(*GetGeomecDoc(), *m_wellModel, *this));
}

void IWellGUI::OnCloseModel() { CGUIBase::OnCloseModel(); }

IWellModel *IWellGUI::model() const { return m_wellModel; }
