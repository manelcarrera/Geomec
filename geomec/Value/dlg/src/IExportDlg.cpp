#include "StdAfx.h"

#include "IExportDlg.h"

IExportDlg::IExportDlg(unsigned int nID, CWnd *pParent) : CDialog(nID, pParent) {
  m_selection.RegisterObserver(*new CSelectionObserver(*this));
}

COpenGLNodeSelection &IExportDlg::Selection() { return m_selection; }

const COpenGLNodeSelection &IExportDlg::Selection() const { return m_selection; }

/////
IExportDlg::CSelectionObserver::CSelectionObserver(IExportDlg &dlg) : m_dlg(dlg) {}

void IExportDlg::CSelectionObserver::OnNodeErased(const COpenGLNode &node) { m_dlg.OnSelect(node); }

void IExportDlg::CSelectionObserver::OnNodeInserted(const COpenGLNode &node) { m_dlg.OnSelect(node); }
