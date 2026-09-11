#include "stdafx.h"

#include "ValueType_Delegate.h"
#include "ValueType.h"
#include "AttriValueType.h"
#include "ContextMenuInvoker.h"
#include "IPointSet_Delegate.h"
#include "IPointSet.h"

CValueType_Delegate::CValueType_Delegate(CValueType* valueType)
: IValueComposite_Delegate(valueType)
, m_valueType(valueType)
{
}

bool CValueType_Delegate::Attributes()
{
  CAttriValueType dlg(*m_valueType);

  return dlg.DoModal() == IDOK;
}

void CValueType_Delegate::AppendContextMenu(CContextMenuInvoker &invoker)
{
  invoker.AddCommand(_T("&Export"),
    *(new CSingleCommandTemplate <CValueType_Delegate> (*this,
      &CValueType_Delegate::Export, &CValueType_Delegate::CanExport)));
  invoker.AddSeparator();

  IValueComposite_Delegate::AppendContextMenu(invoker);
}

bool CValueType_Delegate::CanDestroy() const
{
  return m_valueType->CanDestroy();
}

bool CValueType_Delegate::Destroy()
{
  return m_valueType->Destroy();
}

bool CValueType_Delegate::CanExport() const
{
  return m_valueType->CanExport();
}

void CValueType_Delegate::Export()
{
  dynamic_cast <IPointSet_Delegate*> (
    m_valueType->PointSet().getDelegate())->Export();
}
