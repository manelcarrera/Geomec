#include "stdafx.h"
#include "Geomec.h"
#include "Upscaling.h"
#include "ThinLayerUpscaling.h"
#include "CamClayUpscaling.h"
#include "3dmodel.h"
#include "BaseEntryTypes.h"
#include "FormationBase.h"
#include "HexaModel.h"
#include "PointSet.h"
#include "GeoSurface.h"
#include "ThinLayerUpscalingCreateTargetPointset.h"
#include "HexaMesh.h"
#include "GeoProgress.h"
#include "PointSet_Delegate.h"
#include "FormationBase_Delegate.h"
#include "MeshSampler_Delegate.h"
#include "UpscalingObserver.h"

CUpscalingObserver::CUpscalingObserver(IUpscalingNodeBase& node,
                                       CTreeNode &parent,
                                       const BOOL rename,
                                       const enum REMOVE_TYPE remove,
                                       HTREEITEM hInsertAfter,
                                       BOOL bInitialUpdate,
                                       BOOL bSortAfterInsertion)
: TUpscalingObserver(node, parent, rename, remove, hInsertAfter, bInitialUpdate, bSortAfterInsertion)
{
}

BOOL CUpscalingObserver::OnFilter(const CGraphNode& t) const
{
  return (dynamic_cast<const CUpscalingFormationsBranch*>  (&t) != 0 ||
          dynamic_cast<const IUpscalingPointsetBranchBase*>(&t) != 0);
}

CTreeNode* CUpscalingObserver::InsertChild(CGraphNode& t)
{
  CUpscalingFormationsBranch* pFormationsBranch = dynamic_cast<CUpscalingFormationsBranch*>(&t);
  if(pFormationsBranch)
  {
    typedef CNodeObserver_Delegate<CFormationBase, CFormationBase_Delegate, CDummyNode, CDummyObserver, FALSE, UNLINK_ITEM> TFormationObserver;
    typedef CNodeObserver_Delegate<CUpscalingFormationsBranch, CUpscalingFormationsBranch_Delegate, CFormationBase, TFormationObserver, FALSE, UNLINK_ITEM> TFormationBranchObserver;
    return new TFormationBranchObserver(*pFormationsBranch,
                                        *this,
                                        FALSE,
                                        FIXED_ITEM,
                                        (HTREEITEM)-1,
                                        true,
                                        false);
  }

  CUpscalingPointsetRangesBranch* pPointsetRangesBranch = dynamic_cast<CUpscalingPointsetRangesBranch*>(&t);
  if(pPointsetRangesBranch)
  {
    typedef CNodeObserver_Delegate<CPointSet, CPointSet_Delegate, CDummyNode, CDummyObserver, FALSE, UNLINK_ITEM> TPointsetObserver;
    typedef CNodeObserver_Delegate<CUpscalingPointsetRangesBranch, CUpscalingPointsetRangesBranch_Delegate, CPointSet, TPointsetObserver, FALSE, UNLINK_ITEM> TUpscalingPointsetBranchObs;
    return new TUpscalingPointsetBranchObs(*pPointsetRangesBranch,
                                                    *this,
                                                    FALSE,
                                                    FIXED_ITEM,
                                                    (HTREEITEM)-1,
                                                    true,
                                                    false);
  }

  CUpscalingTargetPointsetBranch* pTargetPointsetBranch = dynamic_cast<CUpscalingTargetPointsetBranch*>(&t);
  if(pTargetPointsetBranch)
  {
    typedef CNodeObserver_Delegate<CPillarMap, CPillarMap_Delegate, CDummyNode, CDummyObserver, FALSE, UNLINK_ITEM> TPillarMapObserver;
    typedef CNodeObserver_Delegate<CUpscalingTargetPointsetBranch, CUpscalingTargetPointsetBranch_Delegate, CPillarMap, TPillarMapObserver, FALSE, DELETE_ITEM> TUpscalingTargetPointsetBranchObs;
    return new TUpscalingTargetPointsetBranchObs(*pTargetPointsetBranch,
                                                          *this,
                                                          FALSE,
                                                          FIXED_ITEM,
                                                          (HTREEITEM)-1,
                                                          true,
                                                          false);
  }

  assert(FALSE);
  return 0;
}

/////

CUpscalingBranch::CUpscalingBranch(ITreeObject& parent, CModelBase& model)
: TUpscalingBranchBase((CUpscalingEntry&)*model.GraphEntry(MD_BASE_UPSCALING), parent.Ctrl(), FALSE, FIXED_ITEM, parent.Handle(), TVI_LAST, FALSE, FALSE)
{
}

BOOL CUpscalingBranch::OnFilter(const CGraphNode& t) const
{
  return (dynamic_cast<const IUpscalingNodeBase*>(&t) != 0);
}

CTreeNode* CUpscalingBranch::InsertChild(CGraphNode& t)
{
  CThinLayerUpscalingNode* pThinLayerUpscalingNode = dynamic_cast<CThinLayerUpscalingNode*>(&t);
  CCamClayUpscalingNode* pCamClayUpscalingNode = dynamic_cast<CCamClayUpscalingNode*>(&t);
  if(pThinLayerUpscalingNode || pCamClayUpscalingNode)
  {
    IUpscalingNodeBase *node = (pThinLayerUpscalingNode) ? (IUpscalingNodeBase*)pThinLayerUpscalingNode : 
                                                            (IUpscalingNodeBase*)pCamClayUpscalingNode;
    return new CUpscalingObserver(*node,
                                           *this,
                                           FALSE,
                                           DELETE_ITEM,
                                           (HTREEITEM)-1,
                                           false,
                                           false);
  }

  assert(FALSE);
  return 0;
}
