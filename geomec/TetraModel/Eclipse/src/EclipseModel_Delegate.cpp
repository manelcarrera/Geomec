#include "stdafx.h"

#include "EclipseModel_Delegate.h"
#include "EclipseModel.h"
#include "ContextMenuInvoker.h"
#include "EclipseGUI.h"

CEclipseSubModelEntry_Delegate::CEclipseSubModelEntry_Delegate(
  CEclipseSubModelEntry* eclipseSubModelEntry)
: TSubModelEntry_Delegate(eclipseSubModelEntry)
, m_eclipseSubModelEntry(eclipseSubModelEntry)
{
}

void CEclipseSubModelEntry_Delegate::AppendContextMenu(
  CContextMenuInvoker &invoker)
{
  typedef CSingleCommandTemplate <CEclipseSubModelEntry_Delegate> TEntryCommand;

  invoker.AddCommand(_T("&Edit eclipse model"),
  *(new TEntryCommand(*this, &CEclipseSubModelEntry_Delegate::EditSubModel)));
}

void CEclipseSubModelEntry_Delegate::EditSubModel()
{
  CEclipseGUI& model =
  (CEclipseGUI&) (*m_eclipseSubModelEntry->Model().getDelegate());

  model.EditSubModel();
}
