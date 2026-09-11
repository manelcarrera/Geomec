#ifndef _NONMESHEDSURFACEPARAMETERSDLG_H_
#define _NONMESHEDSURFACEPARAMETERSDLG_H_

#include "AttributeTempl.h"
#include "NonMeshedSurfaceParametersNode.h"

class CNonMeshedSurfaceParametersDlg : public CAttributesTemplate<CNonMeshedSurfaceParametersNode>
{
public:
  CNonMeshedSurfaceParametersDlg(CNonMeshedSurfaceParametersNode& parameters, CDialog* pParent = 0);

  virtual void DoDataExchange(CDataExchange* pDX);
};

#endif // _NONMESHEDSURFACEPARAMETERSDLG_H_
