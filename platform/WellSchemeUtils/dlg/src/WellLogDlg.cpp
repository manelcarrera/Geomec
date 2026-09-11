// WellLogDlg.cpp : implementation file
//

#include "stdafx.h"
#include "wellschemeutils.h"
#include "WellLogDlg.h"



#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CWellLogDlg dialog
BEGIN_MESSAGE_MAP(CWellLogDlg, CDialog)
  //{{AFX_MSG_MAP(CWellLogDlg)
    // NOTE: the ClassWizard will add message map macros here
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()


CWellLogDlg::CWellLogDlg(const well::CWellLog *pLog, CWnd* pParent /*=NULL*/)
  : CDialog(CWellLogDlg::IDD, pParent), m_pWellLog(pLog)
{
  //{{AFX_DATA_INIT(CWellLogDlg)
  m_SndCut = _T("");
  m_AvgComp = _T("");
  //}}AFX_DATA_INIT

  
  
}


void CWellLogDlg::DoDataExchange(CDataExchange* pDX)
{
  
  CDialog::DoDataExchange(pDX);

  //{{AFX_DATA_MAP(CWellLogDlg)
  DDX_Control(pDX, IDC_LIST_WELLOG, m_WellLogCtrl);
  DDX_Text(pDX, IDC_STATIC_SNDCUT, m_SndCut);
  DDX_Text(pDX, IDC_STATIC_TOTAVGCOMP, m_AvgComp);
  //}}AFX_DATA_MAP


  
}

BOOL CWellLogDlg::OnInitDialog()
{
  CDialog::OnInitDialog();

  
  m_SndCut.Format("%f", m_pWellLog->SandCutOff());
  m_AvgComp.Format("%f", m_pWellLog->TotalAverageCompressibility());
  UpdateData(FALSE);

  int iColom = 0; 
  CRect rect;
  m_WellLogCtrl.GetClientRect(&rect);
  double width = rect.right / 8;

  m_WellLogCtrl.InsertColumn(iColom++, _T("Original TMD"), LVCFMT_LEFT, width);
  m_WellLogCtrl.InsertColumn(iColom++, _T("Original TVD"), LVCFMT_LEFT, width);
  m_WellLogCtrl.InsertColumn(iColom++, _T("Stretched TMD"), LVCFMT_LEFT, width);
  m_WellLogCtrl.InsertColumn(iColom++, _T("Stretched TVD"), LVCFMT_LEFT, width);
  m_WellLogCtrl.InsertColumn(iColom++, _T("Compressibility"), LVCFMT_LEFT, width);
  m_WellLogCtrl.InsertColumn(iColom++, _T("Shalyness"), LVCFMT_LEFT, width);
  m_WellLogCtrl.InsertColumn(iColom++, _T("Modulation"), LVCFMT_LEFT, width);
  m_WellLogCtrl.InsertColumn(iColom++, _T("Sand"), LVCFMT_LEFT, width);
  
  
  int nIndex;
  CString strValue;
  int i = 0;
  int j;

  for(j = 0; j < m_pWellLog->LogPointSize(); j++)
  {
    iColom = 0;
    nIndex = m_WellLogCtrl.InsertItem(i++, "");
    strValue.Format("%f", m_pWellLog->Depth(well::CWellLog::ORIGINAL_TMD, j));
    m_WellLogCtrl.SetItemText(nIndex, iColom++, strValue);

    strValue.Format("%f", m_pWellLog->Depth(well::CWellLog::ORIGINAL_TVD, j));
    m_WellLogCtrl.SetItemText(nIndex, iColom++, strValue);

    strValue.Format("%f", m_pWellLog->Depth(well::CWellLog::STRETCHED_TMD, j));
    m_WellLogCtrl.SetItemText(nIndex, iColom++, strValue);

    strValue.Format("%f", m_pWellLog->Depth(well::CWellLog::STRETCHED_TVD, j));
    m_WellLogCtrl.SetItemText(nIndex, iColom++, strValue);

    strValue.Format("%f", m_pWellLog->getCompressibility(j));
    m_WellLogCtrl.SetItemText(nIndex, iColom++, strValue);

    strValue.Format("%f", m_pWellLog->getShalyness(j));
    m_WellLogCtrl.SetItemText(nIndex, iColom++, strValue);

    if(m_pWellLog->RTagSize() > 0)
      strValue.Format("%f", m_pWellLog->Modulation(j));
    else
      strValue.Format("N.A.");
    m_WellLogCtrl.SetItemText(nIndex, iColom++, strValue);

    double depth = m_pWellLog->Depth(well::CWellLog::STRETCHED_TMD, j);
    if(m_pWellLog->SandLayerSize() == 0)
      strValue.Format("No"); // no sand at all...
    else if(m_pWellLog->NearestSandLayer(depth).second)
      strValue.Format("Yes");
    else
      strValue.Format("No");
    m_WellLogCtrl.SetItemText(nIndex, iColom++, strValue);
  }

  return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CWellLogDlg message handlers
