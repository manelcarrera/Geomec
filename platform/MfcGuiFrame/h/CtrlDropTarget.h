// CtrlDropTarget.h: interface for the CCtrlDropTarget class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CTRLDROPTARGET_H__FFB37209_5829_4C2D_A94F_9B783E231B87__INCLUDED_)
#define AFX_CTRLDROPTARGET_H__FFB37209_5829_4C2D_A94F_9B783E231B87__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ICtrlObject.h"

class CCtrlDropTarget : public COleDropTarget {

public:
  static TCtrlObjectVec m_vcDraggedObject;
  CCtrlDropTarget();
  virtual ~CCtrlDropTarget();

  // Overrides
  virtual DROPEFFECT OnDragEnter(CWnd *pWnd, COleDataObject *pDataObject, DWORD dwKeyState, CPoint point);

  virtual DROPEFFECT OnDragOver(CWnd *pWnd, COleDataObject *pDataObject, DWORD dwKeyState, CPoint point);

  virtual void OnDragLeave(CWnd *pWnd);
  virtual BOOL OnDrop(CWnd *pWnd, COleDataObject *pDataObject, DROPEFFECT dropEffect, CPoint point);
};

#endif // !defined(AFX_CTRLDROPTARGET_H__FFB37209_5829_4C2D_A94F_9B783E231B87__INCLUDED_)
