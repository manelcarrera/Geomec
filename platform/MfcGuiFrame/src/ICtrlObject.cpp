// ICtrlObject.cpp: implementation of the ICtrlObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ICtrlObject.h"

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

ICtrlObject::ICtrlObject()
{

}

ICtrlObject::~ICtrlObject()
{

}

unsigned int ICtrlObject::SelectedIcon() const
{
  return Icon();	// Default the same as the normal icon
}

unsigned int ICtrlObject::StateIcon() const
{
  return 0;		// No state
}

BOOL ICtrlObject::OnBeginDrag()
{
  return TRUE;
}

DROPEFFECT ICtrlObject::CanDrop(TCtrlObjectVec &vcDragged, BOOL bMove) const
{
  return DROPEFFECT_NONE;	// Default no drop target ..
}

void ICtrlObject::Drop(TCtrlObjectVec &vcDragged, BOOL bMove) 
{
  assert(FALSE);	// Default this function is not called
}

void ICtrlObject::OnDragLeave(TCtrlObjectVec& vcDragged) const
{
  // ignore
}

void ICtrlObject::Delete()
{
  assert(FALSE);	// Default no delete actions ..
}

BOOL ICtrlObject::CanDelete() const
{
  return FALSE;	// Default this function is not called
}

void ICtrlObject::Modify()
{
  assert(FALSE);	// Default no edit actions ..
}

BOOL ICtrlObject::CanModify() const
{
  return FALSE;	// Default this function is not called
}


BOOL ICtrlObject::OnSelect()
{
  return TRUE;
}

void ICtrlObject::OnDoubleClick()
{
  // Nothing happen ..
}

void ICtrlObject::AppendToMenu(CMenu& menu)
{
  // Nothing happen ...
}

/* NOT OBSOLETE */ void ICtrlObject::AppendContextMenu(CContextMenuInvoker &invoker)
{
}

            

void ICtrlObject::ToggleState()
{
  // Nothing happen ...
}

