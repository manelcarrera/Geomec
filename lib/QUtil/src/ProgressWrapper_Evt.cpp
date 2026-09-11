// self
#include "ProgressWrapper_Evt.h"
#include "Events.h" // to forward messages to the doc (GUI thread)
#include "IProgressFactory.h"
#include "Printer.h"
// qt
#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QVBoxLayout>
// std
#include <thread>

namespace {
Printer *printer = Printer::instance(Printer::RunW);
}

const char *CProgressWrapper_Evt::cmd_cp[] = {"New",     "Delete", "AddSteps", "Step",  "Status",
                                              "NextJob", "Enable", "Current",  "Cancel"};

CProgressWrapper_Evt::CProgressWrapper_Evt(eProgress type, const QString &title, bool bCancel, int jobs)
    : CProgressBase(), m_type(type) {
  printer->info("new:%s  title:%s, cancel:%d, jobs:%d ", g_progress_cp[type], title.toStdString().c_str(), bCancel,
                jobs);

  if (m_type == Silent || m_type == DualSilent)
    return;

  _e->broadcast(Cmd(Progress, new CProgressData(CProgressData::New, type, 0, jobs, title, bCancel)));
}

void CProgressWrapper_Evt::on_cancel() {
  printer->info("on cancel");

  _e->broadcast(Cmd(Progress, new CProgressData(CProgressData::Cancel)));
}

//
// Only for testing
//
void CProgressWrapper_Evt::test() {
  for (int j = 0; j < 2; j++) {
    for (int i = 0; i < 100; i++) {
      if (i == 0 && j == 1)
        _e->broadcast(Cmd(Progress, new int(1))); // Next job
      // Step();
      _e->broadcast(Cmd(Progress, new int(0)));
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  }
}

CProgressWrapper_Evt::~CProgressWrapper_Evt() {
  printer->info("delete");

  _e->broadcast(Cmd(Progress, new CProgressData(CProgressData::Delete)));
}

/*!
  Status messages give by the progress indicator are not processed and displayed to the user.
*/

void CProgressWrapper_Evt::BlockStatusMessage(bool bBlockStatusMessage) {
  // m_bBlockStatusMessage = bBlockStatusMessage;
}

// set the total number of steps
void CProgressWrapper_Evt::AddSteps(int num_steps) {
  m_num_steps = 0;

  printer->info("add steps: %d", num_steps);

  if (m_type == Silent || m_type == DualSilent)
    return;

  _e->broadcast(Cmd(Progress, new CProgressData(CProgressData::AddSteps_, num_steps)));
}

// increment
void CProgressWrapper_Evt::Step(int num_steps) {
  m_num_steps += num_steps;
  if (!(m_num_steps % 100))
    printer->info("step : +%d -> %d", num_steps, m_num_steps);

  if (m_type == Silent || m_type == DualSilent)
    return;

  _e->broadcast(Cmd(Progress, new CProgressData(CProgressData::Step_, num_steps)));
}

void CProgressWrapper_Evt::StatusMessage(const QString &msg) {
  printer->info("status msg: %s", msg.toStdString().c_str());

  if (m_type == Silent || m_type == DualSilent)
    return;

  _e->broadcast(Cmd(Progress, new CProgressData(CProgressData::Status_, msg)));
}

void CProgressWrapper_Evt::SetProgress(int nCurrent) {
  printer->info("set progress : current:%d", nCurrent);

  if (m_type == Silent || m_type == DualSilent)
    return;

  _e->broadcast(Cmd(Progress, new CProgressData(CProgressData::Current, nCurrent)));
}

//
// TODO:
//
void CProgressWrapper_Evt::NextJob(const QString &name) {
  printer->info("next job : %s", name.toStdString().c_str());

  if (m_type == Silent || m_type == DualSilent)
    return;

  _e->broadcast(Cmd(Progress, new CProgressData(CProgressData::NextJob_, name)));
}

void CProgressWrapper_Evt::Enable(bool bEnable) {
  printer->info("enable : %d", bEnable);

  if (m_type == Silent || m_type == DualSilent)
    return;

  _e->broadcast(Cmd(Progress, new CProgressData(CProgressData::Enable_, bEnable)));
}