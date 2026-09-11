#ifndef _OIV2DWellPathGraphSceneObserver_h_
#define _OIV2DWellPathGraphSceneObserver_h_

#include "NodeObserver_Delegate.h"
#include "OIV2DWellPathGraphScene.h"
#include "OIV2DWellPathGraphScene_Delegate.h"

typedef CNodeObserver_Delegate<OIV2DWellPathGraphScene, OIV2DWellPathGraphScene_Delegate, CDummyNode, CDummyObserver,
                               FALSE, FIXED_ITEM>
    TOIV2DWellPathGraphSceneObserver;

class OIV2DWellPathGraphSceneObserver : public TOIV2DWellPathGraphSceneObserver {
public:
  OIV2DWellPathGraphSceneObserver(OIV2DWellPathGraphScene &node, CTreeNode &parent, const BOOL rename,
                                  const REMOVE_TYPE remove, HTREEITEM hInsertAfter = TVI_LAST);

  virtual unsigned int Icon() const;
};

#endif // _OIV2DWellPathGraphSceneObserver_h_
