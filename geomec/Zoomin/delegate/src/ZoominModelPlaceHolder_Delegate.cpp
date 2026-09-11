#include "StdAfx.h"

#include "ZoominModelPlaceHolder_Delegate.h"
#include "FemAppDoc.h"
#include "ContextMenuInvoker.h"
#include "GeomecDoc.h"
#include "RenameNodeDlg.h"
#include "ModelBase_Delegate.h"

CZoominModelPlaceHolder_Delegate::CZoominModelPlaceHolder_Delegate(CZoominModelPlaceHolder* zoominModelPlaceHolder)
: COpenGLNode_Delegate(zoominModelPlaceHolder),
  m_zoominModelPlaceHolder(zoominModelPlaceHolder)
{
}

void CZoominModelPlaceHolder_Delegate::AppendContextMenu(CContextMenuInvoker& invoker)
{
  CModelBase_Delegate& modelDelegate = static_cast<CModelBase_Delegate&>(*m_zoominModelPlaceHolder->ChildModel().getDelegate());
  modelDelegate.AppendSwitchTo(invoker);

  typedef CSingleCommandTemplate <CFemAppDoc> TFemAppDocCommand;
  invoker.AddCommand(_T("&Delete"), *(new TFemAppDocCommand(
    *GetGeomecDoc(), &CFemAppDoc::OnContextDelete,
    &CFemAppDoc::OnContextCanDelete)));

  invoker.AddCommand(QObject::tr("Attributes").toStdString(), *new TGraphNode_DelegateCommand(*this, &CGraphNode_Delegate::Attributes));
}

bool CZoominModelPlaceHolder_Delegate::Attributes()
{
  CRenameNodeDlg dlg(*m_zoominModelPlaceHolder);
  return dlg.DoModal() == IDOK;
}
