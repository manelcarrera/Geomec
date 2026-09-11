#ifndef _EditFaultSettings_h_
#define _EditFaultSettings_h_

#include "afxwin.h"

#include "FaultFractureList.h"
#include "HandleFaultFractureBehaviour.h"
#include "HorizonBase.h"
#include "HandleDefaultFaultParameters.h"

// CEditFaultSettings dialog

class CEditFaultSettings : public CDialog
{
	DECLARE_DYNAMIC(CEditFaultSettings)

public:
	CEditFaultSettings(CFemAppModel* femAppModel, CWnd* pParent = NULL);
	CEditFaultSettings(CFemAppModel* femAppModel, const CHorizonBase* horizonBase,
    CWnd* pParent = NULL);
	virtual ~CEditFaultSettings();

  static bool hasFaultFractures(const CFemAppModel* femAppModel);

  virtual BOOL OnInitDialog();

  afx_msg void OnCbnSelchangeFaultFractureComboBox();
  afx_msg void OnBnClickedFaultInitializationMethod();
  afx_msg void OnBnClickedGlobalFaultBehaviour();
  afx_msg void OnEnChangeDefaultFaultParameters();
  afx_msg void OnCheckApplyToAllFaults();

// Dialog Data
	enum { IDD = IDD_EDIT_FAULT_SETTINGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual void OnOK();

	DECLARE_MESSAGE_MAP()

private:
  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

  void handleFaultFractureBehaviour(CDataExchange* pDX, int& currentSelection);
  void handleDefaultFaultParameters(CDataExchange* pDX, int& currentSelection);

  CDoubleQuantity::UNIT Unit() const;

  static int determineInitialSelection(
    const CFaultFractureList& faultFractureList,
    const CHorizonBase* horizonBase);

  CFemAppModel* m_femAppModel;
  CFaultFractureList m_faultFractureList;
  CComboBox m_faultFractureComboBox;
  CHandleFaultFractureBehaviour <CFaultFracture> m_handleFaultFractureBehaviour;
  CButton m_applyBehaviourToAllFaults;
  CButton m_applyInitD0ToAllFaults;
  int m_initialSelection;
};

#endif  // _EditFaultSettings_h_
