#pragma once

#include <qstring.h>

#include "ProgressBase.h"
#include "ProgressDlg_MFC.h"

class CDualProgress : public CProgressBase
{
public:
  CDualProgress(int jobs, const QString& primaryTitle, bool cancel = true,
    CWnd* parent = 0);
  virtual ~CDualProgress();
  //base
  virtual void AddSteps(int steps);
  virtual void Step(int steps = 1);
  virtual void StatusMessage(const QString& message);
  //dual
  virtual void NextJob(const QString& secondaryTitle);

private:
  CDualProgress(const CDualProgress& rhs);
  CDualProgress& operator = (const CDualProgress& rhs);

  CProgressDlg_MFC m_progressDlg;
  QString m_secondaryTitle;
  int m_totalSteps;
  int m_currentSteps;
};
