#if !defined(AFX_WELLINFODLG_H__97C297A1_AC49_4E61_8A18_427273DED5B7__INCLUDED_)
#define AFX_WELLINFODLG_H__97C297A1_AC49_4E61_8A18_427273DED5B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// wellinfodlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWellInfoDlg dialog
#include <qdatetime.h>
#include <qstring.h>

class well::CWellPathBase;
class AFX_EXT_CLASS CWellInfoDlg : public CDialog {
  well::CWellPathBase &m_WellPath;
  QDate m_spud_date;
  // Construction
public:
  CWellInfoDlg(well::CWellPathBase &well_path, CWnd *pParent = NULL); // standard constructor

  // Dialog Data
  //{{AFX_DATA(CWellInfoDlg)
  enum { IDD = IDD_WELLPATH_ATTRI };
  CString m_strCommonName;
  CString m_strCountry;
  CString m_strCustomerName;
  CString m_strWellDescr;
  CString m_strField;
  CString m_strJobNumber;
  CString m_strLocName;
  CString m_strWellType;
  CString m_strCustContact;
  CString m_strDate;
  //}}AFX_DATA

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CWellInfoDlg)
protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
  //}}AFX_VIRTUAL

  // Implementation
protected:
  // Generated message map functions
  //{{AFX_MSG(CWellInfoDlg)
  afx_msg void OnBtnSpudChange();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WELLINFODLG_H__97C297A1_AC49_4E61_8A18_427273DED5B7__INCLUDED_)
