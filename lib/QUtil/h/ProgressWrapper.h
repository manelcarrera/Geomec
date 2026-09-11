#pragma once

#include <qstring.h>

#include <QObject>

class QProgressBar;
class QDialog;
class QVBoxLayout;
class QGridLayout;
class QLabel;
class QPushButton;

class IProgressBase;

#include "ProgressBase.h"
#include "Progress_.h"				// eProgress
#include "ProgressWrapper_Evt.h"	// eCmd

namespace std{ class thread; }

//
// FIXME: mcr 2020-09-02: this wrapps almost nothing, it only skips actions on silent and dual-silent implementation
//	so maybe (think about it) it has no reason to exist ... just move the functionality to the implementation
//
class CProgressWrapper : public CProgressBase  
{
  int m_num_steps;

  IProgressBase* m_progress;

public:

  static const char* cmd_cp[];
  static const char* type_cp[];

private:
  eProgress m_type; // needed to skip msg when Silent type

  IProgressFactory *m_factory;

public slots:
  void on_cancel();

public:
  
  CProgressWrapper(
    eProgress type, 
    const QString &title="", 
    bool bCancel=true,
    int jobs = 1 );

  virtual ~CProgressWrapper();

  void BlockStatusMessage(bool bEnable);

  virtual void AddSteps(int num_steps);
  virtual void Step(int num_steps = 1);
  virtual void StatusMessage(const QString& msg);
  virtual void Enable(bool bEnable);

  void SetProgress(int nCurrent);

  //
  // only IDualProgressBase
  //
  void NextJob(const QString name );


  //
  // for testing purpose
  //
  void test();
};