#if !defined(AFX_EDITFORM_H__D4DBFF88_3B26_4E53_A609_395F417E6482__INCLUDED_)
#define AFX_EDITFORM_H__D4DBFF88_3B26_4E53_A609_395F417E6482__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditForm form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif


#define WM_ON_BB_CHANGED			(WM_USER+0x300)
#define WM_ON_CREATE_CUT_SURFACE	(WM_USER+0x301)

#include "DoubleEdit.h"
#include "LengthQuantity.h"
#include "NamedSurfaceMap.h"


class CCutSurfaceDlg;
class CCutSurfaceFrame;

namespace geo
{
class CBox;
class IPoint;
};

class CEditForm : public CFormView
{
protected:
  CEditForm();           // protected constructor used by dynamic creation
  DECLARE_DYNCREATE(CEditForm)

// Form Data
public:
  //{{AFX_DATA(CEditForm)
  //enum { IDD = IDD_FORMVIEW };
    // NOTE: the ClassWizard will add data members here
  //}}AFX_DATA

  

// Attributes
public:

// Operations
public:

  void Init(
    CCutSurfaceFrame*	frame,
    CCutSurfaceDlg* dlg,
    CDoubleQuantity::UNIT unit,
    BOOL CalculateDefaultZoomBox,
    const geo::CBox* pBox);


// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CEditForm)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual void OnInitialUpdate( );

  //}}AFX_VIRTUAL

// Implementation
protected:
  virtual ~CEditForm();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif


  // Generated message map functions
  //{{AFX_MSG(CEditForm)
  afx_msg void OnButtonGenSurfaces();
  afx_msg void OnButtonClearSurfaces();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()


private:
  LRESULT	  OnValidateDoubleEdit(WPARAM id,LPARAM lparam);
  unsigned int m_iControlToValidate;
  const geo::CBox* m_pBox;

  //edit boxes
  CDoubleEdit	m_MinX,m_MinY,m_MinZ;
  CDoubleEdit	m_MaxX,m_MaxY,m_MaxZ;

  CString m_BaseName;
  CDoubleEdit	m_NodeNrX,m_NodeNrY,m_NodeNrZ;

  //are integers but use doubleedit mechanism
  //no time to create a INT-input box.
  CLengthQuantity m_NrOfNodeX,m_NrOfNodeY,m_NrOfNodeZ;

  CLengthQuantity m_qMinX,m_qMinY,m_qMinZ;
  CLengthQuantity m_qMaxX,m_qMaxY,m_qMaxZ;

  CDoubleQuantity::UNIT m_unit;

  void OnOK();
  void OnCancel();

  const geo::CBox* GetBoundingBox() const;
  BOOL BoundingBoxDefined() const;
  BOOL NodesNrDefined() const;


  CCutSurfaceDlg*     m_pDlg;	
  CCutSurfaceFrame*	m_pFrame;

  void CalculateDefaultZoomBox();

  bool m_IsGenerated;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITFORM_H__D4DBFF88_3B26_4E53_A609_395F417E6482__INCLUDED_)
