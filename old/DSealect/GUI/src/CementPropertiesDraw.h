// CementPropertiesDraw.h: interface for the CCementPropertiesDraw class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CEMENTPROPERTIESDRAW_H__E577ABFA_BD68_412E_9C31_0120A11B4126__INCLUDED_)
#define AFX_CEMENTPROPERTIESDRAW_H__E577ABFA_BD68_412E_9C31_0120A11B4126__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#ifndef __AFXEXT_H__
#include <afxext.h>
#endif


#include "DSealectDrawView.h"


//##ModelId=3C0DC15E033C
class CCementPropertiesDraw : public CDSealectDrawView  
{
public:
	//##ModelId=3C0DC15E034C
	CCementPropertiesDraw();
	//##ModelId=3C0DC15E034D
	virtual ~CCementPropertiesDraw();
protected:
	//{{AFX_VIRTUAL(CCementPropertiesDraw)
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_DATA(CCementPropertiesDraw)
	//}}AFX_DATA


	//{{AFX_MSG(CCementPropertiesDraw)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	DECLARE_DYNCREATE(CCementPropertiesDraw)
};

#endif // !defined(AFX_CEMENTPROPERTIESDRAW_H__E577ABFA_BD68_412E_9C31_0120A11B4126__INCLUDED_)
