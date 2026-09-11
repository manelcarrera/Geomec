#if !defined(AFX_MUDWEIGHTPTSDLG_H__0FCC64FE_112F_431E_B64D_02B0D1A58D98__INCLUDED_)
#define AFX_MUDWEIGHTPTSDLG_H__0FCC64FE_112F_431E_B64D_02B0D1A58D98__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// mudweightptsdlg.h : header file
//

#include "DsbXDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CMudWeightPtsDlg dialog
class CWellPath;
class CHexaFormation;
class CDsbXDlg;

class CMudWeightPtsDlg : public CDialog {
  CWellPath *m_pWellPath;
  const CUnitNode &UnitNode();
  void SetButtonState();
  void UpdateList();

  CButton *m_pRadioSingle;
  CButton *m_pRadioRange;
  CButton *m_pReset;
  CEdit *m_pSinglePoint;
  CEdit *m_pFrom;
  CEdit *m_pTo;
  CEdit *m_pNrOPts;
  typedef std::map<int, CGeoWellPoint *> IndexPointMap;
  typedef std::pair<int, CGeoWellPoint *> IndexPointPair;

  std::set<CGeoWellPoint *> m_stTempPoints;

  IndexPointMap m_mpIndexPoints;
  CWellPath::TWellPointSet m_stPts;
  CWellPath::TWellPointSet m_stTemp;
  bool m_bDefaultOnly;
  // Construction
public:
  CMudWeightPtsDlg(CWellPath *pWellPath, CWnd *pParent = NULL); // standard constructor

  // Dialog Data
  //{{AFX_DATA(CMudWeightPtsDlg)
  enum { IDD = IDD_DSBATCH_SELECT_POINTS };
  double m_dSingleValue;
  double m_dFrom;
  double m_dTo;
  double m_dNrOPts;
  //}}AFX_DATA

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CMudWeightPtsDlg)
protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
  //}}AFX_VIRTUAL

  // Implementation
protected:
  // Generated message map functions
  //{{AFX_MSG(CMudWeightPtsDlg)
  virtual BOOL OnInitDialog();
  afx_msg void OnRadioSinglePoint();
  afx_msg void OnRadioRange();
  afx_msg void OnButtonAdd();
  afx_msg void OnButtonRemove();
  virtual void OnCancel();
  virtual void OnOK();
  afx_msg void OnButtonRecalcdef();
  afx_msg void OnButtonStartDStabor();
  afx_msg void OnButtonStartDSbBatch();
  afx_msg void OnClickListDsbatchPoints(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnItemchangingListDsbatchPoints(NMHDR *pNMHDR, LRESULT *pResult);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MUDWEIGHTPTSDLG_H__0FCC64FE_112F_431E_B64D_02B0D1A58D98__INCLUDED_)
