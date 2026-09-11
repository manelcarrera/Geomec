
#include <memory>

#include "ApplicationInitialization.h"
#include "Platform.h"
#include "RegistryCleanupWrapper.h"
#include "Settings.h"
#include "SettingsFile.h"

static std::auto_ptr<CRegistryCleanupWrapper> registryCleanupWrapper(new CRegistryCleanupWrapper());

CApplicationInitialization::CApplicationInitialization(
    // #ifdef WIN32
    const QString &applicationName, const QString &companyName,
    // #else // !WIN32
    //   const QString& applicationName, const QString& /*companyName*/,
    // #endif  // WIN32
    const QString &fileName)
    : m_applicationName(applicationName) {
  CPlatform::init();

  // #ifdef WIN32
  CSettings::init(companyName, applicationName);
  // #else // !WIN32
  //   CSettingsFile::init(fileName);
  // #endif  // WIN32
}

CApplicationInitialization::~CApplicationInitialization() { delete CPlatform::instance(); }

const QString &CApplicationInitialization::applicationName() const { return m_applicationName; }
