#pragma once

//qt
#include <QString>
#include <QObject>
//own
#include "ProgressBase.h"
#include "Progress_.h" //eProgress


class QLabel;
class QProgressBar;

class QDlg; //own: custom QDialog
class QVBoxLayout;
class QPushButton;

namespace std{ class thread; }

//#define DEBUG_PROGRESS_DLG

class CProgressDlg_Qt : public QObject, public CProgressBase
{
  Q_OBJECT

  QDlg* m_dlg;
  QVBoxLayout* m_v_layout;

  QLabel* m_text;
  QLabel* m_text_2;

  QProgressBar* m_progress;
  QProgressBar* m_progress_2;


  QPushButton* m_btn;

  std::thread* m_thread;

  int m_job_idx;

  int m_num_jobs;

  //
  //
  //
  int m_num_steps;
  int m_cur_step;
  int m_previous_value;
  QString m_title;

#ifdef DEBUG_PROGRESS_DLG
  int m_count;
#endif

private:
  eProgress m_type;

private:
  bool on_progress(int p);
  bool set_progress(int num_steps, int cur_step);

public slots:
  void on_cancel();

public:
  
  CProgressDlg_Qt(
    eProgress type,
    const QString &sTitle="", 
    bool bCancel=true,
    int jobs = 1 );

  virtual ~CProgressDlg_Qt();

  void BlockStatusMessage(bool bEnable);

  virtual void AddSteps(int nSteps);
  virtual void Step(int nSteps = 1);
  virtual void StatusMessage(const QString& sMessage);
  virtual void Enable(bool bEnable);

  virtual void SetProgress(int nCurrent);

  //
  // only for IDualProgressBase
  //
  void NextJob(const QString& name );


  //
  // just for testing
  //
  void test();
  void refresh();

};
