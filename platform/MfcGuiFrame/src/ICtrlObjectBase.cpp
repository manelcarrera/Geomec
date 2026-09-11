// ICtrlObjectBase.cpp: implementation of the ICtrlObjectBase class.
//
//////////////////////////////////////////////////////////////////////

#include "ICtrlObjectBase.h"
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

ICtrlObjectBase::ICtrlObjectBase() {}

ICtrlObjectBase::~ICtrlObjectBase() {}

BOOL ICtrlObjectBase::CanEditText() const {
  // Default text editing is forbidden
  return FALSE;
}

BOOL ICtrlObjectBase::EditText(const QString &strText) {
  // Default text editing is forbidden so function should not be called
  assert(FALSE);
  return FALSE;
}

BOOL ICtrlObjectBase::operator<(const ICtrlObjectBase &object) const {
  if (Text().compare(object.Text(), Qt::CaseSensitive) < 0)
    return TRUE;

  return FALSE;
}