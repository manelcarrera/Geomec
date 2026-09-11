#include "stdafx.h"

#include "GeomecDoc_Listener.h"

#include "Printer.h"

#include "Global.h" // FIXME

CGeomecDoc_Listener::CGeomecDoc_Listener()
    : IListener(CEvents::GuiApp_EH, {Progress, UpdateViews, DisableViews,
                                     Save_Done, // the update views can be performed
                                     Quit_RA,   // neatly ends run analysis process
                                     ChecksError, RunError, Log, DianaMsg, Question, Status, CancelRun}
                // needed in opening model case !!!
      ) {}

CGeomecDoc_Listener::~CGeomecDoc_Listener() {}

void CGeomecDoc_Listener::handle(Cmd cmd) {
  AfxGetApp()->GetMainWnd()->PostMessage(ID_ON_EVENT, (WPARAM)cmd.first, (LPARAM)cmd.second);
  //
  // development-only
  //
  // FIXME: without at least 1 ms, qt dlg on load model remains hidden
  //
  // if(_g->running())
  {
    if (CGlobal::instance()->dsa_delay())
      std::this_thread::sleep_for(std::chrono::milliseconds(CGlobal::instance()->dsa_delay()));
  }
}