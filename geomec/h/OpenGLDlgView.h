// OpenGLDlgView.h: interface for the COpenGLDlgView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPENGLDLGVIEW_H__A7BF431D_F23B_4D51_851E_7E466CE6C7F4__INCLUDED_)
#define AFX_OPENGLDLGVIEW_H__A7BF431D_F23B_4D51_851E_7E466CE6C7F4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CAttriColorGradientDlg;
class CAttriHotSpotDlg;
class IColorScaleNode;

#include "OpenGLView.h"

// class is only used by colorgradient and hotspot dialogs.....
class COpenGLDlgView : public COpenGLView  
{
//	friend CAttriColorGradientDlg;
//	friend CAttriHotSpotDlg;

	CRect m_rcRect; // for colorgradient dialog...
	CRect m_rcLeftRect; // only for hotspot dialog...
	CRect m_rcRightRect; // only for hotspot dialog...
	bool m_bCenterIsTransparent; // only for hotspot dialog...
	IColorScaleNode *m_pColorScale;
public:
	COpenGLDlgView();
	virtual ~COpenGLDlgView();

	void OnCreate(LPCREATESTRUCT lpCreateStruct);
	void DrawColorScale(IColorScaleNode &colorscale);
	void ColorScale(IColorScaleNode &colorscale) {m_pColorScale = &colorscale;}
	void Rect(CRect &rect) {m_rcRect = rect;}
	void SetHotSpotRects(CRect &LeftRect, CRect &RightRect);
	void CenterIsTransparent(const bool bNewVal) {m_bCenterIsTransparent = bNewVal;}
	bool CenterIsTransparent() {return m_bCenterIsTransparent;}
	void DrawScene();

protected:
	
	int OnMouseActivate( CWnd* pDesktopWnd, unsigned int nHitTest, unsigned int message );
	////}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif // !defined(AFX_OPENGLDLGVIEW_H__A7BF431D_F23B_4D51_851E_7E466CE6C7F4__INCLUDED_)


