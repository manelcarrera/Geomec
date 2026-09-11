#include "stdafx.h"

#include "DerivedResult_Delegate.h"
#include "DerivedResult.h"
#include "RpnDlg.h"
#include "ResultTree.h"
#include "3dmodel.h"
#include "MeasuredTopDisplacementsNode.h"
#include "RpnFormationOperand.h"
#include "RpnValueSet.h"
#include "GlobalMessage.h"
#include "ContextMenuInvoker.h"
#include "resultexportdlg.h"
#include "BaseEntryTypes.h"

CDerivedResult_Delegate::CDerivedResult_Delegate(CDerivedResult* derivedResult)
: IResult_Delegate(derivedResult)
, m_derivedResult(derivedResult)
{
}

bool CDerivedResult_Delegate::Attributes()
{
  // We could have a new result

  IValueComponentBase& valueComponent = m_derivedResult->Component(0, 0);
  IValueComponent_Delegate* valueComponent_Delegate = 0;
  std::string name = typeid(valueComponent).name();

  if (name == "class CDerivedResult::CDerivedResultComponent")
  {
  CDerivedResult::CDerivedResultComponent& anotherValueComponent =
      static_cast <CDerivedResult::CDerivedResultComponent&> (valueComponent);

  valueComponent_Delegate = static_cast <
      CDerivedResult_Delegate::CDerivedResultComponent_Delegate*> (
    anotherValueComponent.getDelegate());
  }
  else
  {
  assert(false);

  valueComponent_Delegate = static_cast <IValueComponent_Delegate*> (
      valueComponent.getDelegate());
  }

  bool bRet = valueComponent_Delegate->Attributes();

  if (bRet && !m_derivedResult->StoreOnFile())
  {
  m_derivedResult->SaveToRegistry();
  }

  return bRet;
}

void CDerivedResult_Delegate::AppendContextMenu(CContextMenuInvoker &invoker)
{
  typedef CSingleCommandTemplate <CDerivedResult_Delegate> TCommand;

  invoker.AddCommand("&Export results",
  *(new TCommand(*this, &CDerivedResult_Delegate::Export,
      &CDerivedResult_Delegate::CanExport)));
  invoker.AddSeparator();

  IValueComposite_Delegate::AppendContextMenu(invoker);
}

bool CDerivedResult_Delegate::CanExport() const
{
  return m_derivedResult->CanExport();
}

void CDerivedResult_Delegate::Export()
{
  CResultExportDlg dlg(*m_derivedResult, -1);

  dlg.DoModal();
}

bool CDerivedResult_Delegate::Destroy()
{
  return m_derivedResult->Destroy();
}

CDerivedResult_Delegate::CDerivedResultComponent_Delegate::
  CDerivedResultComponent_Delegate(
  CDerivedResult::CDerivedResultComponent* derivedResultComponent)
: IResultComponent_Delegate(derivedResultComponent)
, m_derivedResultComponent(derivedResultComponent)
{
}

