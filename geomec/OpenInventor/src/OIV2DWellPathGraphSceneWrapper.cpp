#include "stdafx.h"

#include "OIV2DWellPathGraphSceneWrapper.h"

OIV2DWellPathGraphSceneWrapper::OIV2DWellPathGraphSceneWrapper( OIV2DScene& scene ) : OIV2DSceneWrapper(scene)
{
}

bool OIV2DWellPathGraphSceneWrapper::IsOIV2DWellPathGraphScene() const
{
  return true;
}
