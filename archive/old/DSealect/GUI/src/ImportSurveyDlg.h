#pragma once

#include <afxole.h>
#include "ListCtrlBase.h"

class CImportInfo;

class CImportSurveyDlg : public CDialog
{
	DECLARE_DYNAMIC(CImportSurveyDlg)

public:
	CImportSurveyDlg(CImportInfo& ImportInfo, const CString& strFileName, CWnd* pParent = NULL);
	virtual ~CImportSurveyDlg();
  afx_msg void OnReset();

// Dialog Data
	enum { IDD = IDD_ImportSurvey };

protected:
  virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual void OnOK();

	DECLARE_MESSAGE_MAP()

private:
  void Init();

private:
  CImportInfo& m_ImportInfo;
  CString m_strFileName;
  int m_iMDUnit;
  int m_iStartRow;
  int m_iEndRow;
  int m_iMDColumn;
  int m_iInclinationColumn;
  int m_iAzimuthColumn;
  CListCtrlBase m_lcPreview;
};
