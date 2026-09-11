#include "geomec.h"
#include "stdafx.h"

#include "AttriWellZoomInParentResults.h"
#include "ModelBase.h"

CAttriWellZoomInParentResultsDlg::CAttriWellZoomInParentResultsDlg(CModelBase &model, CWnd *pParent)
    : CDialog(IDD_ATTRI_WELLZOOMIN_PARENT_RESULTS, pParent), m_model(model),
      m_nSelection(int(model.ParentResultsDefinition())) {}

void CAttriWellZoomInParentResultsDlg::DoDataExchange(CDataExchange *pDX) { DDX_Radio(pDX, IDC_EQUAL, m_nSelection); }

void CAttriWellZoomInParentResultsDlg::OnOK() {
  UpdateData(TRUE);
  m_model.ParentResultsDefinition(CModelBase::TParentResultsDef(m_nSelection));
  CDialog::OnOK();
}
