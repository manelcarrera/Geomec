#ifndef _ATTRISTRAINDLG_H_
#define _ATTRISTRAINDLG_H_

#include "AttriFormationLoadDlg.h"
#include "StrainLoad.h"

class CAttriStrainDlg : public CAttriFormationLoadDlg<CStrainLoad>
{
public:
	CAttriStrainDlg(CStrainLoad& strain, CWnd* pParent = 0);

protected:
	virtual void UpdateControls();
	virtual void DoDataExchange(CDataExchange* pDX);
  virtual BOOL OnInitDialog();

  // this is a bit tricky, because we combine a radio button with two checkboxes (not cluttering up the UI with too many radio buttons)
  afx_msg void OnUndefinedRadio();
	afx_msg void OnPreDefinedConstant();
  afx_msg void OnPreDefinedConstantCheckbox();

  virtual CString UndefinedLabel() const;
	DECLARE_MESSAGE_MAP()

private:
  CButton m_chVolumetric;
  CButton m_chInPlane;

  bool   m_changingMode;

  double m_dVolume, m_dNormal, m_dLateral, m_dDepth, m_dGradient;
};

#endif // _ATTRISTRAINDLG_H_
