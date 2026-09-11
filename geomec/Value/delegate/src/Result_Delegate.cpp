#include "stdafx.h"

#include "Result_Delegate.h"
#include "Result.h"
#include "3dmodel.h"
#include "ContextMenuInvoker.h"
#include "GUIBase.h"
#include "ResultExportdlg.h"
#include "GocadExportWrapper.h"
#include "GeomecDoc.h"

CResultGroup_Delegate::CResultGroup_Delegate(CResultGroup* resultGroup)
: CGraphNode_Delegate(resultGroup)
, m_resultGroup(resultGroup)
{
}

/* NOT OBSOLETE */ void CResultGroup_Delegate::AppendContextMenu(CContextMenuInvoker &invoker)
{
  typedef CSingleCommandTemplate <CResultGroup> TCommand;
  typedef CSingleCommandTemplate <CResultGroup_Delegate> TCommand_Delegate;

  invoker.AddCommand("&Export results",
    *(new TCommand_Delegate(*this, &CResultGroup_Delegate::Export,
      &CResultGroup_Delegate::CanExport)));

#ifdef RESULTS_IMPORT_KEYFILE
  CGeomecApp* pApp = (CGeomecApp*) (AfxGetApp());

  if (pApp->KeyFile().Unlocked())
  {
#endif
    invoker.AddCommand("&Import results",
      *(new TCommand_Delegate(*this, &CResultGroup_Delegate::Import,
        &CResultGroup_Delegate::CanImport)));
#ifdef RESULTS_IMPORT_KEYFILE
  }
#endif

  if (dynamic_cast <const C3DModel*> (&(m_resultGroup->Model())))
  {
    invoker.AddCommand("Export to Gocad",
      *(new TCommand_Delegate(*this, &CResultGroup_Delegate::ExportGocad,
        &CResultGroup_Delegate::CanExportGocad)));
  }
}

bool CResultGroup_Delegate::CanImport() const
{
  return m_resultGroup->CanImport();
}

void CResultGroup_Delegate::Import()
{
  //wjrx mantis 2976

  CGUIBase* gui = dynamic_cast <CGUIBase*> (GetGeomecDoc()->GUI());

  gui->ImportResults();
}

bool CResultGroup_Delegate::CanExport() const
{
  return m_resultGroup->CanExport();
}

void CResultGroup_Delegate::Export()
{
  CResultExportDlg dlg(*m_resultGroup);

  dlg.DoModal();
}

bool CResultGroup_Delegate::CanExportGocad() const
{
  return m_resultGroup->CanExportGocad();
}

void CResultGroup_Delegate::ExportGocad()
{
  CGocadExportWrapper gew;

  gew.Export(static_cast <CModelBase&> (m_resultGroup->Model()), m_resultGroup);
}

IResult_Delegate::IResult_Delegate(IResult* result)
: IValueComposite_Delegate(result)
, m_result(result)
{
}

/* NOT OBSOLETE */ void IResult_Delegate::AppendContextMenu(CContextMenuInvoker &invoker)
{
  typedef CSingleCommandTemplate <IResult_Delegate> TCommand;

  invoker.AddCommand("&Export results",
    *(new TCommand(*this, &IResult_Delegate::Export,
      &IResult_Delegate::CanExport)));
}

bool IResult_Delegate::CanExport() const
{
  return m_result->CanExport();
}

void IResult_Delegate::Export()
{
  CResultExportDlg dlg(*m_result, -1);

  dlg.DoModal();
}

IResultComponent_Delegate::IResultComponent_Delegate(
  IResultComponent* resultComponent)
: IValueComponent_Delegate(resultComponent)
, m_resultComponent(resultComponent)
{
}

void IResultComponent_Delegate::AppendContextMenu(CContextMenuInvoker &invoker)
{
  typedef CSingleCommandTemplate <IResultComponent_Delegate> TCommand;

  invoker.AddCommand("&Export results",
    *(new TCommand(*this, &IResultComponent_Delegate::Export,
      &IResultComponent_Delegate::CanExport)));
  invoker.AddSeparator();

  CGraphNode_Delegate::AppendContextMenu(invoker);
}

bool IResultComponent_Delegate::CanExport() const
{
  return m_resultComponent->CanExport();
}

void IResultComponent_Delegate::Export()
{
  CResultExportDlg dlg(m_resultComponent->Parent(),
    m_resultComponent->ComponentIndex());

  dlg.DoModal();
}

IScalarResultComponent_Delegate::IScalarResultComponent_Delegate(
  IScalarResultComponent* scalarResultComponent)
: IResultComponent_Delegate(scalarResultComponent)
, m_scalarResultComponent(scalarResultComponent)
{
}
