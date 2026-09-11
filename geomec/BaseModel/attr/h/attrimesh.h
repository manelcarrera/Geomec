#if !defined(AFX_ATTRIMESH_H__F467FD7D_DF55_444D_93DA_580191082AD6__INCLUDED_)
#define AFX_ATTRIMESH_H__F467FD7D_DF55_444D_93DA_580191082AD6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// attrimesh.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAttriMesh dialog

class CMeshBase;
class CAttriMesh : public CDialog {
  CMeshBase &m_mesh;
  // Construction
public:
  CAttriMesh(CMeshBase &mesh, CWnd *pParent = NULL); // standard constructor

  // Dialog Data
  //{{AFX_DATA(CAttriMesh)
  enum { IDD = IDD_ATTRI_MESH };
  //}}AFX_DATA

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CAttriMesh)
protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
  //}}AFX_VIRTUAL

  // Implementation
protected:
  // Generated message map functions
  //{{AFX_MSG(CAttriMesh)
  // NOTE: the ClassWizard will add member functions here
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ATTRIMESH_H__F467FD7D_DF55_444D_93DA_580191082AD6__INCLUDED_)
