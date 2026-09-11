#ifndef _ATTRITEMPERATUREDLG_H_
#define _ATTRITEMPERATUREDLG_H_

#include "AttriFormationLoadDlg.h"
#include "Temperature.h"

class CAttriTemperatureDlg : public CAttriFormationLoadDlg<CTemperature> {
public:
  CAttriTemperatureDlg(CTemperature &temperature, CWnd *pParent = 0);

protected:
  virtual CString UndefinedLabel() const;
  DECLARE_MESSAGE_MAP()
};

#endif // _ATTRITEMPERATUREDLG_H_
