#include "DianaXWrapper.h"

#include "IExecuteDianaDialog.h"

#include <QString>

CDianaXWrapper::CDianaXWrapper(IExecuteDianaDialog *exe_diana_dlg) : IDianaXWrapper(), m_exe_diana_dlg(exe_diana_dlg) {}

CDianaXWrapper::~CDianaXWrapper() {}

void CDianaXWrapper::SetDisplayStopMessage(bool displayStopMessage) {
  m_exe_diana_dlg->SetDisplayStopMessage(displayStopMessage);
}
void CDianaXWrapper::SetShowDefaultMessages(bool showDefaultMessages) {
  m_exe_diana_dlg->SetShowDefaultMessages(showDefaultMessages);
}
void CDianaXWrapper::SetAutoCloseDialog(bool autoCloseDialog) { m_exe_diana_dlg->SetAutoCloseDialog(autoCloseDialog); }
void CDianaXWrapper::SetProgressIsLifesign(bool progressIsLifesign) {
  m_exe_diana_dlg->SetProgressIsLifesign(progressIsLifesign);
}

void CDianaXWrapper::SetRunner(dia::IDianaRunner *runner) { m_exe_diana_dlg->SetRunner(runner); }

bool CDianaXWrapper::GetCalculationResult() { return m_exe_diana_dlg->GetCalculationResult(); }

short CDianaXWrapper::ExecuteDiana() {
  // 0 is Ok
  return m_exe_diana_dlg->ExecuteDiana();
}

void CDianaXWrapper::ExecuteDianaCleanup() { m_exe_diana_dlg->ExecuteDianaCleanup(); }
