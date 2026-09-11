#include "StdAfx.h"
#include "AttriGroupNode.h"

#include "resource.h"

CAttriGroupNode::CAttriGroupNode(CGroupNode& groupNode, CWnd* pParent)
  : CAttributesTemplate<CGroupNode>(IDD_ATTRI_GROUPNODE_DLG, groupNode, pParent)
{
}

BOOL CAttriGroupNode::OnInitDialog()
{
  CAttributesTemplate<CGroupNode>::OnInitDialog();
  UpdateControls();
  return TRUE;
}

void CAttriGroupNode::DoDataExchange(CDataExchange* pDX)
{
  CString	strName;
  CString strDescription;
  int nReadOnly;

  CAttributesTemplate<CGroupNode>::DoDataExchange(pDX);

  if (!pDX->m_bSaveAndValidate)
  {
    strName = Copy().Name().toStdString().c_str();
    strDescription = Copy().Description().toStdString().c_str();
    nReadOnly = Copy().ReadOnly() ? 1 : 0;
  }

  DDX_Text(pDX, IDC_ED_NAME, strName);
  DDX_Text(pDX, IDC_ED_DESCRIPTION1, strDescription);
  DDX_Check(pDX, IDC_CHECK_READONLY, nReadOnly);

  if (pDX->m_bSaveAndValidate)
  {
    Copy().Name((LPCSTR)strName);
    Copy().Description((LPCSTR)strDescription);
    Copy().ReadOnly(nReadOnly);
  }
}

BEGIN_MESSAGE_MAP(CAttriGroupNode, CDialog)
END_MESSAGE_MAP()

void CAttriGroupNode::UpdateControls()
{
  UpdateData(FALSE);
}
