#include "stdafx.h"

#include "NodalValueSet_Delegate.h"
#include "NodalValueSet.h"
#include "AttriNodalValueSet.h"

CNodalValueSet_Delegate::CNodalValueSet_Delegate(CNodalValueSet* nodalValueSet)
: IValueSet_Delegate(nodalValueSet)
, m_nodalValueSet(nodalValueSet)
{
}

bool CNodalValueSet_Delegate::Attributes()
{
  CAttriNodalValueSet dlg(*m_nodalValueSet);

  return dlg.DoModal() == IDOK;
}
