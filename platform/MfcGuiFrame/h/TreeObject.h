// TreeObject.h: interface for the CTreeObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TREEOBJECT_H__507A603F_96D5_4B8C_849F_7F2330AAB358__INCLUDED_)
#define AFX_TREEOBJECT_H__507A603F_96D5_4B8C_849F_7F2330AAB358__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ITreeObject.h"

class CMfcTreeObjectObserver;
class CTreeObject : public ITreeObject {
  CMfcTreeObjectObserver &m_observer;

public:
  CTreeObject(CMfcTreeObjectObserver &observer, CTreeCtrl &ctrl, HTREEITEM hParent, HTREEITEM hInsertAfter);
  CTreeObject(CMfcTreeObjectObserver &observer, ITreeObject &parent, HTREEITEM m_hInsertAfter);
  virtual ~CTreeObject();

  CMfcTreeObjectObserver &Observer() { return m_observer; }

  virtual unsigned int Icon() const;
  virtual unsigned int StateIcon() const;
  virtual QString Text() const;
  virtual void AppendContextMenu(CContextMenuInvoker &invoker);

  virtual COLORREF Color();
  virtual BOOL Bold();
  virtual BOOL Enable();

  virtual void Delete();
  virtual BOOL CanDelete() const;

  virtual BOOL operator<(const ICtrlObjectBase &object) const;
};

#endif // !defined(AFX_TREEOBJECT_H__507A603F_96D5_4B8C_849F_7F2330AAB358__INCLUDED_)
