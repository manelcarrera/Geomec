#include "NonMeshedSurfacePressure_Delegate.h"
#include "StdAfx.h"

#include "AttriNonMeshedSurfacePressure.h"
#include "NonMeshedSurfacePressure.h"

CNonMeshedSurfacePressure_Delegate::CNonMeshedSurfacePressure_Delegate(CNonMeshedSurfacePressure *node)
    : IValueComposite_Delegate(node), m_node(node) {}

bool CNonMeshedSurfacePressure_Delegate::Attributes() {
  CAttriNonMeshedSurfacePressure dlg(*m_node);
  return (dlg.DoModal() == IDOK);
}
