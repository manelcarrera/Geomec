#include "StdAfx.h"

#include "NonMeshedSurface_Delegate.h"
#include "NonMeshedSurface.h"
#include "AttriNameColorDlg.h"

CNonMeshedSurface_Delegate::CNonMeshedSurface_Delegate(CNonMeshedSurface* nonMeshedSurface)
: CColorNode_Delegate(nonMeshedSurface),
  m_nonMeshedSurface(nonMeshedSurface)
{
}

bool CNonMeshedSurface_Delegate::Attributes()
{
  CAttriNameColorDlg<CNonMeshedSurface> dlg(*m_nonMeshedSurface);
  return (dlg.DoModal() == IDOK);
}


/////

CNonMeshedSurfaceEntry_Delegate::CNonMeshedSurfaceEntry_Delegate(CNonMeshedSurfaceEntry* entry)
: CStorageNodeEntry_Delegate<CNonMeshedSurface>(entry),
  m_entry(entry)
{
}
