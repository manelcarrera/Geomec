#ifndef FAULTINITSETTINGSDLG_H_
#define FAULTINITSETTINGSDLG_H_

class CModelBase;

class CFaultInitSettingsDlg : public CDialog
{
public:
	CFaultInitSettingsDlg(CModelBase& model, CWnd* pParent = NULL);   // standard constructor

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual void OnOK();

private:
  CModelBase& m_model;
  int m_nMethod;
};

#endif // FAULTINITSETTINGSDLG_H_
