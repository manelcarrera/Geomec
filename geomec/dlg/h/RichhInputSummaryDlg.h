#pragma once
#include "afxcmn.h"

#if !defined(__RICH_INPUT_SUMMARY_DLG__)
#define __RICH_INPUT_SUMMARY_DLG__

class CModelBase;
class CMaterialServer;

#include "DumpModel.h"

// CRichhInputSummaryDlg dialog

class CRichhInputSummaryDlg : public CDialog {
  DECLARE_DYNAMIC(CRichhInputSummaryDlg)

public:
  CRichhInputSummaryDlg(const CModelBase &model, CWnd *pParent = NULL); // standard constructor
  virtual ~CRichhInputSummaryDlg();

  // Dialog Data
  enum { IDD = IDD_RICH_INPUT_SUMMARY_DIALOG };

protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
  virtual BOOL OnInitDialog();
  DECLARE_MESSAGE_MAP()
private:
  static DWORD CALLBACK EditStreamCallback(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb);

  const CModelBase &m_Model;
  CUnitNode::TUnitType m_Unit;
  CRichEditCtrl m_ctrlSummary;
  // set of parameters we don't want to plot
  std::set<CString> m_stExcludedMatParameters;
  std::vector<const CDepletionStage *> m_vcStages;
  std::vector<const CFormationBase *> m_vcFormations;
  CDumpModel m_dumpModel;

  void AddText(const CString &text);
  void NewLine(int nNewLines = 1);
  void AddBoldText(const CString &text);
  void AddBoldUnderlinedText(const CString &text);
  void AddRedText(const CString &text);

public:
  afx_msg void OnBnClickedSummarySaveButton();
};

#endif
