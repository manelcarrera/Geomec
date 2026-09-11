#if !defined(AFX_ATTRIGLOBALPRESSUREDLG_H__C2EB5BB5_D0EC_40DB_8886_5478B6FC1888__INCLUDED_)
#define AFX_ATTRIGLOBALPRESSUREDLG_H__C2EB5BB5_D0EC_40DB_8886_5478B6FC1888__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// attriglobalpressuredlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAttriGlobalPressureDlg dialog

#include "AttriGlobalFormationLoadDlg.h"
#include "GlobalPressure.h"

class CAttriGlobalPressureDlg : public CAttriGlobalFormationLoadDlg<CGlobalPressure>
{
public:
  CAttriGlobalPressureDlg(CGlobalPressure& pressure, CWnd* pParent = 0);

protected:
  DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_ATTRIGLOBALPRESSUREDLG_H__C2EB5BB5_D0EC_40DB_8886_5478B6FC1888__INCLUDED_)
