#pragma once

#include <qobject.h>

#include <functional> //std::function

#include "Events.h"
#include "EventsDiana_.h"
#include "Interprocess_OLD.h"

// static const int NO_PID = -1;

// FIXME
// typedef std::pair< int, std::string > Cmd;

namespace std {
class thread;
}
class DI;
class IListenerIpc; // GI

//*******************************************************************
//
//  FILE:       QDianaStartUp.h
//  AUTHOR:     htg
//  PROJECT:    TestProject
//  COMPONENT:  CDianaStartUp
//  DATE:       11.09.2003
//  COPYRIGHT:  TNO-DIANA.BV 2003
//  COMMENTS:   Incapsulating Diana Startup and signaling messages,errors and warnings from the
//				executable
//				It is a Singleton (design pattern)
//*******************************************************************
class CDianaStartUp : public QObject {
  Q_OBJECT

  enum eError { NO_E = 0, STP0_E, FFDIR_E, DIAPATH_E, FF_E };

  bool m_quit;

public:
  static const QString FFDIR;
  static const QString FF;
  static const QString STP0;
  static const QString BASE;
  static const QString DISPLAY;
  static const QString USER_LEADER;
  static const QString USER_TRAILER;

  typedef std::function<void(DSA_Icon)> TFunction; // OLD

  //
  // signals from diana process
  //
  // non-dsa only
  //
signals:
  void OnMessage(QString str);
  void OnAppSpcMessage(QString str);
  void OnError(QString str);
  void OnWarning(QString str);
  void OnStopExecuting(bool &bStop); // ask for stopping
  void OnFinished();

public slots:
  // void on_finished();

public:
  // Singleton object
  static CDianaStartUp *instance();

  // start diana
  int RunDiana();
  int RunDiana_ONE_PROCESS();

  void on_diana_msg(eDianaMsg type_, const QString &str = "", bool bStop = false);

  // void register_cb( TFunction function_cb ){ m_function_cb = function_cb; };

  void Print_DianaEnv();
  void Print_Env();
  void SetDianaEnv();
  const QString GetDianaEnv(const QString &key);
  void SetDianaEnv(const QString &key, const QString &value);
  bool SetDianaEnv_v2(QString varName, QString value);

  void RunDianaCleanup();
  void ClearDianaEnv_All() const;
  void ClearDianaEnv_Model() const;

  void var(const QString &key, const QString &val);
  QString var(const QString &key);
  bool is_var(const QString &key);

public:
  /*int pid(){ return m_pid; };
  void pid( int val ){ m_pid = val; };
  bool is_pid( int val ){ return m_pid == val; };*/

  // bool multiprocess(){ return m_pid != NO_PID; };

  void quit(bool val) { m_quit = val; }
  bool quit() { return m_quit; }

private:
  // TFunction m_function_cb;

  // int m_pid; //process manager id

  // static CDianaStartUp* m_pThis;

  std::map<QString, QString> m_vars_m; // BASE, DISPLAY, USER_LEADER, USER_TRAILER

  DI *m_di;
  IListenerIpc *m_gi;

public:
  DI *di() { return m_di; };
  void di(DI *di) { m_di = di; };
  //
  //	+ Quit_HB
  //	+ Quit_Listen
  //	+ Quit_Exe
  //
  void delete_di();

  void gi(IListenerIpc *val) { m_gi = val; };

private:
  int RunDiana_DSA();
  // int RunDiana_DSA_01();
  int RunDiana_DSA_OLD();

  // int RunDiana_ONE_PROCESS();

  eError checks();

  void Print_DianaVars();
  void Print_DianaArgs(const char *argv[], int argc);
  void Free_DianaArgs(const char *argv[], int argc);

  // private constructor/destructor
  CDianaStartUp();
  virtual ~CDianaStartUp();
};