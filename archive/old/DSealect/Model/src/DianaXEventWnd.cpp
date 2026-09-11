// DianaXEventWnd.cpp : implementation file
//

#include "stdafx.h"
#include "DianaXEventWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[] = __FILE__;#endif  // _MSC_VER
#endif

/////////////////////////////////////////////////////////////////////////////
// CDianaXEventWnd

//##ModelId=3DDA0C430239
CDianaXEventWnd::CDianaXEventWnd()
{
	m_bIsSucceded=FALSE;
	m_bStopedByUser =FALSE;
}

//##ModelId=3DDA0C430249
CDianaXEventWnd::~CDianaXEventWnd()
{
}


BEGIN_MESSAGE_MAP(CDianaXEventWnd, CWnd)
	//{{AFX_MSG_MAP(CDianaXEventWnd)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//##ModelId=3DDA0C43023D
void CDianaXEventWnd::ResetFlags()
{
	for(int i=0 ;i<m_vecSuccesMessageFlag.size();i++)
	{
		m_vecSuccesMessageFlag[i] = FALSE;
		m_bStopedByUser;
	}
}

//##ModelId=3DDA0C43023B
int CDianaXEventWnd::AddSuccesMessageString(CString sSuccesMessageString)
{
	m_vecSuccesMessageString.push_back(sSuccesMessageString);
	m_vecSuccesMessageFlag.push_back(FALSE);
	return m_vecSuccesMessageFlag.size()-1;
}

// CDetailedResultsDraw drawing
//##ModelId=3DDA0C43024B
void CDianaXEventWnd::OnDiaMessage(LPCTSTR sMsg)
{
	CString tmp(sMsg);
	tmp.TrimLeft();
	tmp.TrimRight();

	if(tmp.IsEmpty())
		return;

	for(int i=0 ;i<m_vecSuccesMessageString.size();i++)
	{
		if(tmp.Find(m_vecSuccesMessageString[i] ) != -1)
			m_vecSuccesMessageFlag[i] = TRUE;
	}
}

//##ModelId=3DDA0C43024E
void CDianaXEventWnd::OnStopButton(BOOL FAR* ReallyStop) 
{
	m_bStopedByUser=TRUE;
	*ReallyStop=TRUE;
}

//##ModelId=3DDA0C430256
void CDianaXEventWnd::OnCloseDialog() 
{
	// TODO: Add your control notification handler code here
	
}

//##ModelId=3DDA0C430258
void CDianaXEventWnd::OnDiaError(LPCTSTR Error) 
{
	// TODO: Add your control notification handler code here
	
}

//##ModelId=3DDA0C43025B
void CDianaXEventWnd::OnDiaWarning(LPCTSTR Warning) 
{
	// TODO: Add your control notification handler code here
	
}

//##ModelId=3DDA0C430265
void CDianaXEventWnd::OnCallback(BOOL FAR* StopExecution) 
{
	// TODO: Add your control notification handler code here
	
}




BEGIN_EVENTSINK_MAP(CDianaXEventWnd, CWnd)
    //{{AFX_EVENTSINK_MAP(CCalculationWindowDlg)
	ON_EVENT(CDianaXEventWnd, IDC_DIANAX, 1 /* OnDiaMessage */, OnDiaMessage, VTS_BSTR)
	ON_EVENT(CDianaXEventWnd, IDC_DIANAX, 5 /* OnStopButton */, OnStopButton, VTS_PBOOL)
	ON_EVENT(CDianaXEventWnd, IDC_DIANAX, 6 /* OnCloseDialog */, OnCloseDialog, VTS_NONE)
	ON_EVENT(CDianaXEventWnd, IDC_DIANAX, 3 /* OnDiaError */, OnDiaError, VTS_BSTR)
	ON_EVENT(CDianaXEventWnd, IDC_DIANAX, 2 /* OnDiaWarning */, OnDiaWarning, VTS_BSTR)
	ON_EVENT(CDianaXEventWnd, IDC_DIANAX, 4 /* OnCallback */, OnCallback, VTS_PBOOL)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()


/////////////////////////////////////////////////////////////////////////////


//##ModelId=3DDA0C43023A
HWND CDianaXEventWnd::Create()
{
	//Register a window class
	LPCTSTR classname = 0;
	classname = AfxRegisterWndClass(0);

	//Create the window and return it's handle
	CWnd::CreateEx(NULL,classname,NULL,NULL,1,1,1,1,NULL,NULL);
	ASSERT(m_hWnd!=NULL);
	return m_hWnd;
}


/////////////////////////////////////////////////////////////////////////////
// CDianaXEventWnd message handlers
