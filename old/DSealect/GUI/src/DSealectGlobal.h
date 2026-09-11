

#if !defined(DS_DSEALECT_GLOBAL__INCLUDED_)
#define DS_DSEALECT_GLOBAL__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



class CDSealectApp;
class CDSealectDoc;
class CDataModel;

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif
	CString GetMatLibPath();
	CString GetInstallLocation();
	CString GetTempReg();
	CString GetTempParent();
	CString FindValidTempParent();
	CString GetFemGVPath();
	CString GetDianaPath();
  CString GetDianaSharePath();
	CDSealectDoc *GetDoc();
  CDataModel* GetModel();
	void SetUnitlabel(CStatic *pUnitLabel,CDoubleQuantity* pQ);
	CString GetUnitString(CDoubleQuantity* pQ);
#endif
