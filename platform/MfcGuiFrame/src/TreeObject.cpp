// TreeObject.cpp: implementation of the CTreeObject class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "MfcTreeObjectObserver.h"
#include "ContextMenuInvoker.h"
#include "IModelObject.h"
#include "TreeObject.h"
#include <cassert>
#include <qcolor.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CTreeObject::CTreeObject(CMfcTreeObjectObserver& observer, CTreeCtrl &ctrl, HTREEITEM hParent, HTREEITEM hInsertAfter)
: ITreeObject(ctrl, hParent, hInsertAfter), m_observer(observer)
{
  Ctrl().SetItemData(Handle(), (DWORD_PTR)this);
}

CTreeObject::CTreeObject(CMfcTreeObjectObserver& observer, ITreeObject &parent, HTREEITEM hInsertAfter)
: ITreeObject(parent, hInsertAfter), m_observer(observer)
{
  Ctrl().SetItemData(Handle(), (DWORD_PTR)this);
}

CTreeObject::~CTreeObject()
{
  m_observer.m_pTreeObject = 0;
}

unsigned int CTreeObject::Icon() const
{
  return m_observer.onIconId();
}

unsigned int CTreeObject::StateIcon() const
{
  return m_observer.onIconId();
}

QString CTreeObject::Text() const
{
  return (LPCSTR) m_observer.onName();
}

void CTreeObject::AppendContextMenu(CContextMenuInvoker &invoker)
{
  m_observer.appendCommands(invoker);
}

COLORREF CTreeObject::Color()
{
  QRgb c = m_observer.color();
  if( (qRed(c) == 0) && (qGreen(c) == 0) && (qBlue(c) == 0) )
    return ITreeObject::Color();
  return RGB(qRed(c), qGreen(c), qBlue(c));
}

BOOL CTreeObject::Bold()
{
  return m_observer.bold();
}

BOOL CTreeObject::Enable()
{
  return m_observer.enable();
}

void CTreeObject::Delete()
{
  m_observer.destroy();
}

BOOL CTreeObject::CanDelete() const
{
  return m_observer.canDestroy();
}

BOOL CTreeObject::operator<(const ICtrlObjectBase &object) const
{
  const CTreeObject* pObject = dynamic_cast<const CTreeObject*>(&object);
  if(!pObject || (pObject->Parent() != Parent()))
    return ITreeObject::operator <(object);

  const CTreeObject* pParent = dynamic_cast<const CTreeObject*>(pObject->Parent());
  assert(pParent);

  int nThis = -1;
  int nRhs = -1;

  for(int i = 0; i < pParent->m_observer.childSize(); i++)
  {
    if(&pParent->m_observer.childAt(i) == &m_observer.observedObject()) nThis = i;
    if(&pParent->m_observer.childAt(i) == &pObject->m_observer.observedObject()) nRhs = i;
  }
  
  return nThis < nRhs;
}