#include "stdafx.h"

#include "BoundaryInterfaceDef_Delegate.h"
#include "BoundaryInterfaceDef.h"
#include "intfaceattrdlg.h"
#include "GeomecDoc.h"

CBoundaryInterfaceDef_Delegate::CBoundaryInterfaceDef_Delegate(
  CBoundaryInterfaceDef* boundaryInterfaceDef)
: CStorageNode_Delegate(boundaryInterfaceDef)
, m_boundaryInterfaceDef(boundaryInterfaceDef)
{
}

bool CBoundaryInterfaceDef_Delegate::Attributes()
{
  CIntFaceAttrDlg dlg(*m_boundaryInterfaceDef,
    GetGeomecDoc()->UnitNode().Unit());

  return (dlg.DoModal() == IDOK);
}
