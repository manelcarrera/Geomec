// self
#include "ProgressDlg_Qt.h"
#include "Printer.h"
#include "ProgressWrapper.h" //FIXME
#include "util_.h"

// qt
#include <QCoreApplication>
#include <QDialog>
#include <QKeyEvent>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QVBoxLayout>

#include <thread>

#include "Events.h"
#include "Global.h"

//
// custom QDialog to skip ESC keypress
//
class QDlg : public QDialog {
  Q_OBJECT
public:
  void keyPressEvent(QKeyEvent *e) {
    if (e->key() != Qt::Key_Escape)
      QDialog::keyPressEvent(e);
    else { /* skip event */
    }
  }
};

namespace {
Printer *printer = Printer::instance(Printer::RunP);

const char *types_cp[] = {"Geo", "Wait", "Dual", "Silent"};
} // namespace

//
// jobs param determines de number of progress bars instantiated
//
// TODO: reimplement this to don't need to know the jobs numer a priory... NettJob should create a new progress bar each
// tme it's called
//
CProgressDlg_Qt::CProgressDlg_Qt(eProgress type, const QString &title, bool bCancel, int jobs)
    : CProgressBase(), QObject(), m_type(type), m_btn(nullptr), m_job_idx(0), m_num_jobs(jobs), m_thread(nullptr),
      //
      // state
      //
      m_num_steps(0), m_cur_step(0), m_previous_value(0),
      //
      //
      //
      m_title(title) {
  printer->info("new:%s  title:%s, cancel:%d, jobs:%d ", types_cp[m_type], title.toStdString().c_str(), bCancel, jobs);

  if (m_type == eProgress::Silent)
    return;

  //
  // Widgets
  //

  m_v_layout = new QVBoxLayout();

  m_dlg = new QDlg();

  //
  // It has no header
  //
  m_dlg->setWindowFlags(Qt::FramelessWindowHint);

  // progress 1
  m_text = new QLabel(title, m_dlg);
  m_progress = new QProgressBar(m_dlg);

  // progress 2
  if (jobs > 1) {
    m_text_2 = new QLabel(title, m_dlg);
    m_progress_2 = new QProgressBar(m_dlg);
  }

  // cancel btn
  if (bCancel) {
    m_btn = new QPushButton("Cancel", m_dlg);
    m_btn->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum)); // it works!
    connect(m_btn, SIGNAL(clicked()), this, SLOT(on_cancel()));
  }

  //
  // Layout
  //

  m_dlg->setLayout(m_v_layout);

  m_v_layout->addWidget(m_text);
  m_v_layout->addWidget(m_progress);

  if (jobs > 1) {
    m_v_layout->addWidget(m_text_2);
    m_v_layout->addWidget(m_progress_2);
  }

  if (m_btn)
    m_v_layout->addWidget(m_btn, 0, Qt::AlignCenter); // it works!

  m_v_layout->addStretch();

  // m_dlg->setLayout(m_grid_layout);

  // m_dlg->adjustSize();

  m_dlg->setMinimumSize(QSize(400, 100));

  m_dlg->setStyleSheet(QString("background-color: %1;").arg(_g->qt_dlg_bg_color()));

  //
  // FIXME: trying...
  //
  m_dlg->setAttribute(Qt::WA_DeleteOnClose);

  if (m_type != eProgress::Silent)
    m_dlg->show();

  //
  // modal -> blocking so not what we want
  //
  // m_dlg->exec();

  //
  // first line is not needed
  //
  // m_dlg->setWindowState((m_dlg->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
  // m_dlg->activateWindow();

  //
  // this doesn't work, qt events are queued but not treated
  //
  // m_thread = new std::thread(&CProgressDlg_Qt::refresh, this);

#ifdef DEBUG_PROGRESS_DLG
  m_count = 0;
#endif
}

//
// TODO: To be implemented
//
void CProgressDlg_Qt::on_cancel() {
  printer->info("--on cancel--");

  //
  // to avoid more than one click
  //
  disconnect(m_btn, SIGNAL(clicked()), this, SLOT(on_cancel()));

  //
  // ???
  //
  // m_btn->hide();
  // m_dlg->hide();

  //
  // only when running
  //
  if (_g->status() == CGlobal::Running) {
    _g->status(CGlobal::Cancelling);
    _e->broadcast(CancelRun);
  }

  //
  // this crashes
  //
  // throw new CProgressCancel;
}

