#if !defined(AFX_RESULTOVERVIEWDRAW_H__D888D2DE_8CB6_4579_9AE6_8CC10A502586__INCLUDED_)
#define AFX_RESULTOVERVIEWDRAW_H__D888D2DE_8CB6_4579_9AE6_8CC10A502586__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ResultOverviewDraw.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CResultOverviewDraw view

#include "SpreadSheet.h"
#include "DSealectDrawView.h"
#include "Chart2D.h"

class CBatchTableRow;
class CAnalysisPoint;

/*
Copyright (c) 1995 FarPoint
*/

//##ModelId=3DDA0C4F011C
class CThisChart:public CChart2D
{
	public:
	//##ModelId=3DDA0C4F012D
		CThisChart() {}
	//##ModelId=3DDA0C4F012E
		~CThisChart() {}
	// Generated message map functions
protected:
	//{{AFX_MSG(CThisChart)
	//##ModelId=3DDA0C4F012F
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()	
};


//##ModelId=3CA0119D038A
class CResultOverviewDraw : public CDSealectDrawView
{

private:
	//##ModelId=3DDA0C4F0207
	void SetBackColorColRow(long lCol, long lRow);

	void SetRedBackColorColRow(long lCol, long lRow);

	//##ModelId=3DDA0C4F0217
	void SetBorder(long lCol, long lRow, long lStyle, long lType);
	//##ModelId=3DDA0C4F0237
	void FillValidLoadPhaseVectors();
	//##ModelId=3DDA0C4F0245
	double GetReverseCapacity(double dCapacity);

	//##ModelId=3CAAA66300EE
	CSpreadSheet *m_pSpreadSheet;
	//##ModelId=3CA0119D03BA



		

	//##ModelId=3CAAA66300FE
	CChart2D     *m_pChart;

	//##ModelId=3CA0119D03C8
	
	std::vector<CBatchTableRow*> m_vecRows;

	//##ModelId=3DDA0C4F0267
	std::vector<int> m_vecTempLPV;

	//##ModelId=3DDA0C4F0285
	std::vector<int> m_vecValidLoadPhasesFormation;
	//##ModelId=3DDA0C4F0294
	std::vector<int> m_vecValidLoadPhasesCasing;
	//##ModelId=3DDA0C4F02B4
	std::vector<int> m_vecValidLoadPhasesSealant;
	
protected:
	//##ModelId=3CA0119D03D8
	CResultOverviewDraw();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CResultOverviewDraw)

// Attributes
private:
	//##ModelId=3DDA0C4F02C3
	CAnalysisPoint* m_pAnalysisPoint;
	//bool m_bTableChart;
	//##ModelId=3DDA0C4F02D2
	int m_iWellComponent;
	//##ModelId=3DDA0C4F02D3
	int m_iPresentationDraw;

// Operations
public:
	//##ModelId=3DDA0C4F02E1
	void SetPresentationDraw(int iPresentationDraw);

  void SetAnalysisPoint(CAnalysisPoint* pAnalysisPoint);
  void SetWellComponent(int iWellComponent);

  //##ModelId=3DDA0C4F02F1
	void StartDraw();
	
	//##ModelId=3DDA0C4F02F2
	void DrawBarChart();

	//##ModelId=3DDA0C4F0301
	void DrawTable();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CResultOverviewDraw)
	public:
	//##ModelId=3CA0119D03E7
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	//##ModelId=3CA0119E004E
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	//##ModelId=3CA0119E005D
	virtual ~CResultOverviewDraw();
#ifdef _DEBUG
	//##ModelId=3CA0119E005F
	virtual void AssertValid() const;
	//##ModelId=3CA0119E006D
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CResultOverviewDraw)
	//##ModelId=3CA0119E007C
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//##ModelId=3DDA0C4F0310
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//##ModelId=3DDA0C4F0321
	afx_msg void OnMouseDownControl1(short Button, short Shift, long X, long Y);
	//##ModelId=3DDA0C4F035E
	afx_msg void OnPopupCopy();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RESULTOVERVIEWDRAW_H__D888D2DE_8CB6_4579_9AE6_8CC10A502586__INCLUDED_)
