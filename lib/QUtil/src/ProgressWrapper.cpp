//self
#include "ProgressWrapper.h"
#include "Printer.h"
#include "Events.h" // to forward messages to the doc (GUI thread)
#include "IProgressFactory.h"
#include "Global.h"
//qt
#include <QProgressBar>
#include <QDialog>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
//std
#include <thread>

// TODO: asses this
#include "ProgressDlg_Qt.h"

namespace
{
  Printer* printer = Printer::instance(Printer::RunW);
}

CProgressWrapper::CProgressWrapper(
  eProgress type, 
  const QString &title, 
  bool bCancel,
  int jobs)
  :	CProgressBase(),
  m_type(type),
  m_progress(nullptr)
{
  printer->info("new:%s  title:%s, cancel:%d, jobs:%d ", 
    g_progress_cp[type], 
    title.toStdString().c_str(), 
    bCancel, 
    jobs);

  if (m_type == Silent || m_type == DualSilent)
    return;

  m_progress = _g->prog()->create_imp(m_type,title,bCancel,jobs);
}

//
// In principle this should't happen as m_progress (the dlg) also implements this method
//
void CProgressWrapper::on_cancel()
{
  printer->info("on cancel");

  //
  // FIXME
  // 
  static_cast<CProgressDlg_Qt*>( m_progress )->on_cancel();
}

//
// Only for testing
//
void CProgressWrapper::test()
{
  for( int j=0; j < 2; j++ )
  {
    for (int i = 0;i<100;i++)
    {
      if( i == 0 && j==1 )
        _e->broadcast(Cmd(Progress, new int(1))); // Next job
      //Step();
      _e->broadcast(Cmd(Progress, new int (0)));
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  }
}

CProgressWrapper::~CProgressWrapper()
{
  printer->info("delete");

  if(m_progress)
  {
    delete m_progress;
    m_progress = nullptr;
  }
}

/*!
  Status messages give by the progress indicator are not processed and displayed to the user.
*/

void CProgressWrapper::BlockStatusMessage(bool bBlockStatusMessage)
{
  //m_bBlockStatusMessage = bBlockStatusMessage;
}

// set the total number of steps
void CProgressWrapper::AddSteps(int num_steps)
{
  m_num_steps=0;

  printer->info("add steps: %d", num_steps);

  if (m_type == Silent || m_type == DualSilent)
  return;

  m_progress->AddSteps(num_steps);
}

// increment
void CProgressWrapper::Step(int num_steps)
{
  m_num_steps += num_steps;
  if(!(m_num_steps % 100))
    printer->info("step : +%d -> %d", num_steps, m_num_steps);


  if (m_type == Silent || m_type == DualSilent)
  return;

  m_progress->Step(num_steps);
}

void CProgressWrapper::StatusMessage(const QString& msg)
{
  printer->info("status msg: %s", msg.toStdString().c_str());

  if (m_type == Silent || m_type == DualSilent)
  return;

  m_progress->StatusMessage(msg);
}

void CProgressWrapper::SetProgress(int nCurrent)
{
  printer->info("set progress : current:%d", nCurrent);

  if (m_type == Silent || m_type == DualSilent)
  return;

  m_progress->SetProgress(nCurrent);
}

//
// TODO:
//
void CProgressWrapper::NextJob(const QString name)
{
  printer->info("next job : %s", name.toStdString().c_str());

  if (m_type == Silent || m_type == DualSilent)
  return;

  m_progress->NextJob(name);
}

void CProgressWrapper::Enable(bool bEnable)
{
  printer->info("enable : %d", bEnable);

  if (m_type == Silent || m_type == DualSilent)
  return;

  m_progress->Enable(bEnable);
}