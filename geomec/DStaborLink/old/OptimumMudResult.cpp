// OptimumMudResult.cpp: implementation of the COptimumMudResult class.
//
//////////////////////////////////////////////////////////////////////

#include "OptimumMudResult.h"
#include "FormationBase.h"
#include "HexaFormation.h"
#include "Wellpath.h"
#include "geomec.h"
#include "stdafx.h"

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CString CResultValue::GetDsbCaseString(int iDsbCase) {
  if (iDsbCase == -1) {
    return CString("Optimum mudweight not calculated");
  }
  if (iDsbCase == 0) {
    return CString("Unknow error in mudweight calculation");
  }
  if (iDsbCase == 1) {
    return CString("Optimum mudweight found");
  } else if (iDsbCase == 2) {
    return CString("Well always stable");
  } else if (iDsbCase == 3) {
    return CString("Premature analysis failure");
  } else if (iDsbCase == 4) {
    return CString("Well always unstable");
  } else if (iDsbCase == 5) {
    return CString("Unable to determine stable mudweight");
  } else {
    ASSERT(FALSE);
    return CString("Unknow error in mudweight calculation");
  }
}

bool CResultValue::IsCalculated() { return !(m_iStatus < 0); }

CString CResultValue::Info() {
  CString ret;
  switch (m_iStatus) {
  case -1:
    ret = "Not calculated";
    break;
  case 0:
    ret = "Optimum mudweight not found";
    break;
  case 1:
    ret = "Optimum mudweight found";
    break;
  case 2:
    ret = "Well always stable";
    break;
  case 3:
    ret = "Premature analysis failure";
    break;
  case 4:
    ret = "Well always unstable";
    ;
    break;
  case 5:
    ret = "Optimum mudweight not found";
    break;
  case 6:
    ret = "Unable to determine stable mudweight";
    break;
  default:
    ASSERT(FALSE);
    break;
  }

  return ret;
}