#if !defined(AFX_MESHDLGBAR_H__84634AB9_F148_4473_B303_69177DC99C0C__INCLUDED_)
#define AFX_MESHDLGBAR_H__84634AB9_F148_4473_B303_69177DC99C0C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MeshDlgBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMeshDlgBar dialogbar

class CMeshDlgBar : public CDialogBar {
private:
  void ToggleCombos(BOOL bValue1, BOOL bValue2, int Sel1, int Sel2);
  void ResetCombos();
  CGeomecDoc *Doc();
  int m_nCalcSelection;
  int m_nDeplSelection;

  // Construction
public:
  //	CMeshDlgBar(CWnd* pParent = NULL);   // standard constructor
  CMeshDlgBar(); // standard constructor

  // Dialog Data
  //{{AFX_DATA(CMeshDlgBar)
  enum { IDD = IDD_MESH_DIALOG_BAR };
  // NOTE: the ClassWizard will add data members here
  //}}AFX_DATA

  void UpdateLists();
  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CMeshDlgBar)
protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
  //}}AFX_VIRTUAL

  // Implementation
protected:
  // Generated message map functions
  //{{AFX_MSG(CMeshDlgBar)
  afx_msg void OnSelectionCalcType();
  afx_msg void OnSelectionDepStage();
  afx_msg BOOL OnToolTipNotify(UINT id, NMHDR *pTTTStruct, LRESULT *pResult);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MESHDLGBAR_H__84634AB9_F148_4473_B303_69177DC99C0C__INCLUDED_)
