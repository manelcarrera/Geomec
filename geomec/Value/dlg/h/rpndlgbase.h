// RPNDlg.h : header file
//

#if !defined(AFX_RPNDLG_H__3ABACA87_3C3D_11D6_B8F6_CF46BEA9F879__INCLUDED_)
#define AFX_RPNDLG_H__3ABACA87_3C3D_11D6_B8F6_CF46BEA9F879__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CRpnDlg dialog
#include "IListObject.h"
#include "ListCtrlBase.h"
#include "RpnOperand.h"
#include "RpnUnitConverter.h"

namespace rpn {
class CRpnStack;

class CRpnDlgBase : public CDialog
{
public:
  // Class to insert value proxy in the list box
  class CValueProxyListObject : public IListObject
  {
    rpn::CRpnOperand::IValueProxy& m_proxy;
    CRpnDlgBase& m_dlg;
  public:
    CValueProxyListObject(CRpnDlgBase& dlg, CRpnOperand::IValueProxy& proxy);
  CValueProxyListObject(CRpnDlgBase& dlg, CRpnOperand::IValueProxy& proxy, int nListIndex);
    virtual QString Text() const;
    virtual unsigned int Icon() const;
    virtual void OnDoubleClick();
    virtual void Delete();
    virtual BOOL CanDelete() const;
    bool CanRemove() const;
    void Properties();
    bool HasProperties() const;
    virtual void AppendContextMenu(CContextMenuInvoker &invoker);
    virtual BOOL operator<(const ICtrlObjectBase &object) const;
    rpn::CRpnOperand::IValueProxy& Proxy() { return m_proxy; }
  };

protected:
  virtual void OnInitComponentBox();
  virtual void UpdateSwapButton() = 0;
  void OnConvertUnit(CButton& button);
  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
  void InsertUnitConverter(CRpnUnitConverter::TConversion conversion);
private:
  CRpnStack *m_pStack;
  std::vector<std::pair<unsigned int, QString> > m_vcUnitConverters;
protected:
  CListCtrlBase	m_lcOperand;
  CListCtrl		m_lcStack;
  HICON m_hIcon;
public:
  CRpnDlgBase(const CRpnStack& stack, unsigned int uDlgId, CWnd* pParent = NULL);	// standard constructor	
  virtual ~CRpnDlgBase();
  CRpnStack& RpnStack();
  const CRpnStack& RpnStack() const;
  CListCtrl& ListCtrl();
  virtual void OnOK();
  void Swap();
  bool CanSwap() const;
  void UpdateStackDisplay();
    BOOL ValidRpnStack() const;
// Implementation
protected:

  // Generated message map functions
  //{{AFX_MSG(CRpnDlg)
  virtual BOOL OnInitDialog();
  afx_msg void OnPaint();
  afx_msg HCURSOR OnQueryDragIcon();
  afx_msg void OnBtZero();
  afx_msg void OnBtOne();
  afx_msg void OnBtTwo();
  afx_msg void OnBtThree();
  afx_msg void OnBtFour();
  afx_msg void OnBtFive();
  afx_msg void OnBtSix();
  afx_msg void OnBtSeven();
  afx_msg void OnBtEight();
  afx_msg void OnBtNine();
  afx_msg void OnBtPoint();
  afx_msg void OnBtEnter();
  afx_msg void OnBtSign();
  afx_msg void OnBtAdd();
  afx_msg void OnBtClear();
  afx_msg void OnBtClearAll();
  afx_msg void OnBtPI();
  afx_msg void OnBtSub();
    afx_msg void OnBtSubOrSign();
  afx_msg void OnBtMult();
  afx_msg void OnBtDiv();
  afx_msg void OnBtSin();
  afx_msg void OnBtAbs();
  afx_msg void OnBtTan();
  afx_msg void OnBtCos();
  afx_msg void OnBtLn();
  afx_msg void OnBtLog();
  afx_msg void OnBtExp();
  afx_msg void OnBtSqrt();
  afx_msg void OnBtArcSin();
  afx_msg void OnBtArcCos();
  afx_msg void OnBtArcTan();
  afx_msg void OnBtNot();
  afx_msg void OnBtSmaller();
  afx_msg void OnBtGreater();
  afx_msg void OnBtIf();
  afx_msg void OnBtEqual();
  afx_msg void OnBtScn();
  afx_msg void OnBtPow();
  afx_msg void OnBtOr();
  afx_msg void OnBtAnd();
  afx_msg void OnKeyDown( unsigned int nChar, unsigned int nRepCnt, unsigned int nFlags );
  afx_msg void OnBtNotEqual();
  afx_msg void OnBtSmallerEqual();
  afx_msg void OnBtGreaterEqual();
  afx_msg void OnBtTanh();
  afx_msg void OnBtCosh();
  afx_msg void OnBtSinh();
  afx_msg void OnBtMin();
  afx_msg void OnBtMax();
  afx_msg void OnBtRound();
  afx_msg void OnBtSwap();
  afx_msg void OnBtSignFunction();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RPNDLG_H__3ABACA87_3C3D_11D6_B8F6_CF46BEA9F879__INCLUDED_)
