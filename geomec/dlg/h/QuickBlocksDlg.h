#include "Geomec.h"
// #include "afxcmn.h"
#include "TimeLapseControlObjects.h"

class CQBSettings;

// CQuickBlocksDlg dialog

class CQuickBlocksDlg : public CDialog {
  DECLARE_DYNAMIC(CQuickBlocksDlg)

  CQBSettings &m_Source;

public:
  CQuickBlocksDlg(CQBSettings &grid, CWnd *pParent = NULL); // standard constructor
  virtual ~CQuickBlocksDlg();

  // Dialog Data
  enum { IDD = IDD_QB_DLG };

protected:
  virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
  virtual BOOL OnInitDialog();

  DECLARE_MESSAGE_MAP()

  double m_dNorth;
  double m_dEast;
  double m_dLength;
  double m_dWidth;
  double m_dTrack;
  double m_dBin;

public:
  afx_msg void OnBnClickedButtonQbStart();
  afx_msg void OnBnClickedQbAddDeplstage();
  afx_msg void OnBnClickedQbRemoveDeplstage();
  afx_msg void OnBnClickedQbAddDeplstage2();
  afx_msg void OnBnClickedQbRemoveDeplstage2();

protected:
  CTimeLapseComboListCtrl m_lbPressures;
  CTimeLapseComboListCtrl m_lbTemperatures;

public:
  afx_msg void OnBnClickedButtonQbWriteDat();
  afx_msg void OnBnClickedQbSelectForms();
};
