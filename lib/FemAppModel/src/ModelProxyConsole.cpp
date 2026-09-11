
#include "ModelProxyConsole.h"
#include "FemAppModel.h"

CModelProxyConsole::CModelProxyConsole(CFemAppModel &model) : CModelProxy(), m_model(model) {}

CModelProxyConsole::~CModelProxyConsole() {}

void CModelProxyConsole::createContainers() {}

void CModelProxyConsole::detachModel() {}

void CModelProxyConsole::onCloseModel() { m_model.OnCloseModel(); }

void CModelProxyConsole::switchToParent() {}
