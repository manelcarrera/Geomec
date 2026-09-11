// PressureCompositeObserver.cpp: implementation of the CPressureCompositeObserver class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif // _MSC_VER
#include "PressureCompositeObserver.h"
#include "geomec.h"

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPressureCompositeObserver::CPressureCompositeObserver(CPressure &node, CTreeNode &parent, const BOOL rename,
                                                       const enum REMOVE_TYPE remove, HTREEITEM hInsertAfter,
                                                       BOOL bInitialUpdate, BOOL bSortAfterInsertion)
    : CValueCompositeObserver((IValueComposite &)node, parent, rename, remove, hInsertAfter, bInitialUpdate,
                              bSortAfterInsertion) {
  Update();
}

CPressureCompositeObserver::CPressureCompositeObserver(CPressure &node, CTreeCtrl &ctrl, const BOOL rename,
                                                       const enum REMOVE_TYPE remove, HTREEITEM hParent,
                                                       HTREEITEM hInsertAfter, BOOL bInitialUpdate,
                                                       BOOL bSortAfterInsertion)
    : CValueCompositeObserver((IValueComposite &)node, ctrl, rename, remove, hParent, hInsertAfter, bInitialUpdate,
                              bSortAfterInsertion) {
  Update();
}

CPressureCompositeObserver::~CPressureCompositeObserver() {}

UINT CPressureCompositeObserver::StateIcon() const {
  const CPressure *pPressure = dynamic_cast<const CPressure *>(&ObservedItem());
  ASSERT(pPressure);
  ASSERT(pPressure->ComponentSize() == 1);

  CGeomecDoc *pDoc = &((CGeomecApp *)AfxGetApp())->GetDoc();
  ASSERT(pDoc->Model());
  CFemAppModel &model = dynamic_cast<CFemAppModel &>(*pDoc->Model());

  if (pDoc->CurrentScene() == 0)
    return 0;

  if (pDoc->CurrentScene() == &model.DefaultScene())
    return 0;

  if (pDoc->CurrentScene()->IsLinkedTo(pPressure->Component()))
    return IDI_RADIO_CHECKED;

  if (pDoc->CurrentScene()->CanConnectItem(*pPressure))
    return IDI_RADIO_UNCHECKED;

  return 0;
}

void CPressureCompositeObserver::ToggleState() {
  CPressure *pPressure = dynamic_cast<CPressure *>(&ObservedItem());
  ASSERT(pPressure);
  ASSERT(pPressure->ComponentSize() == 1);

  CGeomecDoc *pDoc = &((CGeomecApp *)AfxGetApp())->GetDoc();
  ASSERT(pDoc->CurrentScene());

  if (!pDoc->CurrentScene()->IsLinkedTo(pPressure->Component())) {
    ASSERT(pDoc->CurrentScene()->CanConnectItem(*pPressure));
    pDoc->CurrentScene()->ConnectItem(*pPressure);
  }

  if (Parent())
    Parent()->Update();
}