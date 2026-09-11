#ifndef _OIV2DHistorySceneObserver_h_
#define _OIV2DHistorySceneObserver_h_

#include "NodeObserver_Delegate.h"
#include "OIV2DHistoryScene.h"
#include "OIV2DHistoryScene_Delegate.h"

typedef CNodeObserver_Delegate<OIV2DHistoryScene, OIV2DHistoryScene_Delegate, CDummyNode, CDummyObserver, FALSE,
                               FIXED_ITEM>
    TOIV2DHistorySceneObserver;

class OIV2DHistorySceneObserver : public TOIV2DHistorySceneObserver {
public:
  OIV2DHistorySceneObserver(OIV2DHistoryScene &node, CTreeNode &parent, const BOOL rename, const REMOVE_TYPE remove,
                            HTREEITEM hInsertAfter = TVI_LAST);

  virtual unsigned int Icon() const;
};

#endif // _OIV2DHistorySceneObserver_h_
