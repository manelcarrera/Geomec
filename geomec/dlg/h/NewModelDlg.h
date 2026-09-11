#if !defined(AFX_NEWMODELDLG_H__A6CB1F61_37FC_4F12_A230_3635FCA838E4__INCLUDED_)
#define AFX_NEWMODELDLG_H__A6CB1F61_37FC_4F12_A230_3635FCA838E4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewModelDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNewModelDlg dialog
#include <map>
#include <vector>

// ##ModelId=3B6905BD0196
class CNewModelDlg : public CDialog {
public:
  struct ModelInfo {
    int nType;
    QString sType;
    uint nIcon;
  };

private:
  // ##ModelId=3B6905BD0281
  std::map<unsigned int, int> m_mpResToImage;
  // ##ModelId=3B6905BD02C0
  CImageList *m_pImageList;
  // ##ModelId=3B6905BD030E

  int m_nSelectedModel;

  // ##ModelId=3B6905BE0011
  std::vector<ModelInfo> m_vcModel;
  // ##ModelId=3B6905BE002F
  int IconToImage(const unsigned int uIconID);
  // Dialog Data
  //{{AFX_DATA(CNewModelDlg)
  enum { IDD = IDD_NEW_MODEL_DLG };
  // ##ModelId=3B6905BE004F
  CListCtrl m_lbModel;
  //}}AFX_DATA
  // Construction
public:
  // ##ModelId=3B6905BE007D
  int SelectedModel() const;
  // ##ModelId=3B6905BE008E
  CNewModelDlg(std::vector<ModelInfo> &vcModel, CWnd *pParent = NULL); // standard constructor
  // ##ModelId=3B6905BE00AC
  ~CNewModelDlg();

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CNewModelDlg)
protected:
  // ##ModelId=3B6905BE00BC
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
  //}}AFX_VIRTUAL

  // Implementation
protected:
  // Generated message map functions
  //{{AFX_MSG(CNewModelDlg)
  // ##ModelId=3B6905BE00DB
  virtual BOOL OnInitDialog();
  // ##ModelId=3B6905BE00FA
  virtual void OnCancel();
  // ##ModelId=3B6905BE010B
  virtual void OnOK();
  // ##ModelId=3B6905BE011A
  afx_msg void OnDblclkLbModel(NMHDR *pNMHDR, LRESULT *pResult);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWMODELDLG_H__A6CB1F61_37FC_4F12_A230_3635FCA838E4__INCLUDED_)
