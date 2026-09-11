#if !defined(AFX_ATTRICOLORGRADIENTDLG_H__55EE8125_606E_4A65_90F6_20B2E3976D15__INCLUDED_)
#define AFX_ATTRICOLORGRADIENTDLG_H__55EE8125_606E_4A65_90F6_20B2E3976D15__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// attricolorgradientdlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAttriColorGradientDlg dialog
#include "AttributesTemplate.h" 
#include "ColorGradient.h"

class COpenGLDlgView;

#include "resource.h"

class CAttriColorGradientDlg : public CAttributesTemplate<CColorGradient>
{	
	friend class COpenGLDlgView;
public:
	class CLessRect
	{
	public:
		bool operator()(const CRect &rect1, const CRect &rect2) const 
		{

			return (rect1.left <= rect2.left);
		}
	};

private:
	typedef std::map<CRect, int, CLessRect> TRectMap;
	typedef std::pair<CRect, int> TRectIdPair;
	CPoint m_ptClicked; //point clicked in non-client coordinates...
	BOOL m_bInRect; //true if a small rectangle is clicked (LBUTTON Down)...
	CRect m_rcTempRec; //used to calculate the area that has to be invalidated when a small rectangle is dragged...
	TColor m_nTempColor;
	COpenGLDlgView *m_pOpenGLView;
	CToolTipCtrl *m_pToolTipCtrl;
	TRectMap m_mpRectMap;

	void UpdateToolTip();
	void ClearMapAndToolTip();
  void UpdateRangeControls();
	
	
// Construction
public:
	CAttriColorGradientDlg(CColorGradient& color_gradient, CWnd* pParent = NULL);   // standard constructor
	CRect GetRect(CColorGradient::color_iterator it);
	~CAttriColorGradientDlg();

	
	//{{AFX_DATA(CAttriColorGradientDlg)
	enum { IDD = IDD_ATTRI_COLOR_SCALE };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAttriColorGradientDlg)
	public:
//	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, unsigned int nID, CCreateContext* pContext = NULL);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL PreTranslateMessage(MSG* pMsg) ;

	// Generated message map functions
	//{{AFX_MSG(CAttriColorGradientDlg)
  afx_msg void OnAutoExtremes();
  afx_msg void OnManualExtremes();
	afx_msg void OnPaint();
	afx_msg void OnRButtonDown(unsigned int nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnInsertColorInterval();
	afx_msg void OnChangeColor();
	afx_msg void OnRemoveColor();
	afx_msg void OnMouseMove(unsigned int nFlags, CPoint point);
	afx_msg void OnLButtonDown(unsigned int nFlags, CPoint point);
	afx_msg void OnLButtonUp(unsigned int nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ATTRICOLORGRADIENTDLG_H__55EE8125_606E_4A65_90F6_20B2E3976D15__INCLUDED_)
