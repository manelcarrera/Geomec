#include "DSA_Context.h"
#include "Printer.h"

namespace {
Printer *printer = Printer::instance(Printer::RunA);
}

DSA_Context::DSA_Context() {
  printer->info("context : new");

  out.statuses = dia::IAnalysisStatusContainer();
  out.is_success = false;
  out.save_succeed = false;
  out.path = "";

  input.is_write_input_files = false;
  input.is_write_quad_dat = false; // true for export

  more.path = "";
  more.silent = false;
  more.title = "";
  more.type = Regular;

  scene.main = nullptr;
  scene.history = nullptr;
  scene.wellpath = nullptr;

  // Logs
  time.start = QDateTime::currentDateTime();
  time.save.start = QDateTime::currentDateTime();
  time.save.end = QDateTime::currentDateTime();

  // drc
  drc = nullptr;
  dlg = nullptr;
  dxw = nullptr;
  save = nullptr;
  retrieve = nullptr;
}

DSA_Context::~DSA_Context() { printer->info("context : deleted : nothing to delete"); }
