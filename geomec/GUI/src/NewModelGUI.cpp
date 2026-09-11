#include "stdafx.h"

#include "NewModel.h"
#include "FemAppGUI.h"

void CNewModel::setModel(CFemAppGUI* femAppGUI)
{
  m_modelGUI = femAppGUI;
  setNewModelFunction(&CNewModel::newModelGUI);
}

bool CNewModel::newModelGUI(bool attachToDocument)
{
  return m_modelGUI->NewModel(attachToDocument);
}
