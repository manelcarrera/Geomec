// CasingStressDraw.h: interface for the CCasingStressDraw class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CASINGSTRESSDRAW_H__AABCB5E7_72C3_4F82_95D0_343EF1A8D077__INCLUDED_)
#define AFX_CASINGSTRESSDRAW_H__AABCB5E7_72C3_4F82_95D0_343EF1A8D077__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DSealectDrawView.h"
#include "BaseFormView.h"


//##ModelId=3C0DC1600138
class CCasingStressDraw : public CDSealectDrawView  
{
private:
	

public:
	//##ModelId=3C0DC160013A
	CCasingStressDraw();
	//##ModelId=3C0DC1600148
	virtual ~CCasingStressDraw();
protected:
	//{{AFX_VIRTUAL(CCasingStressDraw)
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_DATA(CCasingStressDraw)
	//}}AFX_DATA


	//{{AFX_MSG(CCasingStressDraw)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	DECLARE_DYNCREATE(CCasingStressDraw)
};

#endif // !defined(AFX_CASINGSTRESSDRAW_H__AABCB5E7_72C3_4F82_95D0_343EF1A8D077__INCLUDED_)
