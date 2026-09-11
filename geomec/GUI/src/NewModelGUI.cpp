#include "stdafx.h"

#include "FemAppGUI.h"
#include "NewModel.h"

void CNewModel::setModel(CFemAppGUI *femAppGUI) {
  m_modelGUI = femAppGUI;
  setNewModelFunction(&CNewModel::newModelGUI);
}

bool CNewModel::newModelGUI(bool attachToDocument) { return m_modelGUI->NewModel(attachToDocument); }
