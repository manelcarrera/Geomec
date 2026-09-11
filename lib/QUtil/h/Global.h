#pragma once

class IProgressFactory;

#include <QString>
#include <thread>

class CGlobal {
  CGlobal();
  ~CGlobal();

public:
  enum eStatus { Idle, Running, Cancelling };

private:
  bool m_dsa;
  bool m_dsa_available;

  eStatus m_status;

  IProgressFactory *m_prog;

  std::thread::id m_gui_thread_id;

  QString m_qt_dlg_bg_color;

public:
  enum eDlg { RunDiana, Progress, NumDlg };

private:
  bool m_mfc_v[NumDlg];

public:
  static CGlobal *instance();

  bool dsa() { return m_dsa; }
  void dsa(bool val) { m_dsa = val; }

  //
  // not really needed
  //
  bool dsa_available() { return m_dsa_available; }
  void dsa_available(bool val) { m_dsa_available = val; }

  //
  // use MFC dialogs
  //
  bool mfc(eDlg dlg) { return m_mfc_v[dlg]; }
  void mfc(eDlg dlg, bool val) { m_mfc_v[dlg] = val; }

  IProgressFactory *prog() { return m_prog; }
  void prog(IProgressFactory *val);

  std::thread::id gui_thread_id() { return m_gui_thread_id; }
  void gui_thread_id(std::thread::id val) { m_gui_thread_id = val; }

  bool is_gui_thread() { return gui_thread_id() == std::this_thread::get_id(); }

  eStatus status() { return m_status; }
  void status(eStatus val) { m_status = val; }

  QString qt_dlg_bg_color() { return m_qt_dlg_bg_color; }
  void qt_dlg_bg_color(const QString &val) { m_qt_dlg_bg_color = val; }
};

namespace {
static CGlobal *_g = CGlobal::instance();
}