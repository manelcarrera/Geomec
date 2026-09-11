// DDXQuantity.h: interface for the CDDXQuantity class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DDXQUANTITY_H__632224BD_3907_403F_AEA0_46B576C3092F__INCLUDED_)
#define AFX_DDXQUANTITY_H__632224BD_3907_403F_AEA0_46B576C3092F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CDependenciesManager.h"

void AFXAPI DDV_Quantity(CDataExchange* pDX, int nIDC, CDoubleQuantity *pQ,CDependenciesManager* pDM,CString strQuantityName);


#endif // !defined(AFX_DDXQUANTITY_H__632224BD_3907_403F_AEA0_46B576C3092F__INCLUDED_)
