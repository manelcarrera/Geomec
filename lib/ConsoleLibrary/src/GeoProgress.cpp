// GeoProgress.cpp: implementation of the CGeoProgress class.
//
//////////////////////////////////////////////////////////////////////

#include "GlobalMessage.h"
#include "clGeoProgress.h"
#include "GeomecStringTable.h"

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
CGeoProgress::CGeoProgress(CWnd* /*parent*/, const QString &sTitle,
  bool bCancel)
: CProgressBase(),
  m_nSteps(0),
  m_nCurStep(0),
  m_strTitle(sTitle),
  m_bDeleteDlg(true),
  m_bCancel(bCancel),
  m_bBlockStatusMessage(false)
{
}

CGeoProgress::CGeoProgress(CWnd* /*parent*/, unsigned int uTitle, bool bCancel)
: CProgressBase(),
  m_nSteps(0),
  m_nCurStep(0),
  m_bDeleteDlg(true),
  m_bCancel(bCancel),
  m_bBlockStatusMessage(false)
{
	m_strTitle = getStringTableEntry(uTitle);
}

CGeoProgress::~CGeoProgress()
{
}

// set the total number of steps
void CGeoProgress::AddSteps(int nSteps)
{
	// Start the next job when we are for the first time here 
	if(m_nSteps == 0)
  {
    StatusMessage(m_strTitle);
  }

	m_nSteps += nSteps;
}

// increment
void CGeoProgress::Step(int nSteps)
{
  int before = m_nCurStep / (m_nSteps / 80.);
  int after = (m_nCurStep + nSteps) / (m_nSteps / 80.);

  if (before != after)
  {
    _m()->outstream() << '*';
  }

	m_nCurStep += nSteps;
}

void CGeoProgress::StatusMessage(const QString& sMessage)
{
  if(sMessage != m_strStatusMessage)
  {
    _m()->outstream() << std::endl <<
      CProgressBase::getDateTimeStamp().toStdString() <<
      sMessage.toStdString() << std::endl;
    m_strStatusMessage = sMessage;
    if(m_nSteps > 0)
    {
      int n = m_nCurStep / (m_nSteps / 80.);
      for(int i = 0; i < n; ++i)
        _m()->outstream() << '*';
    }
  }
}
