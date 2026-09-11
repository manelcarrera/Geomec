// DepthsForAnalysisDraw.h: interface for the CDepthsForAnalysisDraw class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEPTHSFORANALYSISDRAW_H__BDDFD317_2C05_40A9_A633_B2BB8D43AEAB__INCLUDED_)
#define AFX_DEPTHSFORANALYSISDRAW_H__BDDFD317_2C05_40A9_A633_B2BB8D43AEAB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DSealectDrawView.h"

//##ModelId=3C0DC15E003E
class CDepthsForAnalysisDraw : public CDSealectDrawView  
{
public:
	//##ModelId=3C0DC15E0040
	CDepthsForAnalysisDraw();
	//##ModelId=3C0DC15E0041
	virtual ~CDepthsForAnalysisDraw();
protected:
	//{{AFX_VIRTUAL(CDepthsForAnalysisDraw)
	protected:
	//##ModelId=3C0DC15E004E
	virtual void OnDraw(CDC* pDC);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_DATA(CDepthsForAnalysisDraw)
	//}}AFX_DATA


	//{{AFX_MSG(CDepthsForAnalysisDraw)
	//##ModelId=3DDA0C5A0003
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	DECLARE_DYNCREATE(CDepthsForAnalysisDraw)
};

#endif // !defined(AFX_DEPTHSFORANALYSISDRAW_H__BDDFD317_2C05_40A9_A633_B2BB8D43AEAB__INCLUDED_)
