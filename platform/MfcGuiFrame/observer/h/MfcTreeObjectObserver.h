// MfcTreeObjectObserver.h: interface for the CMfcTreeObjectObserver class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MFCTREEOBJECTOBSERVER_H__8594AA37_89DF_46CA_973D_D9BC1813E249__INCLUDED_)
#define AFX_MFCTREEOBJECTOBSERVER_H__8594AA37_89DF_46CA_973D_D9BC1813E249__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ModelObjectObserver.h"
#include <QColor>

class CTreeObject;
class CTreeCtrl;
class IModelObject;
class CContextMenuInvoker;
class ITreeObject;

class CMfcTreeObjectObserver : public CModelObjectObserver
{
  
  friend class CTreeObject;
protected:
  CTreeObject* m_pTreeObject;

  virtual unsigned int onIconId() const = 0;
  virtual unsigned int onStateIconId() const;
  virtual CString onName() const;
  virtual void appendCommands(ICommandInvoker &invoker);

  // Expand and collapse events
  virtual void onExpand();
  virtual void onCollapse();

  typedef QRgb TColor;
  virtual TColor color();
  virtual	bool bold();
  virtual bool enable();

  virtual bool canDestroy() const;
  virtual void destroy();
public:
  CMfcTreeObjectObserver(IModelObject& observed_object, CTreeCtrl &ctrl, HTREEITEM hParent, HTREEITEM hInsertAfter);
  CMfcTreeObjectObserver(IModelObject& observed_object, ITreeObject &parent, HTREEITEM m_hInsertAfter);
  CMfcTreeObjectObserver(IModelObject& observed_object, CMfcTreeObjectObserver& observer);
  virtual ~CMfcTreeObjectObserver();

  HTREEITEM Handle() const;

  bool isExpanded() const;

  // Selection handling
  virtual void select();
  virtual void unSelect();
  virtual bool isSelected();

  const CTreeObject& TreeObject() const {return *m_pTreeObject;}
  CTreeObject& TreeObject() {return *m_pTreeObject;}

  void updateTreeObject();
};

#endif // !defined(AFX_MFCTREEOBJECTOBSERVER_H__8594AA37_89DF_46CA_973D_D9BC1813E249__INCLUDED_)
