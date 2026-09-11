#ifndef _ATTRIINTERFACEMATERIAL_H_
#define _ATTRIINTERFACEMATERIAL_H_

class CInterfaceMaterial;

#include "AttributeTempl.h"

class CAttriInterfaceMaterialDlg : public CAttributesTemplate<CInterfaceMaterial>
{
public:
  CAttriInterfaceMaterialDlg(CInterfaceMaterial& mat, CWnd* pParent = 0);

protected:
  virtual void DoDataExchange(CDataExchange* pDX);
};

#endif // _ATTRIINTERFACEMATERIAL_H_
