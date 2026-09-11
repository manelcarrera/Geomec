#include "stdafx.h"

#include "OIV2DHistorySceneWrapper.h"

OIV2DHistorySceneWrapper::OIV2DHistorySceneWrapper( OIV2DScene& scene ) : OIV2DSceneWrapper(scene)
{
}

bool OIV2DHistorySceneWrapper::IsOIV2DHistoryScene() const
{
  return true;
}
