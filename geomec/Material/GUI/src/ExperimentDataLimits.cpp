// ExperimentDataLimits.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "TabExperiment.h"
#include "ExperimentDataLimits.h"


// CExperimentDataLimits dialog

IMPLEMENT_DYNAMIC(CExperimentDataLimits, CDialog)

CExperimentDataLimits::CExperimentDataLimits
(CExperimentArray &experimentData
, CTabExperiment *tabExperiment
, CWnd* pParent /*=NULL*/
)
: CDialog(CExperimentDataLimits::IDD, pParent)
, m_iLowerLimit(0)
, m_iUpperLimit(100)
, m_iLowerRange(0)
, m_iUpperRange(100) // from 0-100%
, m_vcExperimentData(experimentData)
, m_iExperimentDataIndex(LB_ERR) // no selection
, m_pTabExperiment(tabExperiment)
, m_iInitialLowerLimit(experimentData.size())
{
}

CExperimentDataLimits::~CExperimentDataLimits()
{
}

void CExperimentDataLimits::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
}

// Slider:
// http://www.functionx.com/visualc/controls/slider.htm
// http://people.inf.elte.hu/nacsa/Lectures/Milliomos1%20munkafuzet.pdf p.3
//
BEGIN_MESSAGE_MAP(CExperimentDataLimits, CDialog)
  ON_WM_HSCROLL()
  ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_LIMIT_1, OnSliderReleased) 
  ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_LIMIT_2, OnSliderReleased) 
  ON_LBN_SELCHANGE(IDC_LIST_EXPERIMENT, OnSelchangeExperiments)
END_MESSAGE_MAP()

#ifdef min
#undef min  // To give std::min precedence over #define min(...)
#endif  // min

#ifdef max
#undef max  // To give std::max precedence over #define max(...)
#endif  // max

// CExperimentDataLimits message handlers
BOOL CExperimentDataLimits::OnInitDialog()
{
  CDialog::OnInitDialog();

  CSliderCtrl *s1= (CSliderCtrl *)GetDlgItem(IDC_SLIDER_LIMIT_1);
  CSliderCtrl *s2= (CSliderCtrl *)GetDlgItem(IDC_SLIDER_LIMIT_2);
  CEdit *e1= (CEdit *)GetDlgItem(IDC_EDIT_LIMIT_1_VALUE);
  CEdit *e2= (CEdit *)GetDlgItem(IDC_EDIT_LIMIT_2_VALUE);

  s1->SetRange(m_iLowerRange,m_iUpperRange,TRUE);
  s1->SetPos(m_iLowerRange);

  s2->SetRange(m_iLowerRange,m_iUpperRange,TRUE);
  s2->SetPos(m_iUpperRange);

  CString strText;
  strText.Format("%d", m_iLowerRange); e1->SetWindowText(strText);
  strText.Format("%d", m_iUpperRange); e2->SetWindowText(strText);

  CListBox *pListBox= (CListBox *)GetDlgItem(IDC_LIST_EXPERIMENT);
  for (size_t ii= 0; ii< m_vcExperimentData.size(); ++ii)
  {
    pListBox->AddString(m_vcExperimentData[ii].GetLabel());
  }

  // user friendliness
  // if there is a single item, select it by default
  //
  if (m_vcExperimentData.size() == 1)
  {
    pListBox->SetCurSel(0);
    OnSelchangeExperiments();
  }
  else
  {
    // user friendliness
    // if there are items selected for display in graph, select one
    //
    for (int ii= 0, iSelected= -1
        ; ii< m_vcExperimentData.size() && iSelected < 0
        ; ++ii)
    {
      if (m_vcExperimentData[ii].Selected())
      {
        iSelected= ii;
        pListBox->SetCurSel(ii);
        OnSelchangeExperiments();
        break;
      }
      else
      {
        GetDlgItem(IDC_SLIDER_LIMIT_1)->EnableWindow(FALSE);
        GetDlgItem(IDC_SLIDER_LIMIT_2)->EnableWindow(FALSE);
      }
    }
  }

  OnSliderReleased(NULL,NULL);
  return TRUE;
}

