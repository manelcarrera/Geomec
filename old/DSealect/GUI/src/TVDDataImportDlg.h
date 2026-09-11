#pragma once

#include <afxole.h>
#include "ListCtrlBase.h"

class ITVDDataReader;

class CTVDDataImportDlg : public CDialog
{
	DECLARE_DYNAMIC(CTVDDataImportDlg)

public:
	CTVDDataImportDlg(ITVDDataReader& reader, const CString& strFileName, CWnd* pParent = NULL);
	virtual ~CTVDDataImportDlg();
  afx_msg void OnReset();

// Dialog Data
	enum { IDD = IDD_TVDDataImport };

protected:
  virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual void OnOK();

	DECLARE_MESSAGE_MAP()

private:
  void Init();

private:
  ITVDDataReader& m_reader;
  CString m_strFileName;
  int m_iTVDUnit;
  int m_iDataUnit;
  int m_iStartRow;
  int m_iEndRow;
  int m_iTVDColumn;
  int m_iDataColumn;
  CListCtrlBase m_lcPreview;
};
