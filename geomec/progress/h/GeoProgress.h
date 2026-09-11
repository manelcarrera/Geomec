// GeoProgress.h: interface for the CGeoProgress class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GEOPROGRESS_H__080835FC_DA5D_48B5_B627_91CD872EF0E5__INCLUDED_)
#define AFX_GEOPROGRESS_H__080835FC_DA5D_48B5_B627_91CD872EF0E5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <qstring.h>

class CProgressDlg_MFC;

#include "ProgressBase.h"

class CGeoProgress : public CProgressBase {
  CProgressDlg_MFC *m_pDlg;
  int m_nSteps;
  int m_nCurStep;
  QString m_strTitle;
  bool m_bDeleteDlg;
  bool m_bCancel;
  bool m_bBlockStatusMessage;
  CWnd *m_parent;

public:
  // This constructor creates a progress dialog with the following title
  CGeoProgress(CWnd *parent, const QString &sTitle, bool bCancel = true);
  CGeoProgress(CWnd *parent, unsigned int uTitle, bool bCancel = true);
  // This constructor is for a
  CGeoProgress(CProgressDlg_MFC &dlg, const QString &sTitle);
  CGeoProgress(CProgressDlg_MFC &dlg, unsigned int uTitle);
  virtual ~CGeoProgress();

  void BlockStatusMessage(bool bEnable);

  virtual void AddSteps(int nSteps);
  virtual void Step(int nSteps = 1);
  virtual void StatusMessage(const QString &sMessage);
  virtual void Enable(bool bEnable);

  void SetProgress(int nCurrent);
};

#endif // !defined(AFX_GEOPROGRESS_H__080835FC_DA5D_48B5_B627_91CD872EF0E5__INCLUDED_)
