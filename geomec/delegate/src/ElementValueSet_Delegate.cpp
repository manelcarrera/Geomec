#include "stdafx.h"

#include "ElementValueSet_Delegate.h"
#include "ElementValueSet.h"
#include "AttriElementValueSet.h"

CElementValueSet_Delegate::CElementValueSet_Delegate(
  CElementValueSet* elementValueSet)
: IValueSet_Delegate(elementValueSet)
, m_elementValueSet(elementValueSet)
{
}

bool CElementValueSet_Delegate::Attributes()
{
  CAttriElementValueSet dlg(*m_elementValueSet, 0);

  return dlg.DoModal() == IDOK;
}
