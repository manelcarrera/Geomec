#include "stdafx.h"

#include "WellZoomInGUI.h"
#include "WellZoomInModel.h"

CWellZoomInGUI::CWellZoomInGUI(CFemAppGUI* parentGUI,
  CWellZoomInModel* wellZoomInModel)
: IWellGUI(parentGUI, wellZoomInModel)
, m_wellZoomInModel(wellZoomInModel)
{
}

void CWellZoomInGUI::OnCloseModel()
{
  IWellGUI::OnCloseModel();
}

CWellZoomInModel* CWellZoomInGUI::model() const
{
  return m_wellZoomInModel;
}

bool CWellZoomInGUI::LoadPre381Stream(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &prog)
{
  if(!m_wellZoomInModel->LoadPre(stream, 0, version, prog, false))
    return false;

  if(!m_wellZoomInModel->LoadWellZoomIn1(stream, version, prog))
    return false;

  if (version < CStreamVersion(3, 8, 1))
  {
    LoadScenes(stream, version, prog);
  }

  if(!m_wellZoomInModel->LoadWellZoomIn2(stream, version, prog))
    return false;

  return m_wellZoomInModel->LoadPost(stream, version, prog);
}
