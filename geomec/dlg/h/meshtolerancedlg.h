#if !defined(AFX_MESHTOLERANCEDLG_H__F34ECC07_FC64_4818_A738_E3021B9CF845__INCLUDED_)
#define AFX_MESHTOLERANCEDLG_H__F34ECC07_FC64_4818_A738_E3021B9CF845__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// meshtolerancedlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMeshToleranceDlg dialog

namespace geo {
class CTetMeshBase;
}

class CMeshToleranceDlg : public CDialog {
  double m_dTolerance;
  geo::CTetMeshBase &m_mesh;
  // Construction
public:
  CMeshToleranceDlg(geo::CTetMeshBase &mesh, CWnd *pParent = NULL); // standard constructor

  // Dialog Data
  //{{AFX_DATA(CMeshToleranceDlg)
  enum { IDD = IDD_MESH_TOLERANCE_DLG };
  // NOTE: the ClassWizard will add data members here
  //}}AFX_DATA

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CMeshToleranceDlg)
protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
  //}}AFX_VIRTUAL

  // Implementation
protected:
  // Generated message map functions
  //{{AFX_MSG(CMeshToleranceDlg)
  virtual void OnOK();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MESHTOLERANCEDLG_H__F34ECC07_FC64_4818_A738_E3021B9CF845__INCLUDED_)
