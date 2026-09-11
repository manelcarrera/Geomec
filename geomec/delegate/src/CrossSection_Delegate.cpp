#include "stdafx.h"

#include "3DGUI.h"
#include "3dmodel.h"
#include "BoundaryBase.h"
#include "ContextMenuInvoker.h"
#include "CrossSection_Delegate.h"
#include "GUIFactory.h"
#include "GeomecDoc.h"
#include "IProgressFactory.h"
#include "ISceneWrapper.h"
#include "TnoFileDialog.h"
#include "attrixsecdlg.h"
#include "geoprogress.h"

CCrossSection_Delegate::CCrossSection_Delegate(CCrossSection *crossSection)
    : CStorageNode_Delegate(crossSection), m_crossSection(crossSection) {}

bool CCrossSection_Delegate::Attributes() {
  CAttriXSecDlg dlg(*m_crossSection);

  return dlg.DoModal() == IDOK;
}

void CCrossSection_Delegate::AppendContextMenu(CContextMenuInvoker &invoker) {
  // Add Change position ...

  typedef CSingleCommandTemplate<CCrossSection> TCommand;

  invoker.AddCommand(_T("Flip"), *(new TCommand(*m_crossSection, &CCrossSection::Flip, &CCrossSection::CanFlip)));
  invoker.AddSeparator();

  // Add default menu

  CStorageNode_Delegate::AppendContextMenu(invoker);
}

CCrossSection_Delegate::CCSObject_Delegate::CCSObject_Delegate(CCrossSection::CCSObject *csObject)
    : COpenGLNode_Delegate(csObject), m_csObject(csObject) {}

CCrossSection_Delegate::CCSFormation_Delegate::CCSFormation_Delegate(CCrossSection::CCSFormation *csFormation)
    : CCSObject_Delegate(csFormation), m_csFormation(csFormation) {}

CCrossSection_Delegate::CCSWellPath_Delegate::CCSWellPath_Delegate(CCrossSection::CCSWellPath *csWellPath)
    : CCSObject_Delegate(csWellPath), m_csWellPath(csWellPath) {}

CCrossSectionEntry_Delegate::CCrossSectionEntry_Delegate(CCrossSectionEntry *crossSectionEntry)
    : CStorageNodeEntry_Delegate<CCrossSection>(crossSectionEntry), m_crossSectionEntry(crossSectionEntry) {}

void CCrossSectionEntry_Delegate::AppendContextMenu(CContextMenuInvoker &invoker) {
  // We have two inserts: Graphical and By value

  CContextMenuInvoker *pSubMenu = new CContextMenuInvoker;

  typedef CSingleCommandTemplate<CCrossSectionEntry_Delegate> TCrossSectionCommand_Delegate;

  invoker.AddCommand(_T("Insert cross section"),
                     *(new TCrossSectionCommand_Delegate(*this, &CCrossSectionEntry_Delegate::InsertAny,
                                                         &CCrossSectionEntry_Delegate::CanInsertNew)));
}

bool CCrossSectionEntry_Delegate::CanInsertNew() const {
  bool num_horiz = m_crossSectionEntry->Model().GraphEntry(MD_BASE_HORIZON)->GraphEntryNodes().size();

  return true; // FIXME
  return m_crossSectionEntry->Model().GraphEntry(MD_BASE_HORIZON)->GraphEntryNodes().size() > 1;
}

void CCrossSectionEntry_Delegate::InsertGraphical() {}

void CCrossSectionEntry_Delegate::InsertAny() {
  CModelBase &model = static_cast<CModelBase &>(m_crossSectionEntry->Model());

  geo::CPoint center = .5 * (model.Boundary().Min() + model.Boundary().Max());
  double comp = 1.0 / sqrt(3.0);
  geo::CVector normal(comp, comp, comp);

  CCrossSection *pXSec = new CCrossSection("New cross section", m_crossSectionEntry->Model(), center, normal);
  CCrossSection_Delegate *pXSec_Delegate = new CCrossSection_Delegate(pXSec);

  assert(pXSec);
  assert(pXSec_Delegate);

  if (!pXSec_Delegate->Attributes()) {
    delete pXSec_Delegate;
    delete pXSec;
  }
}
