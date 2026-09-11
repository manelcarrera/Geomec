#if !defined(AFX_MESHDEPTHDLG_H__FE8A82B6_3745_409E_B7C8_6E77CA9CED8B__INCLUDED_)
#define AFX_MESHDEPTHDLG_H__FE8A82B6_3745_409E_B7C8_6E77CA9CED8B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// meshdepthdlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHexaMeshDepthDlg dialog
#include "HexaFormation.h"
#include "ISubListObject.h"
#include "ListCtrlBase.h"

class CHexaMeshDepthDlg : public CDialog
{
  class CFormationListObject : public IListObject
  {
    CHexaFormation& m_formation;
  public:
    // Construction
    CFormationListObject(CListCtrl& ctrl, CHexaFormation& formation);
    const CHexaFormation& Formation() const;
    CHexaFormation& Formation();
    // Functions
    virtual QString Text() const;
    virtual unsigned int Icon() const;

    void Apply();
    BOOL Modified() const;
    virtual BOOL operator<(const ICtrlObjectBase &object) const;
  };
  
  class CMeshDepthObject : public ISubListObject
  {
    int m_nElement;
  public:
    CMeshDepthObject(CFormationListObject &list_object);
    virtual QString Text() const;
    virtual BOOL CanEditText() const;
    virtual BOOL EditText(const QString& strText);
    int Elements() const;
    BOOL IsInteger(const CString& sString) const;
  };
  CFemAppModel& m_model;
// Construction
public:
  CHexaMeshDepthDlg(CFemAppModel& model, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
  //{{AFX_DATA(CHexaMeshDepthDlg)
  enum { IDD = IDD_ATTRI_MESH_DEPTH };
  CListCtrlBase	m_lcFormation;
  //}}AFX_DATA


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CHexaMeshDepthDlg)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual void OnOK();
  //}}AFX_VIRTUAL

// Implementation
protected:

  // Generated message map functions
  //{{AFX_MSG(CHexaMeshDepthDlg)
  virtual BOOL OnInitDialog();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MESHDEPTHDLG_H__FE8A82B6_3745_409E_B7C8_6E77CA9CED8B__INCLUDED_)
