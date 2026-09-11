#if !defined(AFX_MODELGEOLOGYPG_H__1464C1AE_3CC2_4E3D_834B_F2CD1C6DDA23__INCLUDED_)
#define AFX_MODELGEOLOGYPG_H__1464C1AE_3CC2_4E3D_834B_F2CD1C6DDA23__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// modelgeologypg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CModelGeologyPg dialog

//##ModelId=3BC55D610257
class CModelBase;

class CModelGeologyPg : public CPropertyPage
{
  CModelBase &m_model;
// Construction
public:
  CQuantity::UNIT Unit() const;
  //##ModelId=3BC55D610259
  CModelGeologyPg(CModelBase &model);
  //##ModelId=3BC55D61025A
  ~CModelGeologyPg();

// Dialog Data
  //{{AFX_DATA(CModelGeologyPg)
  enum { IDD = IDD_ATTRI_MODEL_GEOLOGY };
  CString	m_faults;
  CString	m_formations;
  //}}AFX_DATA


// Overrides
  // ClassWizard generate virtual function overrides
  //{{AFX_VIRTUAL(CModelGeologyPg)
  protected:
  //##ModelId=3BC55D61025B
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:
  // Generated message map functions
  //{{AFX_MSG(CModelGeologyPg)
  virtual BOOL OnInitDialog();
  afx_msg void OnChangeGravity();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODELGEOLOGYPG_H__1464C1AE_3CC2_4E3D_834B_F2CD1C6DDA23__INCLUDED_)
