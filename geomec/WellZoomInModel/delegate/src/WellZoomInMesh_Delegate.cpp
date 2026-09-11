#include "stdafx.h"

#include "WellZoomInMesh_Delegate.h"
#include "WellZoomInMesh.h"
#include "AttriWellZoomInMeshDlg.h"
#include "FemAppMainWindow.h"

CWellZoomInMesh_Delegate::CWellZoomInMesh_Delegate(
  CWellZoomInMesh* wellZoomInMesh)
: IWellMesh_Delegate(wellZoomInMesh)
, m_wellZoomInMesh(wellZoomInMesh)
{
}

bool CWellZoomInMesh_Delegate::Attributes()
{
  m_wellZoomInMesh->setInvalidateAfterDlg(false);

  CAttriWellZoomInMeshDlg dlg(*m_wellZoomInMesh, FemAppGetMainWnd());
  bool bRet = (dlg.DoModal() == IDOK);

  if (m_wellZoomInMesh->getInvalidateAfterDlg())
  {
  m_wellZoomInMesh->InvalidateMesh();
  m_wellZoomInMesh->CreateMesh();
  }

  return bRet;
}

bool CWellZoomInMesh_Delegate::IsMesh() const
{
  return m_wellZoomInMesh->IsMesh();
}
