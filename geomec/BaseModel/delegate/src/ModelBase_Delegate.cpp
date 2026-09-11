#include "stdafx.h"

#include "ModelBase_Delegate.h"
#include "ModelBase.h"
#include "ContextMenuInvoker.h"
#include "GUIBase.h"
#include "GeomecDoc.h"
#include "RenameNodeDlg.h"

CModelBase_Delegate::CModelBase_Delegate(CModelBase* modelBase)
: CFemAppModel_Delegate(modelBase)
, m_modelBase(modelBase)
{
}

bool CModelBase_Delegate::IsMesh() const
{
  return m_modelBase->IsMesh();
}

void CModelBase_Delegate::AppendSwitchTo(CContextMenuInvoker& invoker)
{
  assert(m_modelBase->parentModel());

  typedef CSingleCommandTemplate <CGUIBase> TGUIBaseCommand;

  CGUIBase* GUI = static_cast <CGUIBase*> (
    GetGeomecDoc()->retrieveMatchingGUI(
      GetGeomecDoc()->GUI(), m_modelBase));

  invoker.AddCommand(_T("Switch to this model"),
    *new TGUIBaseCommand(*GUI, &CGUIBase::SwitchTo,
      &CGUIBase::CanSwitchTo));
}
