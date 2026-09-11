#if !defined(AFX_ATTRIFRACTUREMATRIXPRESSUREDLG_H__CBC552F4_9D84_4625_B9B1_91361BE07528__INCLUDED_)
#define AFX_ATTRIFRACTUREMATRIXPRESSUREDLG_H__CBC552F4_9D84_4625_B9B1_91361BE07528__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FractureMatrixPressure.h"
#include "AttriFormationLoadDlg.h"

class CAttriFractureMatrixPressureDlg : public CAttriFormationLoadDlg<CFractureMatrixPressure>
{
protected:
	virtual void UpdateControls();

public:
	CAttriFractureMatrixPressureDlg(CFractureMatrixPressure& prs, CWnd* pParent = NULL);   // standard constructor

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
  virtual CString UndefinedLabel() const;
  virtual CString previousUndefinedLabel() const;

protected:
	afx_msg void OnGwc();
	afx_msg void OnTransPrevious();
	afx_msg void OnTransSpecify();

	DECLARE_MESSAGE_MAP()

private:
	bool m_bOk;
};

#endif // !defined(AFX_ATTRIFRACTUREMATRIXPRESSUREDLG_H__CBC552F4_9D84_4625_B9B1_91361BE07528__INCLUDED_)
