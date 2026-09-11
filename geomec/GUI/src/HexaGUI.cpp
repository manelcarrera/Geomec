#include "stdafx.h"

#include "HexaGUI.h"
#include "HexaModel.h"
#include "MeshBase.h"
#include "BranchState.h"
#include "GoCadGUI.h"
#include "GoCadModel.h"
#include "GeoProgress.h"
#include "GlobalMessage.h"
#include "resourceIDP.h"
#include "IProgressFactory.h"
#include "resourceIDS.h"
#include "VersionNumbers.h"
#include "ModelLifetimeFacade.h"

CHexaGUI::CHexaGUI(CFemAppGUI* parentGUI, CHexaModel* hexaModel)
: C3DGUI(parentGUI, hexaModel)
, m_hexaModel(hexaModel)
{
}

bool CHexaGUI::CanConvertToTetra() const
{
  return m_hexaModel->Mesh().CanCreateMesh() || m_hexaModel->Mesh().IsMesh();
}

void CHexaGUI::ConvertToTetraModel()
{
  ConvertToTetra();
}

CGUIBase* CHexaGUI::ConvertToTetra()
{
  if (m_hexaModel->BranchState().IsBranch())
  {
    if (_m()->msg(IDP_CONVERTBRANCHTOTETRA, MB_YESNO) == IDNO)
    {
      return 0;
    }
  }

  if (!m_hexaModel->Mesh().IsMesh())
  {
    assert(m_hexaModel->Mesh().CanCreateMesh());
    m_hexaModel->Mesh().CreateMesh();
  }

  // We create a tetra model
  // We treat it as a child model here, even though it may be a root model; but whatever decorators were used for the current model should still hold in the new one
  CGoCadGUI* pTetra =
    new CGoCadGUI(parentGUI(), static_cast<CGoCadModel *>(IModelLifetimeFacade::NewChild(GOCAD_MODEL, m_hexaModel->Logger(), m_hexaModel->getVersionManager())));

  try
  {
    pTetra->Convert(*this->model());
  }

  catch (CProgressCancel* e)
  {
    // cancelled

    delete e;

    IModelLifetimeFacade::Close(pTetra->model());
    pTetra = 0;
  }

  return pTetra;
}

void CHexaGUI::OnCloseModel()
{
  C3DGUI::OnCloseModel();
}

CHexaModel* CHexaGUI::model() const
{
  return m_hexaModel;
}

bool CHexaGUI::LoadPre381Stream(CStorageNode::TSTREAM &stream, CStreamVersion &version, CStorageNode::TPROGRESS &prog)
{
  if(!m_hexaModel->LoadPre(stream, 0, version, prog, true))
    return false;

  if(!m_hexaModel->LoadHexa1(stream, version, prog))
    return false;

	if(version < CStreamVersion(3, 0, 49))
		LoadScenes(stream,version, prog);

  if(!m_hexaModel->LoadHexa2(stream, version, prog))
    return false;

	if ((CStreamVersion(3, 0, 48) < version) &&
		(version < CStreamVersion(3, 8, 1)))
	{
		LoadScenes(stream,version, prog);
	}

  if(!m_hexaModel->LoadHexa3(stream, version, prog))
    return false;

  return m_hexaModel->LoadPost(stream, version, prog);
}
