#pragma once

#include "Interface.h" // IMesherDlg

class CMesherDlg : public CDialog, public IMesherDlg
{
public:
	enum eLogLevel{ Warning, Error, Info, Num };
	//eLogLevel log_level();

	bool is_active( eLogLevel );
	std::string name( eLogLevel level );

	std::vector< std::string > filter_logs( const std::vector <std::string>& msg_v_in );

	void DoModal_(){ DoModal(); }

private:
	std::string m_msg;
	std::vector< std::string > m_msg_v;
	//eLogLevel m_log_level;

	CBrush m_brushOutput_Box;
	COLORREF m_colorOutput_Box;

	void update_logs();

// Construction
public:
	CMesherDlg(CWnd* pParent = NULL);   // standard constructor

	void data( std::vector <std::string> msg_v ){ m_msg_v = msg_v; }

// Dialog Data
	//{{AFX_DATA(CMesherDlg)
	enum { IDD = IDD_ERROR_DLG };

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMesherDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMesherDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnExport();
	afx_msg void OnExportPoints();
	afx_msg void OnCheckBoxClicked();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	CListBox	m_List;
	//}}AFX_DATA
};
