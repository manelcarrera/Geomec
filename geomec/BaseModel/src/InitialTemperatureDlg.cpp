// InitialTemperatureDlg.cpp : implementation file
//

#include "InitialTemperatureDlg.h"
#include "geomec.h"
#include "stdafx.h"

#ifdef _DEBUG
// #define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CInitialTemperatureDlg dialog

CInitialTemperatureDlg::CInitialTemperatureDlg(CGlobalTemperature &temp, CWnd *pParent)
    : CAttriGlobalFormationLoadDlg<CGlobalTemperature>(IDD_INITIAL_TEMPERATURE, temp, pParent) {}

BEGIN_MESSAGE_MAP(CInitialTemperatureDlg, CAttriGlobalFormationLoadDlg<CGlobalTemperature>)
ON_BN_CLICKED(IDC_RD_EXTRA_POLATE, OnExtraPolate)
ON_BN_CLICKED(IDC_RD_PRE_DEFINED, OnPreDefined)
END_MESSAGE_MAP()
