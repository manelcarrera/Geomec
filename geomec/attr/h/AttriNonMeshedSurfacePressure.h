#ifndef _ATTRINONMESHEDSURFACEPRESSURE_H_
#define _ATTRINONMESHEDSURFACEPRESSURE_H_

#include "AttributeTempl.h"
#include "NonMeshedSurfacePressure.h"

class CAttriNonMeshedSurfacePressure : public CAttributesTemplate<CNonMeshedSurfacePressure>
{
public:
  CAttriNonMeshedSurfacePressure(CNonMeshedSurfacePressure& pressure, CWnd* pParent = NULL);

protected:
  virtual BOOL OnInitDialog();
  virtual void DoDataExchange(CDataExchange* pDX);

  afx_msg void OnFormationPressure();
  afx_msg void OnManualPressure();

  DECLARE_MESSAGE_MAP();

private:
  void UpdateControls();
};

#endif // _ATTRINONMESHEDSURFACEPRESSURE_H_
