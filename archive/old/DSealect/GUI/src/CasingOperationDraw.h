// CasingOperationDraw.h: interface for the CCasingOperationDraw class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CASINGOPERATIONDRAW_H__237B6085_E647_459B_A440_097FD4BE26EA__INCLUDED_)
#define AFX_CASINGOPERATIONDRAW_H__237B6085_E647_459B_A440_097FD4BE26EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DSealectDrawView.h"

//##ModelId=3C0DC1600242
class CCasingOperationDraw : public CDSealectDrawView  
{
public:
	//##ModelId=3C0DC1600244
	CCasingOperationDraw();
	//##ModelId=3C0DC1600245
	virtual ~CCasingOperationDraw();

protected:
	//{{AFX_VIRTUAL(CCasingOperationDraw)
	protected:
	//##ModelId=3C0DC1600251
	virtual void OnDraw(CDC* pDC);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_DATA(CCasingOperationDraw)
	//}}AFX_DATA


	//{{AFX_MSG(CCasingOperationDraw)
	//##ModelId=3DDA0C5D0274
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	DECLARE_DYNCREATE(CCasingOperationDraw)
};

#endif // !defined(AFX_CASINGOPERATIONDRAW_H__237B6085_E647_459B_A440_097FD4BE26EA__INCLUDED_)
