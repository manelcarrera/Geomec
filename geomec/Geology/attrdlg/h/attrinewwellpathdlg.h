#ifndef ATTRINEWWELLPATHDLG_H
#define ATTRINEWWELLPATHDLG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NewWellPath.h" 
#include "NewWellPathInput.h"
//#include "AttributesTemplate.h" 
#include "DoubleEdit.h" 
//#include "QuantityInclude.h"
#include "resource.h"
#include "unitnode.h"


/////////////////////////////////////////////////////////////////////////////
// CAttriNewWellPathDlg dialog

class CAttriNewWellPathDlg : public CDialog
{
// Construction
public:
  typedef CNewWellPath::TColor TColor;
  CAttriNewWellPathDlg(CNewWellPathInput &well, CWnd* pParent = NULL);   // standard constructor


// Dialog Data
  //{{AFX_DATA(CAttriNewWellPathDlg)
  enum { IDD = IDD_ATTRI_NEWWELLPATH };
  CButton m_btChangeDate;
  CString	m_strName;
  //}}AFX_DATA


  CLengthQuantity m_dDerElev;
  CLengthQuantity m_dEasting;
  CLengthQuantity m_dNorthing;


  CDoubleEdit m_dEditDerElev;
  CDoubleEdit m_dEditEasting;
  CDoubleEdit m_dEditNorthing;

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CAttriNewWellPathDlg)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:

  // Generated message map functions
  //{{AFX_MSG(CAttriNewWellPathDlg)
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  afx_msg void OnPaint();
  afx_msg void OnStColor();
  afx_msg void OnKillfocusEdName();
  afx_msg LRESULT OnValidateEdit(WPARAM wparam,LPARAM lparam);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
private:
    //debug function to examine wellpointlists	
    void OnButtonChangeDate();
    void FillFrame(const UINT uFrameId, TColor color,CPaintDC &dc);
    TColor SelectColor(TColor nDefaultColor);

    CNewWellPathInput& m_well;

    TColor m_color;

    QDate m_birth_date;
};

#endif // ATTRINEWWELLPATHDLG_H
