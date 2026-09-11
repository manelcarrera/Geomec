#include "StdAfx.h"
#include "geomec.h"

#include "WellCasingNode.h"

#include "EnumerationBranch.h"
#include "OpenGLNodeObserver_Delegate.h"
#include "ValueCompositeObserver.h"
#include "ValueType_Delegate.h"
#include "WellCasingCementInterface.h"
#include "WellCasingCementInterface_Delegate.h"
#include "WellCasingInternalPressure.h"
#include "WellCasingInternalTemperature.h"
#include "WellCasingModel.h"
#include "WellCasingNodeObserver.h"
#include "WellCasingSteel.h"
#include "WellCasingSteelObserver.h"

///// CWellCasingNodeObserver

CWellCasingNodeObserver::CWellCasingNodeObserver(CWellCasingNode &node, CTreeCtrl &ctrl, const BOOL rename,
                                                 const enum REMOVE_TYPE remove, HTREEITEM hParent,
                                                 HTREEITEM hInsertAfter, BOOL bInitialUpdate, BOOL bSortAfterInsertion)
    : TWellCasingNodeObserverBase(node, ctrl, rename, remove, hParent, hInsertAfter, bInitialUpdate,
                                  bSortAfterInsertion) {
  typedef CEnumerationBranch<CWellCasingInternalPressure, CValueCompositeObserver, FALSE, FIXED_ITEM>
      TPressureEnumerator;
  typedef CEnumerationBranch<CWellCasingInternalTemperature, CValueCompositeObserver, FALSE, FIXED_ITEM>
      TTemperatureEnumerator;

  new TPressureEnumerator(*this, _T("Internal Pressures"), IDI_PRESSURES);
  new TTemperatureEnumerator(*this, _T("Internal Temperatures"), IDI_TEMPERATURES);

  Update();
}

BOOL CWellCasingNodeObserver::OnFilter(const CGraphNode &node) const {
  if (dynamic_cast<const CWellCasingSteel *>(&node))
    return true;

  if (dynamic_cast<const CWellCasingCementInterface *>(&node))
    return true;

  return false;
}

CTreeNode *CWellCasingNodeObserver::InsertChild(CGraphNode &node) {
  CWellCasingSteel *pSteel = dynamic_cast<CWellCasingSteel *>(&node);
  if (pSteel) {
    return new TWellCasingSteelObserver(*pSteel, *this, FALSE, FIXED_ITEM, TVI_LAST, TRUE, FALSE);
  }

  CWellCasingCementInterface *pCementInterface = dynamic_cast<CWellCasingCementInterface *>(&node);
  if (pCementInterface) {
    typedef CMaterialServerObserver_Delegate<CWellCasingCementInterfaceMaterialServer,
                                             CWellCasingCementInterfaceMaterialServer_Delegate, CDummyNode,
                                             CDummyObserver, FALSE, FIXED_ITEM>
        TWellCasingCementInterfaceMaterialServerObs;
    return new COpenGLNodeObserver_Delegate<CWellCasingCementInterface, CWellCasingCementInterface_Delegate,
                                            CWellCasingCementInterfaceMaterialServer,
                                            TWellCasingCementInterfaceMaterialServerObs, FALSE, FIXED_ITEM>(
        *pCementInterface, *this, FALSE, FIXED_ITEM, TVI_LAST, TRUE, FALSE);
  }

  assert(FALSE);
  return 0;
}
