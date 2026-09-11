// InPlaceEdit.h: interface for the CInPlaceEdit class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INPLACEEDIT_H__0EC2CC69_27E1_409C_BD26_890D9481ECC9__INCLUDED_)
#define AFX_INPLACEEDIT_H__0EC2CC69_27E1_409C_BD26_890D9481ECC9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// This class including its implementation was taken from codeguru.com. Original author (apart from some tweaks ;)
// is Zafir Anjum.
class CInPlaceEdit : public CEdit {
  // Construction
public:
  CInPlaceEdit(int iItem, int iSubItem, CString sInitText);

  // Attributes
public:
  // Operations
public:
  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CInPlaceEdit)
public:
  virtual BOOL PreTranslateMessage(MSG *pMsg);
  //}}AFX_VIRTUAL

  // Implementation
public:
  virtual ~CInPlaceEdit();

  // Generated message map functions
protected:
  //{{AFX_MSG(CInPlaceEdit)
  afx_msg void OnKillFocus(CWnd *pNewWnd);
  afx_msg void OnNcDestroy();
  afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  //}}AFX_MSG

  DECLARE_MESSAGE_MAP()
private:
  int m_iItem;
  int m_iSubItem;
  CString m_sInitText;
  BOOL m_bESC; // To indicate whether ESC key was pressed
};

#endif // !defined(AFX_INPLACEEDIT_H__0EC2CC69_27E1_409C_BD26_890D9481ECC9__INCLUDED_)
