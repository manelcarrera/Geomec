#ifndef _ATTRIVALUETYPE_H_
#define _ATTRIVALUETYPE_H_

#include "AttributeTempl.h"

class CAttriValueType : public CAttributesTemplate<CValueType> {
public:
  CAttriValueType(CValueType &source, CWnd *pParent = 0);

protected:
  virtual BOOL OnInitDialog();
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual void OnOK();
};

#endif // _ATTRIVALUETYPE_H_
