// CMultiViewSplitter.h: interface for the CMultiViewSplitter class.
//
// Written by Caroline Englebienne of AniWorld, Inc.
// Copyright (c) 2000 AniWorld, Inc. 
/////////////////////////////////////////////////////////////////////////////


#if !defined(AFX_CMultiViewSplitter_H__464C08E9_8989_11D4_B4E3_005004D85AA1__INCLUDED_)
#define AFX_CMultiViewSplitter_H__464C08E9_8989_11D4_B4E3_005004D85AA1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable:4786)
#include <map>
using namespace std;

//##ModelId=3C0DC158029F
class CMultiViewSplitter : public CSplitterWnd  
{

public:
	//##ModelId=3C0DC15802A1
	void HideView(int nViewID);
	//##ModelId=3C0DC15802A3
	CMultiViewSplitter();
	//##ModelId=3C0DC15802AF
	virtual ~CMultiViewSplitter();
	//##ModelId=3C0DC15802B1
	int AddView(int nRow, int nCol, CRuntimeClass * pViewClass, 
				CCreateContext* pContext);
	//##ModelId=3C0DC15802C3
	void ShowView(int nViewID);
	//##ModelId=3C0DC15802C5
	CWnd* GetView(int nViewID);

protected:
	//##ModelId=3CAAA664017B
	map<int, long> m_mapViewPane;
	//##ModelId=3CAAA664019A
	map<long, int> m_mapCurrentViews;
	//##ModelId=3CAAA66401C8
	map<int, CWnd*> m_mapIDViews;

	//##ModelId=3C0DC158032C
	int m_nIDCounter;

	//##ModelId=3C0DC158032D
	CWnd* GetCurrentView(int nRow, int nCol, int * nCurID);
	//##ModelId=3C0DC158033E
	void SetCurrentView(int nRow, int nCol, int nViewID);
	//##ModelId=3C0DC158034D
	int HideCurrentView(int nRow, int nCol);
	//##ModelId=3C0DC158035D
	void GetPaneFromViewID(int nViewID, CPoint * pane);
}; 

#endif // !defined(AFX_CMultiViewSplitter_H__464C08E9_8989_11D4_B4E3_005004D85AA1__INCLUDED_)
