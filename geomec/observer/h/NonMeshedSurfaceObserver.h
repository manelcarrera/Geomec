#ifndef _NONMESHEDSURFACEOBSERVER_H_
#define _NONMESHEDSURFACEOBSERVER_H_

#include "NonMeshedSurface.h"
#include "NonMeshedSurface_Delegate.h"
#include "NonMeshedSurfaceParametersNode.h"
#include "NonMeshedSurfaceParametersNode_Delegate.h"
#include "NonMeshedSurfacePressure.h"
#include "NonMeshedSurfacePressure_Delegate.h"

typedef CNodeObserver_Delegate<CNonMeshedSurfaceParametersNode, CNonMeshedSurfaceParametersNode_Delegate, CValueType, CValueCompositeObserver, FALSE, UNLINK_ITEM> TNonMeshedSurfaceParametersNodeObserver;
//typedef CNodeObserver_Delegate<CNonMeshedSurfacePressure, CNonMeshedSurfacePressure_Delegate, CValueType, CValueCompositeObserver, FALSE, UNLINK_ITEM> TNonMeshedSurfacePressureObserver;
typedef COpenGLNodeObserver_Delegate<CNonMeshedSurface, CNonMeshedSurface_Delegate, CGraphNode, CDummyObserver, TRUE, DELETE_ITEM> TNonMeshedSurfaceObserver;

class CNonMeshedSurfaceObserver : public TNonMeshedSurfaceObserver
{
public:
  CNonMeshedSurfaceObserver(CNonMeshedSurface& node,
                            CTreeNode& parent,
                            const BOOL rename,
                            const enum REMOVE_TYPE remove,
                            HTREEITEM hInsertAfter = TVI_LAST,
                            const BOOL bInitialUpdate = TRUE)
                            : TNonMeshedSurfaceObserver(node, parent, rename, remove, hInsertAfter, FALSE)
  {
    if(bInitialUpdate)
      Update();
  }

  CNonMeshedSurfaceObserver(CNonMeshedSurface& node,
                            CTreeCtrl& ctrl,
                            const BOOL rename,
                            const enum REMOVE_TYPE remove,
                            HTREEITEM hParent = TVI_ROOT,
                            HTREEITEM hInsertAfter = TVI_LAST,
                            const BOOL bInitialUpdate = TRUE)
                            : TNonMeshedSurfaceObserver(node, ctrl, rename, remove, hParent, hInsertAfter, FALSE)
  {
    if(bInitialUpdate)
      Update();
  }

  virtual BOOL OnFilter(const CGraphNode& node) const
  {
    return (dynamic_cast<const CNonMeshedSurfaceParametersNode*>(&node) != 0 ||
            dynamic_cast<const CNonMeshedSurfacePressure*>(&node) != 0);
  }

  virtual CTreeNode* InsertChild(CGraphNode& t)
  {
//    CTreeNode* pTreeNode = TNonMeshedSurfaceObserver::InsertChild(t);

    CNonMeshedSurfaceParametersNode* pNode = dynamic_cast<CNonMeshedSurfaceParametersNode*>(&t);
    CNonMeshedSurfacePressure* pPressure = dynamic_cast<CNonMeshedSurfacePressure*>(&t);

    if(pNode)
      return new TNonMeshedSurfaceParametersNodeObserver(*pNode, *this, FALSE, FIXED_ITEM);
    else if(pPressure)
      return new CValueCompositeObserver(*pPressure, *this, FALSE, FIXED_ITEM);
//      return new TNonMeshedSurfacePressureObserver(*pPressure, *this, FALSE, FIXED_ITEM);

    assert(false);
    return 0;
  }
};

typedef CStateBranch_Delegate<CNonMeshedSurfaceEntry, CNonMeshedSurfaceEntry_Delegate, CNonMeshedSurface, CNonMeshedSurfaceObserver, CUndefinedIconProvider, TRUE, DELETE_ITEM> TNonMeshedSurfaceEntryObserver;

#endif // _NONMESHEDSURFACEOBSERVER_H_
