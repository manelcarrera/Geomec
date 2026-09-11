#ifndef _OIVSCENEOBSERVER_H_
#define _OIVSCENEOBSERVER_H_

#include "Geomec.h"
#include "GraphEntry_Delegate.h"
#include "ISceneWrapper.h"
#include "NodeObserver_Delegate.h"
#include "OIVSceneBase.h"
#include "OIVSceneBase_Delegate.h"

typedef CGraphEntryTemp<COIVSceneBase> TOIVSceneBaseEntry;
typedef CGraphEntryTemp_Delegate<COIVSceneBase> TOIVSceneBaseEntry_Delegate;
typedef CNodeObserver_Delegate<COIVSceneBase, COIVSceneBase_Delegate, CDummyNode, CDummyObserver, FALSE, FIXED_ITEM>
    TOIVSceneObserver;

class COIVSceneObserver : public TOIVSceneObserver {
public:
  COIVSceneObserver(COIVSceneBase &scene, CTreeNode &parent, const BOOL rename, const REMOVE_TYPE remove,
                    HTREEITEM hInsertAfter = TVI_LAST)
      : TOIVSceneObserver(scene, parent, rename, remove, hInsertAfter) {}

  virtual unsigned int Icon() const {
    CGeomecDoc *pDoc = &((CGeomecApp *)AfxGetApp())->GetDoc();
    if (pDoc->CurrentScene() && (&ObservedItem() == pDoc->CurrentScene()->OpenGLScene() ||
                                 &ObservedItem() == pDoc->CurrentScene()->OpenInventorScene() ||
                                 &ObservedItem() == pDoc->CurrentScene()->getOIV2DScene()))
      return IDI_RADIO_CHECKED;
    return IDI_RADIO_UNCHECKED;
  }
};

typedef CNodeObserver_Delegate<TOIVSceneBaseEntry, TOIVSceneBaseEntry_Delegate, COIVSceneBase, COIVSceneObserver, FALSE,
                               FIXED_ITEM>
    TOIVSceneEntryObserver;

#endif // _OIVSCENEOBSERVER_H_
