#include "stdafx.h"

#include "ExportDlg.h"
#include "FemAppMainWindow.h"
#include "FormationBase_Delegate.h"
#include "GeomecDoc.h"
#include "TNOFileDialog.h"

IFormationElementSet_Delegate::IFormationElementSet_Delegate(IFormationElementSet *formationElementSet)
    : IElementSet_Delegate(formationElementSet), m_formationElementSet(formationElementSet) {}

CFormationBase_Delegate::CFormationBase_Delegate(CFormationBase *formationBase)
    : CMaterialServerParent_Delegate(formationBase), m_formationBase(formationBase) {}

void CFormationBase_Delegate::AppendContextMenu(CContextMenuInvoker &invoker) {
  invoker.AddCommand(_T("&Export"), *(new CSingleCommandTemplate<CFormationBase_Delegate>(
                                        *this, &CFormationBase_Delegate::Export, &CFormationBase_Delegate::CanExport)));
  invoker.AddSeparator();

  TBase::AppendContextMenu(invoker);
}

bool CFormationBase_Delegate::CanExport() const { return m_formationBase->CanExport(); }

void CFormationBase_Delegate::Export() {
  CGeomecDoc &doc = *GetGeomecDoc();
  IExportFormat::CExportArg arg(doc.UnitNode().Unit());
  const CModelBase &model = (CModelBase &)m_formationBase->Model();

  // Insert elements

  CTnoFileDialog dlg(FALSE, "dat", m_formationBase->Name().toStdString().c_str(),
                     OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                     "ASCII Files (*.dat;*.txt;*.xyz)|"
                     "*.dat;*.txt;*.xyz|Excel Files (*.xls;*.xls*)|*.xls;*.xls*||",
                     FemAppGetMainWnd());

  if (dlg.DoModal() == IDOK) {
    m_formationBase->Export((LPCSTR)dlg.GetPathName(), arg);
  }
}

bool CFormationBase_Delegate::CanDestroy() const { return m_formationBase->CanDestroy(); }

CFormationThickness_Delegate::CFormationThickness_Delegate(CFormationThickness *formationThickness)
    : IValueComposite_Delegate(formationThickness), m_formationThickness(formationThickness) {}

TFormationBaseEntry_Delegate::TFormationBaseEntry_Delegate(TFormationBaseEntry *formationBaseEntry)
    : CGraphEntryTemp_Delegate<CFormationBase>(formationBaseEntry), m_formationBaseEntry(formationBaseEntry) {}
