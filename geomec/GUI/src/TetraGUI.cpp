#include "stdafx.h"

#include "TetraGUI.h"
#include "TetraModel.h"
#include "resourceIDS.h"

CTetraGUI::CTetraGUI(CFemAppGUI* parentGUI, CTetraModel* tetraModel)
: C3DGUI(parentGUI, tetraModel)
, m_tetraModel(tetraModel)
{
}

void CTetraGUI::OnCloseModel()
{
  C3DGUI::OnCloseModel();
}

CTetraModel* CTetraGUI::model() const
{
  return m_tetraModel;
}

bool CTetraGUI::LoadPre381Stream(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &prog)
{
  if(!m_tetraModel->LoadPre(stream, 0, version, prog, true))
  return false;

  if(!m_tetraModel->LoadTetra1(stream, version, prog))
  return false;

  // Load scene
  if ((CStreamVersion(3, 0, 48) < version) &&
    (version < CStreamVersion(3, 8, 1)))
  {
    LoadScenes(stream, version, prog);
  }

  if(!m_tetraModel->LoadTetra2(stream, version, prog))
  return false;

  return m_tetraModel->LoadPost(stream, version, prog);
}
