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
class IProgressFactory;

#include "ProgressBase.h"
#include "Progress_.h" // eProgress
#include "ProgressWrapper_Data.h" // eCmd

namespace std{ class thread; }

//////////////////////////////////////////////////////////////////////
//
//
//				CProgressWrapper_Evt
//
//
//////////////////////////////////////////////////////////////////////

class CProgressWrapper_Evt : public CProgressBase  
{
  int m_num_steps;

public:
  static const char* cmd_cp[];
  static const char* type_cp[];

private:
  eProgress m_type; // needed to skip msg when Silent type

  IProgressFactory *m_factory;

public slots:
  void on_cancel();

public:
  
  CProgressWrapper_Evt(
    eProgress type, 
    const QString &title="", 
    bool bCancel=true,
    int jobs = 1 );

  virtual ~CProgressWrapper_Evt();

  void BlockStatusMessage(bool bEnable);

  virtual void AddSteps(int num_steps);
  virtual void Step(int num_steps = 1);
  virtual void StatusMessage(const QString& msg);
  virtual void Enable(bool bEnable);

  void SetProgress(int nCurrent);

  //
  // only IDualProgressBase
  //
  void NextJob(const QString& name );


  //
  // for testing purpose
  //
  void test();
};