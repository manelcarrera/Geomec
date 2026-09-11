#ifndef _WELLCASINGNODEOBSERVER_H_
#define _WELLCASINGNODEOBSERVER_H_

#include "OpenGLNodeObserver.h"

class CWellCasingModel;
class CWellCasingSteel;
class CWellCasingCementInterface;
class CWellCasingInternalPressure;
class CWellCasingInternalTemperature;

#include "StateBranch_Delegate.h"
#include "UndefinedIconProvider.h"
#include "WellCasingNode_Delegate.h"
#include "resourceIDI.h"

typedef CNodeObserver_Delegate<CGraphNode, CGraphNode_Delegate, CDummyNode, CDummyObserver, FALSE, FIXED_ITEM>
    TGraphNodeObs;
typedef CStateBranch_Delegate<CWellCasingNode, CWellCasingNode_Delegate, CGraphNode, TGraphNodeObs,
                              CUndefinedIconProvider, FALSE, FIXED_ITEM>
    TWellCasingNodeObserverBase;

class CWellCasingNodeObserver : public TWellCasingNodeObserverBase {
public:
  CWellCasingNodeObserver(CWellCasingNode &node, CTreeCtrl &ctrl, const BOOL rename, const enum REMOVE_TYPE remove,
                          HTREEITEM hParent = TVI_LAST, HTREEITEM hInsertAfter = TVI_LAST, BOOL bInitialUpdate = TRUE,
                          BOOL bSortAfterInsertion = TRUE);

  virtual BOOL OnFilter(const CGraphNode &node) const;
  virtual CTreeNode *InsertChild(CGraphNode &node);
};

#endif _WELLCASINGNODEOBSERVER_H_
