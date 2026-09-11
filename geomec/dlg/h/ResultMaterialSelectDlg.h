#pragma once
#include "afxwin.h"
#include <list>


// CResultMaterialSelectDlg dialog

#if !defined(__RESULT_MATERIAL_SELECT_DLG__)
#define __RESULT_MATERIAL_SELECT_DLG__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class IRpnMaterialParameterProxy;

class CResultMaterialSelectDlg : public CDialog
{
  DECLARE_DYNAMIC(CResultMaterialSelectDlg)
  CDepletionStage *m_pTimeStep;
  CModelBase &m_Model;
  std::vector <IRpnMaterialParameterProxy*> m_pSelectedProxies;
  const std::vector<IRpnMaterialParameterProxy*> &m_vcMaterialProxies;
public:
  
  CResultMaterialSelectDlg(CModelBase& model, const std::vector<IRpnMaterialParameterProxy*> &materialProxies, CWnd* pParent = NULL);   // standard constructor
  virtual ~CResultMaterialSelectDlg();


// Dialog Data
  enum { IDD = IDD_SELECT_MATERIAL_RESULT };

  std::vector <IRpnMaterialParameterProxy*> SelectedProxies() { return m_pSelectedProxies; }
  const CDepletionStage *TimeStep() const { return m_pTimeStep; }

protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

  DECLARE_MESSAGE_MAP()
private:

  afx_msg void OnCbnSelchangeTimeStep();
  virtual BOOL OnInitDialog();

  CListCtrl m_lcParameters; // waij TFS 80121
  CComboBox m_cbTimeSteps;
  CImageList m_ImageList;

  void SetupDeplCombo();
  void SetupMatParamListBox();

public:
  afx_msg void OnLbnSelchangeRmsParametersList();
  afx_msg void OnBnClickedOk();
  afx_msg void OnBnClickedCheckSetDepletion();
private:
  CButton m_chSetDepletion;
};


#endif
