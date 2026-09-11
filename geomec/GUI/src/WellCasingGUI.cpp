#include "stdafx.h"

#include "WellCasingGUI.h"
#include "WellCasingModel.h"
#include "resourceIDS.h"

CWellCasingGUI::CWellCasingGUI(CFemAppGUI* parentGUI,
  CWellCasingModel* wellCasingModel)
: IWellGUI(parentGUI, wellCasingModel)
, m_wellCasingModel(wellCasingModel)
{
}

void CWellCasingGUI::SwitchTo()
{
  m_wellCasingModel->SwitchToWarning();

  IWellGUI::SwitchTo();
}

void CWellCasingGUI::OnCloseModel()
{
  IWellGUI::OnCloseModel();
}

CWellCasingModel* CWellCasingGUI::model() const
{
  return m_wellCasingModel;
}

bool CWellCasingGUI::LoadPre381Stream(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &prog)
{
  if(!m_wellCasingModel->LoadPre(stream, 0, version, prog, false))
  return false;

  if(!m_wellCasingModel->LoadWellCasing(stream, version, prog))
  return false;

  if (version < CStreamVersion(3, 8, 1))
  {
  LoadScenes(stream, version, prog);
  }

  return m_wellCasingModel->LoadPost(stream, version, prog);
}
