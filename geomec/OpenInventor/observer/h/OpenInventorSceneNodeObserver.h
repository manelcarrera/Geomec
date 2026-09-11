#ifndef _OpenInventorSceneNodeObserver_h_
#define _OpenInventorSceneNodeObserver_h_

#include "NodeObserver_Delegate.h"
#include "OpenInventorSceneNode.h"
#include "OpenInventorSceneNode_Delegate.h"

typedef CNodeObserver_Delegate<COpenInventorSceneNode, COpenInventorSceneNode_Delegate, CDummyNode, CDummyObserver,
                               FALSE, FIXED_ITEM>
    TOpenInventorSceneNodeObserver;

class COpenInventorSceneNodeObserver : public TOpenInventorSceneNodeObserver {
public:
  COpenInventorSceneNodeObserver(COpenInventorSceneNode &node, CTreeNode &parent, const BOOL rename,
                                 const REMOVE_TYPE remove, HTREEITEM hInsertAfter = TVI_LAST);

  virtual unsigned int Icon() const;
};

#endif // _OpenInventorSceneNodeObserver_h_
