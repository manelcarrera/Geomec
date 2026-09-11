
#include <memory>

#include "ISettings.h"
#include "RegistryCleanupWrapper.h"

CRegistryCleanupWrapper::CRegistryCleanupWrapper() {}

CRegistryCleanupWrapper::~CRegistryCleanupWrapper() { ISettings::cleanup(); }
