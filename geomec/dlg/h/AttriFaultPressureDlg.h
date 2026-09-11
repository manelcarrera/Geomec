#ifndef _ATTRIFAULTPRESSUREDLG_H_

#include "AttributeTempl.h"
#include "FaultPressure.h"

class CAttriFaultPressureDlg : public CAttributesTemplate<CFaultPressure> {
public:
  CAttriFaultPressureDlg(CFaultPressure &faultpressure, CWnd *pParent = 0);

protected:
  virtual void DoDataExchange(CDataExchange *pDX);
  BOOL OnInitDialog();

protected:
  afx_msg void OnExtrapolated();
  afx_msg void OnInHullOnly();
  afx_msg void OnMaximum();
  afx_msg void OnMinimum();
  afx_msg void OnGradient();
  afx_msg void OnMaxGradient();

  DECLARE_MESSAGE_MAP()

private:
  void EnableControls();
  bool Locked() const;
};

#define _ATTRIFAULTPRESSUREDLG_H_
#endif // _ATTRIFAULTPRESSUREDLG_H_
