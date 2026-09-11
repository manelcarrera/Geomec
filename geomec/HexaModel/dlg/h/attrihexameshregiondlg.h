#if !defined(AFX_ATTRIMESHREGIONDLG_H__4B15BBE3_BDB1_466B_B48B_58CC3681AF4C__INCLUDED_)
#define AFX_ATTRIMESHREGIONDLG_H__4B15BBE3_BDB1_466B_B48B_58CC3681AF4C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// attrimeshregiondlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAttriMeshRegionDlg dialog
#include "AttributesTemplate.h"
#include "hexameshregion.h"
#include "HexaFormation.h"
#include "ISubListObject.h"
#include "ListCtrlBase.h"

class CAttriHexaMainMeshRegionDlg : public CAttributesTemplate<CHexaMainMeshRegion>
{
public:
  // Construction
  CAttriHexaMainMeshRegionDlg(CHexaMainMeshRegion& region, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
  //{{AFX_DATA(CAttriHexaMainMeshRegionDlg)
  enum { IDD = IDD_ATTRI_MAIN_MESH_REGION };
  //}}AFX_DATA

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CAttriHexaMainMeshRegionDlg)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:
  // Generated message map functions
  //{{AFX_MSG(CAttriMainMeshRegionDlg)
  afx_msg void OnColor();
  afx_msg void OnPaint();
  //}}AFX_MSG
  virtual BOOL OnInitDialog();
  DECLARE_MESSAGE_MAP()
};

class CAttriHexaSubMeshRegionDlg : public CAttributesTemplate<CHexaSubMeshRegion>
{

  CFemAppModel& m_model;

  class CFormationListObject : public IListObject
  {
  CHexaFormation& m_formation;
  public:
  // Construction
  CFormationListObject(CListCtrl& ctrl, CHexaFormation& formation, int elements);
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
  CMeshDepthObject(CFormationListObject &list_object, int elements);
  virtual QString Text() const;
  virtual BOOL CanEditText() const;
  virtual BOOL EditText(const QString& strText);
  int Elements() const;
  BOOL IsInteger(const CString& sString) const;
  };

  void OnUpdateCombo();
public:
  // Construction
  CAttriHexaSubMeshRegionDlg(CHexaSubMeshRegion& region, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
  //{{AFX_DATA(CAttriHexaSubMeshRegionDlg)
  enum { IDD = IDD_ATTRI_SUB_MESH_REGION };
  CComboBox m_cbExpX;
  CComboBox m_cbExpY;
  CListCtrlBase	m_lcFormation;
  //}}AFX_DATA

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CAttriHexaSubMeshRegionDlg)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:
  // Generated message map functions
  //{{AFX_MSG(CAttriHexaSubMeshRegionDlg)
  afx_msg void OnColor();
  afx_msg void OnPaint();
  //}}AFX_MSG
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnLvnItemchangedLcFormation(NMHDR *pNMHDR, LRESULT *pResult);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ATTRIMESHREGIONDLG_H__4B15BBE3_BDB1_466B_B48B_58CC3681AF4C__INCLUDED_)
