#pragma once
#include "afxdtctl.h"

class LicenseData;

// CLicenseBorrowDlg dialog

class CLicenseBorrowDlg : public CDialog
{
  DECLARE_DYNAMIC(CLicenseBorrowDlg)

public:
  CLicenseBorrowDlg(CWnd* pParent = NULL);   // standard constructor
  virtual ~CLicenseBorrowDlg();

  virtual BOOL OnInitDialog();

  afx_msg void OnBnClickedReturnLicense();
  afx_msg void OnBnClickedRequestBorrow();
  afx_msg void OnBnClickedBorrowDianaButton();

// Dialog Data
  enum { IDD = IDD_LICENSE_BORROW_DIALOG };

protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

  DECLARE_MESSAGE_MAP()

private:
  void fillBorrowList();

  LicenseData* m_LicenseData;
  CDateTimeCtrl m_BorrowEndDate;

  CFont m_font;
};
