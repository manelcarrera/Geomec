#ifndef _ATTRIFAULTPARAMETERS_H_
#define _ATTRIFAULTPARAMETERS_H_

class CFaultParametersNode;

#include "AttributeTempl.h"

class CAttriFaultParameters : public CAttributesTemplate<CFaultParametersNode>
{
public:
  CAttriFaultParameters(CFaultParametersNode& node, CWnd* pParent = 0);

protected:
  virtual void DoDataExchange(CDataExchange* pDX);
  BOOL OnInitDialog();

  afx_msg void OnCohesionExtrapolated();
  afx_msg void OnCohesionUse();
  afx_msg void OnCohesionSameAsPrev();
  afx_msg void OnCohesionValue();
  afx_msg void OnFrictionExtrapolated();
  afx_msg void OnFrictionUse();
  afx_msg void OnFrictionSameAsPrev();
  afx_msg void OnFrictionValue();
  afx_msg void OnDStiffSameAsPrev();
  afx_msg void OnDStiffValue();

  DECLARE_MESSAGE_MAP()

private:
  void EnableControls();
  bool Locked();
};

#endif // _ATTRIFAULTPARAMETERS_H_
