#include "stdafx.h"

#include "GraphMainFrameProgress.h"
#include "GraphMainFrm.h"
#include "FemAppMainWindow.h"

CGraphMainFrameProgress::CGraphMainFrameProgress()
: CProgressBase()
, m_message()
, m_graphMainFrm(dynamic_cast <CGraphMainFrm*> (FemAppGetMainWnd()))
, m_messageSet(false)
{
  assert(m_graphMainFrm != 0);
}

CGraphMainFrameProgress::~CGraphMainFrameProgress()
{
  m_graphMainFrm->EndProgress();
}

void CGraphMainFrameProgress::AddSteps(int steps)
{
  assert(m_messageSet);

  m_graphMainFrm->StartProgress(m_message.toStdString().c_str(), steps);
}

void CGraphMainFrameProgress::Step(int steps)
{
  for (int step = 0; step < steps; ++step)
  {
    m_graphMainFrm->ProgressStep();
  }
}

void CGraphMainFrameProgress::StatusMessage(const QString& message)
{
  m_message = message;
  m_messageSet = true;
}
