#include "stdafx.h"

#include "3DGUI.h"
#include "3dmodel.h"
#include "AttriHexaMeshRegionDlg.h"
#include "ContextMenuInvoker.h"
#include "GeomecDoc.h"
#include "HexaBoundary.h"
#include "HexaMeshDepthDlg.h"
#include "HexaMeshRegion_Delegate.h"
#include "ISceneWrapper.h"
#include "OpenInventorSceneNode.h"

CHexaMeshRegionBase_Delegate::CHexaMeshRegionBase_Delegate(CHexaMeshRegionBase *hexaMeshRegionBase)
    : CMeshRegionBase_Delegate(hexaMeshRegionBase), m_hexaMeshRegionBase(hexaMeshRegionBase) {}

void CHexaMeshRegionBase_Delegate::AppendContextMenu(CContextMenuInvoker &invoker) {
  // Append copy, paste and delete

  invoker.AddCommand(_T("&Modify"), *(new TGraphNode_DelegateCommand(*this, &CGraphNode_Delegate::Edit,
                                                                     &CGraphNode_Delegate::CanEdit)));
  invoker.AddSeparator();

  CMeshRegionBase_Delegate::AppendContextMenu(invoker);
}

bool CHexaMeshRegionBase_Delegate::CanEdit() const {
  return !(static_cast<const CModelBase &>(m_hexaMeshRegionBase->Model())).BranchState().IsBranch();
}

bool CHexaMeshRegionBase_Delegate::CanDestroy() const { return m_hexaMeshRegionBase->CanDestroy(); }

bool CHexaMeshRegionBase_Delegate::Destroy() { return m_hexaMeshRegionBase->Destroy(); }

CHexaMainMeshRegion_Delegate::CHexaMainMeshRegion_Delegate(CHexaMainMeshRegion *hexaMainMeshRegion)
    : CHexaMeshRegionBase_Delegate(hexaMainMeshRegion), m_hexaMainMeshRegion(hexaMainMeshRegion) {}

bool CHexaMainMeshRegion_Delegate::Attributes() {
  CAttriHexaMainMeshRegionDlg dlg(*m_hexaMainMeshRegion);

  return dlg.DoModal() == IDOK;
}

bool CHexaMainMeshRegion_Delegate::Edit() {
  CGeomecDoc *pDoc = GetGeomecDoc();
  C3DGUI &model = (C3DGUI &)*(dynamic_cast<C3DGUI *>(pDoc->GUI()));

  COpenInventorSceneNode &oivNode = model.OpenInventorScene();
  pDoc->CurrentScene(&oivNode);
  oivNode.EditHexaMainMeshRegion(*m_hexaMainMeshRegion);
  return true;
}

bool CHexaMainMeshRegion_Delegate::CanDestroy() const { return m_hexaMainMeshRegion->CanDestroy(); }

CHexaSubMeshRegion_Delegate::CHexaSubMeshRegion_Delegate(CHexaSubMeshRegion *hexaSubMeshRegion)
    : CHexaMeshRegionBase_Delegate(hexaSubMeshRegion), m_hexaSubMeshRegion(hexaSubMeshRegion) {}

bool CHexaSubMeshRegion_Delegate::Attributes() {
  CAttriHexaSubMeshRegionDlg dlg(*m_hexaSubMeshRegion);

  return dlg.DoModal() == IDOK;
}

bool CHexaSubMeshRegion_Delegate::Edit() {
  CGeomecDoc *pDoc = GetGeomecDoc();
  C3DGUI &model = (C3DGUI &)*(dynamic_cast<C3DGUI *>(pDoc->GUI()));

  COpenInventorSceneNode &oivNode = model.OpenInventorScene();
  pDoc->CurrentScene(&oivNode);
  oivNode.EditHexaSubMeshRegion(*m_hexaSubMeshRegion);
  return true;
}

bool CHexaSubMeshRegion_Delegate::PropertiesOK() {
  CAttriHexaSubMeshRegionDlg dlg(*m_hexaSubMeshRegion);

  return (dlg.DoModal() == IDOK);
}

CHexaMeshRegionEntry_Delegate::CHexaMeshRegionEntry_Delegate(CHexaMeshRegionEntry *hexaMeshRegionEntry)
    : CGraphEntryTemp_Delegate<CHexaMeshRegionBase>(hexaMeshRegionEntry), m_hexaMeshRegionEntry(hexaMeshRegionEntry) {}

bool CHexaMeshRegionEntry_Delegate::Attributes() {
  CHexaMeshDepthDlg dlg(m_hexaMeshRegionEntry->Model());

  return dlg.DoModal() == IDOK;
}

void CHexaMeshRegionEntry_Delegate::AppendContextMenu(CContextMenuInvoker &invoker) {
  typedef CSingleCommandTemplate<CHexaMeshRegionEntry_Delegate> THexaMeshRegionEntry_DelegateCommand;

  // We have two inserts: Graphical rectangular and polygonal

  CContextMenuInvoker *pSubMenu = new CContextMenuInvoker;

  pSubMenu->AddCommand(_T("&Rectangular"), *(new THexaMeshRegionEntry_DelegateCommand(
                                               *this, &CHexaMeshRegionEntry_Delegate::CreateMeshRegionRect,
                                               &CHexaMeshRegionEntry_Delegate::CanCreateMeshRegion)));
  pSubMenu->AddCommand(_T("&Polygonal"), *(new THexaMeshRegionEntry_DelegateCommand(
                                             *this, &CHexaMeshRegionEntry_Delegate::CreateMeshRegionPoly,
                                             &CHexaMeshRegionEntry_Delegate::CanCreateMeshRegion)));
  invoker.AddInvoker("Create mesh region", *pSubMenu);
  invoker.AddSeparator();
  invoker.AddCommand(_T("&Attributes"), *(new TGraphNode_DelegateCommand(*this, &CGraphNode_Delegate::Attributes)));
}

bool CHexaMeshRegionEntry_Delegate::CanCreateMeshRegion() const {
  CModelBase &model = (CModelBase &)m_hexaMeshRegionEntry->Model();

  return (model.Boundary().State() != CBoundaryBase::DEFAULT_DEFINED && !model.BranchState().IsBranch());
}

void CHexaMeshRegionEntry_Delegate::CreateMeshRegionPoly() {
  // Make the top view scene active

  CGeomecDoc *pDoc = GetGeomecDoc();
  C3DGUI &model = (C3DGUI &)*(dynamic_cast<C3DGUI *>(pDoc->GUI()));

  COpenInventorSceneNode &oivNode = model.OpenInventorScene();
  pDoc->CurrentScene(&oivNode);
  oivNode.CreateHexaMeshRegion(false);
}

void CHexaMeshRegionEntry_Delegate::CreateMeshRegionRect() {
  // Make the top view scene active

  CGeomecDoc *pDoc = GetGeomecDoc();
  C3DGUI &model = (C3DGUI &)*(dynamic_cast<C3DGUI *>(pDoc->GUI()));
  COpenInventorSceneNode &oivNode = model.OpenInventorScene();
  pDoc->CurrentScene(&oivNode);
  oivNode.CreateHexaMeshRegion(true);
}
