
#include "NewModel.h"
#include "FemAppModel.h"

void CNewModel::setModel(CFemAppModel* femAppModel)
{
  m_modelConsole = femAppModel;
  setNewModelFunction(&CNewModel::newModelConsole);
}

bool CNewModel::newModel(bool attachToDocument)
{
  return (this->*m_newModelFunc)(attachToDocument);
}

bool CNewModel::newModelConsole(bool attachToDocument)
{
  return m_modelConsole->NewModel(attachToDocument);
}

void CNewModel::setNewModelFunction(TNewModelFunc func)
{
  m_newModelFunc = func;
}
