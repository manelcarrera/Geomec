#include "stdafx.h"

#include "ISceneWrapper.h"
#include "OpenInventorSceneNodeObserver.h"

COpenInventorSceneNodeObserver::COpenInventorSceneNodeObserver(COpenInventorSceneNode &node, CTreeNode &parent,
                                                               const BOOL rename, const REMOVE_TYPE remove,
                                                               HTREEITEM hInsertAfter)
    : TOpenInventorSceneNodeObserver(node, parent, rename, remove, hInsertAfter) {}

unsigned int COpenInventorSceneNodeObserver::Icon() const {
  if (GetGeomecDoc()->CurrentScene() && GetGeomecDoc()->CurrentScene()->IsOpenInventorScene())
    return IDI_RADIO_CHECKED;
  return IDI_RADIO_UNCHECKED;
}
