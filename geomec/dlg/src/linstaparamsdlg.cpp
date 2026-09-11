// linstaparamsdlg.cpp : implementation file
//

#include "stdafx.h"
#include "geomec.h"
#include "linstaparamsdlg.h"
#include "ModelBase.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CLinstaParamsDlg dialog


CLinstaParamsDlg::CLinstaParamsDlg(CModelBase& model, CWnd* pParent /*=NULL*/)
: CDialog(CLinstaParamsDlg::IDD, pParent),
  m_model(model)
{
  //{{AFX_DATA_INIT(CLinstaParamsDlg)
  //}}AFX_DATA_INIT
  m_iMaxIter = model.IniStressMaxIterations();
  m_dTolcon = model.IniStressConvCriterion();
}


void CLinstaParamsDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CLinstaParamsDlg)
  DDX_Text(pDX, IDC_ED_MAXITE, m_iMaxIter);
  DDX_Text(pDX, IDC_ED_TOLCON, m_dTolcon);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLinstaParamsDlg, CDialog)
  //{{AFX_MSG_MAP(CLinstaParamsDlg)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLinstaParamsDlg message handlers

void CLinstaParamsDlg::OnOK() 
{
  if(!UpdateData(TRUE))
    return;

  m_model.IniStressMaxIterations(m_iMaxIter);
  m_model.IniStressConvCriterion(m_dTolcon);
  m_model.LinstaOnlyInitialStress(false);
  
  CDialog::OnOK();
}
