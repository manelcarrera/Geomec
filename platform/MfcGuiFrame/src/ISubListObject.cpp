// ISubListObject.cpp: implementation of the ISubListObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ISubListObject.h"

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif  // _MSC_VER
//#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ISubListObject::ISubListObject(IListObject &list_object, int nSubIndex)
: m_list_object(list_object), m_pCustomControl(0), m_nIndex(nSubIndex)
{
  // Register ourselves ...
  VERIFY( m_list_object.m_mpSubObject.insert(IListObject::TSubListMap::value_type(nSubIndex, this)).second);
}

ISubListObject::~ISubListObject()
{
}

const IListObject& ISubListObject::ListObject() const
{
  return m_list_object;
}

IListObject& ISubListObject::ListObject()
{
  return m_list_object;
}

int ISubListObject::Index() const
{
  return m_nIndex;
}

CRect ISubListObject::Rect() const
{
  CRect ret;
  ListObject().Ctrl().GetSubItemRect(ListObject().Index(), Index(), LVIR_BOUNDS, ret);

  return ret;
}

void ISubListObject::SetCustomControl(CWnd *pWnd)
{
  m_pCustomControl = pWnd;
}

const CWnd *ISubListObject::Control() const
{
  return m_pCustomControl;
}

CWnd *ISubListObject::Control()
{
  return m_pCustomControl;
}

CListCtrl& ISubListObject::Ctrl() const
{
  return ListObject().Ctrl();
}
