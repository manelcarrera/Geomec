
#include "stdafx.h"

#include "DualProgress.h"
#include "IProgressFactory.h"

CDualProgress::CDualProgress(int jobs, const QString& primaryTitle,
  bool cancel, CWnd* parent)
: CProgressBase()
, m_progressDlg(jobs, primaryTitle.toStdString().c_str(), cancel, parent)
, m_secondaryTitle()
, m_totalSteps(0)
, m_currentSteps(0)
{
  m_progressDlg.StartDialog();
}

CDualProgress::~CDualProgress()
{
  m_progressDlg.DestroyWindow();
}

void CDualProgress::AddSteps(int steps)
{
  m_totalSteps += steps;

  // Force an update event!

  if (!m_progressDlg.SetProgress(m_totalSteps, m_currentSteps))
  {
  throw new CProgressCancel;
  }
}

void CDualProgress::Step(int steps)
{
  m_currentSteps += steps;

  if (!m_progressDlg.SetProgress(m_totalSteps, m_currentSteps))
  {
  throw new CProgressCancel;
  }
}

void CDualProgress::StatusMessage(const QString& message)
{
  // Sets the secondary title!

  m_progressDlg.SetTitle(message.toStdString().c_str());
  m_secondaryTitle = message;
}

void CDualProgress::NextJob(const QString& secondaryTitle)
{
  m_progressDlg.NextJob(secondaryTitle.toStdString().c_str());
  m_secondaryTitle = secondaryTitle;
  m_totalSteps = 0;
  m_currentSteps = 0;
}
