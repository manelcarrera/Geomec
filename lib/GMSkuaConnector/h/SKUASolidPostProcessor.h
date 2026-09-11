#pragma once

#include "SKUAIPostProcessor.h"


namespace gm_skua
{

class INTERFACE_GMSKUACONNECTOR SKUASolidPostProcessor : public SKUAIPostProcessor
{
public:
  using SKUAIPostProcessor::SKUAIPostProcessor;

  void PostProcess();

private:
  void FixVolumeReferences();
  void FixSurfaceReferences();
  void FixCollectBodyTrianglesFromTetras();
  void FixSurfaceBodyTriangles();
  void FixSurfaceTypes();
  void FixSurfaceTopBottom();
  void FixVolumeTypes();
  void FixSharedVertices();
};

}
