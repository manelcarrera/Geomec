#include "stdafx.h"

#include "ISceneWrapper.h"
#include "OIV2DHistorySceneObserver.h"

OIV2DHistorySceneObserver::OIV2DHistorySceneObserver(OIV2DHistoryScene &node, CTreeNode &parent, const BOOL rename,
                                                     const REMOVE_TYPE remove, HTREEITEM hInsertAfter)
    : TOIV2DHistorySceneObserver(node, parent, rename, remove, hInsertAfter) {}

unsigned int OIV2DHistorySceneObserver::Icon() const {
  ISceneWrapper *sceneWrapper = GetGeomecDoc()->CurrentScene();
  return (sceneWrapper != 0 && sceneWrapper->IsOIV2DHistoryScene()) ? IDI_RADIO_CHECKED : IDI_RADIO_UNCHECKED;
}
