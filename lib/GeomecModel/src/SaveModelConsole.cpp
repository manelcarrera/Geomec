
#include "SaveModelConsole.h"
#include "ModelLifetimeFacade.h"

CSaveModelConsole::CSaveModelConsole(CModelBase &model, const QString &sAppVersion)
    : ISaveModel(), m_model(model), m_sAppVersion(sAppVersion) {}

CSaveModelConsole::~CSaveModelConsole() {}

bool CSaveModelConsole::saveModel(const QString &fileName, bool bSwitch) {
  return IModelLifetimeFacade::SaveMain(CModelBase::CModelLoadSaveDefault(m_sAppVersion), m_model, fileName, bSwitch);
}
