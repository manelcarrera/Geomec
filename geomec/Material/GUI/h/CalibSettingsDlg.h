#if !defined(AFX_CALIBSETTINGSDLG_H__2D708C90_6291_4A4B_A996_773B9564C1F4__INCLUDED_)
#define AFX_CALIBSETTINGSDLG_H__2D708C90_6291_4A4B_A996_773B9564C1F4__INCLUDED_

class CCalibSettingsDlg : public CDialog {
public:
  CCalibSettingsDlg(double dConvCrit, int iMaxNumIter, CWnd *pParent = NULL);
  double ConvergenceCriterion() const;
  int MaxNumIterations() const;

private:
  double m_dConvCrit;
  int m_iMaxNumIter;

protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
};

#endif // !defined(AFX_CALIBSETTINGSDLG_H__2D708C90_6291_4A4B_A996_773B9564C1F4__INCLUDED_)
