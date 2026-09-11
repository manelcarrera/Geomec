#if !defined(AFX_ATTRIVALUETYPEDLG_H__4F2CD2A5_0AFF_4CB4_AF02_5D5D89186482__INCLUDED_)
#define AFX_ATTRIVALUETYPEDLG_H__4F2CD2A5_0AFF_4CB4_AF02_5D5D89186482__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// attrivaluetypedlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRenameNodeDlg dialog

#include "resource.h"

class CRenameNodeDlg : public CDialog
{
private:
  CString m_strName;
	CGraphNode& m_node;

// Construction
public:
	CRenameNodeDlg(CGraphNode &node, CWnd* pParent = NULL);   // standard constructor

	enum { IDD = IDD_RENAME_NODE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
  virtual void OnOK();
};

#endif // !defined(AFX_ATTRIVALUETYPEDLG_H__4F2CD2A5_0AFF_4CB4_AF02_5D5D89186482__INCLUDED_)
