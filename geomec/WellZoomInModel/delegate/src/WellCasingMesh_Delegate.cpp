#include "stdafx.h"

#include "AttriWellCasingMeshDlg.h"
#include "WellCasingMesh.h"
#include "WellCasingMesh_Delegate.h"

CWellCasingMesh_Delegate::CWellCasingMesh_Delegate(CWellCasingMesh *wellCasingMesh)
    : IWellMesh_Delegate(wellCasingMesh), m_wellCasingMesh(wellCasingMesh) {}

bool CWellCasingMesh_Delegate::Attributes() {
  m_wellCasingMesh->setInvalidateAfterDlg(false);

  CAttriWellCasingMeshDlg dlg(*m_wellCasingMesh);
  bool bRet;

  {
    COperation oper(m_wellCasingMesh->Model());

    bRet = (dlg.DoModal() == IDOK);

    if (m_wellCasingMesh->getInvalidateAfterDlg()) {
      m_wellCasingMesh->InvalidateMesh();
    }
  }

  return bRet;
}

bool CWellCasingMesh_Delegate::IsMesh() const { return m_wellCasingMesh->IsMesh(); }
