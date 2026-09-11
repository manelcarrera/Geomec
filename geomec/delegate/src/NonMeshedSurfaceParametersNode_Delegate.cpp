#include "StdAfx.h"
#include "NonMeshedSurfaceParametersNode_Delegate.h"

#include "NonMeshedSurfaceParametersNode.h"
#include "NonMeshedSurfaceParametersDlg.h"

CNonMeshedSurfaceParametersNode_Delegate::CNonMeshedSurfaceParametersNode_Delegate(CNonMeshedSurfaceParametersNode* node)
: CStorageNode_Delegate(node),
  m_node(node)
{
}

bool CNonMeshedSurfaceParametersNode_Delegate::Attributes()
{
  CNonMeshedSurfaceParametersDlg dlg(*m_node);
  return dlg.DoModal() == IDOK;
}
