#pragma once

#include "GMSkuaConnectorExport.h"

namespace gm_skua {

class INTERFACE_GMSKUACONNECTOR IProgressHandler {
public:
  IProgressHandler() {}
  virtual ~IProgressHandler() {}

  virtual bool Step(int number) { return true; } // returning false works as 'cancel'
};

} // namespace gm_skua
