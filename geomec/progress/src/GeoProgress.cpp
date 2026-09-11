// GeoProgress.cpp: implementation of the CGeoProgress class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "geomec.h"
#include "GeoProgress.h"
#include "ProgressDlg_MFC.h"
#include "GeomecStringTable.h"
#include "IProgressFactory.h"
#include "FemAppMainWindow.h"

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif  // _MSC_VER
//#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CGeoProgress::CGeoProgress(CWnd* parent, const QString &sTitle, bool bCancel)
: CProgressBase(),
  m_bCancel(bCancel),
  m_bDeleteDlg(true),
  m_nCurStep(0),
  m_nSteps(0),
  m_bBlockStatusMessage(false),
  m_pDlg(0),
  m_strTitle(sTitle)
, m_parent(parent)
{
}

CGeoProgress::CGeoProgress(CWnd* parent, unsigned int uTitle, bool bCancel)
: CProgressBase(),
  m_bCancel(bCancel),
  m_bDeleteDlg(true),
  m_nCurStep(0),
  m_bBlockStatusMessage(false),
  m_nSteps(0),
  m_pDlg(0)
, m_parent(parent)
{
  m_strTitle = getStringTableEntry(uTitle);
}

CGeoProgress::CGeoProgress(CProgressDlg_MFC &dlg, const QString &sTitle)
: CProgressBase(),
  m_bCancel(false),		// Not used
  m_bDeleteDlg(false),
  m_nCurStep(0),
  m_nSteps(0),
  m_bBlockStatusMessage(false),
  m_pDlg(&dlg),
  m_strTitle(sTitle)
, m_parent(FemAppGetMainWnd())
{
}

CGeoProgress::CGeoProgress(CProgressDlg_MFC &dlg, unsigned int uTitle)
: CProgressBase(),
  m_bCancel(false),		// Not used
  m_bDeleteDlg(false),
  m_nCurStep(0),
  m_nSteps(0),
  m_bBlockStatusMessage(false),
  m_pDlg(&dlg)
, m_parent(FemAppGetMainWnd())
{
  m_strTitle = getStringTableEntry(uTitle);
}

CGeoProgress::~CGeoProgress()
{
  if(m_bDeleteDlg && m_pDlg)
  {
    m_pDlg->DestroyWindow();
    delete m_pDlg;
  }
}

/*!
  Status messages give by the progress indicator are not processed and displayed to the user.
*/

void CGeoProgress::BlockStatusMessage(bool bBlockStatusMessage)
{
  m_bBlockStatusMessage = bBlockStatusMessage;
}

// set the total number of steps
void CGeoProgress::AddSteps(int nSteps)
{
  // When we don't have a dialog create one
  if(!m_pDlg)
  {
    assert(m_bDeleteDlg);	// Should be deleted afterwards
    m_pDlg = new CProgressDlg_MFC(m_bCancel, m_parent);
    m_pDlg->StartDialog();
  }

  // Start the next job when we are for the first time here 
  if(m_nSteps == 0)
  {
    m_pDlg->NextJob(m_strTitle.toStdString().c_str());
  StatusMessage(m_strTitle);
  }

  m_nSteps += nSteps;
}

// increment
void CGeoProgress::Step(int nSteps)
{
  assert(m_pDlg); // must first call SetNrOfSteps

  m_nCurStep += nSteps;
  //wjrx assert(m_nCurStep <= m_nSteps); // too far!

  if(!m_pDlg->SetProgress(m_nSteps, m_nCurStep))
    throw new CProgressCancel;
}

void CGeoProgress::StatusMessage(const QString& sMessage)
{
  // When we don't have a dialog create one
  if(!m_pDlg)
  {
    assert(m_bDeleteDlg);	// Should be deleted afterwards
    m_pDlg = new CProgressDlg_MFC(m_bCancel, m_parent);
    m_pDlg->StartDialog();
  }

  assert(m_pDlg);
  if(!m_bBlockStatusMessage)
    m_pDlg->SetTitle(sMessage.toStdString().c_str());
}

void CGeoProgress::SetProgress(int nCurrent)
{
  assert(m_pDlg); // must first call AddSteps
  assert(nCurrent <= m_nSteps); // too far

  m_nCurStep = nCurrent;

  if(!m_pDlg->SetProgress(m_nSteps, m_nCurStep))
    throw new CProgressCancel;
}


void CGeoProgress::Enable(bool bEnable)
{
  m_pDlg->ShowWindow(SW_HIDE);
}