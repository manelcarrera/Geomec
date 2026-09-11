#ifndef _UPSCALINGBRANCHOBSERVER_H_
#define _UPSCALINGBRANCHOBSERVER_H_

class CFormationBase;

#include "MeshSampler.h"

class CModelBase;
class C3DModel;
class CUpscalingFormationsBranch;
class CUpscalingPointsetRangesBranch;
class CUpscalingTargetPointsetBranch;

#include "DeletableNodeObserver_Delegate.h"
#include "StateBranch_Delegate.h"
#include "UndefinedIconProvider.h"
#include "Upscaling_Delegate.h"

typedef CDeletableNodeObserver_Delegate<IUpscalingNodeBase, IUpscalingNodeBase_Delegate, CGraphNode, CDummyObserver,
                                        FALSE, FIXED_ITEM>
    TUpscalingObserver;
class CUpscalingObserver : public TUpscalingObserver {
public:
  CUpscalingObserver(IUpscalingNodeBase &node, CTreeNode &parent, const BOOL rename, const enum REMOVE_TYPE remove,
                     HTREEITEM hInsertAfter = TVI_LAST, BOOL bInitialUpdate = TRUE, BOOL bSortAfterInsertion = TRUE);

  virtual BOOL OnFilter(const CGraphNode &t) const;
  virtual CTreeNode *InsertChild(CGraphNode &t);
};

typedef CStateBranch_Delegate<CUpscalingEntry, CUpscalingEntry_Delegate, CGraphNode, CDummyObserver,
                              CUndefinedIconProvider, TRUE, DELETE_ITEM>
    TUpscalingBranchBase;

class CUpscalingBranch : public TUpscalingBranchBase {
public:
  CUpscalingBranch(ITreeObject &parent, CModelBase &model);
  virtual BOOL OnFilter(const CGraphNode &t) const;
  virtual CTreeNode *InsertChild(CGraphNode &t);
};

#endif // _UPSCALINGBRANCHOBSERVER_H_
