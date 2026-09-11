#include "stdafx.h"

#include "Attriboundarydlg.h"
#include "BoundaryInterfaceDef.h"
#include "BoundaryInterfaceDef_Delegate.h"
#include "ContextMenuInvoker.h"
#include "HexaBoundary.h"
#include "HexaBoundary_Delegate.h"

template <> BOOL EnableBoundarySpecificWindow<CHexaBoundary>(CDialog *dialog, int dialogItem, BOOL bEnable) {
  return dialog->GetDlgItem(dialogItem)->EnableWindow(bEnable);
}

template <> BOOL SetBoundarySpecificReadOnly<CHexaBoundary>(CDialog *dialog, int dialogItem, BOOL bReadOnly) {
  return ((CEdit *)dialog->GetDlgItem(dialogItem))->SetReadOnly(bReadOnly);
}

template <> void SetBoundarySpecificWindowText<CHexaBoundary>(CDialog *dialog, int dialogItem, LPCTSTR string) {
  dialog->GetDlgItem(dialogItem)->SetWindowText(string);
}

template <>
void BoundarySpecificDDX_Point<CHexaBoundary>(CGeomecAttriTemp<CHexaBoundary> *dialog, CDataExchange *pDX, int nID_X,
                                              int nID_Y, int nID_Z, geo::IPoint &point, int nID_UNIT1, int nID_UNIT2,
                                              int nID_UNIT3) {
  dialog->DDX_Point(pDX, nID_X, nID_Y, nID_Z, point, nID_UNIT1, nID_UNIT2, nID_UNIT3);
}

template <> void BoundarySpecificDDX_Text<CHexaBoundary>(CDataExchange *pDX, int nIDC, double &value) {
  DDX_Text(pDX, nIDC, value);
}

CHexaBoundary_Delegate::CHexaBoundary_Delegate(CHexaBoundary *hexaBoundary)
    : CBoundaryBase_Delegate(hexaBoundary), m_hexaBoundary(hexaBoundary) {}

bool CHexaBoundary_Delegate::Attributes() {
  CAttriBoundaryDlg<CHexaBoundary> dlg(*m_hexaBoundary, IDD_ATTRI_HEXA_BOUNDARY);

  return dlg.DoModal() == IDOK;
}

void CHexaBoundary_Delegate::AppendContextMenu(CContextMenuInvoker &invoker) {
  typedef CSingleCommandTemplate<CHexaBoundary> TBoundaryCommand;
  typedef CSingleCommandTemplate<CHexaBoundary_Delegate> TBoundaryCommand_Delegate;

  if (!m_hexaBoundary->Model().parentModel()) {
    invoker.AddCommand(_T("&Apply boundary interfaces"),
                       *(new TBoundaryCommand(*m_hexaBoundary, &CHexaBoundary::ToggleInterfaces, 0,
                                              &CHexaBoundary::CreateInterfaces)));
  }

  invoker.AddCommand(_T("&Interface attributes"),
                     *(new TBoundaryCommand_Delegate(*this, &CHexaBoundary_Delegate::InterfaceAttributes,
                                                     &CHexaBoundary_Delegate::CanCreateInterfaces)));
  invoker.AddSeparator();

  CBoundaryBase_Delegate::AppendContextMenu(invoker);
}

bool CHexaBoundary_Delegate::CanCreateInterfaces() const {
  const CModelBase &model = (const CModelBase &)(m_hexaBoundary->Model());

  return model.IsMesh() && m_hexaBoundary->CreateInterfaces();
}

void CHexaBoundary_Delegate::InterfaceAttributes() {
  assert(typeid(m_hexaBoundary->InterfaceDefinition()).name() == typeid(CBoundaryInterfaceDef).name());

  CBoundaryInterfaceDef_Delegate *boundaryInterfaceDef_Delegate =
      static_cast<CBoundaryInterfaceDef_Delegate *>(m_hexaBoundary->InterfaceDefinition().getDelegate());

  boundaryInterfaceDef_Delegate->Attributes();
}
