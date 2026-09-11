#if !defined(AFX_RPNEDITFORMULADLG_H__CEB1FE47_FB48_42E2_AFF5_EC425C1438E3__INCLUDED_)
#define AFX_RPNEDITFORMULADLG_H__CEB1FE47_FB48_42E2_AFF5_EC425C1438E3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RpnEditFormulaDlg.h : header file
//

#include "RPN_INC.h"
#include <queue>
#include <set>
#include <string>

class CModelBase;

/////////////////////////////////////////////////////////////////////////////
// CRpnEditFormulaDlg dialog

class CRpnEditFormulaDlg : public CDialog
{
private:
  std::string m_sInfixFormula;
  std::queue<Shunt::CShunt::Op_t> &m_qsRPNFormula;
  Shunt::CShunt m_shunt;
  bool m_bValidatedOk;
  bool m_bCaughtException;
  std::set<std::string> m_namedOperands;

public:
// Construction
	CRpnEditFormulaDlg
        ( const CModelBase &model
        , const std::string & input
        , const std::set<std::string> & operandNames
        , std::queue<Shunt::CShunt::Op_t> &output
        , CWnd* pParent =NULL
        );

// Dialog Data
	//{{AFX_DATA(CRpnEditFormulaDlg)
	enum { IDD = IDD_RPN_EDIT_FORMULA };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRpnEditFormulaDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRpnEditFormulaDlg)
		// NOTE: the ClassWizard will add member functions here
	afx_msg void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
    bool checkOperandNames( const std::list<Shunt::CRpnToken *> & tokens);
    void addOperandName( const std::string & operandName);
    const CModelBase &m_model;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RPNEDITFORMULADLG_H__CEB1FE47_FB48_42E2_AFF5_EC425C1438E3__INCLUDED_)
