// AnalysisDraw.h: interface for the CAnalysisDraw class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ANALYSISDRAW_H__8FD1001D_2033_4918_AEF1_AACD2C2A2827__INCLUDED_)
#define AFX_ANALYSISDRAW_H__8FD1001D_2033_4918_AEF1_AACD2C2A2827__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DSealectDrawView.h"

class CBatchTableRow;

//##ModelId=3C0DC16100DA
class CAnalysisDraw : public CDSealectDrawView  
{
private:
	//##ModelId=3DDA0C600060
	void SetSize(); 
public:
	//##ModelId=3C0DC16100EA
	CAnalysisDraw();
	//##ModelId=3C0DC16100EB
	virtual ~CAnalysisDraw();

protected:
	DECLARE_DYNCREATE(CAnalysisDraw)

	//{{AFX_VIRTUAL(CAnalysisDraw)
	public:
	protected:
	//##ModelId=3C0DC16100ED
	virtual void OnDraw(CDC* pDC);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_DATA(CAnalysisDraw)
	//}}AFX_DATA


	//{{AFX_MSG(CAnalysisDraw)
	//##ModelId=3DDA0C600082
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//##ModelId=3DDA0C6000A0
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()


};

#endif // !defined(AFX_ANALYSISDRAW_H__8FD1001D_2033_4918_AEF1_AACD2C2A2827__INCLUDED_)
