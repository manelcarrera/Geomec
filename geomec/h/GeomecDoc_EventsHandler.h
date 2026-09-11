#pragma once

#include "Events.h"

#include "AnalysisType.h"

class CRunAnalysis;
class IExeDianaDlg;
class IProgressBase;
class CDianaStartUp;
class CGeomecDoc;
class CTest;
class TI;

namespace std {
class thread;
}

namespace doc {
namespace eh {
class TI;
}
} // namespace doc

class CGeomecDoc_EventsHandler {
private:
  CRunAnalysis *m_ra;
  IExeDianaDlg *m_dlg;
  IProgressBase *m_prg;

  CDianaStartUp *m_dsu;

  CGeomecDoc *m_doc;

  std::thread *m_th;

  CTest *m_test;

  TI *m_ti;

private:
  void on_prg(const Cmd &cmd);
  void on_diana_msg(const Cmd &cmd);
  std::string error_msg(const Cmd &cmd);
  void on_checks_error(const Cmd &cmd);
  void on_log(const Cmd &cmd);
  void on_status(const Cmd &cmd);
  void on_question(const Cmd &cmd);
  void on_quit_ra();
  void on_hb();
  void on_test(Cmd &cmd);

public:
  CGeomecDoc_EventsHandler();
  ~CGeomecDoc_EventsHandler();

  void handle(const Cmd &cmd);

  CRunAnalysis *ra() { return m_ra; }
  bool test() { return m_test != nullptr; };
  bool unittest() { return m_ti != nullptr; };

  void start(const CAnalysisType::TAnalysisType antype, bool bWriteInputFiles, bool bWriteQuadDat);
};