bool CDerivedResult_Delegate::CDerivedResultComponent_Delegate::Attributes()
{
  CDerivedResult& result =
  dynamic_cast <CDerivedResult&> (m_derivedResultComponent->Parent());

  // Create index in registry

  if (0 > result.Index())
  {
  if (!result.RegisterResult())
  {
      // Failure. Too many registry result

      QString sMessage;

      if (result.StoreOnFile())
      {
    sMessage = QString("The maximum of %1 derived results is reached.").arg(COMPOSITE_BASE - DERIVED_RESULT_BASE);
      }
      else
      {
    sMessage = QString("The maximum of %1 registry results is reached.").arg(DERIVED_RESULT_BASE - REGISTRY_RESULT_BASE);
      }

      _m()->msg(sMessage);

      return false;
  }
  }

  // Do we have a stack?

  if (m_derivedResultComponent->Stack() == 0)
  {
  m_derivedResultComponent->CreateStack();

  // Insert X, Y, Z coordinates

  CModelBase& model =
      dynamic_cast <CModelBase&> (m_derivedResultComponent->Model());

  new CDerivedResult::CResultCoordinateProxy(IDS_RC_NORTHING,
      *m_derivedResultComponent->Stack(),
      CDerivedResult::CResultCoordinateProxy::CO_X);
  new CDerivedResult::CResultCoordinateProxy(IDS_RC_EASTING,
      *m_derivedResultComponent->Stack(),
      CDerivedResult::CResultCoordinateProxy::CO_Y);
  new CDerivedResult::CResultCoordinateProxy(IDS_RC_DEPTH,
      *m_derivedResultComponent->Stack(),
      CDerivedResult::CResultCoordinateProxy::CO_Z);

  C3DModel* p3DModel = dynamic_cast <C3DModel*> (&m_derivedResultComponent->Model());

  if (p3DModel && result.StoreOnFile())
  {
      // measured top displacements

      new CRpnTopDisplacementProxy <0, IDS_MEASURED_TOP_DISPL_N> (*p3DModel,
    *m_derivedResultComponent->Stack());
      new CRpnTopDisplacementProxy <1, IDS_MEASURED_TOP_DISPL_E> (*p3DModel,
    *m_derivedResultComponent->Stack());
      new CRpnTopDisplacementProxy <2, IDS_MEASURED_TOP_DISPL_D> (*p3DModel,
    *m_derivedResultComponent->Stack());
  }

  // Insert an IsReservoir

  new CRpnReservoirProxy(model.Mesh(), *m_derivedResultComponent->Stack());

  if (result.StoreOnFile())
  {
      // value sets

      const TPointSetEntry& ps_entry = static_cast <const TPointSetEntry&> (
    *m_derivedResultComponent->Model().GraphEntry(MD_BASE_POINTSET));
      const TPointSetEntry::TNodeSet& stPointSetNodes = ps_entry.EntryNodes();
      TPointSetEntry::TNodeSet::const_iterator it;

      for (it = stPointSetNodes.begin(); it != stPointSetNodes.end(); ++it)
      {
    IPointSet& ps = **it;
    int i;
    int dim = int(ps.Dimension()); // skip the coordinate values

    for (i = dim; i < ps.ValueSetSize(); ++i)
    {
          IValueSet& vs = ps.ValueSet(i);

          if (!vs.Component()) // valueset is not referenced
          {
      new CRpnValueSet::CValueSetProxy(ps.ValueSet(i),
              *m_derivedResultComponent->Stack());
          }
    }
      }
  }
  }

  // Update the formations

  if (result.StoreOnFile())
  {
  m_derivedResultComponent->UpdateFormations();
  }

  // Do the dialog

  CRpnDlg dlg(*m_derivedResultComponent);

  return dlg.DoModal() == IDOK;
}

void CDerivedResult_Delegate::CDerivedResultComponent_Delegate::
  AppendContextMenu(CContextMenuInvoker &invoker)
{
}

CDerivedResultGroup_Delegate::CDerivedResultGroup_Delegate(
  CDerivedResultGroup* derivedResultGroup)
: CResultGroup_Delegate(derivedResultGroup)
, m_derivedResultGroup(derivedResultGroup)
{
}

void CDerivedResultGroup_Delegate::AppendContextMenu(CContextMenuInvoker &invoker)
{
  typedef CSingleCommandTemplate <CDerivedResultGroup_Delegate> TCommand;

  invoker.AddCommand(_T("&Create result"),
  *(new TCommand(*this,
      &CDerivedResultGroup_Delegate::OnCreateDerivedResult)));
}

void CDerivedResultGroup_Delegate::OnCreateDerivedResult()
{
  // Create a result

  CDerivedResult* pNewResult = new CDerivedResult(*m_derivedResultGroup);
  CDerivedResult_Delegate* pNewResult_Delegate =
  new CDerivedResult_Delegate(pNewResult);

  if (!pNewResult_Delegate->Attributes())
  {
  delete pNewResult_Delegate;
  delete pNewResult;
  return;
  }

  m_derivedResultGroup->Modified();
}
