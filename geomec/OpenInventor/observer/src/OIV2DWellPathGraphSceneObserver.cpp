#include "stdafx.h"

#include "OIV2DWellPathGraphSceneObserver.h"
#include "ISceneWrapper.h"

OIV2DWellPathGraphSceneObserver::OIV2DWellPathGraphSceneObserver(
    OIV2DWellPathGraphScene& node,
    CTreeNode& parent,
    const BOOL rename,
    const REMOVE_TYPE remove,
    HTREEITEM hInsertAfter)
    : TOIV2DWellPathGraphSceneObserver(node, parent, rename, remove, hInsertAfter)
{
}

unsigned int OIV2DWellPathGraphSceneObserver::Icon() const
{
    ISceneWrapper* sceneWrapper = GetGeomecDoc()->CurrentScene();
    return (sceneWrapper != 0 && sceneWrapper->IsOIV2DWellPathGraphScene())
        ? IDI_RADIO_CHECKED
        : IDI_RADIO_UNCHECKED;
}
