
#include <memory>

#include "RegistryCleanupWrapper.h"
#include "ISettings.h"

CRegistryCleanupWrapper::CRegistryCleanupWrapper()
{
}

CRegistryCleanupWrapper::~CRegistryCleanupWrapper()
{
  ISettings::cleanup();
}
