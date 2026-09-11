#ifndef _TABEXPERIMENT_H_
#define _TABEXPERIMENT_H_

#include "ExperimentData.h"

class CMatParamDlg;

class CTabExperiment : public CDialog
{
public:
  CTabExperiment(CMatParamDlg& dlg, CWnd* pParent = NULL);
  unsigned int ID();

  CMatParamDlg& Dlg();
  bool ShowAllExperiments() const;

  CExperimentArray& ExperimentData();
  const CExperimentArray& ExperimentData() const;

  void OnFileListUpdated();
  void setInitialLowerLimit(
  bool useInitialLowerLimit, const std::vector <int>& initialLowerLimit);
  void getSelectedItems(int& total, int& selected, int* items[]) const;

protected:
  virtual void DoDataExchange(CDataExchange* pDX);
  virtual BOOL OnInitDialog();

private:
  afx_msg void OnInsert();
  afx_msg void OnDelete();
  afx_msg void OnSelchangeListFiles();
  afx_msg void OnGammasall();
  afx_msg void OnGammasselected();
//	afx_msg void OnClientValidateEdxWeight(long ValidationMode);
  afx_msg LRESULT OnEditLostFocus(WPARAM wParam, LPARAM lParam);

//	DECLARE_EVENTSINK_MAP()
  DECLARE_MESSAGE_MAP()

  CString FileNameFromPathName(const CString& strPathName) const;
//	BOOL CancellingDialog();
  void UpdateFileList();
  void UpdatePictureRadios();
  void UpdateWeightFactor();
//	void SetWeightFactorOfCurrentExperiment(double dVal);

private:
  enum { IDD = IDD_TABEXPERIMENT };

  int		m_iShowInPicture;
  CExperimentArray m_vcExperimentData;
  CMatParamDlg& m_Dlg;
  std::vector<CString> m_vcExperimentFileName;
//	BOOL m_bDoMessage;
  int m_iCurrentSel;
  bool m_bUseInitialLowerLimit;
  std::vector <int> m_iInitialLowerLimit;
};

#endif // _TABEXPERIMENT_H_
