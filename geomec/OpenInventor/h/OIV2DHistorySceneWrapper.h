#ifndef _OIV2DHistorySceneWrapper_h_
#define _OIV2DHistorySceneWrapper_h_

#include "OIV2DSceneWrapper.h"

class OIV2DHistorySceneWrapper : public OIV2DSceneWrapper {

public:
  explicit OIV2DHistorySceneWrapper(OIV2DScene &scene);

  virtual bool IsOIV2DHistoryScene() const;
};

#endif // _OIV2DHistorySceneWrapper_h_
