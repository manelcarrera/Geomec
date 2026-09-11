#ifndef _OIV2DWellPathGraphSceneWrapper_h_
#define _OIV2DWellPathGraphSceneWrapper_h_

#include "OIV2DSceneWrapper.h"

class OIV2DWellPathGraphSceneWrapper : public OIV2DSceneWrapper
{

public:

  explicit OIV2DWellPathGraphSceneWrapper(OIV2DScene& scene);

  virtual bool IsOIV2DWellPathGraphScene() const;
};

#endif  // _OIV2DWellPathGraphSceneWrapper_h_