void CProgressDlg_Qt::refresh() {
  QCoreApplication::processEvents();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void CProgressDlg_Qt::test() {
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

CProgressDlg_Qt::~CProgressDlg_Qt() {
  printer->info("delete");

  if (m_type == eProgress::Silent)
    return;

  delete m_text;
  delete m_progress;

  if (m_num_jobs > 1) {
    delete m_text_2;
    delete m_progress_2;
  }

  if (m_btn)
    delete m_btn;

  delete m_v_layout;

  delete m_dlg;

  //
  // simulate thread, test-only
  //
  /*if (m_thread && m_thread->joinable())
  {
    m_thread->join();
    delete m_thread;
    m_thread=nullptr;
  }*/

  //
  // forbidden in constructor and destructor
  //
  // throw new CProgressCancel;
}

/*!
  Status messages give by the progress indicator are not processed and displayed to the user.
*/

void CProgressDlg_Qt::BlockStatusMessage(bool bBlockStatusMessage) {
  // m_bBlockStatusMessage = bBlockStatusMessage;
}

// set the total number of steps
void CProgressDlg_Qt::AddSteps(int nSteps) {
  printer->info("add steps: %d", nSteps);

  if (m_type == eProgress::Silent)
    return;

  //
  // FIXME: crash when m_num_jobs == 1
  //
  /*if(!m_num_steps)
  {
    NextJob(m_title);
    //
    // ????
    //
    StatusMessage(m_title);
  }*/
  m_num_steps += nSteps;
}

//
// this crashes:
//
//	- not the reason why:
//		- m_dlg->activateWindow
//		- Printer
//
void CProgressDlg_Qt::Step(int nSteps) {
  m_cur_step += nSteps;
  bool res = set_progress(m_num_steps, m_cur_step);
  if (!res) {
    // cancel
  }

  //
  // no need so far
  //
  // printer->info("is_foreground_process: %s", util::is_foreground_process( Printer::pid() ) ? "yes":"no" );
  //
  // surprisingly it works!
  //
  //
  // don't force the focus as it's annoying when you switch to another application to ebe get back to gm
  //
  // m_dlg->activateWindow();

  //
  // force process events !!!!
  //
  // doing only here should be enough as this is call often enough
  //

  //
  // why only when 'Self'
  //
  // if(CProgressWrapper::execute() == CProgressWrapper::Self)
  if (!_g->dsa())
    QCoreApplication::processEvents();

  //
  // this doesn't do the job
  //
  // m_dlg->update();

  //
  // FIXME: just to test dsa run (too fast so cancel btn can't be pushed)
  //
#ifdef DEBUG_PROGRESS_DLG
  if (_g->running()) {
    m_count++;
    if (m_count == 10) {
      // count=0;
      // emit m_btn->clicked();
      on_cancel();
    }
  }
#endif
  //
  //
  //
}

void CProgressDlg_Qt::StatusMessage(const QString &msg) {
  printer->info("status msg:%s", msg.toStdString().c_str());

  if (m_type == eProgress::Silent)
    return;

  QLabel *text = m_num_jobs > 1 ? m_text_2 : m_text;

  text->setText(msg);
}

//
// private
//
bool CProgressDlg_Qt::on_progress(int i) {
  //
  // min always 0
  //
  auto set = [](QProgressBar *p, int max, int pos) {
    p->setMinimum(0);
    p->setMaximum(max);
    p->setValue(pos);
  };

  if (m_num_jobs > 1) {
    set(m_progress_2, m_num_jobs * 1000, i + (m_job_idx - 1) * 1000);

    set(m_progress, 1000, i);

  } else {
    set(m_progress, 1000, i);
  }

  m_previous_value = i;

  return true;
}
bool CProgressDlg_Qt::set_progress(int num_steps, int cur_step) {
  int n = cur_step / (num_steps * 0.001);

  if (n != m_previous_value)
    return on_progress(n);

  return true;
}

void CProgressDlg_Qt::SetProgress(int nCurrent) {
  printer->info("set progress : current:%d", nCurrent);

  if (m_type == eProgress::Silent)
    return;

  m_cur_step = nCurrent;

  bool res = set_progress(m_num_steps, m_cur_step);
  if (!res) {
    // throw new CProgressCancel;
  }
}

//
//
//

void CProgressDlg_Qt::NextJob(const QString &name) {
  printer->info("next job : %s", name.toStdString().c_str());

  if (m_type == eProgress::Silent)
    return;

  m_job_idx++;

  QLabel *text = m_num_jobs > 1 ? m_text_2 : m_text;
  text->setText(name);
}

void CProgressDlg_Qt::Enable(bool bEnable) {
  printer->info("enable : %d", bEnable);

  if (m_type == eProgress::Silent)
    return;

  if (bEnable)
    m_dlg->show();
  else
    m_dlg->hide();
}

//////////////////////////////////////////////////////////
//
//
//				QDlg
//
//////////////////////////////////////////////////////////
//
// custom QDialog to skip ESC keypress
//
#ifdef KK
void QDlg::keyPressEvent(QKeyEvent *e) {
  if (e->key() != Qt::Key_Escape)
    QDialog::keyPressEvent(e);
  else { /* skip event */
  }
}
#endif