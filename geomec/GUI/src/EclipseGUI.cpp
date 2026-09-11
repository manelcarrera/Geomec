#include "stdafx.h"

#include "AttriEclipseSubModel.h"
#include "EclipseGUI.h"
#include "EclipseModel.h"
#include "ModelLifetimeFacade.h"
#include "TnoFileDialog.h"

CEclipseGUI::CEclipseGUI(CFemAppGUI *parentGUI, CEclipseModel *eclipseModel)
    : CTetraGUI(parentGUI, eclipseModel), m_eclipseModel(eclipseModel) {}

bool CEclipseGUI::NewModel(bool bAttachToDocument) {
  // We start with a file dialog where the user can select
  // the eclipse file he want to open.

  CTnoFileDialog file_dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                          "Eclipse Files (*.grid;*.ecl)|*.grid; *.ecl|All Files (*.*)|*.*||");

  if (file_dlg.DoModal() == IDOK) {
    m_eclipseModel->setFileName((LPCSTR)file_dlg.GetPathName());
  } else {
    return false; // File dialog is cancled
  }

  if (m_eclipseModel->NewModel(bAttachToDocument)) {
    if (!EditSubModel()) {
      IModelLifetimeFacade::Close(m_eclipseModel);
      return false;
    }
  } else {
    return false;
  }

  return true;
}

void CEclipseGUI::OnCloseModel() { CTetraGUI::OnCloseModel(); }

bool CEclipseGUI::EditSubModel() {
  CAttriEclipseSubModel dlg(*m_eclipseModel);

  if (dlg.DoModal() == IDOK) {
    m_eclipseModel->CreateSubBoundary();

    return true;
  }

  return false;
}

CEclipseModel *CEclipseGUI::model() const { return m_eclipseModel; }

bool CEclipseGUI::LoadPre381Stream(CStorageNode::TSTREAM &stream, CStreamVersion &version,
                                   CStorageNode::TPROGRESS &prog) {
  if (!CTetraGUI::LoadPre381Stream(stream, version, prog))
    return false;

  return m_eclipseModel->LoadEclipse(stream, version, prog);
}
