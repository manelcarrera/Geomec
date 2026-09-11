#include "stdafx.h"

#include "ContextMenuInvoker.h"
#include "GocadElementSet.h"
#include "GocadElementSet_Delegate.h"
#include "SelectGocadSolidDlg.h"

namespace {

#ifdef SKUA_NEW
bool selectGocadSolidDlg(const std::vector<const gm_skua::SKUAParseData *> &solids, std::vector<int> &vcSolids)
#else
bool selectGocadSolidDlg(const std::vector<QSharedPointer<CGocadData::CTSolid>> &solids, std::vector<int> &vcSolids)
#endif
{
  CSelectGocadSolidDlg dlg(solids);

  if (dlg.DoModal() != IDOK) {
    return false;
  }

  vcSolids.clear();
  for (int i = 0; i < dlg.SelectedSize(); ++i)
    vcSolids.push_back(dlg.SelectedEntry(i));

  return true;
}

} // anonymous namespace

CGocadElementSet_Delegate::CGocadElementSet_Delegate(CGocadElementSet *gocadElementSet)
    : CElementSet_Delegate(gocadElementSet), m_gocadElementSet(gocadElementSet) {}

void CGocadElementSet_Delegate::AppendContextMenu(CContextMenuInvoker &invoker) {
  invoker.AddCommand(_T("Import in Gocad model"), *(new CSingleCommandTemplate<CGocadElementSet_Delegate>(
                                                      *this, &CGocadElementSet_Delegate::importInGocadModel,
                                                      &CGocadElementSet_Delegate::canImportInGocadModel)));
  invoker.AddSeparator();

  CElementSet_Delegate::AppendContextMenu(invoker);
}

bool CGocadElementSet_Delegate::canImportInGocadModel() const { return m_gocadElementSet->canImportInGocadModel(); }

void CGocadElementSet_Delegate::importInGocadModel() { m_gocadElementSet->importInGocadModel(selectGocadSolidDlg); }