void CExperimentDataLimits::OnSliderReleased(NMHDR* pNMHDR, LRESULT* pResult)
{
  for (size_t s = 0; s < m_vcExperimentData.size(); ++s)
  {
    m_iInitialLowerLimit[s] = std::min(
      100 * m_vcExperimentData[s].getLowerLimitFraction(),
      100 * m_vcExperimentData[s].getUpperLimitFraction());
  }

  CSliderCtrl *s1= (CSliderCtrl *)GetDlgItem(IDC_SLIDER_LIMIT_1);
  CSliderCtrl *s2= (CSliderCtrl *)GetDlgItem(IDC_SLIDER_LIMIT_2);
  CEdit *e1= (CEdit *)GetDlgItem(IDC_EDIT_LIMIT_1_VALUE);
  CEdit *e2= (CEdit *)GetDlgItem(IDC_EDIT_LIMIT_2_VALUE);

  m_iLowerLimit= std::min(s1->GetPos() , s2->GetPos());
  m_iUpperLimit= std::max(s1->GetPos() , s2->GetPos());
  s1->SetSelection(m_iLowerLimit, m_iUpperLimit);
  s2->SetSelection(m_iLowerLimit, m_iUpperLimit);

  if (m_iExperimentDataIndex != LB_ERR)
  {
    m_vcExperimentData[m_iExperimentDataIndex].
      setLowerLimitFraction(m_iLowerLimit/100.0);
    m_vcExperimentData[m_iExperimentDataIndex].
      setUpperLimitFraction(m_iUpperLimit/100.0);
  }
  
  CString strText;
  strText.Format("%d",s1->GetPos()); e1->SetWindowText(strText);
  strText.Format("%d",s2->GetPos()); e2->SetWindowText(strText);

  // hack to force update in drawing the selection
  s1->SetRangeMax(m_iUpperRange,TRUE);
  s2->SetRangeMax(m_iUpperRange,TRUE);

  // This redraws the graph
  if (m_pTabExperiment)
  {
    m_pTabExperiment->setInitialLowerLimit(
      true, m_iInitialLowerLimit);
    m_pTabExperiment->OnFileListUpdated();
    m_pTabExperiment->setInitialLowerLimit(
      false, m_iInitialLowerLimit);
  }
}

// http://www.codeproject.com/KB/list/synchscroll.aspx
// http://www.tenouk.com/visualcplusmfc/visualcplusmfc5c.html
//
void CExperimentDataLimits::OnHScroll(unsigned int nSBCode, unsigned int nPos, CScrollBar* pScrollBar)
{
  CSliderCtrl* pSlide = (CSliderCtrl*) pScrollBar;
  CString strText;
  strText.Format("%d", pSlide->GetPos());

  if ((CSliderCtrl *)GetDlgItem(IDC_SLIDER_LIMIT_1) == pSlide)
  {
    SetDlgItemText(IDC_EDIT_LIMIT_1_VALUE, strText);
  }
  else
  if ((CSliderCtrl *)GetDlgItem(IDC_SLIDER_LIMIT_2) == pSlide)
  {
    SetDlgItemText(IDC_EDIT_LIMIT_2_VALUE, strText);
  }
}

void CExperimentDataLimits::OnSelchangeExperiments()
{
  CListBox *pListBox = (CListBox *) GetDlgItem(IDC_LIST_EXPERIMENT);
  m_iExperimentDataIndex = pListBox->GetCurSel();
  if (m_iExperimentDataIndex != LB_ERR)
  {
    CSliderCtrl *s1= (CSliderCtrl *)GetDlgItem(IDC_SLIDER_LIMIT_1);
    CSliderCtrl *s2= (CSliderCtrl *)GetDlgItem(IDC_SLIDER_LIMIT_2);
    s1->EnableWindow(TRUE);
    s2->EnableWindow(TRUE);

    // Attempt to be user friendly
    //
    if (s1->GetPos() <= s2->GetPos())
    {
      s1->SetPos((int)(
        100*m_vcExperimentData[m_iExperimentDataIndex].getLowerLimitFraction()));
      s2->SetPos((int)(
        100*m_vcExperimentData[m_iExperimentDataIndex].getUpperLimitFraction()));
    }
    else
    {
      s1->SetPos((int)(
        100*m_vcExperimentData[m_iExperimentDataIndex].getUpperLimitFraction()));
      s2->SetPos((int)(
        100*m_vcExperimentData[m_iExperimentDataIndex].getLowerLimitFraction()));
    }
    OnSliderReleased(NULL,NULL);
    OnHScroll(0,0,(CScrollBar *)s1);
    OnHScroll(0,0,(CScrollBar *)s2);
  }
  else // I don't expect this to happen ...
  {
    GetDlgItem(IDC_SLIDER_LIMIT_1)->EnableWindow(FALSE);
    GetDlgItem(IDC_SLIDER_LIMIT_2)->EnableWindow(FALSE);
  }
}

