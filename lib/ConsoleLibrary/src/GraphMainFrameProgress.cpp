
#include "clGraphMainFrameProgress.h"

CGraphMainFrameProgress::CGraphMainFrameProgress() : CProgressBase(), m_message(), m_messageSet(false) {}

CGraphMainFrameProgress::~CGraphMainFrameProgress() {}

void CGraphMainFrameProgress::AddSteps(int /*steps*/) { assert(m_messageSet); }

void CGraphMainFrameProgress::Step(int /*steps*/) {}

void CGraphMainFrameProgress::StatusMessage(const QString &message) {
  m_message = message;
  m_messageSet = true;
}
