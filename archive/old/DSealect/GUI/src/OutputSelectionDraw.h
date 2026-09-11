// OutputSelectionDraw.h: interface for the COutputSelectionDraw class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OUTPUTSELECTIONDRAW_H__70DBE4D6_A44F_4086_8894_E099EBA1D431__INCLUDED_)
#define AFX_OUTPUTSELECTIONDRAW_H__70DBE4D6_A44F_4086_8894_E099EBA1D431__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DSealectDrawView.h"

//##ModelId=3C0DC1580222
class COutputSelectionDraw : public CDSealectDrawView  
{
public:
	//##ModelId=3C0DC1580224
	COutputSelectionDraw();
	//##ModelId=3C0DC1580232
	virtual ~COutputSelectionDraw();
protected:

	//{{AFX_VIRTUAL(COutputSelectionDraw)
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_DATA(COutputSelectionDraw)
	//}}AFX_DATA


	//{{AFX_MSG(COutputSelectionDraw)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	DECLARE_DYNCREATE(COutputSelectionDraw)
};

#endif // !defined(AFX_OUTPUTSELECTIONDRAW_H__70DBE4D6_A44F_4086_8894_E099EBA1D431__INCLUDED_)
