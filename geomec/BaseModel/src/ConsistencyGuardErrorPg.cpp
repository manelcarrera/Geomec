#include "stdafx.h"

#include "ConsistencyGuardErrorPg.h"
#include "ModelBase.h"
#include "resource.h"


CConsistencyGuardErrorPg::CConsistencyGuardErrorPg(CModelBase *pModel)
  : CPropertyPage(IDD_CG_ERROR_PG)
  , m_pModel(pModel)
{
}

BEGIN_MESSAGE_MAP(CConsistencyGuardErrorPg, CDialog)
  ON_BN_CLICKED(IDC_CG_ERROR_PG_CLEAR, OnClearErrors)
END_MESSAGE_MAP()


BOOL CConsistencyGuardErrorPg::OnInitDialog()
{
  UpdateData(false);

  return TRUE;
}

void CConsistencyGuardErrorPg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);

  QStringList errors;
  m_pModel->GetConsistencyGuard()->GetErrors(errors);

  CEdit *canvas = (CEdit *)GetDlgItem(IDC_CG_ERROR_PG_ERRORS);

  if (errors.size() > 0)
  canvas->SetWindowText(errors.join("\r\n").toStdString().c_str());
  else
  canvas->SetWindowText("No errors");
}

void CConsistencyGuardErrorPg::OnClearErrors()
{
  m_pModel->GetConsistencyGuard()->ClearErrors();
  UpdateData(false);
}

