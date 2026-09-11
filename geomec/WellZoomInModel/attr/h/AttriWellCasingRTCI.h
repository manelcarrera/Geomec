#ifndef _ATTRIWELLCASINGRTCI_H_
#define _ATTRIWELLCASINGRTCI_H_

#include "AttributesTemplate.h"
#include "WellCasingRTCI.h"

class CAttriWellCasingRTCI : public CAttributesTemplate<CWellCasingRTCI> {
public:
  CAttriWellCasingRTCI(CWellCasingRTCI &rtci, CWnd *pParent = 0);

protected:
  virtual void DoDataExchange(CDataExchange *pDX);
};

#endif // _ATTRIWELLCASINGRTCI_H_
