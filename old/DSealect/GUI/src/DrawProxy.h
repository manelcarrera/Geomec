// DrawProxy.h: interface for the CDrawProxy class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRAWPROXY_H__8DFE5A7B_D57D_42DF_A754_E38CFF546ECB__INCLUDED_)
#define AFX_DRAWPROXY_H__8DFE5A7B_D57D_42DF_A754_E38CFF546ECB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DSealectDrawView.h"

//##ModelId=3C0DC15D0251
class CDrawProxy  
{
private:
	//##ModelId=3CAAA66C0236
	CView *m_pView;
	
public:
	//##ModelId=3C0DC15D02A2
	CDrawProxy();
	//##ModelId=3C0DC15D02A3
	virtual ~CDrawProxy();

	//##ModelId=3C0DC15D02AF
	void View(CView* newval){m_pView=newval;}
	//##ModelId=3C0DC15D02B1
	CView* View(){return m_pView;}

	//##ModelId=3C0DC15D02BF
	virtual void OnDraw(CDC* pDC)=0;
	//##ModelId=3C0DC15D02C2
	virtual void OnMouseMove(UINT nFlags, CPoint point)=0; 
	//##ModelId=3C0DC15D02DE
	virtual void OnLButtonUp(UINT nFlags, CPoint point)=0; 
	//##ModelId=3C0DC15D02EE
	virtual void OnLButtonDown(UINT nFlags, CPoint point)=0; 
	//##ModelId=3C0DC15D02FE
	virtual void OnRButtonDown(UINT nFlags, CPoint point)=0; 
	//##ModelId=3C0DC15D0302
	virtual void OnRButtonDblClk(UINT nFlags, CPoint point)=0; 
};

#endif // !defined(AFX_DRAWPROXY_H__8DFE5A7B_D57D_42DF_A754_E38CFF546ECB__INCLUDED_)
