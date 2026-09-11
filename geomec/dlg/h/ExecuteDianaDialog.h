#if !defined(AFX_EXECUTEDIANADIALOG_H__5453F23D_1522_492a_97DD_5A3E874B586F__INCLUDED_)
#define AFX_EXECUTEDIANADIALOG_H__5453F23D_1522_492a_97DD_5A3E874B586F__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ExecuteDianaDialog.h : header file
//

class CDianaXCtrl;
class CProgressDlg_MFC;
class CPropExchange;

namespace dia
{
  class IDianaRunner;
}

#include "resource.h"
#include "ExecuteDianaSignals.h"

#include "IExecuteDianaDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CExecuteDianaDialog dialog

class CExecuteDianaDialog : public CDialog, public IExecuteDianaDialog
{

	enum Status{ Running, Finished };
	Status m_status;

private:
  CExecuteDianaSignals m_exe_diana_signals;

  //dia::IDianaRunner   *m_runner;

// Construction
public:
	CExecuteDianaDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CExecuteDianaDialog();

	bool m_bStop;
	bool m_bAutoCloseDialog; //only close if not succeded 
	bool m_bAlwaysAutoCloseDialog; //alway close dialog

	void AddMessage( QString Message );
	void AddWarning( QString Warning );
	void DisplayError( QString Error );
	int CallBack();
	void Finish();
	void Start(bool itemsValid);

// Dialog Data
	//{{AFX_DATA(CExecuteDianaDialog)
	enum { IDD = IDD_DIANAX };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExecuteDianaDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void DisplayLastItem( CListBox *ListBox );
	CDianaXCtrl *GetControl();

  //void ProcessStatus(QString Message);

	// Generated message map functions
	//{{AFX_MSG(CExecuteDianaDialog)
	afx_msg void OnButStop();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// CDianaXCtrl

public:
	void OnMessage(QString str);			
	void OnAppSpcMessage(QString str);			
	void OnError(QString str);			
	void OnWarning(QString str);			
	void OnStopExecuting(bool &bStop);	
	//void OnFinished(); 

private:
//	DECLARE_DYNCREATE(CDianaXCtrl)

// Constructor
public:
//	CDianaXCtrl();
	char m_pchOldCwd[_MAX_PATH];
	bool m_bShowDialog;
	//bool m_bCalculationSucceeded;
	bool m_bDisplayStopMessage;
	bool m_bDisplayDefaultMessages;
	//QString m_strSucceedMessage;
	QString m_strUserLeader;
	QString m_strUserTrailer;
	long m_nTotalMessages;
	long m_nTotalAppSpcMessages;
	long m_nCurrentMessages;
	long m_nCurrentAppSpcMessages;
	QString m_strDialogCaption;

	static CDianaXCtrl *GetMe();
	void DoFireOnCallback( int *bStop );
//	void DoFireOnDiaMessage( LPCTSTR Message ) {FireOnDiaMessage( Message );}
//	void DoFireOnDiaWarning( LPCTSTR Warning ) {FireOnDiaWarning( Warning );}
//	void DoFireOnDiaError( LPCTSTR Error ) {FireOnDiaError( Error );}
//	void DoFireOnStop( BOOL FAR *ReallyStop ) {FireOnStopButton( ReallyStop );}
	void StepProgressBar();
	void CloseDialog();

	void show(bool bNewValue){ m_bShowDialog = bNewValue; }
	bool is_show(){ return m_bShowDialog; }

	short init();
	short end(short iret);
	void clear();


  //void SetRunner(dia::IDianaRunner *runner);

	CProgressDlg_MFC *m_pDlgDianaCtrl;
	bool m_bProgressIsLifesign;

protected:
	bool m_bSizing;
	bool m_bChildIsRunning;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDianaXCtrl)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
  virtual void ExecuteDianaCleanup();
  //}}AFX_VIRTUAL

// Implementation
protected:
//	~CDianaXCtrl();

//	DECLARE_OLECREATE_EX(CDianaXCtrl)    // Class factory and guid
//	DECLARE_OLETYPELIB(CDianaXCtrl)      // GetTypeInfo
//	DECLARE_PROPPAGEIDS(CDianaXCtrl)     // Property page IDs
//	DECLARE_OLECTLTYPE(CDianaXCtrl)		// Type name and misc status

// Message maps
	//{{AFX_MSG(CDianaXCtrl)
	afx_msg void OnSize(unsigned int nType, int cx, int cy);
	//}}AFX_MSG
