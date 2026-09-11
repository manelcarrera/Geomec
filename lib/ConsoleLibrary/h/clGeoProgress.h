// GeoProgress.h: interface for the CGeoProgress class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLGEOPROGRESS_H__667A34C8_800F_43ae_B947_5812D11F7608__INCLUDED_)
#define AFX_CLGEOPROGRESS_H__667A34C8_800F_43ae_B947_5812D11F7608__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <qstring.h>

class CWnd;

#include "ProgressBase.h"

class CGeoProgress : public CProgressBase  
{
  int m_nSteps;
  int m_nCurStep;
  QString m_strTitle;
  bool m_bDeleteDlg;
  bool m_bCancel;
  bool m_bBlockStatusMessage;
  QString m_strStatusMessage;
public:
  // This constructor creates a progress dialog with the following title
  CGeoProgress(CWnd* parent, const QString &sTitle, bool bCancel = true);
  CGeoProgress(CWnd* parent, unsigned int uTitle, bool bCancel = true);
  virtual ~CGeoProgress();

  virtual void AddSteps(int nSteps);
  virtual void Step(int nSteps = 1);
  virtual void StatusMessage(const QString& sMessage);
};

#endif // !defined(AFX_GEOPROGRESS_H__667A34C8_800F_43ae_B947_5812D11F7608__INCLUDED_)
