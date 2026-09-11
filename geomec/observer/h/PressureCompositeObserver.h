// PressureCompositeObserver.h: interface for the CPressureCompositeObserver class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PRESSURECOMPOSITEOBSERVER_H__17B5C0BE_37EC_4FB6_A109_485CC99A718D__INCLUDED_)
#define AFX_PRESSURECOMPOSITEOBSERVER_H__17B5C0BE_37EC_4FB6_A109_485CC99A718D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ValueCompositeObserver.h"
#include "Pressure.h"

class CPressureCompositeObserver : public CValueCompositeObserver  
{
public:
  virtual ~CPressureCompositeObserver();

  CPressureCompositeObserver(CPressure& node,
              CTreeNode &parent,
              const BOOL rename,
              const enum REMOVE_TYPE remove,
              HTREEITEM hInsertAfter = TVI_LAST,
              BOOL bInitialUpdate = FALSE,
              BOOL bSortAfterInsertion = TRUE );

  CPressureCompositeObserver(CPressure& node,
              CTreeCtrl &ctrl,
              const BOOL rename,
              const enum REMOVE_TYPE remove,
              HTREEITEM hParent = TVI_ROOT,
              HTREEITEM hInsertAfter = TVI_LAST,
              BOOL bInitialUpdate = FALSE,
              BOOL bSortAfterInsertion = TRUE);
  virtual UINT StateIcon() const;
  virtual void ToggleState();
};

#endif // !defined(AFX_PRESSURECOMPOSITEOBSERVER_H__17B5C0BE_37EC_4FB6_A109_485CC99A718D__INCLUDED_)
