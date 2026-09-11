#include "stdafx.h"

#include "AttriPointSetDlg.h"
#include "ContextMenuInvoker.h"
#include "ElementSet.h"
#include "ElementSet_Delegate.h"
#include "ExportDlg.h"
#include "FemAppMainWindow.h"
#include "GeomecDoc.h"
#include "ModelBase.h"
#include "TNOFileDialog.h"

IElementSet_Delegate::IElementSet_Delegate(IElementSet *elementSet)
    : IPointSet_Delegate(elementSet), m_elementSet(elementSet) {}

void IElementSet_Delegate::Export() {
  CGeomecDoc &doc = *GetGeomecDoc();
  IExportFormat::CExportArg arg(doc.UnitNode().Unit());
  const CModelBase &model = (CModelBase &)m_elementSet->Model();

  CTnoFileDialog dlg(FALSE, "dat", m_elementSet->Name().toStdString().c_str(), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                     "ASCII Files (*.dat;*.txt;*.xyz)|*.dat;*.txt;*.xyz|"
                     "Excel Files (*.xls;*.xls*)|*.xls;*.xls*||",
                     FemAppGetMainWnd());

  if (dlg.DoModal() == IDOK) {
    m_elementSet->Export(arg, (LPCSTR)dlg.GetPathName());
  }
}

CElementSet_Delegate::CElementSet_Delegate(CElementSet *elementSet)
    : IElementSet_Delegate(elementSet), m_elementSet(elementSet) {}

bool CElementSet_Delegate::Attributes() {
  CAttriPointSetTemplate<CElementSet> dlg(*m_elementSet);

  if (dlg.DoModal() == IDOK) {
    m_elementSet->Modified(); // wjrx mantis 3335

    return true;
  } else {
    return false;
  }
}

void CElementSet_Delegate::AppendContextMenu(CContextMenuInvoker &invoker) {
  invoker.AddCommand(_T("A&ssign to model boundary"),
                     *(new CSingleCommandTemplate<CElementSet>(*m_elementSet, &CElementSet::AssignToBoundary,
                                                               &CElementSet::CanAssignToBoundary)));
  invoker.AddCommand(_T("&Export"),
                     *(new CSingleCommandTemplate<CElementSet_Delegate>(*this, &CElementSet_Delegate::Export)));
  invoker.AddSeparator();

  IElementSet_Delegate::AppendContextMenu(invoker);
}
