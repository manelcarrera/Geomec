#if !defined(AFX_ATTRIANALYSISPOINT_H__5C23AD1F_3BDF_4736_9F98_A7FDAD042FBC__INCLUDED_)
#define AFX_ATTRIANALYSISPOINT_H__5C23AD1F_3BDF_4736_9F98_A7FDAD042FBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// attrianalysispoint.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAttriAnalysisPoint dialog

#include "AnalysisPoint.h"
#include "AttributesTemplate.h"

class CAttriAnalysisPoint : public CGeomecAttriTemp<CAnalysisPoint>
{
// Construction
public:
  CAttriAnalysisPoint(CAnalysisPoint& point, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
  //{{AFX_DATA(CAttriAnalysisPoint)
  enum { IDD = IDD_ATTRI_ANALYSIS_POINT };
    // NOTE: the ClassWizard will add data members here
  //}}AFX_DATA


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CAttriAnalysisPoint)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:

  // Generated message map functions
  //{{AFX_MSG(CAttriAnalysisPoint)
  afx_msg void OnColor();
  afx_msg void OnPaint();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ATTRIANALYSISPOINT_H__5C23AD1F_3BDF_4736_9F98_A7FDAD042FBC__INCLUDED_)
