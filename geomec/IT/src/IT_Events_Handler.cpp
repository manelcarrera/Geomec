#include "stdafx.h"

#include "GeomecPythonInterface.h" // m_safe_queue
#include "IT_Command.h"
#include "IT_Command_Task.h"
#include "IT_Events_Handler.h"
#include "Printer.h"
#include "SafeQueue.h"

#include "GuiCommandTask.h"

IT_Events_Handler::IT_Events_Handler(GeomecPythonInterface *gm_iface_)
    : gm_iface(gm_iface_) {} // : IEventsHandler( gm_iface_gui_ ){}

void IT_Events_Handler::run() {
  bool b_quit = false;
  Printer::instance()->debug("C++ : Req <- C++_Req_Q (events loop)");
  while (!b_quit) {
    Command cmd = gm_iface->requests_queue().dequeue();

    int cmd_id = cmd.cmd();
    switch (cmd_id) {
    case Command::eType::IT1_Control_Quit:
      b_quit = true;
      break;
    default:
      while (gm_iface->m_processing) // FIXME
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
      if (cmd_id == Command::eType::Control_Ini || cmd_id == Command::eType::Control_End)
        gm_iface->results_queue().enqueue(cmd); // Don't need to be treated
      else {
        Printer::instance()->debug("[EH] task : %s", cmd.name().c_str());
        gm_iface->m_processing = true;
        Launch_IT_Command_Task(gm_iface, cmd);
      }
      break;
    }
  }
  Printer::instance()->debug("EH : loop out");
}