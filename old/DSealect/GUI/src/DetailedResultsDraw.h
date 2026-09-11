// DetailedResultsDraw.h: interface for the CDetailedResultsDraw class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DETAILEDRESULTSDRAW_H__721B56C9_A422_4616_9CF0_C9701E224D64__INCLUDED_)
#define AFX_DETAILEDRESULTSDRAW_H__721B56C9_A422_4616_9CF0_C9701E224D64__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SpreadSheet.h"
#include "DSealectDrawView.h"
#include "FgvView.h"
#include "axisx.h"
#include "Chart2D.h"

class CBatchTableRow;
class CAnalysisPoint;

//##ModelId=3DDA0C59012C
class CDetailedResultsDraw : public CDSealectDrawView
{

private:
	//##ModelId=3DDA0C59012E
	int m_iScreenHeight;
	//##ModelId=3DDA0C59013B
	int m_iScreenWidth;
	//##ModelId=3DDA0C59013C
	void SetBackColorColRow(long lCol, long lRow);

	void SetRedBackColorColRow(long lCol, long lRow,bool bRedYesWhiteNo);

	//##ModelId=3DDA0C59013F
	void SetBorder(long lCol, long lRow, long lStyle, long lType);
	//##ModelId=3DDA0C590144
	void FillValidLoadPhaseVectors();
	//##ModelId=3DDA0C59014B
	void FillValidTablesVector();
	//##ModelId=3DDA0C59014C
	void FillPlotPointsVector();
	//##ModelId=3DDA0C59014D
	double FindMaxPlotDamageLevel();
	//##ModelId=3DDA0C59014E
	double GetReverseCapacity(double dCapacity);

	//##ModelId=3DDA0C590151
	CSpreadSheet *m_pSpreadSheet;
	//##ModelId=3DDA0C59015C
	CChart2D     *m_pBarChart;
	//##ModelId=3DDA0C59016B
	CChart2D	 *m_pLineChart;
	//##ModelId=3DDA0C590170
	CFgvView     *m_pFgvView;
	//##ModelId=3DDA0C59017B
	CFgvView     *m_pFgvLegend;
	//##ModelId=3DDA0C59017F
	int			m_iShowMode;
	//##ModelId=3DDA0C59018A
	CAxisX		 *m_pAxis;
	

	//##ModelId=3DDA0C59019A
	std::vector<CBatchTableRow*> m_vecRows;
	//##ModelId=3DDA0C5901AA
	std::vector<int> m_vecTempLPV;
	//##ModelId=3DDA0C5901C9
	std::vector<int> m_vecValidLoadPhasesFormation;
	//##ModelId=3DDA0C5901D9
	std::vector<int> m_vecValidLoadPhasesCasing;
	//##ModelId=3DDA0C5901F8
	std::vector<int> m_vecValidLoadPhasesSealant;

	//##ModelId=3DDA0C590207
	std::vector<CString> m_vecLoadLabelsToPlot;

	//##ModelId=3DDA0C590227
	std::vector<int> m_vecValidTables;

	//std::vector< std::pair<double,double> > m_vecPointsToPlot;
	//##ModelId=3DDA0C590236
	std::vector< std::vector<double> > m_vecPointsToPlot;

	//##ModelId=3DDA0C590246
	CAnalysisPoint* m_pAnalysisPoint;
	
protected:
	//##ModelId=3DDA0C59023A
	CDetailedResultsDraw();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDetailedResultsDraw)

// Attributes
public:
	//##ModelId=3DDA0C590256
	CBatchTableRow* m_pBatchTableRow;
	//##ModelId=3DDA0C590264
	int m_iResultType;
	//##ModelId=3DDA0C590274
	int m_iScenario;
	//##ModelId=3DDA0C590283
	int m_iTypeOfDamage;
	//##ModelId=3DDA0C590284
	int m_iPlotSpace;
	//##ModelId=3DDA0C590293
	int m_iPresent;
	
// Operations
public:
	//##ModelId=3DDA0C5902A3
	void StartDraw();
	//##ModelId=3DDA0C5902A4
	void DrawBarChart();
	//##ModelId=3DDA0C5902A5
	void DrawLineChart();
	//##ModelId=3DDA0C5902A6
	void DrawTable();
	//##ModelId=3DDA0C5902B2
	void DrawFgvView();

  void SetAnalysisPoint(CAnalysisPoint* pAnalysisPoint);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDetailedResultsDraw)
	public:
	//##ModelId=3DDA0C5902B3
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	//##ModelId=3DDA0C5902F1
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	//##ModelId=3DDA0C590300
	virtual ~CDetailedResultsDraw();
#ifdef _DEBUG
	//##ModelId=3DDA0C590302
	virtual void AssertValid() const;
	//##ModelId=3DDA0C590304
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CDetailedResultsDraw)
	//##ModelId=3DDA0C590310
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//##ModelId=3DDA0C59032F
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//##ModelId=3DDA0C59033F
	afx_msg void OnMouseDownControl1(short Button, short Shift, long X, long Y);
	//##ModelId=3DDA0C59035E
	afx_msg void OnPopupCopy();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DETAILEDRESULTSDRAW_H__721B56C9_A422_4616_9CF0_C9701E224D64__INCLUDED_)
