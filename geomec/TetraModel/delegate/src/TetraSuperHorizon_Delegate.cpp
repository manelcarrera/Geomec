#include "stdafx.h"

#include "ContextMenuInvoker.h"
#include "FemAppModel.h"
#include "ModelBase.h"
#include "TetraEntryTypes.h"
#include "TetraMesh.h"
#include "TetraSuperHorizon.h"
#include "TetraSuperHorizon_Delegate.h"

CTetraSuperHorizon_Delegate::CTetraSuperHorizon_Delegate(CTetraSuperHorizon *tetraSuperHorizon)
    : CTetraHorizonBase_Delegate(tetraSuperHorizon), m_tetraSuperHorizon(tetraSuperHorizon) {}

bool CTetraSuperHorizon_Delegate::Attributes() {
  if (C3DHorizon_Delegate::Attributes()) {
    if (!m_tetraSuperHorizon->IsLinkedTo(*m_tetraSuperHorizon->Model().GraphEntry(MD_TETRA_SUPER_HORIZON))) {
      m_tetraSuperHorizon->LinkTo(*m_tetraSuperHorizon->Model().GraphEntry(MD_TETRA_SUPER_HORIZON));
    }

    return true;
  }

  return false;
}

CTetraSuperHorizonEntry_Delegate::CTetraSuperHorizonEntry_Delegate(CTetraSuperHorizonEntry *tetraSuperHorizonEntry)
    : CStorageNodeEntry_Delegate<CTetraSuperHorizon>(tetraSuperHorizonEntry),
      m_tetraSuperHorizonEntry(tetraSuperHorizonEntry) {}

void CTetraSuperHorizonEntry_Delegate::AppendContextMenu(CContextMenuInvoker &invoker) {
  // Export all ...

  typedef CSingleCommandTemplate<CTetraSuperHorizonEntry_Delegate> TTetraSuperHorizonEntryCommand_Delegate;

  invoker.AddCommand(_T("Create horizon"), *(new TTetraSuperHorizonEntryCommand_Delegate(
                                               *this, &CTetraSuperHorizonEntry_Delegate::CreateHorizon,
                                               &CTetraSuperHorizonEntry_Delegate::CanCreateHorizon)));
}

bool CTetraSuperHorizonEntry_Delegate::CanCreateHorizon() const {
  CModelBase &model = (CModelBase &)(m_tetraSuperHorizonEntry->Model());
  const CTetraMesh &mesh = dynamic_cast<const CTetraMesh &>(model.Mesh());

  return mesh.CanInvalidateMesh();
}

void CTetraSuperHorizonEntry_Delegate::CreateHorizon() {
  QString name;

  name = QString("New Horizon %1").arg(m_tetraSuperHorizonEntry->GraphEntryNodes().size() + 1);

  CTetraSuperHorizon *pHorizon = new CTetraSuperHorizon(name, 0, m_tetraSuperHorizonEntry->Model(), FALSE);
  CTetraSuperHorizon_Delegate *pHorizon_Delegate = new CTetraSuperHorizon_Delegate(pHorizon);

  if (!pHorizon_Delegate->Attributes()) {
    delete pHorizon_Delegate;
    delete pHorizon;
  } else if (pHorizon->SurfaceSize() == 1 && pHorizon->Name() == name) {
    // Take over more meaningfull name than default name

    pHorizon->Name(pHorizon->Surface(0).Name());
  }
}
