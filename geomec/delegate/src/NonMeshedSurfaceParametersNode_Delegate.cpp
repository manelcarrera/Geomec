#include "NonMeshedSurfaceParametersNode_Delegate.h"
#include "StdAfx.h"

#include "NonMeshedSurfaceParametersDlg.h"
#include "NonMeshedSurfaceParametersNode.h"

CNonMeshedSurfaceParametersNode_Delegate::CNonMeshedSurfaceParametersNode_Delegate(
    CNonMeshedSurfaceParametersNode *node)
    : CStorageNode_Delegate(node), m_node(node) {}

bool CNonMeshedSurfaceParametersNode_Delegate::Attributes() {
  CNonMeshedSurfaceParametersDlg dlg(*m_node);
  return dlg.DoModal() == IDOK;
}
