// ValueCompositeObserver.cpp: implementation of the CValueCompositeObserver class.
//
//////////////////////////////////////////////////////////////////////

#include "ValueCompositeObserver.h"
#include "RpnValueSet.h"
#include "ValueType.h"
#include "geomec.h"
#include "stdafx.h"

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
// #define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CValueCompositeObserver::CValueCompositeObserver(IValueComposite &node, CTreeNode &parent, const BOOL rename,
                                                 const enum REMOVE_TYPE remove, HTREEITEM hInsertAfter,
                                                 BOOL bInitialUpdate, BOOL bSortAfterInsertion)
    : TValueBaseObserver(node, parent, rename, remove, hInsertAfter, FALSE, bSortAfterInsertion), m_uMode(0) {
  Update();
}

CValueCompositeObserver::CValueCompositeObserver(IValueComposite &node, CTreeCtrl &ctrl, const BOOL rename,
                                                 const enum REMOVE_TYPE remove, HTREEITEM hParent,
                                                 HTREEITEM hInsertAfter, BOOL bInitialUpdate, BOOL bSortAfterInsertion)
    : TValueBaseObserver(node, ctrl, rename, remove, hParent, hInsertAfter, FALSE, bSortAfterInsertion), m_uMode(0) {
  Update();
}

BOOL CValueCompositeObserver::OnFilter(const CStorageNode &t) const {
  const IValueComponentBase *pComponent = dynamic_cast<const IValueComponentBase *>(&t);
  if (pComponent) {
    const IValueComposite *pComposite = dynamic_cast<const IValueComposite *>(&ObservedItem());
    return pComposite->ComponentSize(m_uMode) > 1;
  }

  const CValueType *pValueType = dynamic_cast<const CValueType *>(&t);
  if (pValueType) {
    return TRUE;
  }

  return FALSE;
}

CTreeNode *CValueCompositeObserver::InsertChild(CStorageNode &t) {
  IValueComponentBase *pComponent = dynamic_cast<IValueComponentBase *>(&t);
  if (pComponent) {
    return new TValueComponentObserver(*pComponent, *this, FALSE, FIXED_ITEM);
  }

  CValueType *pValueType = dynamic_cast<CValueType *>(&t);
  if (pValueType) {
    return new CValueCompositeObserver(*pValueType, *this, TRUE, UNLINK_ITEM);
  }

  assert(FALSE);

  return 0;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CValueReferenceObserver::CValueReferenceObserver(IValueComposite &node, CTreeNode &parent, const BOOL rename,
                                                 const enum REMOVE_TYPE remove, HTREEITEM hInsertAfter,
                                                 BOOL bInitialUpdate, BOOL bSortAfterInsertion)
    : CValueCompositeObserver(node, parent, rename, remove, hInsertAfter, bInitialUpdate, bSortAfterInsertion) {}

CValueReferenceObserver::CValueReferenceObserver(IValueComposite &node, CTreeCtrl &ctrl, const BOOL rename,
                                                 const enum REMOVE_TYPE remove, HTREEITEM hParent,
                                                 HTREEITEM hInsertAfter, BOOL bInitialUpdate, BOOL bSortAfterInsertion)
    : CValueCompositeObserver(node, ctrl, rename, remove, hParent, hInsertAfter, bInitialUpdate, bSortAfterInsertion) {}

BOOL CValueReferenceObserver::OnFilter(const CStorageNode &t) const {
  const IValueComponentBase *pComponent = dynamic_cast<const IValueComponentBase *>(&t);

  if (pComponent) {
    for (size_t r = 0; r < t.referenceSize(); ++r) {
      const CRpnValueSet *rpnValueSet = dynamic_cast<const CRpnValueSet *>(&t.referenceAt(r));

      if (rpnValueSet) {
        return TRUE;
      }
    }
  }

  return CValueCompositeObserver::OnFilter(t);
}
