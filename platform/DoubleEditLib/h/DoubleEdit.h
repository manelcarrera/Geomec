#if !defined(AFX_DOUBLEEDIT_H__DC2F6C83_F2CB_4FD1_A147_032A9443A8AA__INCLUDED_)
#define AFX_DOUBLEEDIT_H__DC2F6C83_F2CB_4FD1_A147_032A9443A8AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DoubleEdit.h : header file
//

#define WM_VALIDATE_DOUBLE_EDIT (WM_USER + 0x16)

/////////////////////////////////////////////////////////////////////////////
// CDoubleEdit window

// ##ModelId=3C0DC15D034B
class CDoubleEdit : public CEdit {
  // Construction
public:
  // ##ModelId=3C0DC15D035C
  CDoubleEdit();
  // ##ModelId=3C0DC15D039A
  static BOOL IsNumericValue(CString str);
  // ##ModelId=3C60D579038B
  static CString Format(double dblVal, bool IntegerInput = false);
  // Attributes
public:
  // Operations
public:
  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CDoubleEdit)
  //}}AFX_VIRTUAL

  // Implementation
public:
  // ##ModelId=3C2044040000
  CString GetString();
  // ##ModelId=3C0DC15D036B
  void MakeEmpty();
  // ##ModelId=3C0DC15D036C
  void ShowValue(const double &NewValue);
  // ##ModelId=3C0DC15D036E
  BOOL IsEmpty();
  // ##ModelId=3C0DC15D036F
  double GetValue();
  // ##ModelId=3C0DC15D037A
  virtual ~CDoubleEdit();

  void UseAsIntegerInput();
  // mantis 2537 wjrx
  void setAllowNegativeNumbers(bool val) { m_bAllowNegativeNumbers = val; }

  // Generated message map functions
protected:
  //{{AFX_MSG(CDoubleEdit)
  // ##ModelId=3C0DC15D038A
  afx_msg void OnChar(unsigned int nChar, unsigned int nRepCnt, unsigned int nFlags);
  // ##ModelId=3C0DC15D038F
  afx_msg void OnKeyUp(unsigned int nChar, unsigned int nRepCnt, unsigned int nFlags);
  // afx_msg void OnKillFocus( CWnd* pNewWnd );

  // ##ModelId=3C8D9E1D0292
  afx_msg void OnKillfocus();
  //}}AFX_MSG

  DECLARE_MESSAGE_MAP()
private:
  // ##ModelId=3C8D9E1D02A2
  void DoValidation();

  // ##ModelId=3C60D57903D9
  void SetModifiedFlag();
  bool m_UseAsIntegerInput;
  bool m_bAllowNegativeNumbers; // mantis 2537 wjrx
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DOUBLEEDIT_H__DC2F6C83_F2CB_4FD1_A147_032A9443A8AA__INCLUDED_)
