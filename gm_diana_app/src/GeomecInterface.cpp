#include "GeomecInterface.h"

// std
#include <cstdio>
#include <iostream>
#include <thread>
// own
#include "DianaRunner.h"
#include "Printer.h"
// qt
#include <sstream> // to_string

GI::GI(const std::string &token)
    : IListener(CEvents::GI_EH, {}), IListenerIpc(IListenerIpc::Client, token), m_wait(CEvents::GI_EH) {
  m_printer_ipc->debug("new >>");

  m_dr = new DR(this);

  push_ipc(make_cmd(eCmd::LaunchDiana_Ack_));
}

//
// doing quit_thread( m_thread_run ) in Linux
//
// terminate called after throwing an instance of 'std::system_error'
//	what():  Invalid argument
//	what():  No such porcess
//
GI::~GI() {
  m_printer_ipc->debug("delete >>");
  delete m_dr;
  m_printer_ipc->info("<< delete");
}

void GI::wait() { m_wait.wait(); }

void GI::handle(Cmd cmd) {
  eCmd cmd_id = (eCmd)cmd.first;

  switch (cmd_id) {
  ////////////////////////////////////////////
  //
  // execute cmds coming from gm_app
  //
  ////////////////////////////////////////////
  case eCmd::RunDiana: {
    push_ipc(eCmd::RunDiana_Ack_);
    //
    m_dr->push(cmd);
    break;
  }
  case eCmd::HeartBeat: {
    push_ipc(HeartBeat_Ack);
    break;
  }
  case eCmd::QuitDiana: {
    push_ipc(QuitDiana_Ack);
    //
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // FIXME: so much?
    //
    if (m_dr->done())
      m_wait.signal();
    else
      m_dr->push(cmd);
    break;
  }
  //
  // force quit diana before having finished execution
  //
  case eCmd::QuitDiana_Done: {
    push_ipc(cmd);
    //
    m_wait.signal();
    break;
  }
  case eCmd::RunDiana_Done_: // run diana thread has finished the task
  {
    push_ipc(cmd);
    break;
  }
  default: {
    m_printer->error("cmd not handled : %s", g_cmd_s[cmd.first].c_str());
    break;
  }
  }
}

//
// needed !!!
//
void GI::received(Cmd cmd) { push(cmd); }
