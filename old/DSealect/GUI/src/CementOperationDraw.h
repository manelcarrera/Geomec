// CementOperationDraw.h: interface for the CCementOperationDraw class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CEMENTOPERATIONDRAW_H__17FB6734_9055_4B22_9C1F_EC4BEC11984D__INCLUDED_)
#define AFX_CEMENTOPERATIONDRAW_H__17FB6734_9055_4B22_9C1F_EC4BEC11984D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DSealectDrawView.h"

//##ModelId=3C0DC15F0196
class CCementOperationDraw : public CDSealectDrawView  
{
public:
	//##ModelId=3C0DC15F01A6
	CCementOperationDraw();
	//##ModelId=3C0DC15F01A7
	virtual ~CCementOperationDraw();
protected:
	//{{AFX_VIRTUAL(CCementOperationDraw)
	protected:
	//##ModelId=3C0DC15F01A9
	virtual void OnDraw(CDC* pDC);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_DATA(CCementOperationDraw)
	//}}AFX_DATA


	//{{AFX_MSG(CCementOperationDraw)
	//##ModelId=3DDA0C5B02A3
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	DECLARE_DYNCREATE(CCementOperationDraw)
};

#endif // !defined(AFX_CEMENTOPERATIONDRAW_H__17FB6734_9055_4B22_9C1F_EC4BEC11984D__INCLUDED_)
