#if !defined(AFX_INITIALTEMPERATUREDLG_H__C56266FC_C73E_4EE2_93B5_92BC702E19F7__INCLUDED_)
#define AFX_INITIALTEMPERATUREDLG_H__C56266FC_C73E_4EE2_93B5_92BC702E19F7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InitialTemperatureDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInitialTemperatureDlg dialog
#include "AttriGlobalFormationLoadDlg.h"
#include "GlobalTemperature.h"

class CInitialTemperatureDlg : public CAttriGlobalFormationLoadDlg<CGlobalTemperature> {
public:
  CInitialTemperatureDlg(CGlobalTemperature &temp, CWnd *pParent = 0);

protected:
  DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_INITIALTEMPERATUREDLG_H__C56266FC_C73E_4EE2_93B5_92BC702E19F7__INCLUDED_)
