#if !defined(AFX_ANALYSISPOINTLIST_H__BF91CADD_F3DE_4EE4_8F31_AED027C72CF5__INCLUDED_)
#define AFX_ANALYSISPOINTLIST_H__BF91CADD_F3DE_4EE4_8F31_AED027C72CF5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AnalysisPointList.h : header file
//
#include "CAnalysisPoint.h"

/////////////////////////////////////////////////////////////////////////////
// CAnalysisPointList window



//##ModelId=3C204407030E
typedef BOOL (*AnalysisPointOkFuncPtr)(CAnalysisPoint* point);

//##ModelId=3C204407036B
class CAnalysisPointList : public CListCtrl
{
public:
  class IUserColumnInfo
  {
  public:
    virtual ~IUserColumnInfo(){};

    // column title (shown in the header of the list)
    virtual CString Title() const = 0;

    // column text for the given analysis point
    virtual CString Text(CAnalysisPoint& analysisPoint) const = 0;
  };

private:
	//##ModelId=3CAAA6720236
	AnalysisPointOkFuncPtr m_pAnalysisPointOkFunc;
	//##ModelId=3CAAA6720256
	CAnalysisPoint* m_pCurrentAnalysisPoint;

  const IUserColumnInfo* m_pUserColumnInfo;
  int m_iUserColumn;

// Construction
public:

  // CAnalysisPointList takes ownership of the IUserColumnInfo
	//##ModelId=3C20440703AA
	CAnalysisPointList(AnalysisPointOkFuncPtr pAnalysisPointOk, const IUserColumnInfo* pUserColumnInfo = 0);
	//##ModelId=3C20440703AC
	CAnalysisPointList(const IUserColumnInfo* pUserColumnInfo = 0);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAnalysisPointList)
	public:
	//##ModelId=3C20440703BA
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	//##ModelId=3C20440703CC
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//##ModelId=3C20440703D9
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	//##ModelId=3C20440703DB
	CAnalysisPoint* GetSelectedAnalysisPoint();
	//##ModelId=3C20440703DC
	void Update();
	//##ModelId=3C20440703DD
	virtual ~CAnalysisPointList();

	// Generated message map functions
protected:
	//{{AFX_MSG(CAnalysisPointList)
	//##ModelId=3C20440703DF
	afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	//##ModelId=3C2044080003
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	//##ModelId=3C2044080010
	void CreateColums();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ANALYSISPOINTLIST_H__BF91CADD_F3DE_4EE4_8F31_AED027C72CF5__INCLUDED_)
