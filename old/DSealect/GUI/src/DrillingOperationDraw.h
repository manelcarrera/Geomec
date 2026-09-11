// DrillingOperationDraw.h: interface for the CDrillingOperationDraw class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRILLINGOPERATIONDRAW_H__A5DCE94E_A657_40DC_A99F_B4E58453B257__INCLUDED_)
#define AFX_DRILLINGOPERATIONDRAW_H__A5DCE94E_A657_40DC_A99F_B4E58453B257__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DSealectDrawView.h"
#include "AxisX.h"

//##ModelId=3C0DC15D0203
class CDrillingOperationDraw : public CDSealectDrawView  
{
private:
	//##ModelId=3DDA0C580062
	CAxisX *m_pAxisXInclination;
	//##ModelId=3DDA0C580071
	CAxisX *m_pAxisXAzimuthWNorth;
	//##ModelId=3DDA0C580081
	CAxisX *m_pAxisX3D;
	
public:
	//##ModelId=3C0DC15D0205
	CDrillingOperationDraw();
	//##ModelId=3C0DC15D0206
	virtual ~CDrillingOperationDraw();
protected:
	DECLARE_DYNCREATE(CDrillingOperationDraw)

	//{{AFX_VIRTUAL(CDrillingOperationDraw)
	public:
	//##ModelId=3DDA0C580090
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	//##ModelId=3C0DC15D0208
	virtual void OnDraw(CDC* pDC);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_DATA(CDrillingOperationDraw)
	//}}AFX_DATA


	//{{AFX_MSG(CDrillingOperationDraw)
	//##ModelId=3DDA0C5800BF
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

};

#endif // !defined(AFX_DRILLINGOPERATIONDRAW_H__A5DCE94E_A657_40DC_A99F_B4E58453B257__INCLUDED_)
