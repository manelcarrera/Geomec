// attriglobalpressuredlg.cpp : implementation file
//

#include "stdafx.h"
#include "geomec.h"
#include "attriglobalpressuredlg.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CAttriGlobalPressureDlg dialog

CAttriGlobalPressureDlg::CAttriGlobalPressureDlg(CGlobalPressure& pressure, CWnd* pParent)
: CAttriGlobalFormationLoadDlg<CGlobalPressure>(IDD_ATTRI_GLOBAL_PRESSURE, pressure, pParent)
{
}

BEGIN_MESSAGE_MAP(CAttriGlobalPressureDlg, CAttriGlobalFormationLoadDlg<CGlobalPressure>)
  ON_BN_CLICKED(IDC_RD_EXTRA_POLATE, OnExtraPolate)
  ON_BN_CLICKED(IDC_RD_PRE_DEFINED, OnPreDefined)
END_MESSAGE_MAP()
