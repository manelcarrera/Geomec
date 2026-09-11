#include "stdafx.h"

#include "AttriWellZoomInModel.h"
#include "BoundaryInterfaceDef_Delegate.h"
#include "ContextMenuInvoker.h"
#include "FemAppMainWindow.h"
#include "IWellModel.h"
#include "IWellModel_Delegate.h"

IWellModel_Delegate::IWellModel_Delegate(IWellModel *wellModel)
    : CModelBase_Delegate(wellModel), m_wellModel(wellModel) {}

bool IWellModel_Delegate::Attributes() {
  CAttriWellZoomInModelDlg dlg(*m_wellModel, m_wellModel->AttributesDialogCaption().toStdString().c_str(),
                               FemAppGetMainWnd());

  return (dlg.DoModal() == IDOK);
}

IWellModel_Delegate::CBoundary_Delegate::CBoundary_Delegate(IWellModel::CBoundary *boundary)
    : CBoundaryBase_Delegate(boundary), m_boundary(boundary) {}

void IWellModel_Delegate::CBoundary_Delegate::AppendContextMenu(CContextMenuInvoker &invoker) {
  typedef CSingleCommandTemplate<IWellModel_Delegate::CBoundary_Delegate> TBoundaryCommand;

  invoker.AddCommand(_T("&Interface attributes"),
                     *(new TBoundaryCommand(*this, &IWellModel_Delegate::CBoundary_Delegate::InterfaceAttributes)));
}

bool IWellModel_Delegate::CBoundary_Delegate::CanEdit() const { return false; }

bool IWellModel_Delegate::CBoundary_Delegate::CanModify() const { return false; }

void IWellModel_Delegate::CBoundary_Delegate::InterfaceAttributes() {
  assert(typeid(m_boundary->InterfaceDefinition()).name() == typeid(CBoundaryInterfaceDef).name());

  CBoundaryInterfaceDef_Delegate *boundaryInterfaceDef_Delegate = static_cast<CBoundaryInterfaceDef_Delegate *>(
      const_cast<CBoundaryInterfaceDef &>(m_boundary->InterfaceDefinition()).getDelegate());

  boundaryInterfaceDef_Delegate->Attributes();
}

IWellModel_Delegate::CBoundary_Delegate::IWellSupportNode_Delegate::IWellSupportNode_Delegate(
    IWellModel::CBoundary::IWellSupportNode *wellSupportNode)
    : CBaseSupportNode_Delegate(wellSupportNode), m_wellSupportNode(wellSupportNode) {}

IWellModel_Delegate::CBoundary_Delegate::CDisplacementSupport_Delegate::CDisplacementSupport_Delegate(
    IWellModel::CBoundary::CDisplacementSupport *displacementSupport)
    : IWellSupportNode_Delegate(displacementSupport), m_displacementSupport(displacementSupport) {}

IWellModel_Delegate::CBoundary_Delegate::CPressureSupport_Delegate::CPressureSupport_Delegate(
    IWellModel::CBoundary::CPressureSupport *pressureSupport)
    : IWellSupportNode_Delegate(pressureSupport), m_pressureSupport(pressureSupport) {}
