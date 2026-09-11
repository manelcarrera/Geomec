#include "stdafx.h"

#include "3DHorizon.h"
#include "3DHorizon_Delegate.h"
#include "AttriHexaHorizon.h"
#include "ContextMenuInvoker.h"
#include "TNOFileDialog.h"

C3DHorizon_Delegate::C3DHorizon_Delegate(C3DHorizon *a3dHorizon)
    : CHorizonBase_Delegate(a3dHorizon), m_3dHorizon(a3dHorizon) {}

bool C3DHorizon_Delegate::Attributes() {
  CAttri3DHorizonDlg dlg(*m_3dHorizon);

  if (dlg.DoModal() == IDOK) {
    // If not linked to entry

    if (!m_3dHorizon->IsLinkedTo(*m_3dHorizon->Model().GraphEntry(MD_BASE_HORIZON))) {
      // Link to entry ..

      assert(m_3dHorizon->Model().GraphEntry(MD_BASE_HORIZON));
      m_3dHorizon->LinkTo(*m_3dHorizon->Model().GraphEntry(MD_BASE_HORIZON));

      CModelBase &model = dynamic_cast<CModelBase &>(m_3dHorizon->Model());

      model.InvalidateMesh();
    }

    m_3dHorizon->UpdateFaultParameters();
    m_3dHorizon->Modified();

    return true;
  }

  return false;
}

void C3DHorizon_Delegate::AppendContextMenu(CContextMenuInvoker &invoker) {
  typedef CSingleCommandTemplate<C3DHorizon_Delegate> THorizonCommand;

  if (m_3dHorizon->ConstantDepth()) {
    invoker.AddCommand(_T("&Export constant depth surface"),
                       *(new THorizonCommand(*this, &C3DHorizon_Delegate::ExportConstantDepthSurface)));
    invoker.AddSeparator();
  }

  CColorNode_Delegate::AppendContextMenu(invoker);
}

void C3DHorizon_Delegate::ExportConstantDepthSurface() {
  QString sFileName;
  QString sName = m_3dHorizon->Name();
  int idxdot = sName.indexOf('.');

  if (idxdot >= 0) {
    sName = sName.left(idxdot);
  }

  sFileName = QString("%1.ts").arg(sName);

  CTnoFileDialog dlg(FALSE, "ts", sFileName.toStdString().c_str(), OFN_OVERWRITEPROMPT, "GoCad surface (*.ts)|*.ts||");

  if (dlg.DoModal() == IDOK) {
    m_3dHorizon->ExportConstantDepthSurface((LPCSTR)dlg.GetPathName());
  }
}
