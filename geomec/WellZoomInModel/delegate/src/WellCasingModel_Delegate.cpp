#include "stdafx.h"

#include "ContextMenuInvoker.h"
#include "WellCasingModel.h"
#include "WellCasingModel_Delegate.h"

CWellCasingModel_Delegate::CWellCasingModel_Delegate(CWellCasingModel *wellCasingModel)
    : IWellModel_Delegate(wellCasingModel), m_wellCasingModel(wellCasingModel) {}

void CWellCasingModel_Delegate::AppendContextMenu(CContextMenuInvoker &invoker) {
  AppendSwitchTo(invoker);

  invoker.AddCommand(_T("&Attributes"), *(new TGraphNode_DelegateCommand(*this, &CGraphNode_Delegate::Attributes)));
}

CWellCasingFormationEntry_Delegate::CWellCasingFormationEntry_Delegate(
    CWellCasingFormationEntry *wellCasingFormationEntry)
    : CWellFormationEntry_Delegate(wellCasingFormationEntry), m_wellCasingFormationEntry(wellCasingFormationEntry) {}
