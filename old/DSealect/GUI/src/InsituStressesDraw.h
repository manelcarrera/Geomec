// InsituStressesDraw.h: interface for the CInsituStressesDraw class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INSITUSTRESSESDRAW_H__92622BD8_1BF4_4FB2_A818_3264BFBE412C__INCLUDED_)
#define AFX_INSITUSTRESSESDRAW_H__92622BD8_1BF4_4FB2_A818_3264BFBE412C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DSealectDrawView.h"
#include "axisx.h"
#include "Chart2D.h"

class CFormation;

//##ModelId=3C0DC15A00FA
class CInsituStressesDraw : public CDSealectDrawView  
{
private:

	//##ModelId=3CA011A10232
	bool m_bMouseIsDown;
	//##ModelId=3DDA0C5201B9
	bool m_bIsLineDrawn;
	//##ModelId=3DDA0C5201C8
	bool m_bVerticalScroll;
	//##ModelId=3CA011A10242
	double m_lastY;
	
	//##ModelId=3C8D9E160244
	CAxisX *m_pAxis;
	//##ModelId=3C8D9E160264
	CChart2D* m_pChart;

	//##ModelId=3CA011A10261
	void DrawDepthLine(double dYVal=-1.0);

	//##ModelId=3DDA0C5201D8
	void DrawCoordSystem(CFormation *pFormation);

public:
	//##ModelId=3C0DC15A00FC
	CInsituStressesDraw();
	//##ModelId=3C0DC15A0109
	virtual ~CInsituStressesDraw();

protected:

	//{{AFX_VIRTUAL(CInsituStressesDraw)
	public:
	//##ModelId=3C6A14C203A7
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	//##ModelId=3C6A14C3001D
	virtual void OnInitialUpdate();
	protected:
	//##ModelId=3C0DC15A010B
	virtual void OnDraw(CDC* pDC);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_DATA(CInsituStressesDraw)
	//}}AFX_DATA


	//{{AFX_MSG(CInsituStressesDraw)
	//##ModelId=3C6A14C3002D
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//##ModelId=3C0DC15A012A
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//##ModelId=3C0DC15A011C
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//##ModelId=3C0DC15A010E
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//##ModelId=3DDA0C5201E7
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	DECLARE_DYNCREATE(CInsituStressesDraw)
};

#endif // !defined(AFX_INSITUSTRESSESDRAW_H__92622BD8_1BF4_4FB2_A818_3264BFBE412C__INCLUDED_)
