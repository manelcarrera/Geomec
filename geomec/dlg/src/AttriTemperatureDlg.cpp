#include "stdafx.h"
#include "geomec.h"

#include "AttriTemperatureDlg.h"

CAttriTemperatureDlg::CAttriTemperatureDlg(CTemperature& temperature, CWnd* pParent)
: CAttriFormationLoadDlg<CTemperature>(temperature, IDD_ATTRI_TEMPERATURE, pParent)
{
}

CString CAttriTemperatureDlg::UndefinedLabel() const
{
  return _T("Global values");
}

BEGIN_MESSAGE_MAP(CAttriTemperatureDlg, CDialog)
	// base class methods
	ON_BN_CLICKED(IDC_RB_CONSTANT, OnConstant)
	ON_BN_CLICKED(IDC_RD_UNDEFINED, OnUndefined)
	ON_BN_CLICKED(IDC_RD_PRE_DEFINED, OnPreDefined)
	ON_BN_CLICKED(IDC_RD_EXTRA_POLATE, OnExtraPolate)
END_MESSAGE_MAP()
