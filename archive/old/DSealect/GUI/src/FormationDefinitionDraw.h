// FormationDefinitionDraw.h: interface for the CFormationDefinitionDraw class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FORMATIONDEFINITIONDRAW_H__C4C3E79D_7592_4BCE_8121_6E5B9FA7DB9B__INCLUDED_)
#define AFX_FORMATIONDEFINITIONDRAW_H__C4C3E79D_7592_4BCE_8121_6E5B9FA7DB9B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DSealectDrawView.h"
#include "axisx.h"

//##ModelId=3C0DC15B008C
class CFormationDefinitionDraw : public CDSealectDrawView  
{
private:
	//##ModelId=3C8D9E1B005F
	bool m_bMouseIsDown;
	
public:
	//##ModelId=3C0DC15B009D
	CFormationDefinitionDraw();
	//##ModelId=3C0DC15B009E
	virtual ~CFormationDefinitionDraw();
protected:
	//{{AFX_VIRTUAL(CFormationDefinitionDraw)
	public:
	//##ModelId=3C6A14C501C3
	virtual void OnInitialUpdate();
	//##ModelId=3C6A14C501C5
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	//##ModelId=3C0DC15B00AC
	virtual void OnDraw(CDC* pDC);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_DATA(CFormationDefinitionDraw)
	//}}AFX_DATA


	//{{AFX_MSG(CFormationDefinitionDraw)
	//##ModelId=3C0DC15B00BD
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//##ModelId=3C0DC15B00AF
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//##ModelId=3C0DC15B00B3
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//##ModelId=3DDA0C5400AF
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	DECLARE_DYNCREATE(CFormationDefinitionDraw)
};

#endif // !defined(AFX_FORMATIONDEFINITIONDRAW_H__C4C3E79D_7592_4BCE_8121_6E5B9FA7DB9B__INCLUDED_)
