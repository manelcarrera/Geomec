#include "stdafx.h"

#include "FaultParametersNode_Delegate.h"
#include "FaultParametersNode.h"
#include "AttriFaultParameters.h"

CFaultParametersNode_Delegate::CFaultParametersNode_Delegate(
  CFaultParametersNode* faultParametersNode)
: CStorageNode_Delegate(faultParametersNode)
, m_faultParametersNode(faultParametersNode)
{
}

bool CFaultParametersNode_Delegate::Attributes()
{
  CAttriFaultParameters dlg(*m_faultParametersNode);

  return (dlg.DoModal() == IDOK);
}
