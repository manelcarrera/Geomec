#pragma once

#include "resource.h"

// CFgvProgressDlg dialog

class CFgvProgressDlg : public CDialog
{
	DECLARE_DYNAMIC(CFgvProgressDlg)

public:
	CFgvProgressDlg(int hProcess, const CString& strCaption, CWnd* pParent = NULL);   // standard constructor
	virtual ~CFgvProgressDlg();

// Dialog Data
	enum { IDD = IDD_FemgvProgress };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual void OnCancel();
  virtual BOOL OnInitDialog();
  void OnTimer(UINT_PTR nIDEvent);

	DECLARE_MESSAGE_MAP()

private:
  HANDLE m_hProcess;
  UINT_PTR m_nTimer;
  double m_dProgVal;
  double m_dProgInc;
  CString m_strCaption;
};
