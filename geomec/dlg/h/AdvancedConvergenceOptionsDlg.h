#ifndef _ADVANCEDCONVERGENCEOPTIONS_H_
#define _ADVANCEDCONVERGENCEOPTIONS_H_

class CModelBase;

class CAdvancedConvergenceOptionsDlg : public CDialog {
public:
  CAdvancedConvergenceOptionsDlg(CModelBase &model, CWnd *pParent = NULL);

protected:
  virtual BOOL OnInitDialog();
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual void OnOK();

  afx_msg void OnDisable();
  afx_msg void OnDisableMinAngle();
  afx_msg void OnDisableMinRatio();
  afx_msg void OnDisableMinVolume();

  DECLARE_MESSAGE_MAP()

private:
  void UpdateControls();

private:
  CModelBase &m_model;

  int m_nDisable;
  int m_nDisableMinAngle;
  double m_dMinAngle;
  int m_nDisableMinRatio;
  double m_dMinRatio;
  int m_nDisableMinVolume;
  double m_dMinVolume;
};

#endif // _ADVANCEDCONVERGENCEOPTIONS_H_