//	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CDianaXCtrl)
	//}}AFX_DISPATCH
	// version 1.0
	//afx_msg BOOL GetShowDialog();
	afx_msg bool GetShowDialog();
	afx_msg void SetShowDialog(bool bNewValue);
	//afx_msg QString GetSuccessMessageString();
	//afx_msg void SetSuccessMessageString(QString lpszNewValue);
	afx_msg long GetNumberOfMessages();
	afx_msg void SetNumberOfMessages(long nNewValue);
	afx_msg bool GetDisplayStopMessage();
public:
	afx_msg void SetDisplayStopMessage(bool bNewValue);
protected:
	//afx_msg BOOL GetShowDefaultMessages();
	afx_msg bool GetShowDefaultMessages();
public:
	afx_msg void SetShowDefaultMessages(bool bNewValue);
	afx_msg bool SetEnvironmentVar(QString VarName, QString Value);
	afx_msg short ExecuteDiana();
	afx_msg bool GetCalculationResult();
protected:
	afx_msg bool SetUserMagic(QString Leader, QString Trailer);

	// version 1.1
	afx_msg void SetDialogCaption(QString lpszNewValue);
	afx_msg QString GetDialogCaption();
public:
	afx_msg void SetAutoCloseDialog(bool bNewValue);
protected:
	afx_msg bool GetAutoCloseDialog();

	// version 2.1
	afx_msg bool GetProgressIsLifesign();
public:
	afx_msg void SetProgressIsLifesign(bool bNewValue);

	// version 2.2
protected:
	afx_msg long GetNumberOfAppSpcMessages();
	afx_msg void SetNumberOfAppSpcMessages(long n);
//	DECLARE_DISPATCH_MAP()

	//version 2.3
	afx_msg void SetAlwaysAutoCloseDialog(bool bNewValue);
	afx_msg bool GetAlwaysAutoCloseDialog();

	afx_msg void AboutBox();

// Event maps
	//{{AFX_EVENT(CDianaXCtrl)
	//}}AFX_EVENT
	// version 1.0
/*
	void FireOnDiaMessage(LPCTSTR Message)
		{FireEvent(eventidOnDiaMessage,EVENT_PARAM(VTS_BSTR), Message);}
	void FireOnDiaWarning(LPCTSTR Warning)
		{FireEvent(eventidOnDiaWarning,EVENT_PARAM(VTS_BSTR), Warning);}
	void FireOnDiaError(LPCTSTR Error)
		{FireEvent(eventidOnDiaError,EVENT_PARAM(VTS_BSTR), Error);}
	void FireOnCallback(BOOL FAR* StopExecution)
		{FireEvent(eventidOnCallback,EVENT_PARAM(VTS_PBOOL), StopExecution);}
	void FireOnStopButton(BOOL FAR* ReallyStop)
		{FireEvent(eventidOnStopButton,EVENT_PARAM(VTS_PBOOL), ReallyStop);}
	void FireOnCloseDialog()
		{FireEvent(eventidOnCloseDialog,EVENT_PARAM(VTS_NONE));}
*/
//	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
	//{{AFX_DISP_ID(CDianaXCtrl)
	//}}AFX_DISP_ID
	// version 1.0
	dispidShowDialog = 1L,
	dispidSuccessMessageString = 2L,
	dispidNumberOfMessages = 3L,
	dispidDisplayStopMessage = 4L,
	dispidShowDefaultMessages = 5L,
	dispidSetEnvironmentVar = 6L,
	dispidExecuteDiana = 7L,
	dispidGetCalculationResult = 8L,
	dispidSetUserMagic = 9L,
	eventidOnDiaMessage = 1L,
	eventidOnDiaWarning = 2L,
	eventidOnDiaError = 3L,
	eventidOnCallback = 4L,
	eventidOnStopButton = 5L,
	eventidOnCloseDialog = 6L,

	// version 1.1
	dispidDialogCaption = 10L,
	dispidAutoCloseDialog = 11L,

	// version 2.1
	dispidProgressIsLifesign = 12L,

	// version 2.2
	dispidNumberOfAppSpcMessages = 13L,

	//version 2.3
	dispidAlwaysAutoCloseDialog =14L
	};
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXECUTEDIANADIALOG_H__5453F23D_1522_492a_97DD_5A3E874B586F__INCLUDED_)
