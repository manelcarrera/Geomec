#if !defined(AFX_DIANAXEVENTWND_H__5D2B2B18_E92E_4D72_9281_4ED75C492BFF__INCLUDED_)
#define AFX_DIANAXEVENTWND_H__5D2B2B18_E92E_4D72_9281_4ED75C492BFF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DianaXEventWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDianaXEventWnd window


#define  IDC_DIANAX		1

//##ModelId=3DDA0C43019A
class CDianaXEventWnd : public CWnd
{
private :
	//##ModelId=3DDA0C4301AA
	BOOL m_bIsSucceded;
	//##ModelId=3DDA0C4301AB
	BOOL m_bStopedByUser;
	//##ModelId=3DDA0C430219
	std::vector<CString> m_vecSuccesMessageString;
	//##ModelId=3DDA0C430229
	std::vector<BOOL> m_vecSuccesMessageFlag;
// Construction
public:
	//##ModelId=3DDA0C430239
	CDianaXEventWnd();
	//##ModelId=3DDA0C43023A
	HWND Create();

// Attributes
public:

// Operations
public:
	//##ModelId=3DDA0C43023B
	int AddSuccesMessageString(CString sSuccesMessageString);
	//##ModelId=3DDA0C43023D
	void ResetFlags();

	//##ModelId=3DDA0C430246
	BOOL IsSucceded(int SuccesID) {return m_vecSuccesMessageFlag[SuccesID];}
	//##ModelId=3DDA0C430248
	BOOL StopedByUser() {return m_bStopedByUser;}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDianaXEventWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	//##ModelId=3DDA0C430249
	virtual ~CDianaXEventWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDianaXEventWnd)
		// NOTE - the ClassWizard will add and remove member functions here.
	//##ModelId=3DDA0C43024B
	afx_msg void OnDiaMessage(LPCTSTR Message);
	//##ModelId=3DDA0C43024E
	afx_msg void OnStopButton(BOOL FAR* ReallyStop);
	//##ModelId=3DDA0C430256
	afx_msg void OnCloseDialog();
	//##ModelId=3DDA0C430258
	afx_msg void OnDiaError(LPCTSTR Error);
	//##ModelId=3DDA0C43025B
	afx_msg void OnDiaWarning(LPCTSTR Warning);
	//##ModelId=3DDA0C430265
	afx_msg void OnCallback(BOOL FAR* StopExecution);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIANAXEVENTWND_H__5D2B2B18_E92E_4D72_9281_4ED75C492BFF__INCLUDED_)
