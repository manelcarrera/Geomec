#include "SaveModelTest.h"
#include "ModelLifetimeFacade.h"

CSaveModelTest::CSaveModelTest(CModelBase &model, const QString &sAppVersion)
    : ISaveModel(), m_model(model), m_sAppVersion(sAppVersion) {}

CSaveModelTest::~CSaveModelTest() {}

bool CSaveModelTest::saveModel(const QString &fileName, bool bSwitch) {
  return IModelLifetimeFacade::SaveMain(CModelBase::CModelLoadSaveDefault(m_sAppVersion), m_model, fileName, bSwitch);
}
