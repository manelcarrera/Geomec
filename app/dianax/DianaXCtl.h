#if !defined(AFX_DIANAXCTL_H__3A01BC25_BA9F_11D2_B3B4_00104B2556D9__INCLUDED_)
#define AFX_DIANAXCTL_H__3A01BC25_BA9F_11D2_B3B4_00104B2556D9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "ProgressDlg.h"
#include "resource.h"
#include "stdafx.h"
#include <afxtempl.h>
#include <qstring.h>

class ProgressDlg;

// DianaXCtl.h : Declaration of the CDianaXCtrl ActiveX Control class.

/////////////////////////////////////////////////////////////////////////////
// CDianaXCtrl : See DianaXCtl.cpp for implementation.

// just for the moc compiler
class QDumm : public COleControl, public QObject {};

class CDianaXCtrl : public QDumm {
  Q_OBJECT
  void Connect();
  void DisConnect();

private slots:
  void OnMessage(QString str);
  void OnAppSpcMessage(QString str);
  void OnError(QString str);
  void OnWarning(QString str);
  void OnStopExecuting(bool &bStop);
  void OnFinished();

private:
  DECLARE_DYNCREATE(CDianaXCtrl)

  // Constructor
public:
  CDianaXCtrl();
  char m_pchOldCwd[_MAX_PATH];
  BOOL m_bShowDialog;
  BOOL m_bCalculationSucceeded;
  BOOL m_bDisplayStopMessage;
  BOOL m_bDisplayDefaultMessages;
  CString m_strSucceedMessage;
  CString m_strUserLeader;
  CString m_strUserTrailer;
  long m_nTotalMessages;
  long m_nTotalAppSpcMessages;
  long m_nCurrentMessages;
  long m_nCurrentAppSpcMessages;
  CString m_strDialogCaption;
  BOOL m_bAutoCloseDialog;
  BOOL m_bAlwaysAutoCloseDialog;
  static CList<CDianaXCtrl *, CDianaXCtrl *> s_lstControlStack;

  static CDianaXCtrl *GetMe();
  void DoFireOnCallback(int *bStop);
  void DoFireOnDiaMessage(LPCTSTR Message) { FireOnDiaMessage(Message); }
  void DoFireOnDiaWarning(LPCTSTR Warning) { FireOnDiaWarning(Warning); }
  void DoFireOnDiaError(LPCTSTR Error) { FireOnDiaError(Error); }
  void DoFireOnStop(BOOL FAR *ReallyStop) { FireOnStopButton(ReallyStop); }
  void DeleteDlg();
  void StepProgressBar();
  void CloseDialog();

  CProgressDlg *m_pDlg;
  BOOL m_bProgressIsLifesign;

protected:
  BOOL m_bSizing;
  BOOL m_bChildIsRunning;

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CDianaXCtrl)
public:
  virtual void OnDraw(CDC *pdc, const CRect &rcBounds, const CRect &rcInvalid);
  virtual void DoPropExchange(CPropExchange *pPX);
  virtual void OnResetState();
  //}}AFX_VIRTUAL

  // Implementation
protected:
  ~CDianaXCtrl();

  DECLARE_OLECREATE_EX(CDianaXCtrl) // Class factory and guid
  DECLARE_OLETYPELIB(CDianaXCtrl)   // GetTypeInfo
  DECLARE_PROPPAGEIDS(CDianaXCtrl)  // Property page IDs
  DECLARE_OLECTLTYPE(CDianaXCtrl)   // Type name and misc status

  // Message maps
  //{{AFX_MSG(CDianaXCtrl)
  afx_msg void OnSize(unsigned int nType, int cx, int cy);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

  // Dispatch maps
  //{{AFX_DISPATCH(CDianaXCtrl)
  //}}AFX_DISPATCH
  // version 1.0
  afx_msg BOOL GetShowDialog();
  afx_msg void SetShowDialog(BOOL bNewValue);
  afx_msg BSTR GetSuccessMessageString();
  afx_msg void SetSuccessMessageString(LPCTSTR lpszNewValue);
  afx_msg long GetNumberOfMessages();
  afx_msg void SetNumberOfMessages(long nNewValue);
  afx_msg BOOL GetDisplayStopMessage();
  afx_msg void SetDisplayStopMessage(BOOL bNewValue);
  afx_msg BOOL GetShowDefaultMessages();
  afx_msg void SetShowDefaultMessages(BOOL bNewValue);
  afx_msg BOOL SetEnvironmentVar(LPCTSTR VarName, LPCTSTR Value);
  afx_msg short ExecuteDiana(LPCTSTR ExeName, LPCTSTR WorkingDir, LPCTSTR ComFile, LPCTSTR DatFile, LPCTSTR FilosFile,
                             LPCTSTR BaseName);
  afx_msg BOOL GetCalculationResult();
  afx_msg BOOL SetUserMagic(LPCTSTR Leader, LPCTSTR Trailer);

  // version 1.1
  afx_msg void SetDialogCaption(LPCTSTR lpszNewValue);
  afx_msg BSTR GetDialogCaption();
  afx_msg void SetAutoCloseDialog(BOOL bNewValue);
  afx_msg BOOL GetAutoCloseDialog();

  // version 2.1
  afx_msg BOOL GetProgressIsLifesign();
  afx_msg void SetProgressIsLifesign(BOOL bNewValue);

  // version 2.2
  afx_msg long GetNumberOfAppSpcMessages();
  afx_msg void SetNumberOfAppSpcMessages(long n);
  DECLARE_DISPATCH_MAP()

  // version 2.3
  afx_msg void SetAlwaysAutoCloseDialog(BOOL bNewValue);
  afx_msg BOOL GetAlwaysAutoCloseDialog();

  afx_msg void AboutBox();

  // Event maps
  //{{AFX_EVENT(CDianaXCtrl)
  //}}AFX_EVENT
  // version 1.0
  void FireOnDiaMessage(LPCTSTR Message) { FireEvent(eventidOnDiaMessage, EVENT_PARAM(VTS_BSTR), Message); }
  void FireOnDiaWarning(LPCTSTR Warning) { FireEvent(eventidOnDiaWarning, EVENT_PARAM(VTS_BSTR), Warning); }
  void FireOnDiaError(LPCTSTR Error) { FireEvent(eventidOnDiaError, EVENT_PARAM(VTS_BSTR), Error); }
  void FireOnCallback(BOOL FAR *StopExecution) { FireEvent(eventidOnCallback, EVENT_PARAM(VTS_PBOOL), StopExecution); }
  void FireOnStopButton(BOOL FAR *ReallyStop) { FireEvent(eventidOnStopButton, EVENT_PARAM(VTS_PBOOL), ReallyStop); }
  void FireOnCloseDialog() { FireEvent(eventidOnCloseDialog, EVENT_PARAM(VTS_NONE)); }
  DECLARE_EVENT_MAP()

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

    // version 2.3
    dispidAlwaysAutoCloseDialog = 14L
  };
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIANAXCTL_H__3A01BC25_BA9F_11D2_B3B4_00104B2556D9__INCLUDED)
