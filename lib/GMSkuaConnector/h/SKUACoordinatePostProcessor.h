#pragma once

#include "SKUAIPostProcessor.h"


namespace gm_skua
{

class INTERFACE_GMSKUACONNECTOR SKUACoordinatePostProcessor : public SKUAIPostProcessor
{
public:
  using SKUAIPostProcessor::SKUAIPostProcessor;

  void PostProcess();

private:
  void FixCoordinates();
};

}

