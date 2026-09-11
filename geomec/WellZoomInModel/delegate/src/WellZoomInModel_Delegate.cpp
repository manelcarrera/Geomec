#include "stdafx.h"

#include "WellZoomInModel_Delegate.h"
#include "WellZoomInModel.h"
#include "WellZoomInGUI.h"
#include "ContextMenuInvoker.h"
#include "GeomecDoc.h"

CWellZoomInModel_Delegate::CWellZoomInModel_Delegate(
  CWellZoomInModel* wellZoomInModel)
: IWellModel_Delegate(wellZoomInModel)
, m_wellZoomInModel(wellZoomInModel)
{
}

void CWellZoomInModel_Delegate::AppendContextMenu(CContextMenuInvoker &invoker)
{
  typedef CSingleCommandTemplate <CWellZoomInModel> TWellZoomInModelCommand;
  typedef CSingleCommandTemplate <CWellZoomInGUI> TWellZoomInGUICommand;

  CWellZoomInGUI* wellZoomInGUI = dynamic_cast <CWellZoomInGUI*> (
    GetGeomecDoc()->retrieveMatchingGUI(
      GetGeomecDoc()->GUI(), m_wellZoomInModel));

  invoker.AddCommand(_T("Switch to this model"),
    *new TWellZoomInGUICommand(*wellZoomInGUI, &CWellZoomInGUI::SwitchTo,
      &CWellZoomInGUI::CanSwitchTo));
  invoker.AddCommand(_T("Delete"),
    *new TWellZoomInModelCommand(*m_wellZoomInModel,
      &CWellZoomInModel::DestroyZoomInModel));
  invoker.AddCommand(_T("&Attributes"), *(new TGraphNode_DelegateCommand(
    *this, &CGraphNode_Delegate::Attributes)));
}
