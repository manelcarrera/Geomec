// RPNDlg.h : header file
//

#if !defined(AFX_RPNDLG_H__INCLUDED_)
#define AFX_RPNDLG_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CRpnDlg dialog
#include "DerivedResult.h"
class CRpnValueSet;

#include "rpndlgbase.h"

class CRpnDlg : public rpn::CRpnDlgBase {
private:
  class CNewItemListObject : public IListObject {
    bool m_bMaterialItem;
    CRpnDlg &m_dlg;

    void SelectMaterialItem();
    void SelectResultItem();
    bool CheckUniqueName(const std::string &name) const;

  public:
    CNewItemListObject(CRpnDlg &dlg, bool bMaterialItem);
    virtual QString Text() const;
    virtual unsigned int Icon() const;
    virtual void OnDoubleClick();
  };
  class CUnAssignedListObject : public IListObject {
    CRpnDlg &m_dlg;
    CString m_sDescription;

  public:
    CUnAssignedListObject(const CString &sDescription, CRpnDlg &dlg);
    virtual QString Text() const;
    virtual unsigned int Icon() const;
    virtual DROPEFFECT CanDrop(TCtrlObjectVec &vcDragged, BOOL bMove) const;
    virtual void Drop(TCtrlObjectVec &vcDragged, BOOL bMove);
  };
  CRpnValueSet *m_pValueSet;
  CDerivedResult::CDerivedResultComponent *m_pResult;
  BOOL m_bNameEqualToFormula;
  CString m_sName;

protected:
  virtual void OnInitComponentBox();
  virtual void UpdateSwapButton();
  virtual void UpdateEditFormulaButton();

public:
  CRpnDlg(CRpnValueSet &value_set, CWnd *pParent = NULL);
  CRpnDlg(CDerivedResult::CDerivedResultComponent &result, CWnd *pParent = NULL);
  virtual ~CRpnDlg();
  const CString &Name() const { return m_sName; }
  BOOL StoreGlobal() const;
  const CModelBase &Model() const;
  CModelBase &Model();
  const CDerivedResult::CDerivedResultComponent *Result() const;

  // Dialog Data
  //{{AFX_DATA(CRpnDlg)
  enum { IDD = IDD_RPN_DIALOG };

  //}}AFX_DATA

  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CRpnDlg)
protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
  //}}AFX_VIRTUAL

  // Implementation
protected:
  // Generated message map functions
  //{{AFX_MSG(CRpnDlg)
  afx_msg void OnBtEditFormula();
  afx_msg void OnNameChange();
  afx_msg void OnCkName();
  afx_msg void OnStackItemChanged(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg BOOL CRpnDlg::PreTranslateMessage(MSG *pMsg); // wjrx mantis 686
  afx_msg void OnBtConvertUnit();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RPNDLG_H__INCLUDED_)
