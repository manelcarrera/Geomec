#ifndef _SetGlobalFaultBehaviourDialog_h_
#define _SetGlobalFaultBehaviourDialog_h_

#include "afxwin.h"

#include "SetGlobalFaultBehaviour.h"

class CSetGlobalFaultBehaviourDialog : public CDialogEx
{
  DECLARE_DYNAMIC(CSetGlobalFaultBehaviourDialog)

public:
  CSetGlobalFaultBehaviourDialog(CFemAppModel* femAppModel,
    CWnd* pParent = NULL);
  virtual ~CSetGlobalFaultBehaviourDialog();

  virtual BOOL OnInitDialog();

  afx_msg void OnBnClickedRadioButton();

  enum { IDD = IDD_GLOBAL_FAULT_BEHAVIOUR };

protected:
  virtual void DoDataExchange(CDataExchange* pDX);

  DECLARE_MESSAGE_MAP()

private:
  virtual void OnOK();

  void enableControls();
  void setUnitNames(CDataExchange* pDX) const;
  std::vector <double> getValues() const;

  void DoDataExchangeModeType(CDataExchange* pDX);
  void DoDataExchangeGradient(CDataExchange* pDX);

  CSetGlobalFaultBehaviour m_setGlobalFaultBehaviour;
  CButton m_maximum;
  CButton m_minimum;
  CButton m_gradient;
  CButton m_maximumGradient;
  double m_editReferenceDepth;
  double m_editReferencePressure;
  double m_editGradient;
  bool m_firstTime;
  int m_valueGlobalFaultBehaviour;
};

#endif  // _SetGlobalFaultBehaviourDialog_h_
